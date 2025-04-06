#include <Arduino.h>
#include <WiFiS3.h>
#include <template.h>

const char* ssid = "RobotAP";
const char* password = "robot1234";
WiFiServer server(80);

// Pin definitions
// Ultrasonic sensors
#define E_FRONT 12  // Echo pin for front sensor
#define T_FRONT 13  // Trigger pin for front sensor
#define E_BACK 9    // Echo pin for back sensor
#define T_BACK 8    // Trigger pin for back sensor
#define E_LEFT 3    // Echo pin for left sensor
#define T_LEFT 2    // Trigger pin for left sensor
#define E_RIGHT 7   // Echo pin for right sensor
#define T_RIGHT 4   // Trigger pin for right sensor

// Motor control pins
#define MOTOR_L1 10  // Left motor forward
#define MOTOR_L2 11  // Left motor backward
#define MOTOR_R3 5   // Right motor forward
#define MOTOR_R4 6   // Right motor backward

// Constants
#define SAFE_DISTANCE 30        // Safe distance in cm
#define CRITICAL_DISTANCE 10    // Critical distance for emergency turns
#define MIN_SPEED 25           // Minimum speed when turning
#define MAX_SPEED 255          // Maximum motor speed

// Motor indices
#define MOTOR_LEFT 0
#define MOTOR_RIGHT 1

// Global variables
volatile unsigned long lastSensorReadTime = 0;

// Sensor distances array, making accesing the distances easier
volatile int distances[4] = {100, 100, 100, 100}; // Front, Back, Left, Right
bool autonomousMode = false;



// Function declarations
void setMotorSpeed(int motorIndex, int speed);
int readDistance(int trigPin, int echoPin);
void updateSensors();
void autonomousNavigation();
void setupWiFiAP();
void handleWebServer();
String getMapDataString();
void processCommand(String command);

void setup() {

  Serial.begin(9600);

  // Configuring sensor pins
  pinMode(T_FRONT, OUTPUT);
  pinMode(E_FRONT, INPUT);
  pinMode(T_BACK, OUTPUT);
  pinMode(E_BACK, INPUT);
  pinMode(T_LEFT, OUTPUT);
  pinMode(E_LEFT, INPUT);
  pinMode(T_RIGHT, OUTPUT);
  pinMode(E_RIGHT, INPUT);
  
  // Configuring motor pins
  pinMode(MOTOR_L1, OUTPUT);
  pinMode(MOTOR_L2, OUTPUT);
  pinMode(MOTOR_R3, OUTPUT);
  pinMode(MOTOR_R4, OUTPUT);
  
  // Initializing motors to stop
  setMotorSpeed(MOTOR_LEFT, 0);
  setMotorSpeed(MOTOR_RIGHT, 0);
  
  // Setting up WiFi Access Point
  setupWiFiAP();
}

void loop() {
  updateSensors();
  handleWebServer();
  if (autonomousMode) {
    autonomousNavigation();
  } else {
    setMotorSpeed(MOTOR_LEFT, 0);
    setMotorSpeed(MOTOR_RIGHT, 0);
  }
}

void setupWiFiAP() {
  Serial.println("Setting up AP");
  WiFi.beginAP(ssid, password);
  delay(2000);
  
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
    
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        
        if (c == '\n') {
          if (currentLine.length() == 0) {
            // Read request line
            String requestLine = client.readStringUntil('\r');
            
            // Process GET request
            if (requestLine.indexOf("GET /control") >= 0) {
              int cmdStart = requestLine.indexOf("cmd=") + 4;
              String command = "";
              for (int i = cmdStart; i < requestLine.length(); i++) {
                if (requestLine[i] == ' ' || requestLine[i] == '&') break;
                command += requestLine[i];
              }
              
              processCommand(command);
              client.println("HTTP/1.1 200 OK");
              client.println("Content-Type: text/plain");
              client.println("Connection: close");
              client.println();
              client.println("Command received: " + command);
            } 
            // Map Data Request
            else if (requestLine.indexOf("GET /data") >= 0) {
              client.println("HTTP/1.1 200 OK");
              client.println("Content-Type: text/plain");
              client.println("Connection: close");
              client.println();
              // Send the map data string
              client.println(getMapDataString());
            } 
            else {
              client.println("HTTP/1.1 200 OK");
              client.println("Content-Type: text/html");
              client.println("Connection: close");
              client.println();
              // Render the default webpage
              client.print(webpageTemplate);
            }
            break;
          } else {
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

  // Guard against empty commands
  if (command.length() == 0) return;

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

// Updated motor control (-255 to 255)
void setMotorSpeed(int motorIndex, int speed) {
  // Constrain speed to valid range
  speed = constrain(speed, -MAX_SPEED, MAX_SPEED);
  
  if (motorIndex == MOTOR_LEFT) {
    if (speed >= 0) {
      // Forward
      analogWrite(MOTOR_L1, speed);
      analogWrite(MOTOR_L2, 0);
    } else {
      // Backward
      analogWrite(MOTOR_L1, 0);
      analogWrite(MOTOR_L2, -speed);
    }
  } 
  else if (motorIndex == MOTOR_RIGHT) {
    if (speed >= 0) {
      // Forward
      analogWrite(MOTOR_R3, speed);
      analogWrite(MOTOR_R4, 0);
    } else {
      // Backward
      analogWrite(MOTOR_R3, 0);
      analogWrite(MOTOR_R4, -speed);
    }
  }
}

// Read distance from ultrasonic sensor
int readDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  unsigned long duration = pulseIn(echoPin, HIGH, 23000); 
  
  // Calculate distance in cm
  if (duration == 0) {
    return 400; // Return max range if timeout
  } else {
    return duration * 0.034 / 2;
  }
}

// Update all sensor readings
void updateSensors() {
  if (millis() - lastSensorReadTime >= 30) {
    distances[0] = readDistance(T_FRONT, E_FRONT); 
    distances[1] = readDistance(T_BACK, E_BACK);   
    distances[2] = readDistance(T_LEFT, E_LEFT);   
    distances[3] = readDistance(T_RIGHT, E_RIGHT); 
    lastSensorReadTime = millis();
  }
}

void autonomousNavigation() {

  // Initialize motor speeds to maximum, the robot will firstly move forward and then 
  // adjust based on sensor readings to avoid obstacles.
  int leftSpeed = MAX_SPEED;
  int rightSpeed = MAX_SPEED;
  
  // Front obstacle detection
  if (distances[0] < SAFE_DISTANCE) {
    int turnIntensity = map(distances[0], CRITICAL_DISTANCE, SAFE_DISTANCE, MAX_SPEED, MIN_SPEED);
    turnIntensity = constrain(turnIntensity, MIN_SPEED, MAX_SPEED);
    
    // Decide which way to turn
    if (distances[2] > distances[3]) {
      // Turn right
      leftSpeed = -turnIntensity;
      rightSpeed = turnIntensity;
    } else {
      // Turn left
      leftSpeed = turnIntensity; 
      rightSpeed = -turnIntensity;
    }
  }
  // Left obstacle detection
  else if (distances[2] < SAFE_DISTANCE) {
    int turnFactor = map(distances[2], CRITICAL_DISTANCE, SAFE_DISTANCE, MAX_SPEED/2, MIN_SPEED/2);
    turnFactor = constrain(turnFactor, MIN_SPEED/2, MAX_SPEED/2);
    
    leftSpeed = MAX_SPEED;
    rightSpeed = MAX_SPEED - turnFactor;
  }
  // Right obstacle detection
  else if (distances[3] < SAFE_DISTANCE) {
    int turnFactor = map(distances[3], CRITICAL_DISTANCE, SAFE_DISTANCE, MAX_SPEED/2, MIN_SPEED/2);
    turnFactor = constrain(turnFactor, MIN_SPEED/2, MAX_SPEED/2);
    
    leftSpeed = MAX_SPEED - turnFactor;
    rightSpeed = MAX_SPEED;
  }
  // No backward obstacle detection as it is not necessary for the now, the robot will only move forward until the end of its days
  setMotorSpeed(MOTOR_LEFT, leftSpeed);
  setMotorSpeed(MOTOR_RIGHT, rightSpeed);
}