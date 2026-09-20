# Documentação técnica para projetista mecânico

**Produto:** cortador de grama autônomo  
**Público:** projeto de chassi, lâmina, trem de força, carenagem e envelopes de sensores  
**Data de referência:** setembro de 2026

O produto é a **máquina de corte**, não um módulo de navegação. Este documento descreve o cortador que o software já modela — chassi, rodas, lâmina, bumpers, sensores e eletrônica de bordo. Dimensões numéricas vêm do firmware, do simulador e da bancada L298N. Onde o valor ainda não existe no código, está marcado como **a definir (TBD)** — é o que o projeto mecânico precisa fechar.

A navegação inercial (IMU) é um subsistema de localização. Ela só funciona se o chassi, a lâmina e os encoders estiverem corretos.

---

## 1. Conceito da máquina

Cortador terrestre de **tração diferencial** (duas rodas motrizes independentes) cuja função é **cortar o gramado de ponta a ponta**, com esta missão:

1. Alinhar-se a uma borda (cerca, muro, meio-fio).
2. Percorrer o perímetro até fechar a área de corte.
3. Preencher o interior em zigue-zague com a lâmina ligada.
4. Desviar de obstáculos por contato (bumpers) e por distância (IR/ToF).
5. Pausar com chuva, parar em emergência e, quando previsto, retornar à base para carga.

Não há direção Ackermann. Curvas são feitas invertendo o sentido das rodas (`ROTATING_CLOCK` / `ROTATING_COUNTER`). Qualquer assimetria de diâmetro, atrito ou encoder entre esquerda e direita aparece como erro de heading e de faixa não cortada — deve ser minimizada na mecânica.

Lâmina, guarda, sensor de borda, chuva e e-stop fazem parte do produto tanto quanto as rodas. Vários desses canais já existem no estado do firmware (`lawn_mower_status`) mesmo quando a leitura ainda não está completa.

---

## 2. Arquitetura física (blocos)

```
        [bumpers 8x no perímetro da carenagem]
                    ┌──────────────────────┐
     IRDA esq. ──── │                      │ ──── IRDA dir.
                    │   lâmina (embaixo)   │
     IMU (rígido)   │   chassi / bateria   │   encoder × 2
                    │   STM32 + Pi + L298N │
     IRDA traseiro ─┤                      ├─ IRDA dianteiro
                    └──────────┬───────────┘
                         roda E     roda D
                         (TBD GPIO) (PB10/PB11)
```

Há **duas placas de controle** no cortador:

| Placa | Função mecânica / elétrica |
|-------|----------------------------|
| STM32 (Blue Pill STM32F103C8 ou IoT-Interface STM32F103RBT6) | Tempo real: motores, encoders, IMU, bumpers |
| Raspberry Pi Zero W | Supervisão, UART, futuro mapa / IoT |

Alimentação de **motores ≠ alimentação lógica**. Motores no `Vs` do L298N (ex.: 12 V de bateria). Pi e STM32 em 3,3 V / 5 V regulados. **GND comum obrigatório.** Nunca alimentar motores pelo pino 5 V do Pi.

---

## 3. Trem de força

### 3.1 Rodas

- Duas rodas motrizes, esquerda e direita, eixo transversal comum (ou o mais próximo possível).
- Firmware trata distâncias em **contagem de pulsos de encoder**, não em milímetros. O projetista precisa entregar a constante:

  `mm_por_pulso = (π · D_roda) / (PPR · redução · modo_quadratura)`

  até o software converter pulsos → metros.

- Passos de movimento já usados no código (referência de escala, não de mm):

  | Símbolo | Pulsos | Uso |
  |---------|--------|-----|
  | `SOFT_FORWARD` / `SOFT_BACKWARD` | ±15 | Ajuste fino, encostar na borda |
  | `TURN_CLOCK` / `TURN_COUNTER` | ±50 | Giro no lugar |
  | Teste de integração Pi | 30 | Avanço de bancada |

- Bitola (distância entre centros das rodas) **TBD**. Entra no cálculo de giro: `pulsos_giro_90° ≈ (π/2 · bitola / 2) / mm_por_pulso` em cada roda, em sentidos opostos.

### 3.2 Motores e ponte H

Bancada validada: **L298N dual H-bridge**.

| Canal L298N | Roda | Notas |
|-------------|------|--------|
| IN1 / IN2, OUT1 / OUT2 | Direita | Já no firmware STM32: PB10 / PB11 |
| IN3 / IN4, OUT3 / OUT4 | Esquerda | Bancada no Pi (GPIO 22/23); firmware STM32 ainda sem GPIO |

Lógica de direção (não inverter no chicote sem atualizar o software):

| Sentido | INx | INy |
|---------|-----|-----|
| Frente | 1 | 0 |
| Ré | 0 | 1 |
| Parado | 0 | 0 |

ENA/ENB hoje com **jumpers** (motores sempre habilitados, sem PWM). Se o projeto mecânico exigir rampa de torque ou velocidade variável, prever ENA/ENB em PWM e dissipação extra no L298N (o módulo esquenta em 12 V contínuo).

Requisitos mecânicos dos motores:

- Torque de partida em grama molhada + lâmina acoplada (pior caso).
- Folga angular baixa: folga no redutor vira erro de encoder e de mapa.
- Se o sentido físico ficar invertido: **trocar o par OUT** daquela roda, não “corrigir” no software sem alinhamento com o encoder.

### 3.3 Encoders

Quadratura por roda.

| Roda | Canal A | Canal B | Estado no firmware |
|------|---------|---------|--------------------|
| Direita | PA0 (TIM2 CH1, ambas as bordas) | PA4 (GPIO, sentido) | Implementado |
| Esquerda | Previsto TIM2 CH2 (tipicamente PA1) | Previsto PA5 | Não habilitado no init |

Recomendações de projeto:

- Disco ou encoder de eixo **rígido** com o eixo da roda (sem correia elástica).
- PPR alto o suficiente para os 15 pulsos de “soft move” corresponderem a um deslocamento visível (ordem de centímetros, não metros).
- Sinais 3,3 V. Pull-up no STM32 (PA0/PA4 já em pull-up interno).
- Blindagem / afastamento do cabo de potência do L298N (EMI).
- Acesso para teste: o software consegue **emular** pulsos (1 tick / 200 ms) com o cortador no cavalete.

**Cavalete de desenvolvimento:** o README de bancada exige rodas no ar. Prever furos ou apoios para elevar o chassi sem desmontar as rodas.

---

## 4. IMU BNO080 (localização da máquina)

O cortador estima a própria pose integrando a aceleração linear do BNO080 (relatórios a **100 ms**). Deriva é inevitável se a IMU estiver mal montada — e deriva vira faixa de grama por cortar.

### 4.1 Posição no chassi

- O mais próximo do **centro geométrico** entre as duas rodas (origem natural do modelo 2D).
- Base **rígida** ao chassi (não em tampa de plástico que flexiona).
- Eixos alinhados ao veículo: X frente, Y esquerda (convenção a confirmar no software; hoje Euler é roll/pitch/yaw a partir do quaternion).
- Longe da lâmina, do L298N e de motores (vibração + campo magnético).
- Nivelado em pitch/roll com o plano das rodas; um wedge de 2–3° vira componente de gravidade na aceleração horizontal.

### 4.2 Isolamento de vibração

Filtro no firmware: média de 10 amostras + high-pass. Isso **não substitui** isoladores. Use silent-blocks leves; isoladores muito macios atrasam o heading em manobras.

### 4.3 Elétrica da IMU

- I2C1: **PB6 SCL, PB7 SDA**, 100 kHz, ADDR **0x4A** (pino ADDR em pull-down).
- Cabo curto, 3,3 V, GND comum. Sem level-shifter 5 V.

---

## 5. Sensor de distância (IRDA / ToF)

No software o sensor é “IRDA”. No caminho ROS usa-se **VL53L0X** (ToF, alcance típico 0,05–2,0 m). Há dois conceitos de montagem — o projetista precisa escolher um e fechar com firmware:

### 5.1 Quatro sensores fixos (modelo da struct)

`irda_distance[4]`: **FRONT, REAR, LEFT, RIGHT**, valor em **cm**.

- Eixo óptico horizontal, altura acima da grama alta (falso positivo em folha) e abaixo de obstáculos de jardim (vasos, pés de móvel).
- Altura sugerida de trabalho: **80–150 mm** do solo (TBD em teste de campo).
- Sem interferência mútua: não apontar dois ToF um para o outro; se necessário, offset temporal (firmware).
- Janela óptica na carenagem: material IR-transparente ou recorte. Evitar reflexo interno (pintura fosca).

### 5.2 Um sensor em torreta (modelo do simulador e do ROS)

Comentários do simulador:

- Varredura **180°** (código) ou **360° / 1,8°** (200 pontos, nota de hardware).
- Frequência **30 Hz** (~33 ms entre medidas).
- Motor de scan **externo** ao `sensor_node.py` — precisa de eixo, fim de curso ou encoder de ângulo.
- Alcance simulado de parede: **1410 mm**. Range útil de projeto: **50 mm a 2000 mm**.

Se a torreta for adotada:

- Centro de rotação conhecido em relação à IMU (offset X/Y/Z em mm, para o software somar pose + polar).
- Folga 360° ou batentes a ±90° se a varredura for só frontal.
- Cabo em laço de serviço (não torcer o I2C até o fim da vida).

O simulador de mapa trata um **obstáculo linear à direita** da máquina. O algoritmo de cobertura **começa procurando borda direita** (`align_to_right_edge`). Mantenha o lado direito com o melhor FOV e o bumper direito confiável.

---

## 6. Bumpers (contato)

Oito zonas independentes, índice no firmware = enum `Direction`:

| Índice | Zona | GPIO atual (Blue Pill) | Modo no init |
|--------|------|------------------------|--------------|
| 0 | FRONT | PB0 | Input pull-up |
| 1 | REAR | PB1 | Input pull-up |
| 2 | RIGHT | PB2 | Input pull-up — **usado para alinhar na borda** |
| 3 | LEFT | PB8 | Input pull-up |
| 4 | FRONT_RIGHT | PB9 | Input pull-up |
| 5 | FRONT_LEFT | PB13 | Hoje configurado como **saída** — revisar no layout |
| 6 | REAR_RIGHT | PB14 | Input, sem pull |
| 7 | REAR_LEFT | PC13 | Hoje **saída** (conflito com LED onboard da Blue Pill) |

Implicações para o projetista:

- Carenagem com **saia flutuante** ou palhetas por setor, curso de 3–8 mm antes do fim de curso rígido.
- Um microswitch (NF, para GND) por setor; o firmware espera nível que possa ser lido como “hit”. Polaridade final: coordenar quando `read_bumpers()` for completado (hoje os pinos são lidos, mas o array de status ainda não é atualizado).
- **RIGHT** é o sensor de política de borda. Não compartilhe o mesmo palete com FRONT_RIGHT.
- Cantos (FRONT_RIGHT, etc.) precisam de overlap geométrico para não existir “buraco” entre setores.
- PC13 na Blue Pill é o LED: se REAR_LEFT for bumper real, **não use PC13** — escolher outro GPIO na placa de produção (IoT-Interface).

Área fechada no software: retorno a menos de **20 cm** do ponto inicial após **15 s**. O perímetro mecânico do cortador (raio de giro + saia) deve caber nessa histerese; um chassi muito longo nunca “fecha” o loop de borda.

---

## 7. Conjunto de corte (função principal do produto)

Campos no estado: `blade_motor_status`, `blade_speed_rpm`, `edge_sensor`.

`Edge_sensor`: `OFF`, `TOUCH`, `HARD_TOUCH`, `CUT`, `BLOCKED`. Interpretação mecânica:

| Estado | Significado físico esperado |
|--------|-----------------------------|
| TOUCH | Disco / palpador encosta na borda de corte (cerca, guia) |
| HARD_TOUCH | Sobrecarga — afastar |
| CUT | Filamento ou faca em corte normal |
| BLOCKED | Travamento — parar |
| OFF | Sensor ausente ou desligado — tratar como falha |

A lâmina é o implemento do produto. Sem guarda, interlock e sensor de borda, a máquina não é um cortador vendável — é só um chassi que anda.

Requisitos:

- Lâmina abaixo do chassi, guarda em conformidade com norma de cortadores (projeção, interlock de elevação).
- Altura de corte ajustável (TBD: faixas típicas de gramado residencial).
- Acesso para troca da lâmina sem desmontar o chassi inteiro.
- Sensor de borda no **lado direito** (a missão segue a borda direita).
- Desacoplar vibração da lâmina da IMU (subchassi ou silent-blocks diferentes).
- Canal de encoder ou tachômetro da lâmina: previsto no firmware (`blade_motor_callback`) mas não implementado — reservar um GPIO / TIM.

---

## 8. Chuva, bateria e paradas

- `rain_detected`: 1 = pausar o corte. Posicionar o sensor de chuva no topo, fora do fluxo da lâmina e de irrigação direta do aspersor (ou aceitar pausa em irrigação).
- Bateria: tensão, corrente, % e flag de carga estão no estado. Mecânica: compartimento acessível, ventilar o L298N, fusível no `Vs`.
- `is_emergency_stop`: botão de cogumelo acessível **sem** abaixar-se sobre a lâmina; NC para GND, independente do Pi (o STM32 deve conseguir zerar IN1–IN4 sozinho).
- Carga: `charging_status` — se houver dock, o retorno à base (`return_to_base`) precisa de geometria de engate (TBD: cones, imãs, contato).

---

## 9. Envelope, massas e CG

Valores de trabalho do simulador (ambiente do jardim de teste, não o envelope do cortador):

| Parâmetro | Valor | Uso |
|-----------|-------|-----|
| Célula de mapa / sala de teste | 1,41 m × 1,41 m | Validação indoor |
| Alcance de parede simulada | 1410 mm | Calibração de ToF |
| Incremento de pose no sim | 100 mm a cada 3 scans | Ordem de grandeza de passo |

Do cortador, o projetista deve fechar e devolver ao software:

- Diâmetro e largura das rodas  
- Bitola e wheelbase (se houver caster)  
- Altura livre do solo e da lâmina  
- Massa total e posição do CG (altura do CG vs. bitola: estabilidade em rampa)  
- Offset IMU → centro do eixo  
- Offset de cada IRDA / centro da torreta  
- Curso e força de disparo dos bumpers  

Caster(s) dianteiro/traseiro: se existirem, devem girar livremente; travamento gera heading falso na integração inercial.

---

## 10. Interface elétrica que a mecânica precisa respeitar

### 10.1 STM32 Blue Pill (protótipo)

| Função | Pino |
|--------|------|
| Encoder dir. A | PA0 |
| Encoder dir. B | PA4 |
| Motor dir. IN1 / IN2 | PB10 / PB11 |
| I2C IMU SCL / SDA | PB6 / PB7 |
| USART1 debug TX/RX | PA9 / PA10 |
| USART2 app Pi TX/RX | PA2 / PA3 |
| Bumpers | PB0, PB1, PB2, PB8, PB9, PB13, PB14, PC13 |

### 10.2 Raspberry Pi Zero W ↔ STM32 (UART)

Cruzamento: TX do Pi → RX do STM32, GND comum. Na IoT-Interface o link de app **e** o bootloader usam USART1 (PA9/PA10) no header P4.

### 10.3 Bancada L298N no Pi (não é o chicote de produção)

| L298N | BCM | Pino físico |
|-------|-----|-------------|
| IN1 | 17 | 11 |
| IN2 | 27 | 13 |
| IN3 | 22 | 15 |
| IN4 | 23 | 16 |
| GND | GND | 6/9/14… |

Produção: saídas digitais do conector **J2 da IoT-Interface** → L298N, não os GPIOs de bancada.

### 10.4 SPI (não usado no app atual)

Se no futuro: PA7 MOSI, PA6 MISO, PA5 SCK, PA4 NSS — **PA4 hoje é encoder B**. SPI e encoder direito são **mutuamente exclusivos** nesse pino. Qualquer placa nova deve separar NSS e encoder.

---

## 11. Ambiente e materiais

- IP: poeira de grama + água. Conectores com trava; ToF e IMU em volume seco.
- Temperatura: eletrônica e bateria à sombra da carenagem; L298N com fresta de ar.
- Interferência: cabos de encoder e I2C distantes dos pares OUT do H-bridge.
- Manutenção: acesso a baterias, lâmina, Pi (SSH) e STM32 (SWD P2 ou UART boot).
- Teste de campo: superfície plana primeiro (o simulador assume plano); rampas exigem melhor fusão IMU + encoder e altura de lâmina que não scalpe o gramado.

---

## 12. Segurança de projeto (não negociável com o software)

1. Rodas no ar em qualquer teste de motor até a FSM de bumpers estar viva.
2. Lâmina desconectada ou com interlock enquanto se valida UART/MOVE.
3. E-stop corta ponte H mesmo se o Pi travar.
4. Centro de gravidade baixo: o cortador não deve empinar com `SOFT_FORWARD` em grama.
5. Bumpers devem atuar **antes** da lâmina encontrar o obstáculo (overhang da saia > raio da lâmina).

---

## 13. Entregáveis do projetista para o time de software

Checklist mínimo para calibrar o firmware:

1. Tabela mm/pulso por roda (medida, não teórica).  
2. Bitola e sentido físico “frente”.  
3. Desenho de origem: IMU, eixos X/Y, offsets dos ToF.  
4. Mapa bumper ↔ GPIO da placa de produção (sem PC13).  
5. Massa, CG, altura da lâmina e da ótica.  
6. Foto / STEP da montagem da IMU (planaridade).  

Com isso o software converte pulsos em metros, polar em mapa e borda direita no perímetro real de corte.

Documentos irmãos: `docs/01-documentacao-tecnica-software.md` (software da máquina), `docs/03-business-and-market.md` (produto e mercado).
