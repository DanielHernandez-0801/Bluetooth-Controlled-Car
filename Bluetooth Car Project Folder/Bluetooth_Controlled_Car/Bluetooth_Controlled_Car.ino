// Bluetooth Controlled Car

int motor_left[] = {10, 5};
int motor_right[] = {9, 6};
#define ENABLE 7

#define green 8
#define red 12
#define trigPinFront 4
#define FrontEcho 3

char command = 0;
bool Switch = false;
//PWM - To slow down left and right turns
int PWMSpeed = 125;
const int range = 5;

void setup() {
  //Configure internal pullup on the RX pin
  pinMode(0, INPUT_PULLUP);
  Serial.begin(9600); // Default connection rate for bluetooth module

  // LEDs and Sensor
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(trigPinFront, OUTPUT);
  pinMode(FrontEcho, INPUT);

  // Setup motors
  int i;
  for (i = 0; i < 2; i++) {
    pinMode(motor_left[i], OUTPUT);
    pinMode(motor_right[i], OUTPUT);
  }
  pinMode(ENABLE, OUTPUT);
  digitalWrite(ENABLE, HIGH);

  digitalWrite(red, LOW);
  digitalWrite(green, LOW);
  motor_stop(); //initially motors are off
}

void loop() {

  if (Switch) {
    if (TooClose()) {
      digitalWrite(red, HIGH);
      digitalWrite(green, LOW);
    } else {
      digitalWrite(red, LOW);
      digitalWrite(green, HIGH);
    }
  }

  if (Serial.available() > 0)
  {
    command = Serial.read();

    if (command == '1')
    {
      Switch = !Switch;
      if (Switch) {
        digitalWrite(red, LOW);
        digitalWrite(green, HIGH);
      } else {
        digitalWrite(red, LOW);
        digitalWrite(green, LOW);
      }
    }

    if (TooClose() && Switch) {
      digitalWrite(red, HIGH);
      digitalWrite(green, LOW);
      motor_stop();
      delay(50);
      while (TooClose()) {
        if (Serial.available() > 0) {
          command = Serial.read();
        }
        if (command == '1') {
          Switch = !Switch;
          digitalWrite(red, LOW);
          digitalWrite(green, LOW);
          break;
        }
        MoveWithCaution();
      }
    } else if (Switch) {
      digitalWrite(red, LOW);
      digitalWrite(green, HIGH);
      Move();
    }
  }
}

//Motor Controls
void motor_stop() {
  Serial.print("Motor :  STOP\n");
  digitalWrite(motor_left[0], LOW);
  digitalWrite(motor_left[1], LOW);

  digitalWrite(motor_right[0], LOW);
  digitalWrite(motor_right[1], LOW);
  delay(25);
}

void turn_right() { //right
  //Slow down speed by using PWM
  Serial.print("Motor :  LEFT\n");
  digitalWrite(motor_left[0], LOW);
  digitalWrite(motor_left[1], HIGH);

  digitalWrite(motor_right[0], LOW);
  digitalWrite(motor_right[1], HIGH);

}

void turn_left() { //left
  //Slow down speed by using PWM
  //Slow down speed by using PWM
  Serial.print("Motor :  RIGHT\n");
  digitalWrite(motor_left[0], HIGH);
  digitalWrite(motor_left[1], LOW);

  digitalWrite(motor_right[0], HIGH);
  digitalWrite(motor_right[1], LOW);

}

void drive_forward() { //forward
  Serial.print("Motor :  FORWARD\n");
  digitalWrite(motor_left[0], LOW);
  digitalWrite(motor_left[1], HIGH);

  digitalWrite(motor_right[0], HIGH);
  digitalWrite(motor_right[1], LOW);
}

void drive_backward() { //backward
  Serial.print("Motor :  BACK\n");
  digitalWrite(motor_left[0], HIGH);
  digitalWrite(motor_left[1], LOW);

  digitalWrite(motor_right[0], LOW);
  digitalWrite(motor_right[1], HIGH);
}

void Move() {
  if (command == '2') {
    drive_forward();
  } else if (command == '3') {
    drive_backward();
  } else if (command == '5') {
    turn_right();
  } else if (command == '4') {
    turn_left();
  } else if (command == '9') {
    motor_stop();
  } else {
    motor_stop();
  }
}

void MoveWithCaution() {
  if (command == '2') {
    motor_stop();
  } else if (command == '3') {
    drive_backward();
  } else if (command == '5') {
    turn_right();
  } else if (command == '4') {
    turn_left();
  } else if (command == '9') {
    motor_stop();
  } else {
    motor_stop();
  }
}

bool TooClose() {
  int inches = CheckDistanceFront();

  if (inches < range) {
    digitalWrite(red, HIGH);
    digitalWrite(green, LOW);
    return true;
  }
  digitalWrite(red, LOW);
  digitalWrite(green, HIGH);
  return false;
}

long CheckDistanceFront() {
  digitalWrite(trigPinFront, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPinFront, HIGH);
  delayMicroseconds(5);
  digitalWrite(trigPinFront, LOW);

  long Duration = pulseIn(FrontEcho, HIGH);
  long inches = microsecondsToInches(Duration);

  return inches;
}

long microsecondsToInches(long microseconds) {
  return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds) {
  return microseconds / 29 / 2;
}
