/**
 * Sensor Controlled DOOM
 * 
 * The next sketch controlls the classic DOOM game using a Circuit Playground Express board.
 * For movement - tilt the CPX to the appropriate direction
 * For shooting - yell at the CPX (say PEW!)
 * For Item using - use the right button of the CPX
 * If you got bored and you are into cheating - light the CPX with a flashlight and see what happens!
 * 
 * Link to play doom on a web browser: https://www.retrogames.cz/play_414-DOS.php
 * 
 * The circuit: Circuit Playground Express only :)
 * 
 * Video Link: https://www.youtube.com/watch?v=9Vf3yiVmu5s
 * 
 * Created By: Idan Kringel
 */


//imports
#include <Adafruit_CircuitPlayground.h>
#include <Mouse.h>
#include <Keyboard.h>

// Macros
#define FORWARD_THRESHOLD 1.0
#define BACKWARD_THRESHOLD 5.0
#define SIDE_THRESHOLD 3.5
#define VOICE_THRESHOLD 71
#define MOUSE_VELOCITY 15
#define CHEAT_MODE_THRESHOLD 600

//global variables
float X, Y;
boolean right_button_pressed = false;
boolean mouse_pressed = false;
int last_mouse_press = 0;
int last_cheat_mode_time = 0;
static const char walls_cheat[] = "idspispopd";

void setup() {
  CircuitPlayground.begin();
  Keyboard.begin();
  Mouse.begin();
}

/**
 * Handle the CPX tilting to move the character
 */
void handleMovement(){ 
  // get X and Y from accelarometer
  X = CircuitPlayground.motionX();
  Y = CircuitPlayground.motionY();
  //check if player should move forward
  if (Y < -FORWARD_THRESHOLD) {
    Mouse.move(0, -MOUSE_VELOCITY, 0);
  }
  //check if player should move backwards
  else if (Y > BACKWARD_THRESHOLD) {
    Mouse.move(0, MOUSE_VELOCITY, 0);
  }
  //check if player should move sideways
  if (X < -SIDE_THRESHOLD) {
    Mouse.move(MOUSE_VELOCITY, 0 , 0);
  }
  else if (X > SIDE_THRESHOLD) {
    Mouse.move(-MOUSE_VELOCITY, 0, 0);
  }
}

/**
 * Detect high noise (like PEW) and make the character shoot
 */
void handleShooting(){
  if (!mouse_pressed && CircuitPlayground.mic.soundPressureLevel(10) > VOICE_THRESHOLD) {
    Mouse.press(MOUSE_LEFT);
    mouse_pressed = true;
    last_mouse_press = millis();
  }
  else if(mouse_pressed && millis() - last_mouse_press > 100){
    mouse_pressed = false;
    Mouse.release(MOUSE_LEFT);
  }
}

/**
 * Detect button press and make the character use item (space button)
 */
void handleButtonPress(){
  if (CircuitPlayground.rightButton()) {
    right_button_pressed = true;
  }
  else if(right_button_pressed){
    Keyboard.print(" ");
    right_button_pressed = false;
  }
}

/**
 * Detect high amount of light and enter \ exit cheat mode accordingly.
 */
void handleCheatMode(){
  // verify if last cheat mode activation \ deactivation was over 10 seconds ago and if amount of light is more than the threshold
  if(millis() - last_cheat_mode_time > 10000 && CircuitPlayground.lightSensor() > CHEAT_MODE_THRESHOLD){
    for(int i = 0; i < strlen(walls_cheat); i++){
      Keyboard.press(walls_cheat[i]);
      delay(10);
      Keyboard.release(walls_cheat[i]);
      delay(200);
    }
    last_cheat_mode_time = millis();
  }
}

void loop() {
  // Check if the slide switch is enabled (on +) and if not then just exit out
  // and run the loop again.  This lets you turn on/off the mouse movement with
  // the slide switch.
  if (!CircuitPlayground.slideSwitch()) {
    return;
  }
  handleMovement();
  handleButtonPress();
  handleCheatMode();
  handleShooting();
}
