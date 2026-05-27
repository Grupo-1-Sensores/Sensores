// SECRETS.EXAMPLE.CPP
// Modelo de credenciais. Copie este arquivo para src/secrets.cpp e preencha
// com os valores reais. src/secrets.cpp esta no .gitignore e NAO deve ser
// versionado. Este arquivo fica na raiz do projeto de proposito: o PlatformIO
// compila apenas o que esta em src/, entao o modelo nao entra no build.
#include "secrets.h"
#include <Arduino.h>

// Configuracoes do WiFi
const char *WIFI_SSID = "SSID";
const char *WIFI_SENHA = "SENHA_WIFI";

// ===================================
// CONEXAO AWS
// ===================================
const bool USAR_AWS_IOT = true;

const char *AWS_IOT_ENDPOINT = "broker-aws";

const char AWS_CERT_CA[] PROGMEM = R"EOF(

)EOF";

const char AWS_CERT_CRT[] PROGMEM = R"CRT(

)CRT";

const char AWS_CERT_PRIVATE[] PROGMEM = R"KEY(

)KEY";

const int AWS_IOT_PORT = 8883;

const char *AWS_IOT_CLIENT_ID = "xxxxxxxx";

// ===================================
// TOPICOS
// ===================================
const char *TOPICOS_PUBLICAR[] = {
    "senai134/sensores/alerta",
    "senai134/sensores/log",
    "senai134/sensores/status",
};
const int TOTAL_TOPICOS_PUBLICAR = sizeof(TOPICOS_PUBLICAR) / sizeof(TOPICOS_PUBLICAR[0]);

const char *TOPICOS_RECEBER[] = {
    "senai134/controle/sensores",
};
const int TOTAL_TOPICOS_RECEBER = sizeof(TOPICOS_RECEBER) / sizeof(TOPICOS_RECEBER[0]);
