#include <Arduino.h>
#include "config.h"
#include "sensores.h"
#include "alertas.h"
#include <ArduinoJson.h>
#include "WifiManager.h"
#include "MqttManager.h"

static ulong ultimaPublicacao = 0;
static ulong ultimaPublicacaoSom = 0;
static bool primeiraPublicacaoFeita = false;


void tratarJsonComando(const String &mensagem);
void tratarMensagemRecebida(const char *topico, const String &mensagem);
void tratarJsonComandoValida(const String &mensagem);
void tratarJsonComandoRecebimento(const String &mensagem);

JsonDocument docEnvio;
JsonDocument docEnvioSom;

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
	atualizarSom();

	if (lerSensores())
	{
		verificarAlertas();
	}


	if (getLeitura().valida && (!primeiraPublicacaoFeita || millis() - ultimaPublicacao > INTERVALO_PUBLICACAO_MS))
	{
		LeituraSensores leitura = getLeitura();

		docEnvio["sensores"]["temperatura"] = leitura.temperatura;
		docEnvio["sensores"]["umidade"] = leitura.umidade;
		docEnvio["sensores"]["som"] = leitura.som;

		publicarJson(TOPICO_DASH, docEnvio);
		docEnvio.clear();

		Serial.printf("Temperatura: %.1f C | Umidade: %.1f %% | Som:  %.1f db\n", leitura.temperatura, leitura.umidade, leitura.som);
		ultimaPublicacao = millis();
		primeiraPublicacaoFeita = true;
	}

	if (millis() - ultimaPublicacaoSom > INTERVALO_PUBLICACAO_MS_SOM)
	{
		float db = getDbMedio();
		db = round(db * 10.0) / 10.0;
		docEnvioSom["sensores"]["som"] = db;

		publicarJson(TOPICO_SOM, docEnvioSom);
		docEnvioSom.clear();
		Serial.printf("\nEnviando %.1f db\n", db);

		ultimaPublicacaoSom = millis();
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
		return;
	}
	Serial.println("Tópico: " + String(topico));
	Serial.println("Mensagem: " + mensagem);

	for (int i = 0; i < obterTotalTopicosRecebimento(); i++)
	{
		if (strcmp(topico, obterTopicoRecebimento(i)) != 0)
			continue;

		switch (i)
		{
		case TOPICO_ESP:
			tratarJsonComando(mensagem);
			break;

		case TOPICO_SHARED:
			tratarJsonComandoRecebimento(mensagem);
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

void tratarJsonComandoValida(const String &mensagem)
{
	JsonDocument doc;
	DeserializationError erro = deserializeJson(doc, mensagem);

	if (erro)
	{
		Serial.println("Erro ao interpretar JSON");
		Serial.println(erro.c_str());
		return;
	}

	static unsigned long tempoEsperaRecebimento = 0;
	if (millis() - tempoEsperaRecebimento > 15000 && !doc["sensorResponseTrue"])
	{
		LeituraSensores leitura = getLeitura();
		tempoEsperaRecebimento = millis();

		docEnvio["sensores"]["temperatura"] = leitura.temperatura;
		docEnvio["sensores"]["umidade"] = leitura.umidade;
		docEnvio["sensores"]["som"] = leitura.som;

		publicarJson(TOPICO_SHARED_PUB, docEnvio);
		docEnvio.clear();
	}
	else
	{
		Serial.println("Mensagem recebida");
	}
}
void tratarJsonComandoRecebimento(const String &mensagem)
{
	JsonDocument doc;
	DeserializationError erro = deserializeJson(doc, mensagem);

	if (erro)
	{
		Serial.println("Erro ao interpretar JSON");
		Serial.println(erro.c_str());
		return;
	}

	if (doc["sensores"]["comando"].is<JsonObject>())
	{
		LeituraSensores leitura = getLeitura();

		docEnvio["sensores"]["temperatura"] = leitura.temperatura;
		docEnvio["sensores"]["umidade"] = leitura.umidade;
		docEnvio["sensores"]["som"] = leitura.som;

		publicarJson(TOPICO_SHARED_PUB, docEnvio);
		docEnvio.clear();
	}
}