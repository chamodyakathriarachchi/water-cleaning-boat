#include <Servo.h>


#define BOAT_ENA 5
#define BOAT_IN1 22
#define BOAT_IN2 23

#define BOAT_ENB 6
#define BOAT_IN3 24
#define BOAT_IN4 25


#define CONV_ENA 9
#define CONV_IN1 26
#define CONV_IN2 27

#define CONV_ENB 10
#define CONV_IN3 28
#define CONV_IN4 29


#define TRIG_PIN 2
#define ECHO_PIN 3


#define SERVO_PIN 4

Servo scanServo;


#define IR_PIN 30


#define VOLTAGE_PIN A0

float voltageRatio = 5.0;

float calibration = 1.00;


int boatSpeed = 110;
int turnSpeed = 100;
int conveyorSpeed = 120;


int obstacleDistance = 45;


const unsigned long CONVEYOR_TIME = 000;

bool conveyorRunning = false;

unsigned long conveyorStartTime = 0;


float LOW_BATTERY_VOLTAGE = 10.0;


void setup()
{
  Serial.begin(9600);

  pinMode(BOAT_ENA, OUTPUT);
  pinMode(BOAT_IN1, OUTPUT);
  pinMode(BOAT_IN2, OUTPUT);

  pinMode(BOAT_ENB, OUTPUT);
  pinMode(BOAT_IN3, OUTPUT);
  pinMode(BOAT_IN4, OUTPUT);


  pinMode(CONV_ENA, OUTPUT);
  pinMode(CONV_IN1, OUTPUT);
  pinMode(CONV_IN2, OUTPUT);

  pinMode(CONV_ENB, OUTPUT);
  pinMode(CONV_IN3, OUTPUT);
  pinMode(CONV_IN4, OUTPUT);


  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);


  pinMode(IR_PIN, INPUT);


  scanServo.attach(SERVO_PIN);
  scanServo.write(90);


  stopBoat();
  stopConveyor();


  Serial.println("==============================");
  Serial.println(" AUTONOMOUS TRASH BOAT");
  Serial.println("==============================");

  Serial.println("System started");
  Serial.println("Boat will move forward");
  Serial.println("IR controls conveyor only");
  Serial.println("Conveyor runs for 7 seconds");
  Serial.println("==============================");

  delay(1000);
}

void loop()
{

  float batteryVoltage = readBatteryVoltage();
  int batteryPercentage = calculateBatteryPercentage(batteryVoltage);

  Serial.print("Battery: ");
  Serial.print(batteryVoltage);
  Serial.print(" V | ");

  Serial.print(batteryPercentage);
  Serial.println("%");


  if (batteryVoltage <= LOW_BATTERY_VOLTAGE)
  {
    Serial.println("LOW BATTERY!");
    Serial.println("Stopping boat and conveyor.");

    stopBoat();
    stopConveyor();

    conveyorRunning = false;

    delay(1000);

    return;
  }

  if (conveyorRunning)
  {
    if (millis() - conveyorStartTime >= CONVEYOR_TIME)
    {
      stopConveyor();

      conveyorRunning = false;

      Serial.println("------------------------------");
      Serial.println("9 seconds completed.");
      Serial.println("Conveyor stopped.");
      Serial.println("Boat continues moving.");
      Serial.println("------------------------------");
    }
  }

  int irState = digitalRead(IR_PIN);



  if (irState == LOW && !conveyorRunning)
  {
    Serial.println("------------------------------");
    Serial.println("TRASH DETECTED!");
    Serial.println("Boat CONTINUES MOVING");
    Serial.println("Conveyor ON for 9 seconds");
    Serial.println("------------------------------");

    startConveyor();

    conveyorRunning = true;

    conveyorStartTime = millis();
  }


  long distance = getDistance();

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");


  if (distance <= 0)
  {
    Serial.println("Invalid ultrasonic reading.");

    moveForward();

    delay(100);

    return;
  }



  if (distance <= obstacleDistance)
  {
    Serial.println("==============================");
    Serial.println("OBSTACLE DETECTED!");
    Serial.println("==============================");

    stopBoat();

    delay(300);

    scanServo.write(150);

    delay(500);

    long leftDistance = getDistance();

    Serial.print("Left distance: ");
    Serial.print(leftDistance);
    Serial.println(" cm");

    scanServo.write(30);

    delay(500);

    long rightDistance = getDistance();

    Serial.print("Right distance: ");
    Serial.print(rightDistance);
    Serial.println(" cm");

    scanServo.write(90);

    delay(300);


    if (leftDistance > obstacleDistance &&
        leftDistance > rightDistance)
    {
      Serial.println("Turning LEFT");

      turnLeft();

      delay(700);
    }

    else if (rightDistance > obstacleDistance &&
             rightDistance > leftDistance)
    {
      Serial.println("Turning RIGHT");

      turnRight();

      delay(700);
    }

    else
    {
      Serial.println("Both directions blocked.");
      Serial.println("Reversing.");

      reverseBoat();

      delay(800);

      turnRight();

      delay(800);
    }

    moveForward();
  }


  else
  {
    moveForward();
  }


  delay(100);
}


void moveForward()
{
  digitalWrite(BOAT_IN1, HIGH);
  digitalWrite(BOAT_IN2, LOW);

  digitalWrite(BOAT_IN3, HIGH);
  digitalWrite(BOAT_IN4, LOW);

  analogWrite(BOAT_ENA, boatSpeed);
  analogWrite(BOAT_ENB, boatSpeed);
}


void reverseBoat()
{
  digitalWrite(BOAT_IN1, LOW);
  digitalWrite(BOAT_IN2, HIGH);

  digitalWrite(BOAT_IN3, LOW);
  digitalWrite(BOAT_IN4, HIGH);

  analogWrite(BOAT_ENA, turnSpeed);
  analogWrite(BOAT_ENB, turnSpeed);
}


void turnLeft()
{
  digitalWrite(BOAT_IN1, LOW);
  digitalWrite(BOAT_IN2, HIGH);

  digitalWrite(BOAT_IN3, HIGH);
  digitalWrite(BOAT_IN4, LOW);

  analogWrite(BOAT_ENA, turnSpeed);
  analogWrite(BOAT_ENB, turnSpeed);
}


void turnRight()
{
  digitalWrite(BOAT_IN1, HIGH);
  digitalWrite(BOAT_IN2, LOW);

  digitalWrite(BOAT_IN3, LOW);
  digitalWrite(BOAT_IN4, HIGH);

  analogWrite(BOAT_ENA, turnSpeed);
  analogWrite(BOAT_ENB, turnSpeed);
}


void stopBoat()
{
  digitalWrite(BOAT_IN1, LOW);
  digitalWrite(BOAT_IN2, LOW);

  digitalWrite(BOAT_IN3, LOW);
  digitalWrite(BOAT_IN4, LOW);

  analogWrite(BOAT_ENA, 0);
  analogWrite(BOAT_ENB, 0);
}


void startConveyor()
{
  digitalWrite(CONV_IN1, HIGH);
  digitalWrite(CONV_IN2, LOW);

  digitalWrite(CONV_IN3, HIGH);
  digitalWrite(CONV_IN4, LOW);

  analogWrite(CONV_ENA, conveyorSpeed);
  analogWrite(CONV_ENB, conveyorSpeed);
}

void stopConveyor()
{
  digitalWrite(CONV_IN1, LOW);
  digitalWrite(CONV_IN2, LOW);

  digitalWrite(CONV_IN3, LOW);
  digitalWrite(CONV_IN4, LOW);

  analogWrite(CONV_ENA, 0);
  analogWrite(CONV_ENB, 0);
}


long getDistance()
{
  digitalWrite(TRIG_PIN, LOW);

  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);

  delayMicroseconds(10);

  digitalWrite(TRIG_PIN, LOW);


  long duration = pulseIn(ECHO_PIN, HIGH, 30000);


  if (duration == 0)
  {
    return -1;
  }


  long distance = duration * 0.034 / 2;

  return distance;
}


float readBatteryVoltage()
{
  long total = 0;

  const int samples = 10;


  for (int i = 0; i < samples; i++)
  {
    total += analogRead(VOLTAGE_PIN);

    delay(2);
  }


  float average = total / (float)samples;


  float sensorVoltage =
    average * 5.0 / 1023.0;


  float batteryVoltage =
    sensorVoltage * voltageRatio;


  batteryVoltage *= calibration;


  return batteryVoltage;
}


int calculateBatteryPercentage(float voltage)
{
  if (voltage >= 12.6)
    return 100;

  else if (voltage >= 12.4)
    return 90;

  else if (voltage >= 12.2)
    return 70;

  else if (voltage >= 12.0)
    return 50;

  else if (voltage >= 11.7)
    return 30;

  else if (voltage >= 11.4)
    return 10;

  else
    return 0;
}