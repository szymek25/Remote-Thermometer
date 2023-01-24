#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPI.h>
#include <RF24.h>
#include <string.h>
#include <printf.h>
RF24 radio(9,10);

OneWire oneWire(A5); //Podłączenie do A5
DallasTemperature sensors(&oneWire);

void setup() {
  Serial.begin(9600);
  sensors.begin();
  radio.begin();
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(0x76);
  radio.openWritingPipe(0xF0F0F0F0E1LL);
  const uint64_t pipe = 0xE8E8F0F0E1LL;
  radio.openReadingPipe(1, pipe);
  radio.enableDynamicPayloads();
  radio.powerUp();
  printf_begin();
}

void loop() {
  radio.startListening();
  char receivedMessage[32] = {0};
  if(radio.available()) {
    radio.read(receivedMessage, sizeof(receivedMessage));
    Serial.println(receivedMessage);
    radio.stopListening();

    String stringMessage(receivedMessage);
    if(stringMessage == "GETTEMPERATURE") {
      sensors.requestTemperatures();//Pobranie temperatury czujnika
      char text[5];
      float temp = sensors.getTempCByIndex(0);
      String tempString;
      tempString = String(temp);
      strcpy(text, tempString.c_str());
      Serial.println(text);
      radio.write(&text, sizeof(text));
    }
  }

  delay(1000);
}
