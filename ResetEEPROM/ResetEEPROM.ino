#include <EEPROM.h>

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  for (int i = 0; i < 255; i++) {
    EEPROM.write(i, 0);
  }
}

void loop() {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
}
