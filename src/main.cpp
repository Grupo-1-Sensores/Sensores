#include <Arduino.h>
#include "config.h"
#include "sensores.h"
#include "alertas.h"
#include <ArduinoJson.h>
#include "WifiManager.h"
#include "MqttManager.h"

static ulong ultimaPublicacao = 0;
static ulong ultimaPublicacaoSom = 0;
static const ulong delayValidacao = 50;

const int VALOR_SOM_MAX = 2700;
const int VALOR_SOM_MIN = 100;
const int ADC_MAX = 4095;
const float VREF = 3.3;
const unsigned long JANELA_MS = 100;
const int MAX_AMOSTRAS = 1000;
const float REF_RMS = 0.00631;
float DB_OFFSET = 35.0;

void tratarJsonComando(const String &mensagem);
void tratarMensagemRecebida(const char *topico, const String &mensagem);
float lerDbMedio();
void tratarJsonComandoValida(const String &mensagem);

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
	lerDbMedio();

	int valorSomMin = 0;
	int valorSomMax = 0;

	if (lerSensores())
	{
		verificarAlertas();
	}

	if (getLeitura().valida && millis() - ultimaPublicacao > INTERVALO_PUBLICACAO_MS)
	{
		LeituraSensores leitura = getLeitura();
		float db = lerDbMedio();
		db = round(db * 10.0) / 10.0;

		docEnvio["sensores"]["temperatura"] = leitura.temperatura;
		docEnvio["sensores"]["umidade"] = leitura.umidade;
		docEnvio["sensores"]["som"] = db;

		publicarJson(TOPICO_DASH, docEnvio);
		docEnvio.clear();

		Serial.printf("Temperatura: %.1f C | Umidade: %.1f %% | Som:  %.1f db\n", leitura.temperatura, leitura.umidade, leitura.som);
		ultimaPublicacao = millis();
	}

	if (millis() - ultimaPublicacaoSom > INTERVALO_PUBLICACAO_MS_SOM)
	{
		float db  = lerDbMedio();
		db = round(db * 10.0) / 10.0;
		docEnvio["sensores"]["som"] = db;

		publicarJson(TOPICO_DASH, docEnvio);
		docEnvio.clear();

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
	}
	Serial.println("Tópico: " + String(topico));
	Serial.println("Mensagem: " + mensagem);

	for (int i = 0; i < obterTotalTopicosRecebimento(); i++)
	{
		if (strcmp(topico, obterTopicoRecebimento(i)) != 0)
			continue;

		switch (i)
		{
		case TOPICO_DASH:
			tratarJsonComando(mensagem);
			break;

		case TOPICO_CONTROLE:
			
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

float lerDbMedio()
{
	int amostras[MAX_AMOSTRAS];
	unsigned long inicio = millis();
	float somaDb = 0;
	int janelas = 0;

	while (millis() - inicio < 10000)
	{
		unsigned long t = millis();
		int quantidade = 0;
		long somaAdc = 0;

		while ((millis() - t < JANELA_MS) && (quantidade < MAX_AMOSTRAS))
		{
			int leitura = analogRead(PINO_SOM);
			amostras[quantidade] = leitura;
			somaAdc += leitura;
			quantidade++;
			delayMicroseconds(100);
		}
		if (quantidade == 0)
			continue;

		// ---- RMS dessa janela (igual ao seu cálculo de antes) ----
		float mediaAdc = (float)somaAdc / quantidade;
		double somaQuadrados = 0;
		for (int i = 0; i < quantidade; i++)
		{
			float ac = amostras[i] - mediaAdc;
			somaQuadrados += ac * ac;
		}
		float rmsVolts = sqrt(somaQuadrados / quantidade) * (VREF / ADC_MAX);
		if (rmsVolts < 0.000001)
			rmsVolts = 0.000001;

		// ---- dB dessa janela, somado para a média ----
		somaDb += 20.0 * log10(rmsVolts / REF_RMS) + DB_OFFSET;
		janelas++;
	}

	if (janelas == 0)
		return -1;
	return somaDb / janelas; // média dos 10s
}

void tratarJsonComandoValida(const String &mensagem)
{
	JsonDocument doc;
	DeserializationError erro = deserializeJson(doc, mensagem);
	unsigned long tempoEsperaRecebimento = 0;
	if(millis() - tempoEsperaRecebimento > 15000 && !doc["sensorResponseTrue"].is<JsonObject>())
	{
		LeituraSensores leitura = getLeitura();
		tempoEsperaRecebimento = millis();
		float db = lerDbMedio();
		db = round(db * 10.0) / 10.0;

		docEnvio["sensores"]["temperatura"] = leitura.temperatura;
		docEnvio["sensores"]["umidade"] = leitura.umidade;
		docEnvio["sensores"]["som"] = db;

		publicarJson(TOPICO_CONTROLE, docEnvio);
		docEnvio.clear();
	}
	else
	{
		Serial.println("Mensagem recebida");
	}
	

	if (erro)
	{
		Serial.println("Erro ao interpretar JSON");
		Serial.println(erro.c_str());
		return;
	}


	// TODO implementar receber dados de fora.
}