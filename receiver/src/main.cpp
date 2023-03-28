#include <Arduino.h>
#include <RF24.h>

uint8_t address[6] = {0x19, 0x97, 0x7d, 0xf0, 0xdd, 0x7d};

RF24 radio(9, 10, 4000000);

void setup() {
  radio.begin();
  radio.setRadiation(RF24_PA_LOW, RF24_250KBPS);
  radio.setChannel(113);
  radio.setPayloadSize(3);
  radio.openReadingPipe(1, address);
  radio.startListening();
  Serial.begin(57600);
}

void loop() {
  uint8_t pipe;
  static uint32_t last;
  if (radio.available(&pipe)) {
    uint8_t bytes = radio.getPayloadSize();
    uint8_t payload[3];
    radio.read(&payload, bytes > 3 ? 3 : bytes);
    Serial.print("RX: ");
    Serial.print(payload[0], 16);
    Serial.print(payload[1], 16);
    Serial.print(payload[2], 16);

    uint32_t now = millis();
    Serial.print(" - delta: ");
    Serial.print(now - last, 10);

    Serial.println();

    last = now;
  }
}
