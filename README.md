# Monitor Ambiental IoT — ESP32-S3 + AWS IoT Core

Projeto de monitoramento ambiental em tempo real desenvolvido para sala de aula, utilizando ESP32-S3, sensores de temperatura, umidade e nível de ruído, com publicação dos dados via MQTT para a **AWS IoT Core**.

---

## Visão Geral

O dispositivo coleta continuamente dados do ambiente e os publica em tópicos MQTT na nuvem AWS. A leitura de som é processada de forma **não-bloqueante**, calculando o nível em decibéis (dB) via RMS a cada ciclo do loop, sem travar a comunicação WiFi ou MQTT.

---

## Hardware

| Componente | Descrição |
|---|---|
| **ESP32-S3 DevKitM-1** | Microcontrolador principal |
| **DHT22** | Sensor de temperatura e umidade relativa |
| **MAX9814** | Microfone com AGC — leitura de nível de ruído em dB |

---

## Pinagem

### DHT22

| Pino DHT22 | ESP32-S3 |
|---|---|
| VCC | 3.3V |
| GND | GND |
| DATA | GPIO 5 |

### MAX9814

| Pino MAX9814 | ESP32-S3 | Observação |
|---|---|---|
| VDD | 3.3V | Não usar 5V |
| GND | GND | — |
| OUT | GPIO 11 | Entrada analógica ADC1 |
| GAIN | GND / Flutuante / 3.3V | 60 dB / 50 dB / 40 dB |
| AR | Desconectado | Controle attack/release (opcional) |

---

## Arquitetura do Software

```
src/
├── main.cpp              # Loop principal, publicação MQTT, roteamento de mensagens
├── sensores.cpp          # Leitura DHT22 e processamento de dB (não-bloqueante)
├── alertas.cpp           # Verificação de limites e geração de alertas
├── MqttManager.cpp       # Conexão, publicação e recebimento MQTT (AWS IoT)
├── WifiManager.cpp       # Conexão WiFi e sincronização de hora via NTP
└── secrets.cpp           # Credenciais WiFi e certificados AWS  ⚠️ NÃO versionar

include/
├── config.h              # Pinos, limites, intervalos e constantes do sistema
├── sensores.h            # Estrutura LeituraSensores e declarações
├── alertas.h             # Declaração de verificarAlertas()
├── MqttManager.h         # Interface do gerenciador MQTT
├── WifiManager.h         # Interface do gerenciador WiFi
└── secrets.h             # Declarações externas das credenciais
```

---

## Fluxo de Dados

```
Loop principal (sem bloqueio)
│
├── atualizarSom()       ← coleta janela de 100ms de amostras ADC, acumula RMS
├── lerSensores()        ← lê DHT22 a cada 2s e captura getDbMedio()
├── verificarAlertas()   ← verifica limites configurados
│
├── [a cada 120s]  publica temperatura + umidade + som  →  TOPICO_DASH
└── [a cada 1s]    publica nível de ruído               →  TOPICO_SOM
```

---

## Processamento de Som (dB)

A medição de nível sonoro usa o método **RMS (Root Mean Square)**:

1. A cada iteração do `loop()`, `atualizarSom()` coleta amostras ADC por **100 ms**
2. Remove o offset DC calculando a média das amostras
3. Calcula o RMS da componente AC e converte para Volts
4. Aplica `20 × log10(Vrms / Vref) + DB_OFFSET` para obter dB
5. Após **1 segundo** de janelas acumuladas, `getDbMedio()` retorna a média

Essa abordagem garante que o `loop()` **nunca bloqueie**, mantendo WiFi e MQTT sempre responsivos.

---

## Tópicos MQTT

### Publicação

| Constante | Tópico | Conteúdo |
|---|---|---|
| `TOPICO_DASH` | `senai134/equipe/luigi/devices/sensor/tu` | `{"sensores": {"temperatura": X, "umidade": X, "som": X}}` |
| `TOPICO_SOM` | `senai134/equipe/luigi/devices/sensor/som` | `{"sensores": {"som": X}}` |
| `TOPICO_SHARED_PUB` | `senai134/shared/projeto/sensores` | Resposta a requisições de dispositivos externos |

### Recebimento

| Constante | Tópico | Função |
|---|---|---|
| `TOPICO_ESP` | `senai134/equipe/luigi/devices/#` | Comandos internos do dispositivo |
| `TOPICO_SHARED` | `senai134/shared/projeto/sensores` | Requisições de dados de dispositivos externos |

---

## Configuração (`include/config.h`)

| Parâmetro | Valor padrão | Descrição |
|---|---|---|
| `PINO_TEMPERATURA` | `5` | GPIO do DHT22 |
| `PINO_SOM` | `11` | GPIO do MAX9814 |
| `INTERVALO_PUBLICACAO_MS` | `120000` | Intervalo de publicação completa (2 min) |
| `INTERVALO_PUBLICACAO_MS_SOM` | `1000` | Intervalo de publicação do som (1 s) |
| `JANELA_MS` | `100` | Duração de cada janela de amostragem de som |
| `JANELA_MEDIA_SOM_MS` | `1000` | Período de acumulação da média de dB |
| `DB_OFFSET` | `35.0` | Offset de calibração do microfone |
| `REF_RMS` | `0.00631` | Tensão de referência RMS para cálculo de dB |
| `SOM_LIMITE_ALTO` | `70.0` | Limite de alerta de ruído (dB) |
| `TEMPERATURA_MAX` | `30.0` | Limite superior de temperatura (°C) |
| `TEMPERATURA_MIN` | `14.0` | Limite inferior de temperatura (°C) |
| `UMIDADE_MIN` | `30.0` | Umidade mínima (%) |
| `UMIDADE_MAX` | `85.0` | Umidade máxima (%) |

---

## Como Configurar e Compilar

### Pré-requisitos

- [VS Code](https://code.visualstudio.com/) com a extensão [PlatformIO IDE](https://platformio.org/install/ide?install=vscode)
- Conta na [AWS IoT Core](https://aws.amazon.com/iot-core/) com um *Thing* criado e certificados gerados

### 1. Clone o repositório

```bash
git clone https://github.com/seu-usuario/seu-repositorio.git
cd seu-repositorio
```

### 2. Configure as credenciais

Crie o arquivo `src/secrets.cpp` com base no modelo abaixo. **Nunca suba esse arquivo para o repositório.**

```cpp
#include "secrets.h"
#include "arduino.h"

const char *WIFI_SSID     = "SEU_SSID";
const char *WIFI_SENHA    = "SUA_SENHA";

const char *AWS_IOT_ENDPOINT  = "xxxxxxxxxxxx-ats.iot.us-east-1.amazonaws.com";
const int   AWS_IOT_PORT      = 8883;
const char *AWS_IOT_CLIENT_ID = "nome-do-seu-dispositivo";

const char AWS_CERT_CA[]      PROGMEM = R"EOF(-----BEGIN CERTIFICATE-----
(Amazon Root CA 1)
-----END CERTIFICATE-----)EOF";

const char AWS_CERT_CRT[]     PROGMEM = R"CRT(-----BEGIN CERTIFICATE-----
(certificado do Thing)
-----END CERTIFICATE-----)CRT";

const char AWS_CERT_PRIVATE[] PROGMEM = R"KEY(-----BEGIN RSA PRIVATE KEY-----
(chave privada do Thing)
-----END RSA PRIVATE KEY-----)KEY";

const char *TOPICOS_PUBLICAR[] = {
    "seu/topico/sensor/tu",
    "seu/topico/sensor/som",
    "seu/topico/shared"
};
const int TOTAL_TOPICOS_PUBLICAR = sizeof(TOPICOS_PUBLICAR) / sizeof(TOPICOS_PUBLICAR[0]);

const char *TOPICOS_RECEBER[] = {
    "seu/topico/devices/#",
    "seu/topico/shared"
};
const int TOTAL_TOPICOS_RECEBER = sizeof(TOPICOS_RECEBER) / sizeof(TOPICOS_RECEBER[0]);
```

### 3. Adicione `secrets.cpp` ao `.gitignore`

```
src/secrets.cpp
```

### 4. Compile e faça upload

Abra o projeto no VS Code com o PlatformIO e clique em **Build** e depois **Upload**, ou via terminal:

```bash
pio run --target upload
```

Para monitorar o serial:

```bash
pio device monitor --baud 115200
```

---

## Dependências

Gerenciadas automaticamente pelo PlatformIO via `platformio.ini`:

| Biblioteca | Versão |
|---|---|
| `adafruit/DHT sensor library` | ^1.4.7 |
| `thomasfredericks/Bounce2` | ^2.72 |
| `knolleary/PubSubClient` | ^2.8 |
| `bblanchon/ArduinoJson` | ^7.2.2 |

---

## Segurança

> **Atenção:** O arquivo `src/secrets.cpp` contém credenciais WiFi e certificados TLS privados da AWS. Ele **nunca deve ser commitado** no repositório. Adicione-o ao `.gitignore` antes do primeiro commit.

A comunicação com a AWS IoT Core é protegida por **TLS mútuo (mTLS)** utilizando certificados X.509, garantindo autenticidade tanto do broker quanto do dispositivo.

---

## Autores

Projeto desenvolvido para fins educacionais no **SENAI 134**.

- [Ana Clara](https://github.com/MCAnaClara)
- [Gael Salvador](https://github.com/gaelsalvador)
- [Gabriel Selvenca](https://github.com/GabrielSelvenca)
- [Juliano Luz](https://github.com/julianoluz10)
- [Willian Ribeiro](https://github.com/WillR7s)
- [Yarllei Thauã](https://github.com/yarlleith)
