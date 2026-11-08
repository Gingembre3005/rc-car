#include <Servo.h>
#include <IRremote.hpp>

const int IN1 = 8;
const int IN2 = 9;
const int IN3 = 11;
const int IN4 = 12;

const int ENA = 10;
const int ENB = 13;

const int micPin = 2;

const int servoPin = 6;

bool carOn = false;
int currentSpeed = 0;
bool emergencyStop = false;

const unsigned long cooldown = 500;

unsigned long lastUpPress = 0;
unsigned long lastDownPress = 0;

const int IR_PIN = 3;

const int trigPin = 4;
const int echoPin = 5;

Servo steeringServo;


int angle = 90;

void setup() {
  Serial.begin(9600);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(micPin, INPUT);

  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  IrReceiver.begin(IR_PIN, ENABLE_LED_FEEDBACK);

  lastUpPress = millis() - cooldown;
  lastDownPress = millis() - cooldown;

  steeringServo.attach(servoPin);
  steeringServo.write(90);

  stopCar();

}

void loop() {
  if (IrReceiver.decode()){
    
    unsigned long signal = IrReceiver.decodedIRData.decodedRawData;
    unsigned long address = IrReceiver.decodedIRData.address;
    unsigned long command = IrReceiver.decodedIRData.command;
   // Serial.print("Protocol:  ");
    //Serial.println(getProtocolString(IrReceiver.decodedIRData.protocol));
    Serial.print("Address: 0x");
    Serial.println(IrReceiver.decodedIRData.address,HEX);
    Serial.print("Command: 0x");
    Serial.println(IrReceiver.decodedIRData.command, HEX);
    //Serial.print("Flags: ");
    //Serial.println(IrReceiver.decodedIRData.flags);
    IrReceiver.resume();
    //Serial.print("Signal: ");
    //Serial.println(signal, HEX);
  
  if ( address == 0x07 && command == 0x02){
    stopCar();
  }
  if ( address == 0x07&& command == 0x07 && millis() - lastUpPress >= cooldown && emergencyStop == false && currentSpeed + 50 <= 255){
    currentSpeed = currentSpeed + 50;
    startCar(currentSpeed);
    lastUpPress = millis();
  }  
   
  
  if ( address == 0x7 && command == 0xB && millis() - lastDownPress >= cooldown && emergencyStop == false && currentSpeed - 50 >= - 255){
    currentSpeed = currentSpeed - 50;
    startCar(currentSpeed);
    lastDownPress = millis();
    
  } 
  
  if (address == 0x7 && command == 0x65 && angle < 180){
    angle += 45;
    steeringServo.write(angle);
  }

  if( address == 0x7 && command == 0x62 && angle > 0){
    angle -= 45;
    steeringServo.write(angle);
  }
  
  }
  checkDistance();
}

void checkDistance(){
digitalWrite(trigPin, LOW);
delayMicroseconds(2);

digitalWrite(trigPin, HIGH);
delayMicroseconds(10);
digitalWrite(trigPin, LOW);

long duration = pulseIn(echoPin, HIGH, 30000);

if (duration == 0){
  //Serial.println("No echo");

}else{
  float distance = duration * 0.043 / 2;

  Serial.print("Distance:  ");
  Serial.print(distance);
  Serial.println(" cm");

  if (distance < 20){
    emergencyStop = true;
    stopCar();
  }
}

}

void startCar(int value){
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3,HIGH);
  digitalWrite(IN4, LOW);

  
 // analogWrite(ENA,value);
 // analogWrite(ENB, value);
  Serial.print("car started at");
  Serial.println(value);

}

void stopCar(){
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3,LOW);
  digitalWrite(IN4, LOW);

  analogWrite(ENA,0);
  analogWrite(ENB,0);
  currentSpeed = 0;
  Serial.println("car Stopped");

}
