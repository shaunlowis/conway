/*
  Writing Conway's game of life for a microcontroller.
  This file requires the U8g2lib library.
  Code by Shaun Lowis.
  This version includes setting custom rules and support for
  buttons and LEDs to indicate active rules.
*/

#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

/*
  Screen & time configuration, I use a boolean array to keep track of cell states.
  Here, true = alive, false = dead. The timesteps var tracks simulation runtime.
*/

U8G2_ST7920_128X64_F_SW_SPI u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* CS=*/ 10, /* reset=*/ 8);
bool screen[128][64];
int timesteps = 1; // Start at 1 since zero is the initial random grid.

/*
  Initialise button and LED states. Digital pin mapping:
  Colours are simply how I've debugged the initial setup on the breadboard and 
  may not present in the final config. I am well aware that this limits the types of rules I can have.
  I am not interested in adding a ton of different rules as simplicity is elegance :^)

  Green button : pin 0
  White button : pin 1
  Red button   : pin 2

  Green LED    : pin 6
  White LED    : pin 7
  Red LED      : pin 5
*/

// Map the button and LED to the appropriate pins
const int green_button_pin = 2; 
// const int green_led_pin = 6;
// Set their initial states, LEDs start powered off.
// Buttons are considered active when pressed.
// int green_led_state = HIGH;
int green_button_state;
int last_green_button_state = LOW;

// Repeat for other buttons and LEDs
const int white_button_pin = 3; 
// const int white_led_pin = 5;
// int white_led_state = HIGH;
int white_button_state;
int last_white_button_state = LOW;

const int red_button_pin = 4; 
// const int red_led_pin = 7;
// int red_led_state = HIGH;
int red_button_state;
int last_red_button_state = LOW;

// Used to check if the button was toggled.
unsigned long green_debounce_time = 0;  
unsigned long red_debounce_time = 0;
unsigned long white_debounce_time = 0;
unsigned long debounceDelay = 250;    // the debounce time; increase if the output flickers

// Define menu options
enum menu_option {
  DEFAULT_RULES, // green button, LED
  HIGHLIFE, // red button, LED
  MORLEY, // white button, LED
};

// Select a default to assign to selected_option
menu_option selected_option = DEFAULT_RULES;

// Trigger the menu and initial grid functions in loop()
bool menu_displayed = true;
bool draw_initial_grid = true;

void handle_menu() {
  // Display menu options on the LCD
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_u8glib_4_tf);
  u8g2.drawStr(1, 15, "Select a rule, via a [coloured] button:");
  u8g2.drawStr(15, 30, "Default Conway's [green]");
  u8g2.drawStr(15, 40, "Highlife [red]");
  u8g2.drawStr(15, 50, "Morley's Miracle [white]");
  u8g2.sendBuffer();

  while (menu_displayed){
    // ------------------ Control logic, see examples/Debounce.ino: --------------------- //
    // Here I just repeat it for the respective colours and update the game state accordingly.

    // If the switch changed, due to noise or pressing:
    int green_reading = digitalRead(green_button_pin);
    if (green_reading != last_green_button_state) {
      // reset the debouncing timer
      green_debounce_time = millis();
    }
    if ((millis() - green_debounce_time) > debounceDelay) {
      // whatever the reading is at, it's been there for longer than the debounce
      // delay, so take it as the actual current state:

      // if the button state has changed:
      if (green_reading != green_button_state) {
        green_button_state = green_reading;
        // only toggle the LED if the new button state is HIGH
        if (green_button_state == HIGH) {
          // Update Conway rules:
          selected_option = DEFAULT_RULES;
          // set the LED:
          // digitalWrite(green_led_pin, !green_led_state);
          menu_displayed = false; // want to exit the loop if a button is pressed.
          u8g2.clearBuffer();
          u8g2.drawStr(10, 32, "Selected default rules [green]");
          u8g2.sendBuffer();
          delay(2000);
        }
      }
    }
    // save the reading. Next time through the loop, it'll be the lastButtonState:
    last_green_button_state = green_reading;

    // ----------------------------------- WHITE LED ----------------------------------- //
    int white_reading = digitalRead(white_button_pin);
    if (white_reading != last_white_button_state) {
      white_debounce_time = millis();
    }
    if ((millis() - white_debounce_time) > debounceDelay) {
      if (white_reading != white_button_state) {
        white_button_state = white_reading;
        if (white_button_state == HIGH) {
          selected_option = MORLEY;
          // digitalWrite(white_led_pin, !white_led_state);
          menu_displayed = false;
          u8g2.clearBuffer();
          u8g2.drawStr(10, 32, "Selected Morley's Miracle [white]");
          u8g2.sendBuffer();
          delay(2000);
        }
      }
    }
    last_white_button_state = white_reading;

    // ----------------------------------- RED LED ----------------------------------- //
    int red_reading = digitalRead(red_button_pin);
    if (red_reading != last_red_button_state) {
      red_debounce_time = millis();
    }
    if ((millis() - red_debounce_time) > debounceDelay) {
      if (red_reading != red_button_state) {
        red_button_state = red_reading;
        if (red_button_state == HIGH) {
          selected_option = HIGHLIFE;
          // digitalWrite(red_led_pin, !red_led_state);
          menu_displayed = false;
          u8g2.clearBuffer();
          u8g2.drawStr(10, 32, "Selected Highlife [red]");
          u8g2.sendBuffer();
          delay(2000);
        }
      }
    }
    last_red_button_state = red_reading;
  }
}


void draw_start_screen(){
  // Clear the screen:
  u8g2.clearBuffer();

  // Generate some random points at the start of every game.
  // This is inside of the outer edge, same as my iteration.
  for (int rand_starts = 0; rand_starts < 1000; rand_starts++){
    int rand_x = random(1, 127);
    int rand_y = random(1, 63);
    u8g2.drawBox(rand_x, rand_y, 1, 1);
    screen[rand_x][rand_y] = true;
  }

  u8g2.sendBuffer();
  draw_initial_grid = false; // We only want this function called once.
}


bool apply_rules(bool cell, int selected_option, int total){
  // Based off the initial menu selection, apply some different rules.
  if (selected_option == DEFAULT_RULES){
    // Defining Conway's rules:
    // Rule 1: A live cell dies if it has fewer than two live neighbors.
    if (cell && total < 2) {
      cell = false;
    }
    // Rule 2: A live cell with more than three live neighbors dies.
    else if (cell && total > 3) {
      cell = false;
    }
    // Rule 3: A live cell with two or three live neighbors lives on to the next generation.
    else if (cell && (total == 2 || total == 3)) {
      cell = true;
    }
    // Rule 4: A dead cell will be brought back to live if it has exactly three live neighbors.
    else if (cell == false && total == 3) {
      cell = true;
    }
  }

  else if (selected_option == HIGHLIFE){
    // Uses B36/S23 rules, this notation refers to birth and survival rules.
    //  - B36 means a cell is born if it has 3 or 6 neighbours.
    //  - S23 means a cell survives if it has 2 or 3 neighbours.
    if (cell && (total == 2 || total == 3)){
      cell = true;
    }
    else if (cell == false && (total == 3 || total == 6)){
      cell = true;
    }
    else {
      cell = false;
    }
  }

  else if (selected_option == MORLEY){
    // Uses B368/S245 rules, this notation refers to birth and survival rules.
    //  - B368 means a cell is born if it has 3 or 6 or 8 neighbours.
    //  - S23 means a cell survives if it has 2 or 4 or 5 neighbours.
    if (cell && (total == 2 || total == 4 || total == 5)){
      cell = true;
    }
    else if (cell == false && (total == 3 || total == 6 || total == 8)){
      cell = true;
    }
    else {
      cell = false;
    }
  }

  return cell;
}


void setup(void) {
  u8g2.begin();
  // Set a nice font for timestep updating.
  u8g2.setFont(u8g2_font_u8glib_4_tf);
  u8g2.clearBuffer();	

  // Set up button pins as inputs, LEDs as outputs
  pinMode(green_button_pin, INPUT);
  // pinMode(green_led_pin, OUTPUT);

  pinMode(white_button_pin, INPUT);
  // pinMode(white_led_pin, OUTPUT);

  pinMode(red_button_pin, INPUT);
  // pinMode(red_led_pin, OUTPUT);

  // Set the LEDs to be off initially.
  // digitalWrite(green_led_pin, green_led_state);
  // digitalWrite(white_led_pin, white_led_state);
  // digitalWrite(red_led_pin, red_led_state);
}

void loop(void) {
  // This calls the handle_menu() function once and waits until a user selects
  // the appropriate start condition.
  if (menu_displayed){
    handle_menu();
    }

  // After the menu is finished, create the starter grid once.
  if (draw_initial_grid){
    draw_start_screen();
    }

  for (int i = 1; i < 127; i++) {
    for (int j = 1; j < 63; j++) {
      int total = 0;

      if(screen[i - 1][j + 1]){ // top_left_val
        total += 1;
      }
      if(screen[i][j + 1]){ // top_val
        total += 1;
      }
      if(screen[i + 1][j + 1]){ // top_right_val
        total += 1;
      }
      if(screen[i + 1][j]){ // right_val
        total += 1;
      }
      if(screen[i + 1][j - 1]){ // bottom_right_val
        total += 1;
      }
      if(screen[i][j - 1]){ // bottom_val
        total += 1;
      }
      if(screen[i - 1][j - 1]){ // bottom_left_val
        total += 1;
      }
      if(screen[i - 1][j]){ // left_val
        total += 1;
      }

      // Calling this as a function lets us vary the rules for the game, from user input.
      screen[i][j] = apply_rules(screen[i][j], selected_option, total);
      
      // Draw a pixel if the corresponding 'screen' array value is true
      // I prefer this method of updating for visual appeal
      if (screen[i][j]){
        u8g2.setDrawColor(1);
        u8g2.drawPixel(i, j); 
      }
      else {
        u8g2.setDrawColor(0);
        u8g2.drawPixel(i, j);
      }
    }
    u8g2.sendBuffer();
  }

  // Display a counter of how many timesteps the simulation has been running for.
  char formattedString[20];
  sprintf(formattedString, "t=%d", timesteps);

  int fontX = (127 - u8g2.getStrWidth(formattedString));
  int fontY = 63; // Using the 3 pixel height font

  u8g2.setDrawColor(1);
  u8g2.drawStr(fontX, fontY, formattedString);
  u8g2.sendBuffer();

  timesteps += 1;
}