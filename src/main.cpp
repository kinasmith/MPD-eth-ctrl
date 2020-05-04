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
IPAddress ip(192,168,1,55);
EthernetServer server(1616);



String readRequest(EthernetClient*);
void executeRequest(EthernetClient*, String*);

void setup(){
	Ethernet.init(10);
	Serial.begin(9600);
	// while(!Serial) {}d
	Serial.println("begin");
	Ethernet.begin(mac, ip);
	if (Ethernet.hardwareStatus() == EthernetNoHardware) {
		Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
		while (true) {
		delay(1); 
		}
	}
	if (Ethernet.linkStatus() == LinkOFF){
		Serial.println("Ethernet cable is not connected.");
	}
	server.begin();
	Serial.print("Server address:");
	Serial.println(Ethernet.localIP());
	for(int i = 2; i <= 9; i++) { //intialize output pins
		pinMode(i, OUTPUT);
		// Serial.println(i);
	}
}

void loop(){

	EthernetClient client = server.available();
	if (!client){
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
