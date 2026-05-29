#include "alertas.h"
#include "sensores.h"
#include "config.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <MqttManager.h>
#include <WifiManager.h>

static uint8_t somConsecutivos = 0;
JsonDocument docEnvioAlerta;

void verificarAlertas()
{
    LeituraSensores l = getLeitura();
    if (!l.valida)
        return;

    if (l.temperatura > TEMPERATURA_MAX)
    {

        docEnvioAlerta["alerta_temperatura"] = l.temperatura;
        docEnvioAlerta["Hora"] = pegarHora();
        Serial.println("[ALERTA] TEMP_MAX: Temperatura acima do limite");

    }
    else if (l.temperatura < TEMPERATURA_MIN)
    {
        docEnvioAlerta["alerta_temperatura"] = l.temperatura;
        docEnvioAlerta["Hora"] = pegarHora();
        Serial.println("[ALERTA] TEMPERATURA_MIN: Temperatura abaixo do limite");
    }
    else if (getTemperaturaVariacao() >= TEMPERATURA_VARIACAO_MAX)
    {
    docEnvioAlerta["alerta_temperatura"] = l.temperatura;       
    docEnvioAlerta["Hora"] = pegarHora();
     Serial.println("[ALERTA] TEMPERATURA_VARIACAO: Variação brusca de temperatura detectada");
    }

    if (l.umidade < UMIDADE_CRITICA)
    {
        docEnvioAlerta["alerta_umidade"] = l.umidade;
         docEnvioAlerta["Hora"] = pegarHora();
        Serial.println("[ALERTA] UMIDADE_CRITICA: Umidade em nível crítico");
    }

    else if (l.umidade < UMIDADE_MIN)
    {
    docEnvioAlerta["alerta_umidade"] = l.umidade;        
    docEnvioAlerta["Hora"] = pegarHora();
    Serial.println("[ALERTA] UMIDADE_BAIXA: Umidade abaixo do mínimo");
    }

    else if (l.umidade > UMIDADE_MAX)
    {
        docEnvioAlerta["alerta_umidade"] = l.umidade;
        docEnvioAlerta["Hora"] = pegarHora();
        Serial.println("[ALERTA] UMIDADE_ALTA: Umidade acima do máximo");
        
    }

    if (l.som > SOM_LIMITE_ALTO)
    {
        somConsecutivos++;
        if (somConsecutivos >= SOM_CONSECUTIVOS)
        {
            docEnvioAlerta["alerta_ruido"] = analogRead(PINO_SOM);
            docEnvioAlerta["Hora"] = pegarHora();
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