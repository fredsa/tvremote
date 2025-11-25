#include <Arduino.h>

#define IR_SEND_PIN MISO
#define DISABLE_CODE_FOR_RECEIVER
#include <IRremote.hpp>

// #include "admiral.h"
#include "sony.h"


uint8_t sAddress = IR_ADDRESS;
uint8_t sRepeats = IR_REPEATS;
uint8_t sNumberOfBits = IR_NUMBER_OF_BITS;
uint8_t sMinDelayMs = IR_MIN_DELAY_MS;

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

void sony(uint16_t aAddress, uint8_t aCommand, int_fast8_t aNumberOfRepeats, uint8_t numberOfBits) {
}

void loop() {
  static char hexBuffer[3];  // Two hex digits + '\0'.
  static uint8_t count = 0;

  if (!Serial.available()) {
    return;
  }

  char incomingChar = Serial.read();
  // if (incomingChar == '#') {
  //   String d = Serial.readStringUntil('\n');
  //   sRepeats = d.toInt();
  //   Serial.printf("sRepeats: 0x%02x\n", sRepeats);
  // }
  if (isHexadecimalDigit(incomingChar)) {
    hexBuffer[count++] = incomingChar;

    if (count == 2) {
      hexBuffer[2] = '\0';
      uint8_t cmd = (uint8_t)strtol(hexBuffer, NULL, 16);

      // Serial.printf("sendSharp(address=0x%02x, command=0x%02x, repeats=%i)\n", sAddress, cmd, sRepeats);
      // IrSender.sendSharp(sAddress, cmd, sRepeats);

      sony(sAddress, cmd, sRepeats, sNumberOfBits);
      Serial.printf("sendSony(address=0x%02x, command=0x%02x, repeats=%i, numberOfBits=%i)\n", sAddress, cmd, sRepeats, sNumberOfBits);
      IrSender.sendSony(sAddress, cmd, sRepeats, sNumberOfBits);
      delay(sMinDelayMs);

      count = 0;
    }
  } else {
    count = 0;
  }
}
