#include <Arduino.h>


// Defines pin number:

// Front Proximity Sensor
#define E_FRONT 12
#define T_FRONT 13
// Back Proximity Sensor
#define E_BACK 9
#define T_BACK 8
// Left Proximity Sensor
#define E_LEFT 3
#define T_LEFT 2
// Right Proximity Sensor
#define E_RIGHT 7
#define T_RIGHT 4
// Left Proximity Motor
#define MOTOR_L1 10
#define MOTOR_L2 11
// Right Motor
#define MOTOR_R3 5
#define MOTOR_R4 6



long duration;
long distance;

// put function declarations here:

void move(int direction, int speed)
{
 // FORWARD 1 BACK 2 RIGHT 3 LEFT 4
  switch(direction)
  {
    case 1:
      analogWrite(MOTOR_L1, speed);
      analogWrite(MOTOR_L2, 0);
      analogWrite(MOTOR_R3, speed);
      analogWrite(MOTOR_R4, 0);
      break;
    case 2:
      analogWrite(MOTOR_L1, 0);
      analogWrite(MOTOR_L2, speed);
      analogWrite(MOTOR_R3, 0);
      analogWrite(MOTOR_R4, speed);
      break;
    case 3:
      analogWrite(MOTOR_L1, speed);
      analogWrite(MOTOR_L2, 0);
      analogWrite(MOTOR_R3, 0);
      analogWrite(MOTOR_R4, speed);
      break;
    case 4:
      analogWrite(MOTOR_L1, 0);
      analogWrite(MOTOR_L2, speed);
      analogWrite(MOTOR_R3, speed);
      analogWrite(MOTOR_R4, 0);
      break;
    default:
      analogWrite(MOTOR_L1, 0);
      analogWrite(MOTOR_L2, 0);
      analogWrite(MOTOR_R3, 0);
      analogWrite(MOTOR_R4, 0);
      break;
  }
}


// put function definitions here:
void setPins(char out, char in) {
  pinMode(out, OUTPUT); // Sets the output
  pinMode(in, INPUT); // Sets the input
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); // Sets the baud transfer
  
  // Sensors
  // Front
  setPins(T_FRONT, E_FRONT);
  // Back
  setPins(T_BACK, E_BACK);
  // Left
  setPins(T_LEFT, E_LEFT);
  // Right
  setPins(T_RIGHT, E_RIGHT);

  pinMode(MOTOR_L1, OUTPUT);
  pinMode(MOTOR_L2, OUTPUT);
  pinMode(MOTOR_R3, OUTPUT);
  pinMode(MOTOR_R4, OUTPUT);

  // Motors
}

void readDistance(char trig, char echo, String name) {
  // Clears the trigPin
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echo, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2;
  // Prints the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.print(name);
  Serial.print(" ");
  Serial.println(distance);
  delay(50);
}

void loop() {
  // put your main code here, to run repeatedly:
  // readDistance(T_FRONT, E_FRONT, "Front");
  // readDistance(T_BACK, E_BACK, "Back");
  // readDistance(T_LEFT, E_LEFT, "Left");
  // readDistance(T_RIGHT, E_RIGHT, "Right");
  move(1, 255);
  delay(5000);
  move(2, 255);
  delay(5000);
  move(3, 255);
  delay(5000);
  move(4, 255);
  delay(5000);
  move(5, 255);
  delay(5000);
}