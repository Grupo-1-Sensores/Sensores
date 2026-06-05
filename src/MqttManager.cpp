#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <Arduino.h>
#include <PubSubClient.h>

#include "secrets.h"
#include "WifiManager.h"
#include "MqttManager.h"

WiFiClientSecure wifiClientSecure;

PubSubClient mqttClient;

CallbackMensagemMQTT callbackDaAplicacao = nullptr;

void registrarCallbackMensagem(CallbackMensagemMQTT callback)
{
    callbackDaAplicacao = callback;

    if (callbackDaAplicacao != nullptr)
    {
        Serial.println("Callback da aplicação registrado com sucesso.");
    }
    else
    {
        Serial.println("Callback da aplicação não foi registrado.");
    }
}

const char *obterTopicoPublicacao(int indiceTopico)
{
    if (indiceTopico < 0 || indiceTopico >= TOTAL_TOPICOS_PUBLICAR)
    {
        Serial.println("Índice inválido para tópico de publicação: " + String(indiceTopico));
        return "";
    }
    return TOPICOS_PUBLICAR[indiceTopico];
}

const char *obterTopicoRecebimento(int indiceTopico)
{
    if (indiceTopico < 0 || indiceTopico >= TOTAL_TOPICOS_RECEBER)
    {
        Serial.println("Índice inválido para tópico de recebimento: " + String(indiceTopico));
        return "";
    }
    return TOPICOS_RECEBER[indiceTopico];
}

void callbackInternoMQTT(char *topico, byte *payload, unsigned int tamanho)
{
    String mensagem = "";

    for (unsigned int i = 0; i < tamanho; i++)
    {
        mensagem += (char)payload[i];
    }

    Serial.println("===================");
    Serial.println(" Mensagem MQTT recebida");
    Serial.println("====================");
    Serial.println("Tópico: " + String(topico));
    Serial.println("Mensagem: " + mensagem);

    if (callbackDaAplicacao != nullptr)
    {
        callbackDaAplicacao(topico, mensagem);
    }

    else
    {
        Serial.println("Mensagem recebida, mas nenhuma callback da aplicação registrado.");
    }
}

void configurarMQTT()
{
    Serial.println("============================");
    Serial.println("Configurando MQTT...");
    Serial.println("============================");

    wifiClientSecure.setCACert(AWS_CERT_CA);
    wifiClientSecure.setCertificate(AWS_CERT_CRT);
    wifiClientSecure.setPrivateKey(AWS_CERT_PRIVATE);

    mqttClient.setClient(wifiClientSecure);
    mqttClient.setServer(AWS_IOT_ENDPOINT, AWS_IOT_PORT);

    Serial.println("Endpoint AWS IoT: " + String(AWS_IOT_ENDPOINT));
    Serial.println("Porta AWS IoT: " + String(AWS_IOT_PORT));

    mqttClient.setCallback(callbackInternoMQTT);
    Serial.println("Callback interno no MQTT configurado.");
}

void conectarMQTT()
{
    if (!wifiEstaConectado())
    {
        Serial.println("MQTT não pode conectar porque o WiFi está desconectado.");
    }

    Serial.println("===========================");
    Serial.println("Iniciando conexão MQTT...");
    Serial.println("===========================");

    int tentativasMQTT = 0;
    const int maxTentativasMQTT = 5;

    while (!mqttClient.connected() && tentativasMQTT < maxTentativasMQTT)
    {
        Serial.println("Tentando conectar ao broker MQTT. Tentativa: " + String(tentativasMQTT));

        if (mqttClient.connect(AWS_IOT_CLIENT_ID))
        {
            Serial.println("MQTT conectado com sucesso.");

            int totalTopicos = obterTotalTopicosRecebimento();

            Serial.println("Total de tópicos para inscricão: " + String(totalTopicos));

            for (int i = 0; i < totalTopicos; i++)
            {
                const char *topico = obterTopicoRecebimento(i);

                bool inscrito = mqttClient.subscribe(topico);

                if (inscrito)
                {
                    Serial.println("Inscrito no tópico: " + String(topico));
                }
                else
                {
                    Serial.println("Falha ao se inscrever no tópico: " + String(topico));
                }
            }

            publicarMensagem("senai134/equipe/luigi/devices/sensor", "ESP32 conectado ao MQTT");
        }
        else
        {
            Serial.println("Falha ao conectar no MQTT. Código de erro: " + String(mqttClient.state()));
            tentativasMQTT++;
            delay(2000);
        }
    }
    if (!mqttClient.connected())
    {
        Serial.println("Nao foi possível conectar ao broker MQTT após " + String(maxTentativasMQTT) + " tentativas");
    }
}

int obterTotalTopicosRecebimento()
{
    return TOTAL_TOPICOS_RECEBER;
}

void garantirMQTTconectado()
{
    if (!wifiEstaConectado())
    {
        Serial.println("MQTT não reconectado porque o WiFi está desconectado");
        return;
    }

    if (!mqttClient.connected())
    {
        Serial.println("MQTT desconectado. Tentando reconectar...");
        conectarMQTT();
    }
}

void loopMQTT()
{
    mqttClient.loop();
}

void publicarMensagem(const char *topico, const char *mensagem)
{
    if (!mqttClient.connected())
    {
        Serial.println("Não foi possível publicar. MQTT desconectado.");
        return;
    }

    bool publicado = mqttClient.publish(topico, mensagem);

    if (publicado)
    {
        Serial.println("Mensagem publicada via MQTT.");
        Serial.println("Topico: " + String(topico));
        Serial.println("Mensagem: " + String(mensagem));
    }
    else
    {
        Serial.println("Falha ao publicar mensagem no tópico: " + String(topico));
    }
}

void publicarMensagemNoTopico(int indiceTopico, const char *mensagem)
{
    const char *topico = obterTopicoPublicacao(indiceTopico);

    if (strlen(topico) == 0)
    {
        Serial.println("Não foi possível publicar. Índice de tópico inválido: " + String(indiceTopico));
        return;
    }

    publicarMensagem(topico, mensagem);
}

void publicarJson(int indiceTopico, JsonDocument &doc)
{
    String mensagem;
    serializeJson(doc, mensagem);
    publicarMensagemNoTopico(indiceTopico, mensagem.c_str());
}

bool mqttEstaConectado()
{
    return mqttClient.connected();
}