#include <robotsetup.h>

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