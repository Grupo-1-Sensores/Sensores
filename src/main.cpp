#include <Arduino.h>
#include <DHT.h>
#include <Bounce2.h>

#define pinoSom 11
#define TIPODHT DHT22
#define pinoTemp 2

bool comeco = true;

DHT dht(pinoTemp, TIPODHT);
Bounce botao = Bounce();

void sensores();

uint contador = 0;

void setup()
{
  Serial.begin(115200);
}

void loop()
{
}
void sensores()
{

  dht.begin();
  pinMode(pinoSom, INPUT);
  botao.attach(0, INPUT_PULLUP);
  botao.update();

  bool temVolume = digitalRead(pinoSom);
  int volume = analogRead(pinoSom);
  float umidade = dht.readHumidity();
  float tempC = dht.readTemperature();
  float tempAnterior = tempC;

  if (isnan(umidade) || isnan(tempC) || isnan(temVolume))
  {
    Serial.println("Falha ao iniciar o sensores");
    Serial.println("Verifique a conexão e os pinos definidos");
    return;
  }

  if (comeco)
  {
    Serial.println("==== Sensor DHT 22 ====");
    Serial.println("Sensor Iniciado");
    Serial.println("=======================");
    Serial.printf("Temperatura: %.1f C\n\r", tempC);
    Serial.printf("Umidade: %.1f %%\n\r", umidade);
    Serial.printf("Som: %.1f %%\n\r", volume);
    comeco = false;
  }

  if (millis() - contador > 120000)
  {
    Serial.printf("Temperatura: %.1f C\n\r", tempC);
    Serial.printf("Umidade: %.1f %%\n\r", umidade);
    Serial.printf("Som: %.1f %%\n\r", volume);
    contador = millis();
  }

  if (botao.fell())
  {
    Serial.printf("Temperatura: %.1f C\n\r", tempC);
    Serial.printf("Umidade: %.1f %%\n\r", umidade);
    Serial.printf("Som: %.1f %%\n\r", volume);
  }
}
