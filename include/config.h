#ifndef CONFIG_H
#define CONFIG_H

#define PINO_TEMPERATURA 5
#define PINO_SOM 11

#define TIPO_DHT DHT22
#define INTERVALO_DHT_MS 2000UL
#define INTERVALO_PUBLICACAO_MS 120000UL

#define TEMPERATURA_MAX 30.0f
#define TEMPERATURA_MIN 14.0f
#define TEMPERATURA_VARIACAO_MAX 4.0f
#define JANELA_VARIACAO_MS 60000UL

#define UMIDADE_MIN 30.0f
#define UMIDADE_MAX 85.0f
#define UMIDADE_CRITICA 20.0f

#define SOM_LIMITE_ALTO 400
#define SOM_CONSECUTIVOS 5

// Indices dos topicos de publicacao (ordem de TOPICOS_PUBLICAR em secrets.cpp)
enum TopicoPublicacao {
    TOPICO_ALERTA = 0,
    TOPICO_LOG = 1,
    TOPICO_STATUS = 2
};

// Indices dos topicos de recebimento (ordem de TOPICOS_RECEBER em secrets.cpp)
enum TopicoRecebido {
    TOPICO_COMANDO = 0,
};

#endif