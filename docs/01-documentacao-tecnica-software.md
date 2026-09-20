# Documentação técnica para desenvolvedores de software

**Produto:** cortador de grama autônomo  
**Repositório:** `inercial_nav_system` (nome legado; o software cobre a máquina inteira)  
**Público:** firmware embarcado, aplicação Raspberry Pi e simulação desktop  
**Data de referência:** setembro de 2026

---

## 1. Visão do produto

O produto é o **cortador de grama autônomo**: máquina de tração diferencial que percorre o gramado, corta, desvia de obstáculos e volta à base. A navegação inercial (IMU BNO080) é um **subsistema** — junto com encoders, bumpers, sensores de distância, lâmina, chuva e bateria — para localizar a máquina sem GPS contínuo nem fio perimetral.

O controle em tempo real (tração, lâmina, segurança, missão de cobertura) roda em um **STM32F103**. Supervisão, telemetria e comandos de alto nível rodam em um **Raspberry Pi Zero W**. No PC existe um **simulador em C + visualizador em Python** para desenvolver o mapa do jardim sem a máquina.

Três papéis de software convivem no mesmo repositório:

| Papel | Onde | Linguagem | Função |
|-------|------|-----------|--------|
| Firmware da máquina | `hardware/navigation_system_STM32/Core/` | C (HAL STM32) | Rodas, lâmina, encoders, IMU, bumpers, missão de corte, UART |
| Supervisor | `hardware/navigation_system_STM32/Raspi/` | Python 3 | Comandos UART, dashboard de status do cortador, testes de roda |
| Simulador de mapa | raiz: `main.c`, `simulator.c`, `navigation_system.py` | C + Python | Scan polar IRDA + pose → mapa do gramado |

O estado de runtime do cortador é a struct `lawn_mower_status` (`Core/Inc/controller.h`). Ela é serializada em `flat_lawn_mower_status` (`Core/Inc/communication.h`) e enviada packed little-endian para o Pi.

---

## 2. Arquitetura de software da máquina

```
                    ┌─────────────────────────────────────┐
                    │         Raspberry Pi Zero W         │
                    │  Raspi/main.py  ·  UART 115200      │
                    │  Comandos: PING / STATUS / MOVE     │
                    └───────────────┬─────────────────────┘
                                    │ USART2  PA2/PA3
                                    │ (app Blue Pill)
                                    │ USART1  PA9/PA10
                                    │ (IoT-Interface / bootloader)
                    ┌───────────────▼─────────────────────┐
                    │              STM32F103              │
                    │  main.c  →  run_uart_test() loop    │
                    │  controller · actuators · sensor    │
                    │  positioning (BNO080 I2C)           │
                    └─┬──────────┬──────────┬─────────────┘
                      │          │          │
                 I2C1 PB6/PB7  TIM2 IC   GPIO / L298N
                      │          │          │
                   BNO080     encoders   motores / bumpers
                   (IMU)      PA0/PA4    PB10/PB11 (roda dir.)
```

**Partição de responsabilidades**

- **STM32:** a máquina em tempo real — rodas, lâmina, odometria, pose inercial, bumpers, chuva, e-stop, ponte H.
- **Raspberry Pi:** orquestração do cortador, UI de status, testes de integração; em seguida planejamento de cobertura e mapa do jardim.
- **PC (simulador):** gerar `rasp_i2c_in_file.txt` com pontos IRDA e pose, plotar o mapa do gramado.

SPI full-duplex foi estudado (`AI/3-spi-hardware-connections.md`) mas **o caminho em produção atual é UART**. Não misture os dois protocolos no mesmo bring-up.

---

## 3. Mapa do repositório

```
inercial_nav_system/
├── main.c / simulator.c / simulator.h / makefile
│     Simulador desktop de scan IRDA + pose
├── navigation_system.py
│     Lê rasp_i2c_in_file.txt e plota o mapa (matplotlib)
├── sensor_node.py
│     Nó ROS (LaserScan + Imu) para VL53L0X e BNO08x
├── requirements.txt
├── hardware/navigation_system_STM32/
│   ├── Core/Inc, Core/Src     firmware
│   ├── Raspi/                 app Python no Pi
│   ├── motor_test_L298N/      bancada GPIO → L298N (sem STM32)
│   ├── flash_STM32F103_by_raspyberry_pi/
│   └── AI/                    notas de arquitetura e SPI
└── docs/                      esta documentação
```

---

## 4. Firmware STM32

### 4.1 Plataformas

| Placa | MCU | UART de app com o Pi | Observação |
|-------|-----|----------------------|------------|
| Blue Pill (protótipo) | STM32F103C8Tx | USART2 PA2/PA3 | Firmware atual em `main.c` |
| IoT-Interface | STM32F103RBT6 | USART1 PA9/PA10 | Mesmo link do bootloader; ver README de flash |

Relógio: HSE + PLL. USART 115200 8N1. I2C1 a 100 kHz.

### 4.2 Módulos

| Arquivo | Responsabilidade |
|---------|------------------|
| `main.c` | Init HAL, GPIO, TIM2/TIM3, I2C, USART1/2, `m_status`, loop `run_uart_test()` |
| `controller.c/h` | Missão do cortador: borda, zigue-zague, desvio, FIFO de trajetória |
| `actuators.c/h` | Rodas, encoders, emulação de pulsos, GPIO do H-bridge |
| `sensor.c/h` | Bumpers (8), stubs IRDA / chuva / lâmina |
| `positioning.c/h` | BNO080: linear accel + rotation vector, filtros, integração |
| `communication.c/h` | Flatten + serialize `lawn_mower_status` |
| `communication_test.c/h` | Protocolo UART de comandos |
| `menu.c` | Menu ASCII em USART1 (debug de roda) |
| `mov_simulator.c` | Tick de simulação de movimento |

### 4.3 Loop principal

Após o init, o firmware entra em loop chamando `run_uart_test(&m_status)`: bloqueia em `HAL_UART_Receive` de 1 byte de comando e responde. Com `ENCODER_SOURCE_TIME=1` (padrão na IoT-Interface), TIM3 gera um pulso emulado a cada `EMULATE_WHEEL_TICK_MS` (20 ms, 50 pps) enquanto o motor está comandado. TIM2 captura bordas do encoder real só quando a emulação está desligada.

### 4.4 Máquina de estados (`MowerState`)

```
STATE_ALIGN_TO_EDGE
        → STATE_FOLLOW_EDGE
                → STATE_CLOSED_AREA_DETECTED   (volta ao ponto inicial: dist < 20 cm e t > 15 s)
                        → STATE_ZIGZAG
                                ⇄ STATE_AVOID_OBSTACLE  (qualquer bumper)
```

Vários comportamentos ainda estão esboçados (TODOs em `controller.c`). A API de movimento já existe: `move_forward`, `move_backward`, `turn_clock`, `turn_counter`, `stop_movement`. Passos discretos: `SOFT_* = ±15`, `TURN_* = ±50` pulsos.

### 4.5 Atuadores e encoders

- Duas rodas: `WHEEL_RIGHT = 0`, `WHEEL_LEFT = 1`.
- Direções: `FORWARD`, `BACKWARD`, `STOP`, `ROTATING_CLOCK`, `ROTATING_COUNTER`.
- Status: `WHEEL_READY`, `WHEEL_ERROR`, `WHEEL_MOVING`.
- Roda direita: GPIO **PB10 = IN1**, **PB11 = IN2** (L298N). Lógica: forward `1/0`, backward `0/1`, stop `0/0`.
- Roda esquerda: GPIO **PB8/PB9** = J2-4/5 → L298N IN3/IN4.
- Encoder: `MOVE` é sempre em pulsos. Fonte única: `encoder_apply_delta()` em `actuators.c`.
  - Tempo (`ENCODER_SOURCE_TIME=1`, padrão IoT): +1/−1 a cada 20 ms enquanto o motor está ligado (50 pps).
  - Hardware (`ENCODER_SOURCE_TIME=0`): TIM2 — direito PA0/PA4; esquerdo TIM2_CH2 + PA5 (captura ainda não habilitada no init).
- `set_target_count()` zera a posição e para o motor quando `|encoder_position| >= target`.
- `CMD_SET_EMULATE_WHEEL` liga/desliga a fonte de tempo em runtime. Bumpers não são lidos nesta fase.

**Armadilha conhecida:** em `motion_control_init()` o campo `left_encoder_count` da struct de status recebe `pulse_count[ENCODER_RIGHT]`. Testes no Pi usam `left_encoder_count` para a roda direita. Não “corrija” sem atualizar `Raspi/test_right_wheel.py`.

### 4.6 Posicionamento inercial (BNO080) — subsistema de localização

- Endereço I2C: `0x4A << 1` (ADDR em pull-down).
- Relatórios: Accelerometer `0x01`, Linear Acceleration `0x04`, Rotation Vector `0x05`, intervalo `DT = 100 ms`.
- Buffer DMA 20 bytes; parse em `BNO080_ParseInputReport`.
- Accel: escala `0.0001`, média móvel `FILTER_SIZE = 10`, depois high-pass `alpha = 0.9`.
- Quaternion → Euler (roll, pitch, yaw) em radianos.
- Integração: `v += a * dt`, `p += v * dt` com `dt = 0,1 s`.
- `pos[]` na struct de status está documentado em **centímetros**.

`BNO080_activate()` está comentado no `main.c` atual (bypass para testes de UART). Reative quando o IMU estiver na I2C.

### 4.7 Logging

`LOG_DEBUG` / `INFO` / `WARNING` / `ERROR` / `CRITICAL` saem por **USART1**. Formato: timestamp desde boot + arquivo:linha + nível.

---

## 5. Protocolo UART (Pi ↔ STM32)

Definição: `communication_test.h` e `Raspi/main.py`. 115200 8N1, sem RTS/CTS.

| Byte | Comando | Payload extra | Resposta |
|------|---------|---------------|----------|
| `0x01` | `CMD_PING` | — | `"PING"` (4 bytes) |
| `0x02` | `CMD_STATUS` | — | `flat_lawn_mower_status` packed |
| `0x03` | `CMD_MOVE` | `wheel u8`, `direction u8`, `distance i32 LE` | nenhuma |
| `0x04` | `CMD_SET_EMULATE_WHEEL` | `enable u8` | `ack u8` (0 ou 1) |

`wheel`: 0 direita, 1 esquerda, 2 ambas.  
`direction`: 0 FORWARD … 4 ROTATING_COUNTER (igual ao enum C).

O Pi descarta RX residual antes de cada comando (`flush_input`) e espera 50 ms após ping/status, 250 ms após MOVE, para o handler do STM32 terminar.

### Layout packed (`flat_lawn_mower_status`)

Python: `FLAT_MOWER_STATUS_FORMAT` em `Raspi/flat_lawn_mower_status.py`. Qualquer campo novo **obrigatoriamente** altera C packed + format string + `STATUS_STRUCT_SIZE`. Struct packed, little-endian, `bool` como 1 byte.

Não envie ponteiros. A struct de trabalho no STM32 já não usa ponteiros nos campos de telemetria.

---

## 6. Aplicação Raspberry Pi

Diretório: `hardware/navigation_system_STM32/Raspi/`.

| Arquivo | Função |
|---------|--------|
| `main.py` | Cliente UART, dashboard Rich, teste de movimento 30 pulsos |
| `uart_client.py` | pyserial (`/dev/ttyS0` no Pi Zero W, `/dev/ttyAMA0` em Pi 3/4/5) |
| `flat_lawn_mower_status.py` | Unpack da struct |
| `test_right_wheel.py` | Suíte de pulsos da roda direita |
| `test_emulate_wheel.py` | Valida emulação vs encoder real |

Porta padrão: `UART_PORT = /dev/ttyS0`. Override: env `UART_PORT`. Debug remoto: `DEBUG_UART=1` (debugpy :5678).

Dependências típicas: `pyserial`, `rich`. GPIO de bancada (L298N direto no Pi) é **outro** programa: `motor_test_L298N/` — não misturar com o firmware.

Flash do MCU pelo Pi: `flash_STM32F103_by_raspyberry_pi/` (BOOT0/BOOT1/NRST + `stm32flash` em USART1).

---

## 7. Simulador desktop (mapa dinâmico)

Objetivo: desenvolver o pipeline **pose + scan polar → mapa do gramado** sem a máquina no campo.

### 7.1 Modelo (`simulator.c`)

- Scan de `SCAN_LENGHT = 100` passos.
- Ângulo: `θ = step · π / 100` (varrida de 180°).
- Comentários de hardware: resolução 1,8°, 200 medidas/volta, IRDA a 30 Hz (~33 ms).
- Sala simulada 1,41 m × 1,41 m. Parede à direita: `r = 1410 / cos(θ)` para `θ ∈ [0, π/4]`; fora disso `r = -1` (sem hit).
- Pose: `x = 0`, `y` incrementa 100 mm a cada 3 passos de scan. Pitch/roll/yaw = 0.

### 7.2 Fusão no `main.c` desktop

```
get_next_r_theta → get_position
se r ≠ -1:
    (x_local, y_local) = polar_to_cartesian(r, θ)
    irda_world = pos_robô + local
após SCAN_LENGHT:
    grava #irda_scans_x/y e #pos_scans_x/y em rasp_i2c_in_file.txt
```

O arquivo é o “barramento I2C” simulado (legado do nome). Tags:

```
#irda_scans_x:  <floats>
#irda_scans_y:  <floats>
#pos_scans_x:   <floats>
#pos_scans_y:   <floats>
```

### 7.3 Visualizador

`navigation_system.py` lê as quatro séries e plota scatter: IRDA (azul) e POS (vermelho). `SIZE = 1412` e `SCAN_LENGHT = 100` devem permanecer alinhados ao C.

Build:

```bash
make
./program          # Linux
program.exe        # Windows (já há target no makefile)
python navigation_system.py
```

Debug VS Code: configs `main.c` (gdb) e `navigation_system.py` (debugpy).

---

## 8. Nó ROS (`sensor_node.py`)

Publica `/scan` (`sensor_msgs/LaserScan`) e `/imu` (`sensor_msgs/Imu`).

- Ângulo −90° a +90°, incremento 1°, range 0,05–2,0 m.
- VL53L0X e BNO08x via CircuitPython I2C; fallback para simuladores se as libs Adafruit não existirem.
- Rate 10 Hz. O motor de varredura do ToF é considerado **externo** (o script não o aciona).

Não está ligado ao protocolo UART do STM32. Trate como caminho experimental de SLAM/ROS, paralelo ao firmware.

---

## 9. Como começar (checklist)

1. Clone o repo. Para o simulador: `gcc` + `make`; `pip install matplotlib` (venv na raiz).
2. Firmware: STM32CubeIDE ou Makefile do projeto em `hardware/navigation_system_STM32`. MCU **STM32F103C8Tx** (Blue Pill) ou **STM32F103RBT6** (IoT-Interface) — não misture o linker script.
3. Flash: ST-Link **ou** `flash_stm32.py` pelo Pi (placa IoT-Interface, jumpers P3 em Ctrl).
4. No Pi: UART habilitada, usuário em `dialout`/`gpio`, `python3 Raspi/main.py`.
5. Confirme PING antes de MOVE. Use emulação de encoder até a mecânica da roda esquerda existir. Lâmina desconectada em qualquer teste de tração.
6. Qualquer mudança em `flat_lawn_mower_status` exige rebuild C **e** ajuste do `struct` Python.

---

## 10. Convenções

- Unidades: posição de status em **cm**; simulador desktop em **mm**; IRDA de status em **cm**; heading em **graus**; Euler do BNO em **radianos**. Converta na borda — não misture no mesmo array.
- Enums C e constantes Python devem ser cópias literais (comentários “must match STM32”).
- Logs de firmware em USART1; protocolo de app em USART2 (Blue Pill).
- Código de produção de motores: só `actuators.c`. GPIO direto no Pi é só bancada.
- Não commitar `venv/`, `.o`, `.exe` nem dumps grandes (`grid_output.txt`).

---

## 11. Estado atual e débito técnico

Implementado e usável:

- UART comando/resposta, serialização packed, dashboard no Pi.
- Rodas esquerda e direita (J2 + L298N) com alvo de pulsos.
- Emulação tempo→pulso (`ENCODER_SOURCE_TIME`, 50 pps) até os discos existirem.
- Esqueleto da missão de corte (borda → área fechada → zigue-zague → desvio) e FIFO de movimentos.
- Simulador de mapa 2D do gramado.

Aberto:

- Encoder de hardware (ligar TIM2 e `ENCODER_SOURCE_TIME=0`).
- Leitura real de bumpers (GPIO lido, sem atualizar `bumpers[]`).
- IRDA, chuva, RPM da lâmina, bateria (`battery.h` / `system.h` inexistentes).
- Reativar e validar BNO080 no loop.
- Correção de deriva inercial (ZUPT, fusão encoder+IMU, heading do BNO em `heading_deg`).
- Protocolo com framing/CRC (proposto nas notas SPI; UART atual é raw).
- No simulador desktop, o Y do obstáculo usa `pos_x_values[step]` — revisar se deveria ser `pos_y_values`.

---

## 12. Referências internas

- `Core/Inc/controller.h` — contrato de estado.
- `Core/Inc/communication.h` — wire format.
- `hardware/navigation_system_STM32/AI/1-arquitecture.md` — decisão de duas placas.
- `hardware/navigation_system_STM32/AI/3.1-answer-spi-hardware-connections.md` — pinout SPI (não usado no app atual).
- `docs/02-documentacao-tecnica-mecanica.md` — chassi, lâmina, rodas e sensores do cortador.
- `docs/03-business-and-market.md` — linha de negócio e mercado do cortador.
