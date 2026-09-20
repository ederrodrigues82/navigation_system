# Linha de negócio e mercado

**Produto:** cortador de grama autônomo  
**Uso deste texto:** one-pager comercial, placa de produto, pitch e site  
**Data:** setembro de 2026

A navegação inercial, os bumpers, o mapa e o Raspberry Pi são **partes da máquina**. O que se vende é o cortador.

---

## Placa de produto (texto curto)

**Cortador de grama autônomo para jardim.**

A máquina percorre o perímetro, preenche o gramado em zigue-zague, desvia de obstáculos e pausa com chuva. Localiza-se com sensores de bordo — IMU, encoders e distâncias — sem obra de fio perimetral e sem depender de GPS sob a copa das árvores.

**Coloque no gramado. Ele mapeia, corta e para se algo atravessar o caminho ele desvia.**

---



## Headline e subheadline (variantes)

1. **O jardim cortado, do muro ao canteiro.** Autônomo: borda, cobertura em zigue-zague e desvio.
2. **Cortador robô sem fio perimetral.** A máquina vê o terreno; você não enterra cabo.
3. **Corte residencial e de condomínio, instalado no mesmo dia.** Sem GNSS contínuo, sem beacon no jardim.

---



## O problema

Cortar grama é trabalho repetitivo, barulhento e fácil de adiar. Cortadores robóticos maduros resolvem parte disso, mas pedem **fio perimetral**, GPS de jardim ou beacons. A instalação encarece o produto, falha em quintais irregulares e piora sob árvores, onde o GPS degrada.

O comprador — casa, sítio, condomínio — quer a **grama cortada com segurança**, não um kit de sensores. Instalação no mesmo dia, sem obra no gramado, e uma máquina que pare sozinha se bater, chover ou a lâmina travar.

---



## A solução

Um **cortador de grama autônomo** completo: chassi de tração diferencial, lâmina, bumpers, bateria e inteligência de bordo.


| O que a máquina faz | Como                                                   |
| ------------------- | ------------------------------------------------------ |
| Corta o gramado     | Lâmina sob o chassi, missão de cobertura               |
| Descobre a área     | Alinha na borda, percorre o contorno, fecha o polígono |
| Preenche o miolo    | Zigue-zague até o bumper ou o mapa dizer o contrário   |
| Desvia              | Oito zonas de contato + sensores de distância          |
| Sabe onde está      | IMU + encoders (navegação inercial como subsistema)    |
| Opera com segurança | Chuva, e-stop, status da lâmina, retorno à base        |


Eletrônica avançada a bordo: **Microprocesador** de tempo real (rodas, lâmina, sensores) e **CPU** na supervisão (status, mapa, evolução para app/IoT). 

---



## Proposta de valor

- **Corte autônomo de verdade:** perímetro + interior, não passeio aleatório.  
- **Instalação leve:** sem fio enterrado e sem depender de GPS no quintal sombreado.  
- **Segurança de jardim:** bumpers, pausa na chuva, lâmina monitorada, parada de emergência.  
- **Manutenção simples:** lâmina, bumper e bateria como reposição.

---



## Linha de negócio

### 1. Cortador autônomo

Máquina completa: tração, lâmina, bumpers, sensores, bateria e dock quando houver. Receita: venda da unidade (residencial e condomínio).

### 2. Consumíveis e reposição

Lâmina, saia de bumper, rodas, bateria. Receita recorrente de quem já tem a máquina no jardim.

### 3. Serviço e frota

Instalação, revisão sazonal e, no condomínio, várias unidades com mapa por zona. Upsell: app, multi-área, retorno à base com carga.



---



## Mercado

**Primário — residência e condomínio (Brasil e LatAm)**  
Quintais murados, sombra, diarista de jardim cara ou irregular. O comprador compara com cortador manual, contratado e robôs que exigem fio (Husqvarna, Worx e similares).

**Secundário — sítios, pousadas e áreas comuns**  
Gramados delimitados, baixa velocidade, muitos obstáculos (móveis, vasos, brinquedos).

**Fora de escopo no dia um**  
Lavoura e trator com RTK. Este produto é **jardim e pátio**, não hectare.

---



## Diferenciação


| Este cortador                          | Robô típico de prateleira      | “Só um IMU / placa de navegação”    |
| -------------------------------------- | ------------------------------ | ----------------------------------- |
| Máquina que corta, mapeia e para       | Muitas vezes aleatório + fio   | Não corta grama                     |
| Sem fio perimetral obrigatório         | Instalação com cabo            | Não é produto de jardim             |
| Missão: borda → polígono → zigue-zague | Comportamento fechado na caixa | Sem lâmina, sem bumper, sem bateria |
| Peças de reposição de cortador         | Idem                           | Não há lâmina para trocar           |


A marca é o **cortador**. A navegação inercial é o motivo pelo qual ele não precisa de fio nem de GPS no quintal.

---



## Mensagem para cada público

**Dono da casa / síndico**  
“Ele corta o gramado sozinho. Aprende o contorno, desvia do vaso e para se chover. Sem fio no jardim.”

**Projetista mecânico**  
“Estamos fechando o cortador: lâmina, saia de bumpers, bitola e altura de corte. A IMU vai no chassi, não o contrário.”

**Desenvolvedor**  
“O estado da máquina é um só: motores, lâmina, pose, bumpers, bateria. UART com PING, STATUS e MOVE.”

**Investidor**  
“Produto: cortador autônomo. Moat: cobertura sem fio perimetral. Receita: máquina + lâmina/bateria. Mercado: quintal e condomínio.”

---



## Estágio do produto

Protótipo avançado de laboratório da **máquina**: comunicação entre micontrolador e CPU,  encoder, esqueleto da missão de corte, mapa simulado. Próximos marcos de produto:

1. Tração esquerda + direita calibrada e lâmina no chassi.
2. IMU + encoder fundidos (menos deriva no gramado).
3. Bumpers e sensores de distância vivos na missão de corte.
4. Uma volta completa em jardim piloto, grama de fato cortada e mapa gravado.

Usar o texto da placa **como destino do cortador**; em conversa comercial, deixar explícito o estágio.

---



## Tagline 

- Cortador que aprende com seu quintal.

