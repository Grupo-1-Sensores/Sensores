# Monitoramento de Sala de Aula com ESP32

## 📋 Descrição

Este projeto utiliza um ESP32 para monitorar as condições ambientais de uma sala de aula, realizando a leitura de temperatura, umidade e nível de ruído.

Os dados coletados são enviados através do protocolo MQTT em formato JSON, permitindo integração com sistemas de supervisão, dashboards e aplicações IoT.

## 🎯 Objetivo

Monitorar em tempo real as condições ambientais de uma sala de aula, fornecendo informações sobre:

* Temperatura ambiente
* Umidade relativa do ar
* Nível de ruído ambiente

Esses dados podem ser utilizados para análise de conforto térmico, qualidade do ambiente de aprendizagem e automação predial.

## 🔧 Hardware Utilizado

### ESP32

Microcontrolador responsável pela aquisição dos dados dos sensores e publicação das informações via MQTT.

### Sensor DHT22

Sensor digital utilizado para medição de:

* Temperatura (°C)
* Umidade relativa (%)

### Sensor KY-037

Sensor de som utilizado para monitoramento do nível de ruído ambiente.

## 📡 Comunicação

A comunicação dos dados é realizada através do protocolo MQTT.

### Tópico MQTT

Exemplo:

```text
salaaula/monitoramento
```

### Payload JSON

Exemplo de mensagem publicada:

```json
{
  "temperatura": 24.8,
  "umidade": 58.3,
  "ruido": 425
}
```

## 📊 Dados Monitorados

| Variável    | Sensor | Unidade         |
| ----------- | ------ | --------------- |
| Temperatura | DHT22  | °C              |
| Umidade     | DHT22  | %               |
| Ruído       | KY-037 | Valor analógico |

## 🚀 Funcionamento

1. O ESP32 realiza a leitura do sensor DHT22.
2. O ESP32 realiza a leitura do sensor KY-037.
3. Os valores são organizados em uma estrutura JSON.
4. Os dados são publicados em um broker MQTT.
5. Clientes MQTT podem consumir as informações para monitoramento e visualização.

## 🔮 Melhorias Futuras

* Histórico de medições em banco de dados.
* Dashboard web para visualização dos dados.
* Alarmes para níveis elevados de ruído.
* Integração com sistemas de automação predial.
* Envio de notificações em condições críticas.

## 👥 Autores

Projeto desenvolvido para fins educacionais e de monitoramento ambiental utilizando tecnologias IoT baseadas em ESP32.
