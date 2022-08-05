#include <SoftwareSerial.h>
SoftwareSerial sim(10, 11); //SIM800L Tx & Rx is connected to Arduino #10 & #11



String number1 = "+94xxxxxxxx"; // +94 is the country code & add Emergency Department number

int sensor = 8; // PIR is connected with the digital pin 6 of the Arduino
const int LED1 = 5;
const int LED2 = 6;
String Message;
String cmd = "";
int faultSent1 = 0;
int faultSent2 = 0;
int sensorPin = A0;    // select the input pin for the potentiometer
int sensorValue = 0;  // variable to store the value coming from the sensor
void setup() {
  Serial.begin(9600);
  Serial.println("Wait few seconds...");
  delay(5000);
  Serial.println("System Started..");
  sim.begin(9600);
  delay(1000);
  pinMode(sensor, INPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  digitalWrite(LED1, HIGH);
  digitalWrite(LED2, HIGH);
  delay(1000);
  sim.println("AT");                 // Sends an ATTENTION command, reply should be OK
  sim.println("AT+CMGF=1");          // Configuration for sending SMS
  sim.println("AT+CNMI=2,2,0,0,0");// Configuration for receiving SMS
}


void loop() {

  sensorValue = analogRead(sensorPin);
  sensorValue = map(sensorValue, 0, 1023, 500, 3000);
  Serial.println(sensorValue);
  int buttonState = digitalRead(sensor);
  if ((buttonState  == 1) && (faultSent1  == 0)) {
    SendMessage2();
  } else if ((buttonState  == 0) && (faultSent1  == 1)) {

    SendMessage1();
  }
  if (sim.available() > 0) {

    Message = sim.readString();
    Serial.print(Message);
    delay(10);

    if ((buttonState  == 0) && (faultSent2  == 0)) {
      if (Message.indexOf("Gen on") >= 0) {
        digitalWrite(LED1, LOW);
        delay(sensorValue);
        digitalWrite(LED1, HIGH);
        Message = "";
        faultSent2 = 1;
        DeletMessage();
      }
    } else if (buttonState  == 0) {
      if (Message.indexOf("Gen off") >= 0) {
        digitalWrite(LED2, LOW);
        delay(15000);
        Message = "";
        faultSent2 = 0;
        DeletMessage();
      }
    }
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, HIGH);

  }

}

void SendMessage1()
{
  sim.println("AT+CMGF=1");
  delay(1000);
  sim.println("AT+CMGS=\"" + number1 + "\"\r");
  delay(1000);
  String SMS = "CEB POWER CUTOFF";
  sim.println(SMS);
  delay(100);
  sim.println((char)26);
  delay(1000);
  faultSent1 = 0;
}

void SendMessage2()
{
  sim.println("AT+CMGF=1");
  delay(1000);
  sim.println("AT+CMGS=\"" + number1 + "\"\r");
  delay(1000);
  String SMS = "CEB POWER IN";
  sim.println(SMS);
  delay(100);
  sim.println((char)26);
  delay(1000);
  faultSent1 = 1;
}



void DeletMessage()
{
  //delete all sms
  sim.println("AT+CMGD=1,4");
  delay(1000);
  sim.println("AT+CMGDA= \"DEL ALL\"");
  delay(1000);
}
