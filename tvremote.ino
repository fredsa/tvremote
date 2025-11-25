#include <Arduino.h>

#define IR_SEND_PIN MISO
#define DISABLE_CODE_FOR_RECEIVER
#include <IRremote.hpp>

#include "admiral.h"
#include "sony.h"

enum TVType { TV_SONY, TV_ADMIRAL };

TVType selectedTV;
uint8_t sAddress;
uint8_t sRepeats;
uint8_t sNumberOfBits;
uint8_t sMinDelayMs;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);

  delay(1000);

  Serial.println();
  Serial.println("=====================================");
  Serial.printf(__FILE__ ", " __DATE__ "\n");
  Serial.printf("VERSION_IRREMOTE " VERSION_IRREMOTE "\n");
  Serial.printf("IR_SEND_PIN %i\n", IR_SEND_PIN);

  while (true) {
    Serial.println();
    Serial.println("Select TV:");
    Serial.println("1. Sony");
    Serial.println("2. Admiral");
    Serial.print("Enter choice: ");

    while (!Serial.available()) {
      delay(10);
    }

    char choice = Serial.read();
    // Consume rest of line
    while (Serial.available() && Serial.read() != '\n')
      ;

    Serial.println(choice);

    if (choice == '1') {
      selectedTV = TV_SONY;
      sAddress = SONY_IR_ADDRESS;
      sRepeats = SONY_IR_REPEATS;
      sNumberOfBits = SONY_IR_NUMBER_OF_BITS;
      sMinDelayMs = SONY_IR_MIN_DELAY_MS;
      Serial.println("Sony TV selected.");
      break;
    } else if (choice == '2') {
      selectedTV = TV_ADMIRAL;
      sAddress = ADMIRAL_IR_ADDRESS;
      sRepeats = ADMIRAL_IR_REPEATS;
      sMinDelayMs = ADMIRAL_IR_MIN_DELAY_MS;
      Serial.println("Admiral TV selected.");
      break;
    } else {
      Serial.println("Invalid selection.");
    }
  }
}

void loop() {
  static char hexBuffer[3]; // Two hex digits + '\0'.
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

      if (selectedTV == TV_SONY) {
        Serial.printf("sendSony(address=0x%02x, command=0x%02x, repeats=%i, "
                      "numberOfBits=%i)\n",
                      sAddress, cmd, sRepeats, sNumberOfBits);
        IrSender.sendSony(sAddress, cmd, sRepeats, sNumberOfBits);
      } else {
        Serial.printf("sendSharp(address=0x%02x, command=0x%02x, repeats=%i)\n",
                      sAddress, cmd, sRepeats);
        IrSender.sendSharp(sAddress, cmd, sRepeats);
      }
      delay(sMinDelayMs);

      count = 0;
    }
  } else {
    count = 0;
  }
}
