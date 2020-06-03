/**
 * Port: 1616
 * IP Address: 192.168.0.140
 * Input: 2 char Hex String converted to Binary
 * Pin Outputs: 2,3,4,5, 6,7,8,9
 * Also can do A0-A5 for an additional 6 outputs (make these Plated Thru Holes on PCB for additional Expansion possibilities)
 */

#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EEPROM.h>

#define BUF_SIZE 8

// byte mac[] = { 0x89, 0xF0, 0x4F, 0xB0, 0x2C, 0x65 };
byte mac[6] = { 0xBA, 0xBE, 0x00, 0x00, 0x00, 0x00 };
char macstr[18];

IPAddress ip(192,168,1,55);
EthernetServer server(1616);
int eth_led = 2;
bool eth_led_state = false;

String readRequest(EthernetClient*);
void executeRequest(EthernetClient*, String*);
void handleMACaddr();

/**
 *  set led on when Ethernet Initilized. If not init, blink all @ rate
 *  while waiting for TCP connection, pulse LED's to inidicate waiting...
 *  if Ethernet cable not connected, blink ETH light @ rate
 */

void setup(){
	handleMACaddr();
	for (int i = 2; i <= 9; i++) { //intialize output pins
		pinMode(i, OUTPUT);
	}
	Serial.begin(9600);
	// while(!Serial) {}
	//blink all leds 5 times to inidicate Serial was successful.
	for(int i = 0; i <= 5; i++) {
		for (int i = 2; i <= 9; i++)
			digitalWrite(i, HIGH);
		delay(100);
		for (int i = 2; i <= 9; i++)
			digitalWrite(i, LOW);
		delay(100);
	}

	Serial.println("begin");
	/**
	 * if Ethernet shield fails to initialized, blink the LED over the ethernet port forever
	 */
	Ethernet.init(10);
	Ethernet.begin(mac, ip);
	if (Ethernet.hardwareStatus() == EthernetNoHardware) {
		Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
		while (true) {
			digitalWrite(eth_led, HIGH);
			delay(100);
			digitalWrite(eth_led, LOW);
			delay(100);
		}
	}
	/**
	 * if Ethernet Cable is not plugged in, turn LED 2 on
	 */
	if (Ethernet.linkStatus() == LinkOFF){
		digitalWrite(eth_led, HIGH);
		Serial.println("Ethernet cable is not connected.");
	} else digitalWrite(eth_led, LOW);
	server.begin();
	Serial.print("IP address:");
	Serial.print(Ethernet.localIP());
	Serial.print(" Port:1616 Mac Addr:");	
	Serial.println(macstr);
}

void loop(){
	EthernetClient client = server.available();
	if(!client){
		return;
	}
	Serial.println("Client connected");
	String request = readRequest(&client); //gets Serial string
	Serial.println(request); //prints that Serial String out
	executeRequest(&client, &request);

	Serial.println("Client disonnected");
}

String readRequest(EthernetClient* client){
	String request = "";
	while (client->connected()){
		while (client->available()){
			char c = client->read();
			// Serial.write(c); //prints recieved characters
			if ('\n' == c){
				return request;
			}
			request += c;
		}
	}
	return request;
}
void executeRequest(EthernetClient* client, String* request){
	for(int i = 0; i < BUF_SIZE; i++) {
		int pinState = request->charAt(i);
		int pinNum = (BUF_SIZE-i)+1;
		if(pinState == '1') digitalWrite(pinNum, HIGH);
		else if(pinState == '0') digitalWrite(pinNum, LOW);
	}
}

void handleMACaddr() {
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
}