#include <WiFiS3.h>
#include <template.h>
#include <motorsensors.h>

const char* ssid = "Dragos's S22 Ultra";
const char* password = "ghck0723";
WiFiServer server(80);

// Sensor distances array, making accesing the distances easier
volatile int distances[4] = {100, 100, 100, 100}; // Front, Back, Left, Right
bool autonomousMode = false;

void setupWiFiAP() {
    Serial.println("Setting up AP");
    WiFi.begin(ssid, password);
    delay(2);
    
    IPAddress IP = WiFi.localIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);
    
    server.begin();
    Serial.println("Web server started");
  }
  
  void handleWebServer() {
    WiFiClient client = server.available();
    if (client) {
      Serial.println("New client connected");
      String currentLine = "";
      String requestLine = "";  // <--- Capture request line here
      bool isFirstLine = true;
  
      while (client.connected()) {
        if (client.available()) {
          char c = client.read();
          Serial.write(c);  // shows everything
          if (c == '\n') {
            if (currentLine.length() == 0) {
              Serial.println("End of headers");
  
              Serial.println("Request line: " + requestLine);
  
              // Handle /control?cmd=
              if (requestLine.indexOf("GET /control") >= 0) {
                int cmdStart = requestLine.indexOf("cmd=") + 4;
                String command = "";
                for (int i = cmdStart; i < requestLine.length(); i++) {
                  if (requestLine[i] == ' ' || requestLine[i] == '&') break;
                  command += requestLine[i];
                }
  
                command.trim(); // just in case
                Serial.println("Parsed command: " + command);
  
                processCommand(command);
  
                client.println("HTTP/1.1 200 OK");
                client.println("Content-Type: text/plain");
                client.println("Connection: close");
                client.println();
                client.println("Command received: " + command);
              }
              // Handle /data
              else if (requestLine.indexOf("GET /data") >= 0) {
                client.println("HTTP/1.1 200 OK");
                client.println("Content-Type: text/plain");
                client.println("Connection: close");
                client.println();
                client.println(getMapDataString());
              } 
              // Default response
              else {
                client.println("HTTP/1.1 200 OK");
                client.println("Content-Type: text/html");
                client.println("Connection: close");
                client.println();
                client.print(webpageTemplate);
              }
              break;
            } else {
              if (isFirstLine) {
                requestLine = currentLine;
                isFirstLine = false;
              }
              currentLine = "";
            }
          } else if (c != '\r') {
            currentLine += c;
          }
        }
      }
  
      delay(10);
      client.stop();
      Serial.println("Client disconnected");
    }
  }
  
  // Process commands from web interface
  void processCommand(String command) {
  
    Serial.print("Received command: ");
    Serial.println(command);
    
    if (command == "START") {
      autonomousMode = true;
      Serial.println("Autonomous mode enabled");
    } 
    else if (command == "STOP") {
      autonomousMode = false;
      setMotorSpeed(MOTOR_LEFT, 0);
      setMotorSpeed(MOTOR_RIGHT, 0);
      Serial.println("Autonomous mode disabled");
    }
  }
  
  // Get string of current sensor data for web client
  String getMapDataString() {
    String dataString = "";
    // Front sensor
    dataString += "0," + String(distances[0]) + ";";
    // Right sensor
    dataString += "270," + String(distances[3]) + ";";
    // Left sensor
    dataString += "90," + String(distances[2]) + ";";
    // Back sensor
    dataString += "180," + String(distances[1]) + ";";
    
    return dataString;
  }
  