
#ifndef SECRETS_H
#define SECRETS_H

//Configurações do WiFi

extern const char* WIFI_SSID ;
extern const char* WIFI_SENHA;

extern const char* TOPICOS_PUBLICAR[];

extern const int TOTAL_TOPICOS_PUBLICAR;

extern const char* TOPICOS_RECEBER[];

extern const int TOTAL_TOPICOS_RECEBER;

// ===================================
// AWS
// ===================================

extern const bool USAR_AWS_IOT; 

extern const char AWS_CERT_CA[];

extern const char AWS_CERT_CRT[];

extern const char AWS_CERT_PRIVATE[];

extern const char* AWS_IOT_ENDPOINT; // ENDEREÇO DO BROKER IOT.

extern const int AWS_IOT_PORT;

extern const char* AWS_IOT_CLIENT_ID;

#endif