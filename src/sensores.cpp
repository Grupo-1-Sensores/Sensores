#include "sensores.h"
#include "config.h"
#include <DHT.h>

static DHT dht(PINO_TEMPERATURA, TIPO_DHT);

static LeituraSensores leituraAtual;
static unsigned long ultimaLeitura = 0;
static float temperaturaHistorico[5];
static unsigned long tempoHistorico[5];
static uint8_t indexHistorico = 0;

static float ultimoDbMedio = 0;	  // ultima media de 10s ja fechada
static float somaDbAcumulada = 0; // soma das janelas da media atual
static int janelasAcumuladas = 0; // quantas janelas ja entraram
static unsigned long inicioMediaSom = 0;

void sensoresInit()
{
	dht.begin();
	pinMode(PINO_SOM, INPUT);
	inicioMediaSom = millis();
}

bool lerSensores()
{
	if (millis() - ultimaLeitura < INTERVALO_DHT_MS)
		return false;

	ultimaLeitura = millis();

	float temperatura = dht.readTemperature();
	float umid = dht.readHumidity();
	float som = getDbMedio();
	som = round(som * 10.0) / 10.0;

	if (isnan(temperatura) || isnan(umid))
	{
		leituraAtual.valida = false;
		Serial.println("[ERRO] Falha na leitura do DHT22");
		return false;
	}

	leituraAtual = {temperatura, umid, som, true};

	temperaturaHistorico[indexHistorico] = temperatura;
	tempoHistorico[indexHistorico] = millis();
	indexHistorico = (indexHistorico + 1) % 5;

	return true;
}

LeituraSensores getLeitura()
{
	return leituraAtual;
}

float getTemperaturaVariacao()
{
	if (!leituraAtual.valida)
		return 0;

	float maxVar = 0;
	unsigned long agora = millis();

	for (uint8_t i = 0; i < 5; i++)
	{
		if (tempoHistorico[i] == 0)
			continue;
		if (agora - tempoHistorico[i] > JANELA_VARIACAO_MS)
			continue;
		float v = abs(leituraAtual.temperatura - temperaturaHistorico[i]);
		if (v > maxVar)
			maxVar = v;
	}
	return maxVar;
}

void atualizarSom()
{
	static int amostras[MAX_AMOSTRAS];
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
		return;

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

	somaDbAcumulada += 20.0 * log10(rmsVolts / REF_RMS) + DB_OFFSET;
	janelasAcumuladas++;

	if (millis() - inicioMediaSom >= JANELA_MEDIA_SOM_MS)
	{
		if (janelasAcumuladas > 0)
			ultimoDbMedio = somaDbAcumulada / janelasAcumuladas;
		somaDbAcumulada = 0;
		janelasAcumuladas = 0;
		inicioMediaSom = millis();
	}
}

float getDbMedio()
{
	return ultimoDbMedio;
}