#include <Arduino.h>

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

// Global variables
volatile unsigned long lastSensorReadTime = 0;

// Sensor distances array, making accesing the distances easier
volatile int distances[4] = {100, 100, 100, 100}; // Front, Back, Left, Right
bool autonomousMode = false;

// Motor indices
#define MOTOR_LEFT 0
#define MOTOR_RIGHT 1

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
    delay(50);
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