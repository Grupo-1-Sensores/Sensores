#include <Arduino.h>

struct LeituraSensores {
    float temp;
    float umidade;
    int som;
    bool valida;
};

void sensoresInit();
bool lerSensores();
LeituraSensores getLeitura();
float getTempVariacao();