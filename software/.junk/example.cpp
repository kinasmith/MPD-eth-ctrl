/*
Arduino Ethernet Script Server

Created Mars 4, 2014
Mikael Kindborg, Evothings AB
*/

// Include files.
#include <SPI.h>
#include <Ethernet.h>

byte mac[] = { 0x90, 0xA2, 0xDA, 0x0E, 0xD0, 0x93 };
IPAddress ip(192,168,1, 58);
EthernetServer server(1616);

void setup()
{
	Serial.begin(9600);
	while (!Serial) { ; }
    Ethernet.init(10);
	Ethernet.begin(mac, ip);

	// Start the server.
	server.begin();

	// Print status.
	printServerStatus();
}

void loop()
{
	// Listen for incoming client requests.
	EthernetClient client = server.available();
	if (!client)
	{
		return;
	}

	Serial.println("Client connected");

	String request = readRequest(&client);
	executeRequest(&client, &request);

	// Close the connection.
	//client.stop();

	Serial.println("Client disonnected");
}

// Read the request line,
String readRequest(EthernetClient* client)
{
	String request = "";

	// Loop while the client is connected.
	while (client->connected())
	{
		// Read available bytes.
		while (client->available())
		{
			// Read a byte.
			char c = client->read();

			// Print the value (for debugging).
			Serial.write(c);

			// Exit loop if end of line.
			if ('\n' == c)
			{
				return request;
			}

			// Add byte to request line.
			request += c;
		}
	}
	return request;
}

void executeRequest(EthernetClient* client, String* request)
{
	char command = readCommand(request);
	int n = readParam(request);
	if ('O' == command)
	{
		pinMode(n, OUTPUT);
	}
	else if ('I' == command)
	{
		pinMode(n, INPUT);
	}
	else if ('L' == command)
	{
		digitalWrite(n, LOW);
	}
	else if ('H' == command)
	{
		digitalWrite(n, HIGH);
	}
	else if ('R' == command)
	{
		sendResponse(client, String(digitalRead(n)));
	}
	else if ('A' == command)
	{
		sendResponse(client, String(analogRead(n)));
	}
}

// Read the command from the request string.
char readCommand(String* request)
{
	String commandString = request->substring(0, 1);
	return commandString.charAt(0);
}

// Read the parameter from the request string.
int readParam(String* request)
{
	// This handles a hex digit 0 to F (0 to 15).
	char buffer[2];
	buffer[0] = request->charAt(1);
	buffer[1] = 0;
	return (int) strtol(buffer, NULL, 16);
}

void sendResponse(EthernetClient* client, String response)
{
	// Send response to client.
	client->println(response);

	// Debug print.
	Serial.println("sendResponse:");
	Serial.println(response);
}

void printServerStatus()
{
	Serial.print("Server address:");
	Serial.println(Ethernet.localIP());
}