#include <Arduino.h>
#include <RF24.h>

#define PIN_DISABLE_MANUAL 2
#define PIN_IN 3
#define PIN_OUT 4

uint8_t address[6] = {0x19, 0x97, 0x7d, 0xf0, 0xdd, 0x7d};

RF24 radio(9, 10, 4000000);

void setup() {
  digitalWrite(PIN_DISABLE_MANUAL, LOW);
  digitalWrite(PIN_IN, HIGH);
  digitalWrite(PIN_OUT, HIGH);

  pinMode(PIN_DISABLE_MANUAL, OUTPUT);
  pinMode(PIN_IN, OUTPUT);
  pinMode(PIN_OUT, OUTPUT);

  radio.begin();
  radio.setRadiation(RF24_PA_LOW, RF24_250KBPS);
  radio.setChannel(113);
  radio.setPayloadSize(3);
  radio.openReadingPipe(1, address);
  radio.startListening();
}

void loop() {
  uint8_t pipe;
  static uint32_t last;
  static bool on = false;
  uint32_t now = millis();

  if (radio.available(&pipe)) {
    uint8_t bytes = radio.getPayloadSize();
    uint8_t payload[3];
    radio.read(&payload, bytes > 3 ? 3 : bytes);

    if (payload[2] & 0x01) {
      digitalWrite(PIN_DISABLE_MANUAL, HIGH);
      digitalWrite(PIN_IN, LOW);
      digitalWrite(PIN_OUT, HIGH);
      on = true;
    } else if (payload[2] & 0x02) {
      digitalWrite(PIN_DISABLE_MANUAL, HIGH);
      digitalWrite(PIN_IN, HIGH);
      digitalWrite(PIN_OUT, LOW);
      on = true;
    } else {
      digitalWrite(PIN_IN, HIGH);
      digitalWrite(PIN_OUT, HIGH);
      digitalWrite(PIN_DISABLE_MANUAL, LOW);
      on = false;
    }

    last = now;
  }

  if (on && now - last > 1000) {
    on = false;
    digitalWrite(PIN_IN, HIGH);
    digitalWrite(PIN_OUT, HIGH);
    digitalWrite(PIN_DISABLE_MANUAL, LOW);
  }
}
