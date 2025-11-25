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

  configure();
}

void configure() {
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

void processLine(String line) {
  line.trim();
  if (line.length() == 0) {
    return;
  }

  if (line.startsWith("*")) {
    configure();
    return;
  }

  if (line.startsWith("#")) {
    String d = line.substring(1);
    sRepeats = d.toInt();
    Serial.printf("sRepeats: 0x%02x\n", sRepeats);
    return;
  }

  char hexBuffer[3];
  hexBuffer[2] = '\0';

  for (int i = 0; i < (int)line.length() - 1; i++) {
    if (isHexadecimalDigit(line[i]) && isHexadecimalDigit(line[i + 1])) {
      hexBuffer[0] = line[i];
      hexBuffer[1] = line[i + 1];
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
      i++; // Skip the second digit
    }
  }
}

void loop() {
  if (Serial.available()) {
    String line = Serial.readStringUntil('\n');
    processLine(line);
  }
}
