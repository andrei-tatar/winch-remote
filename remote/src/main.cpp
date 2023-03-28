#include <Arduino.h>
#include <LowPower.h>
#include <RF24.h>

#define PIN_B1 2
#define PIN_B2 3

RF24 radio(9, 10, 4000000);
uint8_t address[6] = {0x19, 0x97, 0x7d, 0xf0, 0xdd, 0x7d};

ISR(INT0_vect) {}
ISR(INT1_vect) {}

void setup() {
  pinMode(PIN_B1, INPUT_PULLUP);
  pinMode(PIN_B2, INPUT_PULLUP);

  radio.begin();
  radio.setRadiation(RF24_PA_LOW, RF24_250KBPS);
  radio.setChannel(113);
  radio.setPayloadSize(3);
  radio.openWritingPipe(address);
  radio.powerDown();

  EICRA = 0b1010; // falling edge
}

bool sendPacket(uint8_t sessionCounter, uint8_t counter) {
  uint8_t buttons = (digitalRead(PIN_B1) == LOW ? 0x01 : 0) |
                    (digitalRead(PIN_B2) == LOW ? 0x02 : 0);

  uint8_t buff[3];
  buff[0] = sessionCounter;
  buff[1] = counter;
  buff[2] = buttons;

  radio.powerUp();
  radio.write(buff, 3);
  radio.powerDown();

  return buttons;
}

void loop() {
  EIMSK = 0b0011; // enable button interrupts (for wakup)
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
  EIMSK = 0; // disable button interrupts

  LowPower.powerDown(SLEEP_60MS, ADC_OFF, BOD_OFF); // debounce

  static uint8_t sessionCounter = 0;
  uint8_t counter = 0;
  while (sendPacket(sessionCounter, counter++)) {
    LowPower.powerDown(SLEEP_250MS, ADC_OFF, BOD_OFF);
  }

  LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF); // debounce

  sessionCounter++;
}
