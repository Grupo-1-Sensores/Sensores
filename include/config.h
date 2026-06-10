#ifndef CONFIG_H
#define CONFIG_H

#define PINO_TEMPERATURA 5
#define PINO_SOM 11

#define TIPO_DHT DHT22
#define INTERVALO_DHT_MS 2000UL
#define INTERVALO_PUBLICACAO_MS 120000UL
#define INTERVALO_PUBLICACAO_MS_SOM 1000UL
#define INTERVALO_ALERTA_ERRO_MS 15000UL

// Valor sentinela enviado quando o DHT falha na leitura (Node-RED trata <= -900 como erro)
#define DHT_ERRO -999

#define TEMPERATURA_MAX 30.0f
#define TEMPERATURA_MIN 14.0f
#define TEMPERATURA_VARIACAO_MAX 4.0f
#define JANELA_VARIACAO_MS 60000UL

#define UMIDADE_MIN 30.0f
#define UMIDADE_MAX 85.0f
#define UMIDADE_CRITICA 20.0f

#define SOM_BAIXO 35f

#define SOM_LIMITE_ALTO 100.0f
#define SOM_CONSECUTIVOS 5

#define VALOR_SOM_MAX 2700
#define VALOR_SOM_MIN 100
#define ADC_MAX 4095
#define VREF 3.3f
#define JANELA_MS 100UL
#define JANELA_MEDIA_SOM_MS 1000UL
#define MAX_AMOSTRAS 1000
#define REF_RMS 0.00631f
#define DB_OFFSET 35.0f



// Indices dos topicos de publicacao (ordem de TOPICOS_PUBLICAR em secrets.cpp)
enum TopicoPublicacao {
    TOPICO_DASH = 0,
    TOPICO_SOM = 1,
    TOPICO_SHARED_PUB = 2
};

// Indices dos topicos de recebimento (ordem de TOPICOS_RECEBER em secrets.cpp)
enum TopicoRecebido {
    TOPICO_ESP = 0,
    TOPICO_SHARED = 1
};

#endif