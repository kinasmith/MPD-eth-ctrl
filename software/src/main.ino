#include <SPI.h>
#include <Ethernet.h>

#define BUF_SIZE 8

//mac address of device
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 58); //IP Address of device

EthernetServer server(1616); //TCP Port for device
boolean alreadyConnected = false;

//status led
int status_led_pin = 13;
bool status_led_state = false;

void setup() {
    //intialize output pins
    for (int i = 2; i <= 9; i++) { 
		pinMode(i, OUTPUT);
	}
    pinMode(status_led_pin, OUTPUT);
    Ethernet.init(10); //initialized Ethernet Shield
    Ethernet.begin(mac, ip); 
    Serial.begin(9600); //start debug serieal
    // while (!Serial) {
    //     ;
    // }
    /**
     * If the Shield is missing, blink LED forever
     */
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
     * if Ethernet Cable is disconnected, light LED until it's not
     */
	if (Ethernet.linkStatus() == LinkOFF){
		digitalWrite(status_led_pin, HIGH);
		Serial.println("Ethernet cable is not connected.");
	} else digitalWrite(status_led_pin, LOW);

    server.begin(); //start TCP Server
    //debug print IP Address
    Serial.print("Server address:");
    Serial.println(Ethernet.localIP());
}

void loop() {
    String request = ""; //recieved TCP messages
    EthernetClient client = server.available(); //TCP Client
    /**
     * If the client is already connected, don't reconnect.
     */
    if (client) {
        if (!alreadyConnected) {
            client.flush();
            Serial.println("client connected");
            alreadyConnected = true;
        }
        String request = readRequest(&client); //listen and read TCP messages
        executeRequest(&client, &request); //execute commands
    }
}

/**
 * Listens to TCP port and formats incoming messages
 * All messages must end with a New Line Character
 */
String readRequest(EthernetClient* client){
	String request = "";
    while (client->available()){
        char c = client->read();
        Serial.write(c); //prints recieved characters
        if ('\n' == c){
            return request;
        }
        request += c;
    }
	return request;
}
/**
 * Executes requests based on the incoming messages
 * 'w*******': Writes the output pins of device
 * 'r': Polls what the state of pins 2-9 are and outputs those values as a String formated Binary Value
 *  
 */
void executeRequest(EthernetClient* client, String* request){
    char command = readCommand(request);
    if('w' == command) {
        String writeVals = request->substring(1,9);
        Serial.println(writeVals);
    	for(int i = 0; i < BUF_SIZE; i++) {
            int pinState = writeVals.charAt(i);
            int pinNum = (BUF_SIZE-i)+1;
            if(pinState == '1') digitalWrite(pinNum, HIGH);
            else if(pinState == '0') digitalWrite(pinNum, LOW);
        }
        sendResponse(client, "k");
    }
    else if('r' == command) {
        String pinState;
        for (int i = 9; i >= 2; i--)
            pinState += digitalRead(i);
        sendResponse(client, pinState);
    }
}
/**
 * Seperates the command from the incoming message
 */
char readCommand(String* request)
{
	String commandString = request->substring(0, 1);
	return commandString.charAt(0);
}
/**
 * sends a TCP response
 */
void sendResponse(EthernetClient* client, String response)
{
	client->println(response);
	// Debug print.
	// Serial.print("sendResponse:");
	// Serial.println(response);
}