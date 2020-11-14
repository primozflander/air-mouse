// #include <BleMouse.h>

// BleMouse bleMouse;
// #define BUILTIN_LED  2

// void setup() {
//   Serial.begin(115200);
//   Serial.println("Starting BLE work!");
//   bleMouse.begin();
//   pinMode(BUILTIN_LED, OUTPUT);
// }

// void loop() {
//   if(bleMouse.isConnected()) {
//     digitalWrite(BUILTIN_LED,HIGH);
//     unsigned long startTime;

//     Serial.println("Move mouse pointer up");
//     startTime = millis();
//     while(millis()<startTime+2000) {
//       bleMouse.move(0,-1);
//       delay(100);
//     }
//     delay(500);

//     Serial.println("Move mouse pointer down");
//     startTime = millis();
//     while(millis()<startTime+2000) {
//       bleMouse.move(0,1);
//       delay(100);
//     }
//     delay(500);

//     Serial.println("Move mouse pointer left");
//     startTime = millis();
//     while(millis()<startTime+2000) {
//       bleMouse.move(-1,0);
//       delay(100);
//     }
//     delay(500);

//     Serial.println("Move mouse pointer right");
//     startTime = millis();
//     while(millis()<startTime+2000) {
//       bleMouse.move(1,0);
//       delay(100);
//     }
//     delay(500);
    
//     bleMouse.click(MOUSE_RIGHT);
//     delay(500);
//   }
//   else digitalWrite(BUILTIN_LED,LOW);
// }


//air_mouse_with_click
#include <Wire.h>
//#include <I2Cdev.h>
#include <MPU6050.h>
//#include <SPI.h>
//#include <BLEHIDPeripheral.h>
#include <BleMouse.h>

BleMouse bleMouse;
MPU6050 mpu;
int16_t ax, ay, az, gx, gy, gz;
int vx, vy, vx_prec, vy_prec;
int count=0;

void setup() {
  Serial.begin(9600);
  bleMouse.begin();
  Wire.begin();
  mpu.initialize();
  if (!mpu.testConnection()) {
    Serial.println("Check MPU connection!");
    while (1);
    }
}

void loop() {
  if(bleMouse.isConnected()) {
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    vx = (gx+300)/200;  
    vy = -(gy-100)/200;
    Serial.println(String(vx) + " " + String(vy));
    
    vx = (abs(vx) > 5) ? vx : 0;
    vy = (abs(vy) > 5) ? vy : 0;

    bleMouse.move(vx, vy);
    delay(50);
  }
}

void autoClick()  {
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