# Linha de negócio — plataforma mecânica SU-006

**Produto:** chassi UGV de esteira (`SU-006_chassi`) para navegação inercial e serviço em área delimitada  
**Público:** linha de negócio, produto, vendas OEM, compras e investidor  
**Data:** setembro de 2026  
**Uso:** one-pager comercial, decisão de SKU e conversa com fábrica — não é desenho de cotas

Documento técnico irmão: `docs/04-especificacao-mecanica-projetistas-SU-006.md`.  
Cérebro de navegação: `docs/03-business-and-market.md` e `docs/01-documentacao-tecnica-software.md`.

---

## Placa de produto (texto curto)

**SU-006 — casco industrial de esteira para o cérebro de navegação inercial.**

Veículo terrestre baixo, largo e fechado, com para-choque dianteiro, poço de sensor no deck e compartimento térmico na traseira. Pensa em pátio, condomínio, planta e campo curto — não em rodovia. A esteira tira o fio perimetral e o pneu que fura; o software de bordo tira o GPS contínuo.

**O cliente compra um chassi que já tem envelope de IMU, scan e bumper. Não compra um “tanque de brinquedo” nem um cortador fechado de prateleira.**

![Envelope comercial SU-006](assets/su-006-vistas-produto.jpg)

---

## O que este hardware é (e o que não é)

| É | Não é |
|---|--------|
| Plataforma mecânica do sistema INS | Um tractor com RTK de fazenda |
| UGV skid-steer, payload no deck | Cortador residencial de duas rodas da bancada atual |
| Envelope de sensor (furo circular + volumes no teto) | LiDAR de 50 mil dólares incluso no preço-base |
| Carenagem de serviço (painéis aparafusados) | Monocoque selado impossível de manter |
| Candidato a marca própria e a white-label OEM | Produto de prateleira com preço de e-commerce no dia um |

O repositório de software ainda gira em **protótipo de duas rodas**. O SU-006 é a **aposta de produto** da parte mecânica: o mesmo cérebro (STM32 + Raspberry Pi + IMU + ToF), em um casco que o mercado industrial reconhece como máquina, não como hobby.

---

## Por que esteira, não duas rodas

Para a linha de negócio, a esteira é decisão de **mercado e custo de campo**, não de estética.

- **Terreno:** grama irregular, brita, lama rasa, degrau baixo — o pneu diferencial derrapa e o mapa inercial quebra.  
- **Carga:** o deck largo aceita bateria, driver e payload sem empinar.  
- **Imagem:** amarelo / cinza industrial (Figura 1) comunica “equipamento”, não “brinquedo”.  
- **Preço a pagar:** esteira custa mais, escorrega no giro (a IMU deixa de ser opcional) e pede tensor e reposição. Isso entra no modelo de receita (peças), não só no BOM.

Mensagem interna: **não vender o SU-006 como “o mesmo robô da bancada L298N”.** Vender como o casco no qual o módulo de navegação escala.

---

## Problema que a mecânica resolve

Quem quer autonomia em área fechada hoje escolhe entre:

1. **Cortador fechado** (fio no chão, caixa-preta, sem API).  
2. **UGV militar / inspeção** (preço e lead time de outro segmento).  
3. **Chassi de maker** (roda, perfil de alumínio, zero IP, zero bumper de verdade).

O SU-006 ocupa o meio: **casco com cara de série**, interfaces que o firmware já entende (encoder, bumper, poço de sensor, e-stop), preço de máquina de serviço, não de defesa.

---

## Linha de SKU mecânica

Três ofertas, **mesmo chassi**. O software (`docs/03`) tem a mesma lógica em três camadas; aqui a variável é quanta ferragem o cliente leva.

### SKU M1 — Chassi nu (`SU-006_chassi`)

Deck + caixas de esteira + furos. Sem carenagem amarela, sem barra, sem sensores.

- **Quem compra:** OEM que já tem implemento e pintura.  
- **Receita:** peça metálica + desenho de interface (CSYS_NAV).  
- **Margem:** baixa por unidade, volume e lock-in do cérebro INS.

### SKU M2 — Casco integrado (recomendado como âncora)

Chassi + esteiras + barra dianteira + carenagem + poço ótico + compartimento térmico. Sem lâmina de corte. Com pontos de IMU, bumper e e-stop.

- **Quem compra:** integrador de condomínio, inspeção, pátio, P&D.  
- **Receita:** máquina + kit de navegação (placa + firmware) no mesmo pedido.  
- **Upsell:** torreta ToF/LiDAR, hitch de implemento, dock de carga.

### SKU M3 — Veículo de aplicação (marca própria ou white-label)

M2 + ferramenta (corte, empurrador, inspeção, pulverização leve) + dock.

- **Quem compra:** usuário final B2B.  
- **Receita:** máquina + reposição (esteira, bumper, bateria, talões) + conectividade.  
- **Risco:** norma da ferramenta (corte) e suporte de campo; só abrir quando M2 estiver estável.

**Não lançar M3 de corte no mesmo trimestre que o casco.** A barra da frente, no CAD, é bumper / hitch, não faca. Prometer corte cedo queima a marca.

---

## Onde o dinheiro está no ferro

Custo mecânico (ordem de grandeza qualitativa — compras fecha número):

| Bloco | Peso no BOM | Comentário comercial |
|-------|-------------|----------------------|
| Esteiras + sprockets + tensores | Alto | Item de reposição; padronizar um fornecedor |
| Motores / redutores | Alto | Define torque e preço; não economizar no encoder |
| Casco soldado | Médio | Empeno do deck mata a IMU — qualidade > chapa mais fina |
| Carenagem e tinta | Médio | O que o cliente fotografa; amarelo industrial é posicionamento |
| Barra dianteira | Baixo–médio | Barato no BOM, caro se o bumper falhar em campo |
| Poço ótico / tampas | Baixo | Barato, mas é o diferencial frente a chassi genérico |

Alavanca de margem: **não customizar o casco a cada OEM**. Customizar hitch, cor e payload no deck. O `SU-006_chassi` permanece único.

Reposição (recorrente): esteira, rodas de apoio, bumper, tampa do poço, filtros de grade. Isso é linha de aftermarket, não “custo chato”.

---

## Mercado (encaixe do casco)

**Primário — serviço em área delimitada (B2B)**  
Pátios, condomínios, plantas baixas, campus. Velocidade baixa, obstáculo denso, GPS ruim sob copa / galpão. O SU-006 entra como veículo de **ronda, mapeamento e implemento leve**.

**Secundário — OEM de máquina verde / implemento**  
Quem já vende cortador ou varredor e quer um casco com API. Compra M1 ou M2 + módulo INS. Não compete com a marca deles no varejo; compete com “fazer o chassi internamente”.

**Terciário — P&D e educação**  
Universidade e time de robótica: o furo circular e o deck furado são o argumento. Preço de kit, não de UGV de catálogo europeu.

**Fora de escopo no dia um:** rodovia, last-mile rápido, defesa, agricultura de hectare com RTK. O envelope baixo e a esteira curta não são trator.

---

## Diferenciação mecânica

| SU-006 | Cortador de prateleira | Chassi de perfil / maker |
|--------|------------------------|---------------------------|
| Esteira + casco fechado | Duas rodas + fio | Rodas expostas |
| Poço de sensor no deck | Sensor escondido, sem API | Furo feito na serra |
| Bumper em três apoios | Saia plástica | Sem zona de contato |
| Painéis de serviço | Caixa soldada | Tudo aberto |
| Pensado para IMU rígida | Caixa-preta | IMU no protoboard |

A marca mecânica não é “mais um UGV amarelo”. É **o casco no qual o mapa inercial consegue existir**: origem de coordenadas, bumper de verdade, encoder no sprocket, ar no driver.

---

## Mensagem por público

**OEM / fábrica**  
“Um chassi, três SKUs. Vocês pintam e põem o implemento. Nós entregamos o envelope de IMU, o poço de scan e o bumper já com três zonas.”

**Compras**  
“Esteira e redutor são itens long-lead. Casco e carenagem são locais. Não abrir segunda geometria de deck.”

**Vendas B2B**  
“Máquina baixa, esteira, sem obra de fio. O cérebro mapeia a área. A barra da frente é proteção — ferramenta é opcional.”

**Investidor**  
“O software é o defensible (cobertura + mapa). O SU-006 é o veículo que permite preço de máquina e aftermarket de esteira, em vez de kit de bancada para sempre.”

**Projetista (handoff)**  
Cotas e CSYS estão no documento técnico. Aqui a regra de negócio é: **não mudar bitola, poço e três apoios da barra sem passar por produto** — isso quebra firmware e peças de reposição.

---

## Estágio (honestidade comercial)

O que existe hoje:

- Envelope visual e CAD de conjunto (`SU-006_chassi`, vista RD2).  
- Cérebro de navegação em protótipo (Pi + STM32 + simulador de mapa).  
- Cota de largura da barra já lançada no CAD; demais cotas ainda abertas.

O que **não** vender ainda:

- Autonomia de campo comprovada neste casco.  
- Corte de grama no SU-006.  
- IP certificado.  
- Preço de lista sem BOM de esteira fechado.

Próximos marcos que viram argumento de venda (nessa ordem):

1. Congelar envelope e CSYS_NAV (entregável do projetista).  
2. Um casco soldado com esteiras no ar e encoders vivos.  
3. IMU nivelada + um giro em piso sem perder o mapa.  
4. Bumper dianteiro nas três zonas, e-stop cortando tração.  
5. Piloto em pátio plano com o SKU M2 (sem ferramenta de corte).

Usar a placa de produto como **destino**. Em conversa B2B, o SU-006 é **plataforma em detalhamento**, não máquina homologada.

---

## Copy para ficha / site (≈ 80 palavras)

**SU-006 — chassi UGV para navegação inercial**

Casco industrial de esteira, deck fechado e para-choque em três apoios. Poço de sensor, compartimento térmico e carenagem de serviço. Feito para receber IMU, scan de distância e o módulo INS (STM32 + Raspberry Pi): localiza sem GPS contínuo e mapeia a área.

Três versões: chassi nu para OEM, casco integrado para integradores, veículo de aplicação sob encomenda.

*O cérebro é o software. O SU-006 é o chão em que ele anda.*

---

## Tagline (escolha uma)

- O casco da navegação, não o brinquedo da esteira.  
- Área fechada, esteira no chão, mapa no deck.  
- Industrial por fora. Origem de coordenadas por dentro.

---

## Decisões de negócio que a mecânica precisa agora

Estas perguntas são de **produto**, não de SolidWorks. Sem resposta, o projetista inventa e o BOM explode.

1. M2 é o SKU âncora? (recomendado: sim)  
2. Barra dianteira: só bumper, ou hitch vendável?  
3. Poço circular: ToF de série ou payload do cliente?  
4. Cor: amarelo industrial fixo (marca) ou cor OEM?  
5. Corte: fora do roadmap do SU-006 no ano 1? (recomendado: fora)  
6. Aftermarket de esteira: contrato com um fornecedor antes do piloto.

Quando essas seis estiverem fechadas, o documento do projetista deixa de ser envelope e vira ordem de fabricação.
