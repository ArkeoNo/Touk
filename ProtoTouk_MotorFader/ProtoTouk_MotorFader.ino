const int PIN_BUTTON_1 = 13;
const int PIN_BUTTON_2 = 12;

const int PIN_LED_RED = 11;
const int PIN_LED_GREEN = 10;

// Fader
const int PIN_FADER_D = A0;
const int PIN_FADER_G = A1;
const int FADER_MIDDLE_DEADZONE = 70;
// Fader Motor (FM)
const int PIN_FM_STBY = 6;
const int PIN_FM_AIN1 = 7;
const int PIN_FM_AIN2 = 8;
const int PIN_FM_PWMA = 9;
// const int PIN_FM_BIN1 = ;
// const int PIN_FM_BIN2 = ;
// const int PIN_FM_PWMB = ;
const int FM_MIN_SPEED    = 70;
const int FM_MAX_SPEED    = 255;
const int FM_BASE_SPEED   = 70;
const int FM_MIN_MOVE     = 20;
const int FM_END_DEADZONE = 60;
const int FM_MIN          = 0    - 512; // -512
const int FM_MAX          = 1023 - 512; //  511


void setup() {
  Serial.begin(9600);

  pinMode(PIN_BUTTON_1, INPUT);
  pinMode(PIN_BUTTON_2, INPUT);

  pinMode(PIN_LED_RED, OUTPUT);
  pinMode(PIN_LED_GREEN, OUTPUT);

  pinMode(PIN_FM_AIN1, OUTPUT);
  pinMode(PIN_FM_AIN2, OUTPUT);
  pinMode(PIN_FM_PWMA, OUTPUT);
  digitalWrite(PIN_FM_STBY, HIGH);

  // Wire.begin();
  // motor.init();
  // stopFaderMotor();

  int faderD = analogRead(PIN_FADER_D) - 512;
  if (faderD > -100 && faderD < 100) {
    // On essaie de bouger le moteur
    moveFaderMotor(1, 100);
    delay(300);
    stopFaderMotor();
  }
}

void loop() {
  // Fader Motor
  int faderD = analogRead(PIN_FADER_D) - 512;
  int speedD = calculateMotorSpeed(faderD);
  bool dirD = faderD > 0;
  
  // Potentiometer
  int faderG = analogRead(PIN_FADER_G) - 512;
  int speedG = calculateMotorSpeed(faderG);
  // bool dirG = faderG > 0;
  moveFaderMotorTowards(faderG);


  bool button1 = digitalRead(PIN_BUTTON_1);
  bool button2 = digitalRead(PIN_BUTTON_2);
  
  // Intensité des leds en fonction de la vitesse des faders
  analogWrite(PIN_LED_RED, speedD);
  analogWrite(PIN_LED_GREEN, speedG);

  // Intensité des leds en fonction des boutons
  // digitalWrite(PIN_LED_RED, button1);
  // digitalWrite(PIN_LED_GREEN, button2);
  

  // Debug
  Serial.print("faderD:" + String(faderD) + " faderG:" + String(faderG)
                 + " speedD:" + String(speedD)
                 + " dirD:" + String((dirD * 2 - 1) * 400)
                 + " m:-530 M:530");
  // Serial.println("faderD:" + String(faderD) + " faderG:" + String(faderG)
  //                + " speedD:" + String(speedD) + " speedG:" + String(speedG)
  //                + " dirD:" + String((dirD * 2 - 1) * 400) + " dirF:" + String((dirG * 2 - 1) * 400)
  //                + " m:-530 M:530");
}

// Convertit une valeur de fader (-512, 511) en valeur de vitesse (0, 255)
int calculateMotorSpeed(int faderValue) {
  int faderSpeed = map(abs(faderValue), 0, 512, 0, 255);
  if (faderSpeed < FADER_MIDDLE_DEADZONE) faderSpeed = 0; // Deadzone
  return faderSpeed;
}

// Fait avancer le moteur de fader (fm) vers la position donnée
// faderPos doit être compris entre -512 et 511
void moveFaderMotorTowards(int faderPos) {
  faderPos = constrain(faderPos, FM_MIN + FM_END_DEADZONE, FM_MAX - FM_END_DEADZONE);
  int oldFaderPos = analogRead(PIN_FADER_D) - 512;
  int x = 0;
  int speed = 0;

  if (abs(faderPos - oldFaderPos) > FM_MIN_MOVE) {
    speed = constrain(
      map(
        abs(faderPos - oldFaderPos) / 2,
        0, 1024 / 2,
        FM_MIN_SPEED, FM_MAX_SPEED
      ),
      FM_MIN_SPEED,
      FM_MAX_SPEED
    );
    int direction = faderPos > oldFaderPos;
    moveFaderMotor(direction, FM_BASE_SPEED);
  } else {
    stopFaderMotor();
  }
  Serial.println(" faderPos:" + String(faderPos) + " oldFaderPos:" + String(oldFaderPos) + " x:" + String(x) + " FMSpeed:" + String(speed));

  delay(10);
}

// Fait avancer le moteur de fader (fm)
// fmSpeed doit être compris entre 0 et 255
void moveFaderMotor(bool direction, int fmSpeed) {
  fmSpeed = constrain(fmSpeed, 0, 255);
  if (fmSpeed >= FM_MIN_SPEED) {
    if (direction) {
      digitalWrite(PIN_FM_AIN1, HIGH);
      digitalWrite(PIN_FM_AIN2, LOW);
    } else {
      digitalWrite(PIN_FM_AIN1, LOW);
      digitalWrite(PIN_FM_AIN2, HIGH);
    }
    analogWrite(PIN_FM_PWMA, fmSpeed);
  } else {
    stopFaderMotor();
  }
}

void stopFaderMotor() {
  analogWrite(PIN_FM_PWMA, 0);
  digitalWrite(PIN_FM_AIN1, HIGH);
  digitalWrite(PIN_FM_AIN2, HIGH);
}
