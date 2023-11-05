// Map the button and LED to the appropriate pins
const int green_button_pin = 5; 
const int green_led_pin = 10;
// Set their initial states
int green_led_state = HIGH;
int green_button_state;
int last_green_button_state = LOW;

// Repeat for other buttons and LEDs
const int white_button_pin = 6; 
const int white_led_pin = 11;
int white_led_state = HIGH;
int white_button_state;
int last_white_button_state = LOW;

const int red_button_pin = 7; 
const int red_led_pin = 9;
int red_led_state = HIGH;
int red_button_state;
int last_red_button_state = LOW;

unsigned long green_debounce_time = 0;  // the last time the output pin was toggled
unsigned long red_debounce_time = 0;  // the last time the output pin was toggled
unsigned long white_debounce_time = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

// Define menu options
enum menu_option {
  DEFAULT_RULES, // green button, LED
  CUSTOM_RULE_1, // red button, LED
  CUSTOM_RULE_2, // white button, LED
  // Add more options as needed
};

menu_option selected_option = DEFAULT_RULES;
bool menu_displayed = true;

void handleMenu() {
  // Hopefully this makes the program wait for input here.
  // Repeatedly read pins
  Serial.print("Reached handleMenu(). \n");
  while (menu_displayed){
    int green_reading = digitalRead(green_button_pin);
    // ------------------ Control logic, from examples Debounce.ino: --------------------- //
    // Here I just repeat it for the respective colours and update the game state accordingly.

    // If the switch changed, due to noise or pressing:
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
          digitalWrite(green_led_pin, !green_led_state);
          Serial.print("Goofball green button. \n");
          menu_displayed = false; // want to exit the loop if a button is pressed.
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
          selected_option = CUSTOM_RULE_1;
          digitalWrite(white_led_pin, !white_led_state);
          Serial.print("Silly white button. \n");
          menu_displayed = false;
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
          selected_option = CUSTOM_RULE_2;
          digitalWrite(red_led_pin, !red_led_state);
          Serial.print("Wonky red button. \n");
          menu_displayed = false;
        }
      }
    }
    last_red_button_state = red_reading;
  }
}


void setup() {
  // debug my code
  Serial.begin(9600);
  // Set up button pins as inputs, LEDs as outputs, this may need to be in setup?
  pinMode(green_button_pin, INPUT);
  pinMode(green_led_pin, OUTPUT);

  pinMode(white_button_pin, INPUT);
  pinMode(white_led_pin, OUTPUT);

  pinMode(red_button_pin, INPUT);
  pinMode(red_led_pin, OUTPUT);

  digitalWrite(green_led_pin, green_led_state);
  digitalWrite(white_led_pin, white_led_state);
  digitalWrite(red_led_pin, red_led_state);

  Serial.print("Finished setup.. \n");
}

void loop() {
  // put your main code here, to run repeatedly:

  if (menu_displayed){
    handleMenu(); // Call the menu handling function in setup
  }

  Serial.print("Back to the loop() \n");

}
