#include <Arduino.h>
#include <DHT.h>
#include <Bounce2.h>

#define TIPODHT DHT22
#define pinoTemp 5

bool comeco = true;
const int pinoSom  = 11;

DHT dht(pinoTemp, TIPODHT);
Bounce botao = Bounce();

void sensores();

uint contador = 0;

void setup()
{
  Serial.begin(115200);
  dht.begin();
  pinMode(pinoSom, INPUT);
  botao.attach(0, INPUT_PULLUP);
}

void loop()
{
  sensores();
  botao.update();
}
void sensores()
{
  bool temVolume = digitalRead(pinoSom);
  int volume = analogRead(pinoSom);
  float umidade = dht.readHumidity();
  float tempC = dht.readTemperature();
  float tempAnterior = tempC;

  if(isnan(umidade) || isnan(tempC))
  {
      Serial.println("Falha ao iniciar o sensor");
      Serial.println("Verifique a conexão e o pino definido");
      return;
  }

   if (comeco)
   {
     Serial.println("==== Sensor DHT 22 ====");
     Serial.println("Sensor Iniciado");
     Serial.println("=======================");
     Serial.printf("Temperatura: %.1f C\n\r", tempC);
     Serial.printf("Umidade: %.1f %%\n\r", umidade);
     Serial.printf("Som: %d nm\n\r", volume);
     comeco = false;
   }

  if (millis() - contador > 120000)
  {
    Serial.println("=======================");
    Serial.printf("Temperatura: %.1f C\n\r", tempC);
    Serial.printf("Umidade: %.1f %%\n\r", umidade);
    Serial.printf("Som: %d nm \n\r", volume);
    Serial.println("=======================");
    contador = millis();
  }

  if (botao.fell())
  {
    Serial.println("=======================");
    Serial.printf("Temperatura: %.1f C\n\r", tempC);
    Serial.printf("Umidade: %.1f %%\n\r", umidade);
    Serial.printf("Som: %d nm\n\r", volume);
    Serial.println("=======================");
  }
  
}
