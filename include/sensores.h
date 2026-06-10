#ifndef SENSORES_H
#define SENSORES_H

#include <Arduino.h>

struct LeituraSensores
{
    float temperatura;
    float umidade;
    float som;
    bool valida;
};

void sensoresInit();
bool lerSensores();
LeituraSensores getLeitura();
float getTemperaturaVariacao();
bool dhtEstaComErro();

void atualizarSom();

float getDbMedio();

#endif