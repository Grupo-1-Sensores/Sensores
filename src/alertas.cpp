#include "alertas.h"
#include "sensores.h"
#include "config.h"
#include <Arduino.h>

static uint8_t somConsecutivos = 0;

void verificarAlertas()
{
    LeituraSensores l = getLeitura();
    if (!l.valida) return;

    if (l.temp > TEMP_MAX)
    {
        // TODO Implementar envio de aviso ao MQTT sobre temperatura.
        Serial.println("[ALERTA] TEMP_MAX: Temperatura acima do limite");
    }

    if (l.temp < TEMP_MIN)
        Serial.println("[ALERTA] TEMP_MIN: Temperatura abaixo do limite");

    if (getTempVariacao() >= TEMP_VARIACAO_MAX)
        Serial.println("[ALERTA] TEMP_VARIACAO: Variação brusca de temperatura detectada");

    if (l.umidade < UMIDADE_CRITICA)
        Serial.println("[ALERTA] UMIDADE_CRITICA: Umidade em nível crítico");
    else if (l.umidade < UMIDADE_MIN)
        Serial.println("[ALERTA] UMIDADE_BAIXA: Umidade abaixo do mínimo");
    else if (l.umidade > UMIDADE_MAX)
        Serial.println("[ALERTA] UMIDADE_ALTA: Umidade acima do máximo");

    if (l.som > SOM_LIMITE_ALTO) {
        somConsecutivos++;
        if (somConsecutivos >= SOM_CONSECUTIVOS) {
            Serial.println("[ALERTA] SOM_ALTO: Ruído elevado persistente detectado");
            somConsecutivos = 0;
        }
    } else {
        somConsecutivos = 0;
    }
}