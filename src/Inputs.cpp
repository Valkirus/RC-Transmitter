#include "Inputs.h"

// Define global variables
int potAVal = 0;
int potBVal = 0;
unsigned long potTimer = 0; 
unsigned long potInterval = 50; 

unsigned long gyroTimer = 0;
Vector3 gad;
MPU6050 mpu(Wire);

volatile int count = 0;
int protectedCount = 0;
int previousCount = 0;
bool switchPressed = false;
const int DEBOUNCE_TIME = 1;
unsigned long timeSinceReleased = 0;

const byte encoderPinA = 3; // outputA digital pin2
const byte encoderPinB = 2; // outputB digital pin3

// Define functions
int getPotValue(IOLabels label) {
    if (millis() - potTimer > potInterval) {
        potAVal = map(analogRead(PotA_Pin), 0, 1023, 100, 0);
        potBVal = map(analogRead(PotB_Pin), 0, 1023, 100, 0);
        potTimer = millis();
    }

    if (label == 0) return potAVal;
    if (label == 1) return potBVal;
    else return 0;
}

int getButtonValue(IOLabels label) {
    switch (label) {
        case A: return digitalRead(ButtonA_Pin);
        case B: return digitalRead(ButtonB_Pin);
        case C: return digitalRead(ButtonC_Pin);
        case D: return digitalRead(ButtonD_Pin);
        default: return 0;
    }
}

String getButtonsString() {
    uint8_t valueA = getButtonValue(A);
    uint8_t valueB = getButtonValue(B);
    uint8_t valueC = getButtonValue(C);
    uint8_t valueD = getButtonValue(D);

    String result = "";

    if (valueA == PRESSED) result += "A";
    if (valueB == PRESSED) result += "B";
    if (valueC == PRESSED) result += "C";
    if (valueD == PRESSED) result += "D";

    return "Btns: " + result;
}

int getBumperValue(IOLabels label) {
    switch (label) {
        case A: return digitalRead(BumperA_Pin);
        case B: return digitalRead(BumperB_Pin);
        case C: return digitalRead(BumperC_Pin);
        case D: return digitalRead(BumperD_Pin);
        default: return 0;
    }
}

String getBumpersString() {
    uint8_t valueA = getBumperValue(A);
    uint8_t valueB = getBumperValue(B);
    uint8_t valueC = getBumperValue(C);
    uint8_t valueD = getBumperValue(D);

    String result = "";

    if (valueA == PRESSED) result += "A";
    if (valueB == PRESSED) result += "B";
    if (valueC == PRESSED) result += "C";
    if (valueD == PRESSED) result += "D";

    return "Bumps: " + result;
}

int getSwitchValue(IOLabels label) {
    switch (label) {
        case A: return digitalRead(SwitchA_Pin);
        case B: return digitalRead(SwitchB_Pin);
        case C: return digitalRead(SwitchC_Pin);
        case D: return digitalRead(SwitchD_Pin);
        default: return 0;
    }
}

String getSwitchesString() {
    uint8_t valueA = getSwitchValue(A);
    uint8_t valueB = getSwitchValue(B);
    uint8_t valueC = getSwitchValue(C);
    uint8_t valueD = getSwitchValue(D);

    String result = "";

    if (valueA == ON) result += "A";
    if (valueB == ON) result += "B";
    if (valueC == ON) result += "C";
    if (valueD == ON) result += "D";

    return "Switches: " + result;
}

int getJoyButtonValue(IOLabels label) {
    switch (label) {
        case A: return digitalRead(JoyLeftButton_Pin);
        case B: return digitalRead(JoyRightButton_Pin);
        default: return 0;
    }
}

String getJoyButtonsString() {
    uint8_t valueA = getJoyButtonValue(A);
    uint8_t valueB = getJoyButtonValue(B);

    String result = "";

    if (valueA == PRESSED) result += "A";
    if (valueB == PRESSED) result += "B";

    return "Joys: " + result;
}

Vector2 getJoyValue(IOLabels label){
  switch(label){
    case A: return Vector2(map(analogRead(A6), 0, 1023, 254, 0), map(analogRead(A7), 0, 1023, 0, 254));
    case B: return Vector2(map(analogRead(A2), 0, 1023, 0, 254), map(analogRead(A3), 0, 1023, 0, 254));
    default: return Vector2(0,0);
  }
}

String getJoyValueString(IOLabels label){
  if(label == A){
    Vector2 leftJoyValue = getJoyValue(A);
    return ("JL X: " + String(leftJoyValue.x) + " Y: " + String(leftJoyValue.y));
  }

  if(label == B){
    Vector2 rightJoyValue = getJoyValue(B);
    return ("JL X: " + String(rightJoyValue.x) + " Y: " + String(rightJoyValue.y));
  }

  return "";
}

float filteredX = 0;
float filteredY = 0;
float filteredZ = 0;

// Smoothing factor (between 0 and 1); closer to 1 means smoother but slower to respond
const float alpha = 0.9;

Vector3 readGyro() {  
    mpu.update();
    
    // Apply the low-pass filter to each angle
    filteredX = alpha * filteredX + (1 - alpha) * mpu.getAngleX();
    filteredY = alpha * filteredY + (1 - alpha) * mpu.getAngleY();
    filteredZ = alpha * filteredZ + (1 - alpha) * mpu.getAngleZ();
    
    // Store the filtered values in the gad struct
    gad.x = filteredX;
    gad.y = filteredY;
    gad.z = filteredZ;
    
    return gad;
}
String getGyroString() {
    gad = readGyro();
    return "GX: " + String(gad.x) + " GY: " + String(gad.y) + " GZ: " + String(gad.z);
}

void isrA() {
    if (digitalRead(encoderPinB) != digitalRead(encoderPinA)) {
        count++;
    } else {
        count--;
    }
    delay(5);
}

void isrB() {
    if (digitalRead(encoderPinA) == digitalRead(encoderPinB)) {
        count++;
    } else {
        count--;
    }
    delay(5);
}

int getRotaryEncoderSpins() {
    noInterrupts();
    protectedCount = count / 4;
    interrupts();
    int spins = protectedCount - previousCount;
    previousCount = protectedCount;
    return spins;
}

int getRotaryEncoderTotalSpins() {
    return count/4;
}

int getRotaryEncoderSwitchValue() {
    uint8_t switchValue = digitalRead(RotaryEncoderButton_Pin);
    if (switchValue == PRESSED) return PRESSED;
    return UNPRESSED;

    if (switchValue == PRESSED && !switchPressed) {
        if (millis() - timeSinceReleased > DEBOUNCE_TIME) {
            if (false) Serial.println("Switch Pressed!");
            switchPressed = true;
            return PRESSED;
        }
    }
    if (switchValue == UNPRESSED && switchPressed) {
        if (false) Serial.println("Switch Released.");
        switchPressed = false;
        timeSinceReleased = millis();
        return PRESSED;
    }

    return UNPRESSED;
}

String getRotaryEncoderSwitchString() {
    int rotaryEncoderState = getRotaryEncoderSwitchValue();

    String rotaryPressed = "";
    if (rotaryEncoderState == PRESSED) {
        rotaryPressed = "Pressed";
    }

    if (rotaryEncoderState == UNPRESSED) {
        rotaryPressed = "Unpressed";
    }

    return rotaryPressed;
}

void setupInputs() {
    // Buttons
    pinMode(ButtonA_Pin, INPUT_PULLUP);
    pinMode(ButtonB_Pin, INPUT_PULLUP);
    pinMode(ButtonC_Pin, INPUT_PULLUP);
    pinMode(ButtonD_Pin, INPUT_PULLUP);

    // Bumpers
    pinMode(BumperA_Pin, INPUT_PULLUP);
    pinMode(BumperB_Pin, INPUT_PULLUP);
    pinMode(BumperC_Pin, INPUT_PULLUP);
    pinMode(BumperD_Pin, INPUT_PULLUP);

    // Switches
    pinMode(SwitchA_Pin, INPUT_PULLUP);
    pinMode(SwitchB_Pin, INPUT_PULLUP);
    pinMode(SwitchC_Pin, INPUT_PULLUP);
    pinMode(SwitchD_Pin, INPUT_PULLUP);

    // Joystick Buttons
    pinMode(JoyLeftButton_Pin, INPUT_PULLUP);
    pinMode(JoyRightButton_Pin, INPUT_PULLUP);

    // Gyro
    byte status = mpu.begin();
    while (status != 0) { }  
    mpu.calcOffsets(); 

    // Rotary Encoder
    pinMode(RotaryEncoderButton_Pin, INPUT_PULLUP);
    pinMode(encoderPinA, INPUT);
    pinMode(encoderPinB, INPUT);

    attachInterrupt(digitalPinToInterrupt(encoderPinA), isrA, CHANGE);
    attachInterrupt(digitalPinToInterrupt(encoderPinB), isrB, CHANGE);
}
