#include <TinyMPU6050.h>
#include <BleMouse.h>
#include "src/Led.h"

//add battery measurement
class Button {
  private:
    byte pin;
    byte state;
    byte lastReading;
    unsigned long lastDebounceTime = 0;
    unsigned long debounceDelay = 50;
  public:
    Button(byte pin) {
      this->pin = pin;
      lastReading = LOW;
      init();
    }
    void init() {
      pinMode(pin, INPUT_PULLUP);
      update();
    }
    void update() {
      byte newReading = !digitalRead(pin);
      if (newReading != lastReading) {
        lastDebounceTime = millis();
      }
      if (millis() - lastDebounceTime > debounceDelay) {
        state = newReading;
      }
      lastReading = newReading;
    }
    byte getState() {
      update();
      return state;
    }
    bool isPressed() {
      return (getState() == HIGH);
    }
};
class Smoothing {
  private:
    #define numAvgReadings 50
    float avgValue = 0, sum = 0, filteredValue = 0, lastValue = 0, filterPar = 0.95;
    float readings[numAvgReadings];
    int readIndex = 0;

    void calculateAccelMovingAverage(float value) {
      sum = sum - readings[readIndex];
      readings[readIndex] = value; 
      sum = sum + readings[readIndex]; 
      avgValue = sum / numAvgReadings;
      readIndex = (readIndex + 1) % numAvgReadings; 
    }

    void calculateFilteredValue(float value) {
      filteredValue = lastValue * filterPar + value * (1 - filterPar);
      lastValue = filteredValue;
    }

  public:
    Smoothing() {
      init();
    }

    void init() {
      for (int i = 0; i < numAvgReadings; i++) readings[i] = 0;
    }

    void add(float value) {
        calculateAccelMovingAverage(value);
        calculateFilteredValue(value);
    }

    float getMovingAverage()  {
      return avgValue;
    }

    float getFilteredValue()  {
      return filteredValue;
    }
};
//BleMouse bleMouse;
BleMouse bleMouse("Air mouse");
Button leftClick(15);
Button rightClick(0);
Smoothing X;
Smoothing Y;
Led onboardLed(5);
MPU6050 mpu (Wire);
int16_t ax, ay, az, gx, gy, gz;
int x, int y;

void setup() {
  Serial.begin(9600);
  bleMouse.begin();
  mpu.Initialize();
}

void loop() {
  //Serial.println("Waiting for connection..");
  //delay(1000);
  onboardLed.blinkMs(500);
  mpu.Execute();
  X.add(mpu.GetGyroZ());
  Y.add(mpu.GetGyroY());
  // Serial.print("AngX = ");
  // Serial.print(mpu.GetAngX());
  // Serial.print(" AngY = ");
  // Serial.print(mpu.GetAngY());
  // Serial.print(" AngZ = ");
  // Serial.print(mpu.GetAngZ());
  // Serial.print(" GyroX = ");
  // Serial.print(mpu.GetGyroX());
  // Serial.print(" GyroY = ");
  // Serial.print(mpu.GetGyroY());
  // Serial.print(" GyroZ = ");
  // Serial.print(mpu.GetGyroZ());
  // Serial.print(" leftCLick: ");
  // Serial.print(leftClick.isPressed());
  // Serial.print(" rightCLick: ");
  // Serial.println(rightClick.isPressed());
  if(bleMouse.isConnected()) {
    // vx = mpu.GetAccZ();  
    // vy = mpu.GetAccY();
    x = X.getMovingAverage()/10;  
    y = -Y.getMovingAverage()/10;
    Serial.println(String(vx) + " " + String(vy));
    
    x = (abs(x) > 5) ? x : 0;
    y = (abs(y) > 5) ? y : 0;

    bleMouse.move(x, y);
    if(leftClick.isPressed()) {
      bleMouse.press(MOUSE_LEFT);
    } 
    else
    {
      bleMouse.release(MOUSE_LEFT);
    }
    if(rightClick.isPressed()) {
      bleMouse.press(MOUSE_RIGHT);
    } 
    else
    {
      bleMouse.release(MOUSE_RIGHT);
    }
    //delay(1);
  }
}

void autoClick(int vx, int vy)  {
  static int vx_prec, vy_prec;
  static int count = 0;
  if ( (vx_prec-5)<=vx && vx<=vx_prec+5 && (vy_prec-5)<=vy && vy<=vy_prec+5) { // for checking the cursor doesn't move too much from its actual position: (in this case a 10 pixel square)
    count++;                                                                  
    if(count == 100){ // the click will happen after 2 seconds the pointer has stopped in the 10px square
      if (!bleMouse.isPressed(MOUSE_LEFT)) {
        bleMouse.press(MOUSE_LEFT);
        count = 0;
      }
    }
    else {
      if (bleMouse.isPressed(MOUSE_LEFT)) {
        bleMouse.release(MOUSE_LEFT);
      }
    }
  }
  else {
    vx_prec = vx; // updating values to check the position of the pointer and allow the click
    vy_prec = vy;
    count = 0;
  }
}
