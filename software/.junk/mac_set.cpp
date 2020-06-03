#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EEPROM.h>

byte mac[6] = { 0xBA, 0xBE, 0x00, 0x00, 0x00, 0x00 };
char macstr[18];

void setup() {
  Serial.begin(9600);
  while(!Serial);
  // Random MAC address stored in EEPROM
  if (EEPROM.read(1) == '#') {
    for (int i = 2; i < 6; i++) {
      mac[i] = EEPROM.read(i);
    }
  } else {
    randomSeed(analogRead(0));
    for (int i = 2; i < 6; i++) {
      mac[i] = random(0, 255);
      EEPROM.write(i, mac[i]);
    }
    EEPROM.write(1, '#');
  }
  snprintf(macstr, 18, "%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

  // Start up networking
  Serial.print("DHCP (");
  Serial.print(macstr);
  Serial.print(")...");
  Ethernet.begin(mac);
  Serial.print("success: ");
  Serial.println(Ethernet.localIP());
}

void loop() {
}