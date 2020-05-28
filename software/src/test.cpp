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

#define BUF_SIZE 8

byte mac[] = { 0x89, 0xF0, 0x4F, 0xB0, 0x2C, 0x65 };
IPAddress ip(192,168,1,58);
EthernetServer server(1616);
int status_led_pin = 13;
bool status_led_state = false;

String readRequest(EthernetClient*);
void executeRequest(EthernetClient*, String*);

/**
 *  set led on when Ethernet Initilized. If not init, blink all @ rate
 *  while waiting for TCP connection, pulse LED's to inidicate waiting...
 *  if Ethernet cable not connected, blink ETH light @ rate
 * 
 * Send Acknowledgements on successful write outputs
 * RCV 'r' and send state of all 8 outputs
 * 	use digitalRead() w/o setting pinMode to OUTPUT to read pinState Register
 */

void setup(){
	for (int i = 2; i <= 9; i++) { //intialize output pins
		pinMode(i, OUTPUT);
	}
	Serial.begin(9600);
	while(!Serial) {}
	//blink all leds 5 times to inidicate Serial was successful.
	for(int i = 0; i <= 5; i++) {
		digitalWrite(status_led_pin, HIGH);
		delay(100);
		digitalWrite(status_led_pin, LOW);
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
			digitalWrite(status_led_pin, HIGH);
			delay(100);
			digitalWrite(status_led_pin, LOW);
			delay(100);
		}
	}
	/**
	 * if Ethernet Cable is not plugged in, turn LED 2 on
	 */
	if (Ethernet.linkStatus() == LinkOFF){
		digitalWrite(status_led_pin, HIGH);
		Serial.println("Ethernet cable is not connected.");
	} else digitalWrite(status_led_pin, LOW);
	server.begin();
	Serial.print("Server address:");
	Serial.println(Ethernet.localIP());
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
			Serial.write(c); //prints recieved characters
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