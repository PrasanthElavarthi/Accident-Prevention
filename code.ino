/* This sketch is for Arduino mega. It uses various libraries such as TinyGPS,
    Adafruit Motor shield, Adafruit SSD1306, Adafruit GFX, RF24 which need to be installed
*/
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);
#include <SparkFun_ADXL345.h>         // SparkFun ADXL345 Library
ADXL345 adxl = ADXL345();             // USE FOR I2C COMMUNICATION

const int trigPinS = 27;
const int echoPinS = 26;

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(49, 53); // CE, CSN
const byte address[6] = "00001";
const byte addressa[6] = "00002";

signed long x ;
int count;
signed long t ;
unsigned long text ;
int sens;
#include<AFMotor.h>
AF_DCMotor motor2(2);
AF_DCMotor motor1(1);
AF_DCMotor motor3(3);
AF_DCMotor motor4(4);

#include <TinyGPS.h>
float la, lo, laa, loo;
TinyGPS gps; // create gps object

void setup() {
  // put your setup code here, to run once:
#include <TinyGPS.h>
  float la, lo;
  TinyGPS gps; // create gps object
  Serial.begin(9600);
  adxl.powerOn();                     // Power on the ADXL345

  adxl.setRangeSetting(16);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.display();
  //delay(100);
  display.clearDisplay();


  const int trigPinS = 27;
  const int echoPinS = 26;
  pinMode(trigPinS, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPinS, INPUT); // Sets the echoPin as an Input



  digitalWrite(53, HIGH);
  radio.begin();
  radio.openReadingPipe(0, address);

  radio.openWritingPipe(addressa);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
  t = 0;
  count = 0 ;

  motor2.setSpeed(200);
  motor1.setSpeed(200);
  motor3.setSpeed(200);
  motor4.setSpeed(200);
  motor1.run(FORWARD);
  motor2.run(FORWARD);
  motor3.run(FORWARD);
  motor4.run(FORWARD);
}

void dist() {
  // put your main code here, to run repeatedly:
  long durationS;
  int distanceS;
  digitalWrite(trigPinS, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPinS, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinS, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  durationS = pulseIn(echoPinS, HIGH);
  // Calculating the distance
  distanceS = durationS * 0.034 / 2;
  // Prints the distance on the Serial Monitor
  //Serial.print("DistanceS: ");
  Serial.println(distanceS);

  if (distanceS > 50 and distanceS < 100) {
    //analogWrite(6,50);

    motor2.setSpeed(100);
    motor1.setSpeed(100);
    motor3.setSpeed(100);
    motor4.setSpeed(100);
    motor1.run(FORWARD);
    motor2.run(FORWARD);
    motor3.run(FORWARD);
    motor4.run(FORWARD);


    display.setCursor(0, 1);
    display.println("Closing vehicle");

    display.setCursor(0, 10);
    display.println("Partial Braking");
    display.display();
    display.clearDisplay();


  }

  else {
    display.display();
    display.clearDisplay();
  }

  if ( distanceS<50 and distanceS > 5 ) {
    //analogWrite(6,0);
    motor1.run(RELEASE);
    motor2.run(RELEASE);
    motor3.run(RELEASE);
    motor4.run(RELEASE);
    //digitalWrite(2,HIGH);
    display.setCursor(0, 10);
    display.println("Brakes activated");
    display.display();
    display.clearDisplay();
  }
}

void accl() {

  // Accelerometer Readings
  int x, y, z, a, b;
  adxl.readAccel(&x, &y, &z);         // Read the accelerometer values and store them in variables declared above x,y,z
  Serial.print(x);
  Serial.print(", ");
  Serial.print(y);
  Serial.print(", ");
  Serial.println(z);

  a = x;

  if ( a > b and a - b > 10) {
    sens = sens + 1;
  }
  while ( sens > 0) {
    Serial.println("crash");
    display.setCursor(0, 10);
    display.println("Crash");
    display.display();
    display.clearDisplay();
    digitalWrite(A1, HIGH);



    motor1.run(RELEASE);
    motor2.run(RELEASE);
    motor3.run(RELEASE);
    motor4.run(RELEASE);
    gpsa();

  }
  b = a;

}

void gpsa() {

  while (Serial.available()) { // check for gps data
    if (gps.encode(Serial.read())) // encode gps data
    {
      gps.f_get_position(&la, &lo); // get latitude and longitude
      laa = la;
      loo = lo;
      Serial.print("Latitude: ");
      Serial.print(laa, 6);
      Serial.print("Longitude: ");
      Serial.println(loo, 6);
      const char text1[] = "Crash detected at location";

      radio.stopListening();
      //radio.write(&text1,sizeof(text1));
      //radio.startListening();
      //radio.stopListening();
      radio.write(&laa, sizeof(laa));
      radio.startListening();
      radio.stopListening();

      radio.write(&loo, sizeof(loo));
      radio.startListening();

    }
  }
}

void loop() {
  dist();
  accl();
}
