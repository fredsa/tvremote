#include <Arduino.h>

#define SEND_PWM_BY_TIMER
#define IR_SEND_PIN MISO
#define DISABLE_CODE_FOR_RECEIVER
#include <IRremote.hpp>

#include "admiral_sharp.h"
#include "sony.h"

enum TVType { TV_SONY,
              TV_ADMIRAL_SHARP };

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
    Serial.println("2. Admiral (Sharp)");
    Serial.print("Enter choice: ");

    while (!Serial.available()) {
      delay(10);
    }

    String line = Serial.readStringUntil('\n');
    line.trim();

    Serial.println(line);

    if (line == "1") {
      selectedTV = TV_SONY;
      sAddress = SONY_IR_ADDRESS;
      sRepeats = SONY_IR_REPEATS;
      sNumberOfBits = SONY_IR_NUMBER_OF_BITS;
      sMinDelayMs = SONY_IR_MIN_DELAY_MS;
      Serial.println("Sony TV selected.");
      break;
    } else if (line == "2") {
      selectedTV = TV_ADMIRAL_SHARP;
      sAddress = ADMIRAL_IR_ADDRESS;
      sRepeats = ADMIRAL_IR_REPEATS;
      sMinDelayMs = ADMIRAL_IR_MIN_DELAY_MS;
      Serial.println("Admiral (Sharp) TV selected.");
      break;
    } else {
      Serial.println("Invalid selection.");
    }
  }
  show_settings();
  Serial.println();
  Serial.println("Use above special character followed by hex bye to change these settings.");
  Serial.println();
  Serial.println("To send codes, enter one or more hex bytes.");
  Serial.println();
}

void show_settings() {
  Serial.println();
  Serial.printf("  #%02x  device address\n", sAddress);
  Serial.printf("  r%02x  number of repeats, typically 1, 3, or 5\n", sRepeats);
  Serial.printf("  m%02x  min delay between codes in milliseconds\n", sMinDelayMs);
  Serial.printf("  ^%02x  (Sony) number of bits (0x0c / 0x0f / 0x14)\n", sNumberOfBits);
}

bool handleSetting(String &line, char prefix, uint8_t &variable) {
  if (line.startsWith(String(prefix))) {
    if (line.length() >= 3 && isHexadecimalDigit(line[1]) && isHexadecimalDigit(line[2])) {
      char buf[3] = { line[1], line[2], '\0' };
      variable = (uint8_t)strtol(buf, NULL, 16);
      Serial.println("Setting changed.");
      show_settings();
      return true;
    } else {
      Serial.println("Invalid setting value.");
      return false;
    }
  }
  return false;
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

  if (handleSetting(line, '#', sAddress))
    return;
  if (handleSetting(line, 'r', sRepeats))
    return;
  if (handleSetting(line, 'm', sMinDelayMs))
    return;
  if (handleSetting(line, '^', sNumberOfBits))
    return;

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
      i++;  // Skip the second digit
    }
  }
}

void loop() {
  if (Serial.available()) {
    String line = Serial.readStringUntil('\n');
    processLine(line);
  }
}
