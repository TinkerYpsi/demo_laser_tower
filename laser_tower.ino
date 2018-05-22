#include <Servo.h> 
#include <math.h>

typedef enum _mode
{
  AUTO,
  MANUAL
}MODE;

Servo pan_motor;  // create servo object to control a servo 
Servo tilt_motor;

const int PAN  = A0;  // Analog pin connected to pan potentiometer
const int TILT = A5;  // Analog pin connected to tilt potentiometer
const int BUTTON = 2;     // Digital pin connected to button
const int LASER = 11;     // Digital pin connected to laser diode

long val_pan;             // Value read from pan pot
long val_tilt;            // Value read from tilt pot

int degrees_buffer = 1;   // Ignore moves of fewer than this many degrees
const int PAN_MAX = 160;
const int PAN_MIN = 20;
const int TILT_MAX = 90;
const int TILT_MIN = 28;

const int DELAY_MIN = 500;    // Minimum delay between auto moves in milliseconds
const int DELAY_MAX = 3000;   // Maximum delay between auto moves in milliseconds
const int RAPID_INTERVAL = 6; // Number of moves before running in rapid mode
const int MAX_RAPID_MOVES = 20; // Number of rapid moves to make in rapid mode

MODE mode = MANUAL;       // Whether the servos should be positioned via potentiometer 
                          // position or random selection

void setup() 
{ 
  pan_motor.attach(A2);  // attaches the servo on pin 2 to the servo object 
  tilt_motor.attach(A3);

  pinMode(PAN, INPUT);
  pinMode(TILT, INPUT);
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(LASER, OUTPUT);
  digitalWrite(LASER, HIGH);
  
  Serial.begin(9600);
} 

void loop() 
{ 
  switch(mode)
  {
    case MANUAL:
    {
      runManualMode();
      break;
    }
    case AUTO:
    {
      runAutoMode();
      break;
    }
    default:
    {
      Serial.println("Uh... something is amiss.");
      break;
    }
  }
  updateMode();
} 

void runManualMode()
{
   // resets val_pan and val_tilt each iteration so it can't grow larger than 180
    val_pan = 0;
    val_tilt = 0;
    
    int avg_count = 20;
    
    for(int i = 0; i < avg_count; i++) {
      val_pan += analogRead(PAN);  // value between 0 and 1023
    }
    
    val_pan /= avg_count;           
    val_pan = map(val_pan, 0, 1023, PAN_MIN, PAN_MAX);     // scale to use w/ servo (0-180)
    
    if( abs(val_pan - pan_motor.read()) > degrees_buffer) {
      Serial.print("pan val: ");
      Serial.println(val_pan);
      pan_motor.write(val_pan);            // sets servo position according to the scaled value 
      delay(15);                           // waits for the servo to get there
    } 
  
    for(int i = 0; i < avg_count; i++) {
      val_tilt += analogRead(TILT);
    }
    
    val_tilt /= avg_count;
    val_tilt = map(val_tilt, 0, 1023, TILT_MIN, TILT_MAX);
    
    if(abs(val_tilt - tilt_motor.read()) > degrees_buffer) {
      Serial.print("tilt val: ");
      Serial.println(val_tilt);
      tilt_motor.write(val_tilt);
      delay(15);
    }
}

void runAutoMode()
{
    val_tilt = random(TILT_MIN, TILT_MAX);   // sets val_tilt to random num b/w 20-120
    val_pan = random(PAN_MIN, PAN_MAX);    // sets val_pan to random num b/w 20-120

    static long last_update = 0;
    static int current_delay = 0;
    static int runs_since_last_rapid = 0;
    static int rapid_moves = 0;

    if(millis() - last_update > current_delay)
    {
      // Update tilt motor
      if(abs(val_tilt - tilt_motor.read()) > degrees_buffer) {
        Serial.print("tilt val: ");
        Serial.println(val_tilt);
        tilt_motor.write(val_tilt);
        delay(15);  
      }

      // Update pan motor
      if(abs(val_pan - pan_motor.read()) > degrees_buffer) {
        Serial.print("pan val: ");
        Serial.println(val_pan);
        pan_motor.write(val_pan);            // sets servo position according to the scaled value 
        delay(15);                           // waits for the servo to get there
      }

      // Pick next delay
      if(runs_since_last_rapid > RAPID_INTERVAL || (rapid_moves > 0 && rapid_moves < MAX_RAPID_MOVES))
      {
        current_delay = random(DELAY_MIN / 2, DELAY_MIN);
        rapid_moves++;
        runs_since_last_rapid = 0;
      }
      else
      {
        current_delay = random(DELAY_MIN, DELAY_MAX); 
        runs_since_last_rapid++;
        rapid_moves = 0; 
      }
      last_update = millis();
    }
}

void updateMode()
{
  const int DEBOUNCE_WAIT = 50; // Milliseconds between button checks for debouncing
  static long last_check = 0;
  if(millis() - last_check > DEBOUNCE_WAIT)
  {
    mode = digitalRead(BUTTON) ? MANUAL : AUTO;
    last_check = millis();
  }
}

