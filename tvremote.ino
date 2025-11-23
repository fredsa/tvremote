#include <Arduino.h>

#define IR_SEND_PIN MISO
#define DISABLE_CODE_FOR_RECEIVER
#include <IRremote.hpp>

#include "admiral.h"

uint8_t sAddress = IR_ADDRESS;
uint8_t sRepeats = IR_REPEATS;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);

  delay(1000);

  Serial.println();
  Serial.println("=====================================");
  Serial.printf(__FILE__ ", " __DATE__ "\n");
  Serial.printf("VERSION_IRREMOTE " VERSION_IRREMOTE "\n");
  Serial.printf("IR_SEND_PIN %i\n", IR_SEND_PIN);
}

void loop() {
  static char hexBuffer[3]; // Two hex digits + '\0'.
  static uint8_t count = 0;

  if (!Serial.available()) {
    return;
  }

  char incomingChar = Serial.read();
  if (isHexadecimalDigit(incomingChar)) {
    hexBuffer[count++] = incomingChar;

    if (count == 2) {
      hexBuffer[2] = '\0';
      uint8_t cmd = (uint8_t)strtol(hexBuffer, NULL, 16);

      Serial.printf("sendSharp(address=0x%02x, command=0x%02x, repeats=%i)\n", sAddress, cmd, sRepeats);
      IrSender.sendSharp(sAddress, cmd, sRepeats);
      delay(100);

      count = 0;
    }
  } else {
    count = 0;
  }
}
