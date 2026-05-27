#include <Arduino.h>
#include "config.h"
#include "sensores.h"
#include "alertas.h"
#include <ArduinoJson.h>
#include "WifiManager.h"
#include "MqttManager.h"

static unsigned long ultimaPublicacao = 0;
const char TOPICO_COMANDO[] = "senai134/controle/sensores";

void tratarJsonComando(const String &mensagem);
void tratarMensagemRecebida(const char *topico, const String &mensagem);

void setup() {
    Serial.begin(115200);
    sensoresInit();
    Serial.println("Sistema iniciado");
    conectarWiFi();
    configurarMQTT();
    registrarCallbackMensagem(tratarMensagemRecebida);
    conectarMQTT();

    JsonDocument doc;
}

void loop() {

    garantirWiFiConectado();
    garantirMQTTconectado();
    loopMQTT();
    if (lerSensores()) {
        verificarAlertas();
    }

    if (getLeitura().valida && millis() - ultimaPublicacao > INTERVALO_PUBLICACAO_MS) {
        // TODO implementar sistema de públicar no tópico.
        LeituraSensores leitura = getLeitura();

        JsonDocument docEnvio;

        docEnvio["temperatura"] = leitura.temp;
        docEnvio["umidade"] = leitura.umidade;
        docEnvio["som"] = leitura.som;

        String mensagem;

        serializeJson(docEnvio, mensagem);
        docEnvio.clear();

        publicarMensagemNoTopico(1, mensagem.c_str());
        Serial.printf("Temperatura: %.1f C | Umidade: %1.f %% | Som: %d\n", leitura.temp, leitura.umidade, leitura.som);
        ultimaPublicacao = millis();
    }
}

void tratarMensagemRecebida(const char *topico, const String &mensagem)
{

  Serial.println("==============================");
  Serial.println("Mensagem recebida na Aplicação");
  Serial.println("==============================");

  if (topico == nullptr)
  {
    Serial.println("Tópico MQTT inválido.");
  }
  Serial.println("Tópico: " + String(topico));
  Serial.println("Mensagem: " + mensagem);

  if (strcmp(topico, TOPICO_COMANDO) == 0)
  {
    tratarJsonComando(mensagem);
    return;
  }
  Serial.println("Tópico não tratado: " + String(topico));
}

void tratarJsonComando(const String &mensagem)
{
  JsonDocument doc;
  DeserializationError erro = deserializeJson(doc, mensagem);

  if (erro)
  {
    Serial.println("Erro ao interpretar JSON");
    Serial.println(erro.c_str());
    return;
  }
  } 
