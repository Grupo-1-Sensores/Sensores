//WIFI MANAGER CPP

// WiFiManager.cpp
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "WifiManager.h"
#include "secrets.h"

bool comecoHora = true;
void conectarWiFi()
{
    Serial.println("=============================");
    Serial.println("Iniciando conexão WiFi...");
    Serial.println("=============================");

    // Configura o ESP32 como station, ou seja
    // ele vai se conectar a um roteador existente.
    WiFi.mode(WIFI_STA);

    WiFi.begin(WIFI_SSID, WIFI_SENHA);

    Serial.println("conectando");

    int tentativas = 0;
    const int maxTentativa = 30;

    while (WiFi.status() != WL_CONNECTED && tentativas < maxTentativa)
    {
        
        delay(500);
        Serial.println(".");
        tentativas++;
    }

    Serial.println("\n");

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("WiFi conectado com sucesso!");
        Serial.println("Endereço IP: ");
        Serial.println((WiFi.localIP().toString()));
        Serial.println("\n\r");

        configTime(-3 * 3600, 0, "pool.ntp.org");
    }

    else
    {
        Serial.println("Falha ao conectar ao WiFi.");
        Serial.println("Verifique o SSID, senha e sinal de rede.");
    }
}
void garantirWiFiConectado()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("WiFi desconectado. Tentando reconectar...");
        conectarWiFi();
    }

    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("Não foi possível reconectar ao WiFi.");
    }
}

bool wifiEstaConectado()
{
    return WiFi.status() == WL_CONNECTED;

}
String pegarHora()
{
    struct tm horaNaoFormatada;
    
    if (!getLocalTime(&horaNaoFormatada, 10))
    {
        return String("00:00:00");
    }
    char horaFormatada [20];
    strftime(horaFormatada, sizeof(horaFormatada), "%H:%M:%S", &horaNaoFormatada);
    return String(horaFormatada);
}