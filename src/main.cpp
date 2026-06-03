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

void tratarJsonComando(const String &mensagem);
void tratarMensagemRecebida(const char *topico, const String &mensagem);
int calcularNivelRuido();
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
	calcularNivelRuido();

	int valorSomMin = 0;
	int valorSomMax = 0;

	if (lerSensores())
	{
		verificarAlertas();
	}

	if (getLeitura().valida && millis() - ultimaPublicacao > INTERVALO_PUBLICACAO_MS)
	{
		LeituraSensores leitura = getLeitura();

		docEnvio["sensores"]["temperatura"] = leitura.temperatura;
		docEnvio["sensores"]["umidade"] = leitura.umidade;
		docEnvio["sensores"]["som"] = leitura.som;

		publicarJson(TOPICO_LOG, docEnvio);
		docEnvio.clear();

		Serial.printf("Temperatura: %.1f C | Umidade: %.1f %% | Som:  %d adc\n", leitura.temperatura, leitura.umidade, leitura.som);
		ultimaPublicacao = millis();
	}

	if (millis() - ultimaPublicacaoSom > INTERVALO_PUBLICACAO_MS_SOM)
	{
		int ruido = calcularNivelRuido();

		docEnvio["sensores"]["som"] = String(ruido) + "%";

		publicarJson(TOPICO_LOG, docEnvio);
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
		case TOPICO_LOG:
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

int calcularNivelRuido()
{
	int valorSomMin = 4095;
	int valorSomMax = 0;

	unsigned long inicio = millis();

	while (millis() - inicio < delayValidacao)
	{
		int leitura = analogRead(PINO_SOM);

		if (leitura < valorSomMin)
		{
			valorSomMin = leitura;
		}

		if (leitura > valorSomMax)
		{
			valorSomMax = leitura;
		}
	}

	int amplitude = valorSomMax - valorSomMin;

	int ruidoValor = map(amplitude, VALOR_SOM_MIN, VALOR_SOM_MAX, 0, 100);

	if (ruidoValor < 0)
	{
		ruidoValor = 0;
	}

	if (ruidoValor > 100)
	{
		ruidoValor = 100;
	}

 	//Serial.printf(
	//	"Amplitude: %d | Ruido relativo: %d%%\n", amplitude, ruidoValor);

	return ruidoValor;
}