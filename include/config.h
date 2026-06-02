#ifndef CONFIG_H
#define CONFIG_H

#define PINO_TEMPERATURA 5
#define PINO_SOM 11

#define TIPO_DHT DHT22
#define INTERVALO_DHT_MS 2000UL
#define INTERVALO_PUBLICACAO_MS 10000UL
#define INTERVALO_PUBLICACAO_MS_SOM 10000UL

#define TEMPERATURA_MAX 30.0f
#define TEMPERATURA_MIN 14.0f
#define TEMPERATURA_VARIACAO_MAX 4.0f
#define JANELA_VARIACAO_MS 60000UL

#define UMIDADE_MIN 30.0f
#define UMIDADE_MAX 85.0f
#define UMIDADE_CRITICA 20.0f

#define SOM_BAIXO 35f

#define SOM_LIMITE_ALTO 1200
#define SOM_CONSECUTIVOS 5

// Indices dos topicos de publicacao (ordem de TOPICOS_PUBLICAR em secrets.cpp)
enum TopicoPublicacao {
    TOPICO_LOG = 0,
};

// Indices dos topicos de recebimento (ordem de TOPICOS_RECEBER em secrets.cpp)
enum TopicoRecebido {
    TOPICO_CONTROLE = 0,
};

#endif