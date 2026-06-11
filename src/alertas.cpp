#include "alertas.h"
#include "sensores.h"
#include "config.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <MqttManager.h>
#include <WifiManager.h>

static uint8_t somConsecutivos = 0;
JsonDocument docEnvioAlerta;
bool temErro = false;

void verificarAlertas()
{
    LeituraSensores l = getLeitura();
    if (!l.valida)
        return;

    if (l.temperatura >= TEMPERATURA_MAX)
    {
        docEnvioAlerta["sensores"]["temperatura"] = l.temperatura;
        Serial.println("[ALERTA] TEMP_MAX: Temperatura acima do limite");
    }
    else if (l.temperatura < TEMPERATURA_MIN)
    {
        docEnvioAlerta["sensores"]["temperatura"] = l.temperatura;
        Serial.println("[ALERTA] TEMPERATURA_MIN: Temperatura abaixo do limite");
    }
    else if (getTemperaturaVariacao() >= TEMPERATURA_VARIACAO_MAX)
    {
        docEnvioAlerta["sensores"]["temperatura"] = l.temperatura;
        Serial.println("[ALERTA] TEMPERATURA_VARIACAO: Variação brusca de temperatura detectada");
    }

    if (l.umidade < UMIDADE_CRITICA)
    {
        docEnvioAlerta["sensores"]["umidade"] = l.umidade;
        Serial.println("[ALERTA] UMIDADE_CRITICA: Umidade em nível crítico");
    }

    else if (l.umidade < UMIDADE_MIN)
    {
        docEnvioAlerta["sensores"]["umidade"] = l.umidade;
        Serial.println("[ALERTA] UMIDADE_BAIXA: Umidade abaixo do mínimo");
    }

    else if (l.umidade > UMIDADE_MAX)
    {
        docEnvioAlerta["sensores"]["umidade"] = l.umidade;
        Serial.println("[ALERTA] UMIDADE_ALTA: Umidade acima do máximo");
    }

    if (l.som > SOM_LIMITE_ALTO)
    {
        somConsecutivos++;
        if (somConsecutivos >= SOM_CONSECUTIVOS)
        {
            docEnvioAlerta["sensores"]["som"] = l.som;
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
        docEnvioAlerta["sensores"]["timestamp"] = pegarHora();
        publicarJson(TOPICO_SHARED_PUB, docEnvioAlerta);
        publicarJson(TOPICO_DASH, docEnvioAlerta);
        docEnvioAlerta.clear();
    }
}

void verificarFalhaSensor()
{
    static unsigned long ultimoAlertaErro = 0;

    if (!dhtEstaComErro())
    {
        if (getLeitura().valida && temErro)
        {
            LeituraSensores leitura = getLeitura();

            JsonDocument docEnvio;

            docEnvio["sensores"]["temperatura"] = leitura.temperatura;
            docEnvio["sensores"]["umidade"] = leitura.umidade;
            docEnvio["sensores"]["som"] = leitura.som;
            docEnvio["sensores"]["timestamp"] = pegarHora();

            publicarJson(TOPICO_DASH, docEnvio);
            publicarJson(TOPICO_SHARED_PUB, docEnvio);
            docEnvio.clear();
            temErro = false;
        }
        return;
    }

    if (millis() - ultimoAlertaErro < INTERVALO_ALERTA_ERRO_MS)
        return;

    ultimoAlertaErro = millis();

    docEnvioAlerta["sensores"]["temperatura"] = DHT_ERRO;
    docEnvioAlerta["sensores"]["umidade"] = DHT_ERRO;
    docEnvioAlerta["sensores"]["timestamp"] = pegarHora();
    publicarJson(TOPICO_SHARED_PUB, docEnvioAlerta);
    publicarJson(TOPICO_DASH, docEnvioAlerta);
    docEnvioAlerta.clear();
    temErro = true;

    Serial.println("[ALERTA] DHT_ERRO: Falha persistente na leitura do DHT22");
}