#ifndef CONFIG_H
#define CONFIG_H

#define VERSION        (1)  // firmware version
#define BAUD           (115200)  // How fast is the Arduino talking?
#define MAX_BUF        (64)  // What is the longest message Arduino can store?
#define STEPS_PER_TURN (200)  // depends on your stepper motor.  most are 200.
#define MIN_STEP_DELAY (50.0)
#define MAX_FEEDRATE   (1000000.0/MIN_STEP_DELAY)
#define MIN_FEEDRATE   (0.01)// Minimum feedrate in steps per second
//#define INVERT_Y     (1) 

// Pin definitions for Motor 1 (M1)
#define M1_STEP 2  // Step pin for Motor 1
#define M1_DIR  5  // Direction pin for Motor 1
#define M1_ENA  8  // Enable pin for Motor 1

// Pin definitions for Motor 2 (M2)
#define M2_STEP 3  // Step pin for Motor 2
#define M2_DIR  6  // Direction pin for Motor 2
#define M2_ENA  8  // Enable pin for Motor 2


//DECLARE FUNCTION
extern void m1step(int dir);
extern void m2step(int dir);
extern void disable();
extern void setup_controller();

//DEFINE FUNCTION
void m1step(int dir) {
  digitalWrite(M1_ENA,LOW);
  digitalWrite(M1_DIR,dir==1? HIGH:LOW);
  digitalWrite(M1_STEP,HIGH); // Step Motor 1
  digitalWrite(M1_STEP,LOW);  // Complete step for Motor 1
}

void m2step(int dir) {
  digitalWrite(M2_ENA,LOW);
  digitalWrite(M2_DIR,dir==1?HIGH:LOW);
  digitalWrite(M2_STEP,HIGH);  // Step Motor 2
  digitalWrite(M2_STEP,LOW);  // Complete step for Motor 2
}

void disable() {
  digitalWrite(M1_ENA,HIGH);
  digitalWrite(M2_ENA,HIGH);
}

//set up the pin for each motors
void setup_controller() {
   
  pinMode(M1_ENA,OUTPUT);
  pinMode(M2_ENA,OUTPUT);
  pinMode(M1_STEP,OUTPUT);
  pinMode(M2_STEP,OUTPUT);
  pinMode(M1_DIR,OUTPUT);
  pinMode(M2_DIR,OUTPUT);
}


#endif
