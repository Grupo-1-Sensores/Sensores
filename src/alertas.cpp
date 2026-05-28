#include "alertas.h"
#include "sensores.h"
#include "config.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <MqttManager.h>

static uint8_t somConsecutivos = 0;

void verificarAlertas()
{
    LeituraSensores l = getLeitura();
    if (!l.valida)
        return;
    JsonDocument docEnvioAlerta;

    if (l.temperatura > TEMPERATURA_MAX)
    {

        docEnvioAlerta["alerta_temperatura"] = String("TEMP_MAX: Temperatura acima do limite: ") + String(l.temperatura);
        Serial.println("[ALERTA] TEMP_MAX: Temperatura acima do limite");
    }

    if (l.temperatura < TEMPERATURA_MIN)
    {

        docEnvioAlerta["alerta_temperatura"] = String("TEMPERATURA_MIN: Temperatura abaixo do limite: ") + String(l.temperatura);
        Serial.println("[ALERTA] TEMPERATURA_MIN: Temperatura abaixo do limite");
    }

    if (getTemperaturaVariacao() >= TEMPERATURA_VARIACAO_MAX)
    {
        docEnvioAlerta["alerta_Variacaotemperatura"] = String("TEMPERATURA_VARIACAO: Variação brusca de temperatura detectada: ");
        Serial.println("[ALERTA] TEMPERATURA_VARIACAO: Variação brusca de temperatura detectada");
    }

    if (l.umidade < UMIDADE_CRITICA)
    {
        Serial.println("[ALERTA] UMIDADE_CRITICA: Umidade em nível crítico");
    }

    else if (l.umidade < UMIDADE_MIN)
    {
        Serial.println("[ALERTA] UMIDADE_BAIXA: Umidade abaixo do mínimo");
    }

    else if (l.umidade > UMIDADE_MAX)
    {
        Serial.println("[ALERTA] UMIDADE_ALTA: Umidade acima do máximo");
    }

    if (l.som > SOM_LIMITE_ALTO)
    {
        somConsecutivos++;
        if (somConsecutivos >= SOM_CONSECUTIVOS)
        {
            Serial.println("[ALERTA] SOM_ALTO: Ruído elevado persistente detectado");
            somConsecutivos = 0;
        }
    }
    else
    {
        somConsecutivos = 0;
    }

    if (!docEnvioAlerta.isNull())
    {
        publicarJson(TOPICO_ALERTA, docEnvioAlerta);
        docEnvioAlerta.clear();
    }
}