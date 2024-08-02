#include "config.h"

char  serialBuffer[MAX_BUF]; //Buffer for user input
int   sofar;   // clear         
float px, py;   //current pos in x_y coordinates   

// speeds
float fr =     0;  // feedrate in steps per second (human version)
long  step_delay;  // delay between steps in microseconds (machine version)

// settings
char mode_abs=1;   // absolute mode?




// Function: pause
// Description: Pause for a specified duration in milliseconds and microseconds.
// Parameters:
//   - ms: Duration to pause in milliseconds.
void pause(long ms) {
  delay(ms/1000);
  delayMicroseconds(ms%1000);  // delayMicroseconds doesn't work for values > ~16k.
}

// Function: feedrate
// Description: Set the feedrate (speed of movement) in steps per second.
// Parameters:
//   - new_fr: New feedrate value to set.
void feedrate(float new_fr) {
  if(fr==new_fr) return;  // same as last time?  quit now.

  if(new_fr>MAX_FEEDRATE || new_fr<MIN_FEEDRATE) {  // don't allow crazy feed rates
    Serial.print(F("New feedrate must be greater than "));
    Serial.print(MIN_FEEDRATE);
    Serial.print(F("steps/s and less than "));
    Serial.print(MAX_FEEDRATE);
    Serial.println(F("steps/s."));
    return;
  }
  step_delay = 1000000.0/new_fr; // Calculate step delay based on the new feedrate
  fr = new_fr; // Update the current feedrate
}


// Function: position
// Description: Set the logical position (px, py) of the CNC machine.
// Parameters:
//   - new_px: New x-coordinate position.
//   - new_py: New y-coordinate position.
void position(float new_px,float new_py) {
  // here is a good place to add sanity tests
  px=new_px;
  py=new_py;
}


// Function: line
// Description: Move the machine in a straight line using line algorithm.
// Parameters:
//   - newx: Destination x-coordinate.
//   - newy: Destination y-coordinate.
void line(float newx,float newy) {
  float i;
  float over= 0;
  
  float dx  = newx-px;
  float dy  = newy-py;
  int dirx = dx>0?1:-1;
  #ifdef INVERT_Y
  int diry = dy>0?-1:1;  // because the motors are mounted in opposite directions
  #else
  int diry = dy>0?1:-1;
  #endif
  dx = abs(dx);
  dy = abs(dy);

  if(dx>dy) {
    over = dx/2;
    for(i=0; i<dx; ++i) {
      m1step(dirx); // Move motor 1 
      over += dy;
      if(over>=dx) {
        over -= dx;
        m2step(diry);// Move motor 2
      }
      pause(step_delay);
    }
  } else {
    over = dy/2;
    for(i=0; i<dy; ++i) {
      m2step(diry);// Move motor 2
      over += dx;
      if(over >= dy) {
        over -= dy;
        m1step(dirx);// Move motor 1
      }
      pause(step_delay);
    }
  }

//  px = newx;
//  py = newy;
}


// Function: parseNumber
// Description: Search for a character code in the serial buffer and read the float that follows it.
// Parameters:
//   - code: Character code to search for.
//   - val: Default value to return if code is not found.
// Returns:
//   - Float value found after the code, or default value if code is not found.
float parseNumber(char code,float val) {
  char *ptr=serialBuffer;  // start at the beginning of buffer
  while((long)ptr > 1 && (*ptr) && (long)ptr < (long)serialBuffer+sofar) {  // walk to the end
    if(*ptr==code) {  // if you find code on your walk,
      return atof(ptr+1);  // convert the digits that follow into a float and return it
    }
    ptr=strchr(ptr,' ')+1;  // take a step from here to the letter after the next space
  }
  return val;  // end reached, nothing found, return default val.
}


// Function: output
// Description: Print a string followed by a float to the serial line for debugging.
// Parameters:
//   - code: String to print.
//   - val: Float value to print.
void output(const char *code,float val) {
  Serial.print(code);
  Serial.println(val);
}


// Function: where
// Description: Print the current position (px, py), feedrate (fr), and absolute mode status to serial line.
void where() {
  output("X",px);
  output("Y",py);
  output("F",fr);
  Serial.println(mode_abs?"ABS":"REL");
} 


// Function: help
// Description: Display helpful information about available commands on the serial line.
void help() {
  Serial.println(F("Commands:"));
  Serial.println(F(".........2x2 Automated Position........"));
  Serial.println(F("Position 0: P00"));
  Serial.println(F("Position 1: P01"));
  Serial.println(F("Position 2: P02"));
  Serial.println(F("Position 3: P03"));
  Serial.println(F(".........Manual G-code applicator........"));
  Serial.println(F("G00 [X(steps)] [Y(steps)] [F(feedrate)]; - line"));
  Serial.println(F("G01 [X(steps)] [Y(steps)] [F(feedrate)]; - line"));
  Serial.println(F("G04 P[seconds]; - delay"));
  Serial.println(F("G90; - absolute mode"));
  Serial.println(F("G91; - relative mode"));
  Serial.println(F("G92 [X(steps)] [Y(steps)]; - change logical position"));
  Serial.println(F("M18; - disable motors"));
  Serial.println(F("M100; - this help message"));
  Serial.println(F("M114; - report position and feedrate"));
  Serial.println(F("All commands must end with a newline."));
}

// Function: processCommand
// Description: Read the input buffer, parse recognized commands (G, M), and execute corresponding actions.
void processCommand() {
  int cmd = parseNumber('P',-1); // case for automation pos
  switch(cmd) {
  case 0: { 
   //pos0
    feedrate(400);
    line(-300.0,500.0);
    delay(5000);
    feedrate(400);
    line(300.0,-500.0);
   
    break;}
  case 1: { 
    //pos1
    feedrate(400);
    line(-300.0,1300.0);
    delay(5000);
    feedrate(400);
    line(300.0,-1300.0);
    
    break;}
  case 2: { 
    //pos2
    feedrate(400);
    line(-800.0,1300.0);
    delay(5000);
    feedrate(300);
    line(800.0,-1300.0);
   
    break;}
  case 3: { 
    //pos3
    feedrate(400);
    line(-800.0,500.0);
    delay(5000);
    feedrate(300);
    line(800.0,-500.0);
    
    break;}
  default:  break;
  }
  cmd = parseNumber('G',-1); // Manual implementation
  switch(cmd) {
  case  0:
  case  1: { // line
    feedrate(parseNumber('F',fr));
    line( parseNumber('X',(mode_abs?px:0)) + (mode_abs?0:px),
          parseNumber('Y',(mode_abs?py:0)) + (mode_abs?0:py) );
    break;
    }
    
  case  4:  pause(parseNumber('P',0)*1000);  break;  // dwell
  case 90:  mode_abs=1;  break;  // absolute mode
  case 91:  mode_abs=0;  break;  // relative mode
  case 92:  // set logical position
    position( parseNumber('X',0),
              parseNumber('Y',0) );
    break;
  default:  break;
  }

  cmd = parseNumber('M',-1);
  switch(cmd) {
  case 18:  // disable motors
    disable();
    break;
  case 100:  help();  break;
  case 114:  where();  break;
  default:  break;
  }
}


// Function: ready
// Description: Prepare the input buffer to receive a new message and indicate readiness to receive input.
void ready() {
  sofar=0;  // clear input buffer
  Serial.print(F(">"));  // signal ready to receive input
}

// Function: setup
// Description: Setup function called once on startup. Initializes serial communication and sets default parameters.
void setup() {
  Serial.begin(BAUD);  // open coms

  setup_controller();  
  position(0,0);  // set staring position
  feedrate((MAX_FEEDRATE + MIN_FEEDRATE)/2);  // set default speed

  help();  // give starting help
  ready();
}



// Function: loop
// Description: Main loop function that listens for serial commands and processes them indefinitely.
void loop() {
  // listen for serial commands
  while(Serial.available() > 0) {  // if monitor is available
    
    char c=Serial.read();  // get it
    Serial.print(c);  // repeat it back so I know you got the message
    
        if(sofar<MAX_BUF-1) serialBuffer[sofar++]=c;  // store it
        if((c=='\n') || (c == '\r')) {
         // entire message received
        serialBuffer[sofar]=0;  // end the buffer so string functions work right
        Serial.print(F("\r\n"));  // echo a return character for humans
        processCommand();  // do something with the command
        ready();}
    
  }
}
