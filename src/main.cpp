#include <Arduino.h>
#include "config.h"
#include "sensores.h"
#include "alertas.h"

static unsigned long ultimaPublicacao = 0;

void setup() {
    Serial.begin(115200);
    sensoresInit();
    Serial.println("Sistema iniciado");
}

void loop() {
    if (lerSensores()) {
        verificarAlertas();
    }

    if (getLeitura().valida && millis() - ultimaPublicacao > INTERVALO_PUBLICACAO_MS) {
        // TODO implementar sistema de públicar no tópico.
        LeituraSensores leitura = getLeitura();
        Serial.printf("Temperatura: %.1f C | Umidade: %1.f %% | Som: %d\n", leitura.temp, leitura.umidade, leitura.som);
        ultimaPublicacao = millis();
    }
}
