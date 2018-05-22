// testing git permissions ... continued ...

#include <Servo.h> 
#include <math.h>

Servo pan_motor;  // create servo object to control a servo 
Servo tilt_motor;

int potpin_pan = A0;  // analog pin used to connect the potentiometer
long val_pan;    // variable to read the value from the analog pin 

int potpin_tilt = A5;
long val_tilt;

int button_pin = 2;
int button_state = LOW;
bool button_pressed = false;

int degrees_buffer = 5;

void setup() 
{ 
  pan_motor.attach(A2);  // attaches the servo on pin 2 to the servo object 
  tilt_motor.attach(A3);

  pinMode(button_pin, INPUT);
  pinMode(potpin_pan, INPUT);
  pinMode(potpin_tilt, INPUT);
  pinMode(button_pin, INPUT_PULLUP);
  
  Serial.begin(9600);
} 

void loop() 
{ 
//  Serial.print("button: ");
//  Serial.println(button_pressed);
  
  if(!button_pressed) {
    // resets val_pan and val_tilt each iteration so it can't grow larger than 180
    val_pan = 0;
    val_tilt = 0;
    
    int avg_count = 20;
    
    for(int i = 0; i < avg_count; i++) {
      val_pan += analogRead(potpin_pan);  // value between 0 and 1023
    }
    
    val_pan /= avg_count;           
    val_pan = map(val_pan, 0, 1023, 20, 160);     // scale to use w/ servo (0-180)
    
    if(abs(val_pan - pan_motor.read()) > degrees_buffer) {
      Serial.print("pan val: ");
      Serial.println(val_pan);
      pan_motor.write(val_pan);                  // sets servo position according to the scaled value 
      delay(15);                           // waits for the servo to get there
    } 
  
    for(int i = 0; i < avg_count; i++) {
      val_tilt += analogRead(potpin_tilt);
    }
    
    val_tilt /= avg_count;
    val_tilt = map(val_tilt, 0, 1023, 20, 160);
    
    if(abs(val_tilt - tilt_motor.read()) > degrees_buffer) {
      Serial.print("tilt val: ");
      Serial.println(val_tilt);
      tilt_motor.write(val_tilt);
      delay(15);
    }
  }

  // if button is pressed
  else {
    val_tilt = random(20, 120);   // sets val_tilt to random num b/w 20-120
    val_pan = random(20, 120);    // sets val_pan to random num b/w 20-120

    if(abs(val_tilt - tilt_motor.read()) > degrees_buffer) {
      Serial.print("tilt val: ");
      Serial.println(val_tilt);
      tilt_motor.write(val_tilt);
      delay(15);
    }

    if(abs(val_pan - pan_motor.read()) > degrees_buffer) {
      Serial.print("pan val: ");
      Serial.println(val_pan);
      pan_motor.write(val_pan);                  // sets servo position according to the scaled value 
      delay(15);                           // waits for the servo to get there
      delay(2000);
    } 
  }
  
  button_pressed = checkButton();
} 

//void checkButton() {
//  static bool last_state = false;
//  button_state = digitalRead(button_pin);
//  if(button_state == HIGH && last_state != HIGH) {
//    if(button_pressed) { 
//      button_pressed = false; 
//    }
//    else { 
//      button_pressed = true; 
//    }
//  }
//  last_state = button_state;
//}

bool checkButton() {
  if(digitalRead(button_pin) == LOW) return true;   // if button is pressed
  else return false;                                // if button is not pressed
}

