#include <Arduino.h>
#include "config.h"
#include "sensores.h"
#include "alertas.h"
#include <ArduinoJson.h>
#include "WifiManager.h"
#include "MqttManager.h"

static unsigned long ultimaPublicacao = 0;

void tratarJsonComando(const String &mensagem);
void tratarMensagemRecebida(const char *topico, const String &mensagem);
JsonDocument docEnvio;

void setup()
{
	Serial.begin(115200);
	sensoresInit();
	Serial.println("Sistema iniciado");
	conectarWiFi();
	configurarMQTT();
	registrarCallbackMensagem(tratarMensagemRecebida);
	conectarMQTT();
}

void loop()
{

	garantirWiFiConectado();
	garantirMQTTconectado();
	loopMQTT();
	if (lerSensores())
	{
		verificarAlertas();
	}

	if (getLeitura().valida && millis() - ultimaPublicacao > INTERVALO_PUBLICACAO_MS)
	{
		LeituraSensores leitura = getLeitura();

		docEnvio["temperatura"] = leitura.temperatura;
		docEnvio["umidade"] = leitura.umidade;
		docEnvio["som"] = leitura.som;

		publicarJson(TOPICO_LOG, docEnvio);
		docEnvio.clear();

		Serial.printf("Temperatura: %.1f C | Umidade: %.1f %% | Som:  dB%d\n", leitura.temperatura, leitura.umidade, leitura.som);
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

	for (int i = 0; i < obterTotalTopicosRecebimento(); i++)
	{
		if (strcmp(topico, obterTopicoRecebimento(i)) != 0)
			continue;

		switch (i)
		{
		case TOPICO_COMANDO:
			tratarJsonComando(mensagem);
			break;
		
		default:
			Serial.println("Tópico reconhecido mas sem tratamento: " + String(topico));
			break;
		}
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

	// TODO implementar receber dados de fora.
}