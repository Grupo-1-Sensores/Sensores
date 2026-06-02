#include "sensores.h"
#include "config.h"
#include <DHT.h>

static DHT dht(PINO_TEMPERATURA, TIPO_DHT);

static LeituraSensores leituraAtual;
static unsigned long ultimaLeitura = 0;
static float temperaturaHistorico[5];
static unsigned long tempoHistorico[5];
static uint8_t indexHistorico = 0;

void sensoresInit()
{
    dht.begin();
    pinMode(PINO_SOM, INPUT);
}

bool lerSensores()
{
    if (millis() - ultimaLeitura < INTERVALO_DHT_MS)
        return false;

    ultimaLeitura = millis();

    float temperatura = dht.readTemperature();
    float umid = dht.readHumidity();
    int som = analogRead(PINO_SOM);

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