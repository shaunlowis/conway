/*
Writing Conway's game of life for a microcontroller.
This file requires the U8g2lib library.
Code by Shaun Lowis.
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
  U8g2lib Example Overview:
    Frame Buffer Examples: More RAM intensive, but can get values back from array.
    Page Buffer Examples: Can't use this method as we need to get cell values back from our display.
    U8x8 Text Only Example: No RAM usage, direct communication with display controller. No graphics, 8x8 Text only.
    
*/

U8G2_ST7920_128X64_F_SW_SPI u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* CS=*/ 10, /* reset=*/ 8);
// U8G2_ST7920_128X64_1_SW_SPI u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* CS=*/ 10, /* reset=*/ 8);

// bool screen[128][64];
bool screen[32][16];

void setup(void) {
  u8g2.begin();
  // Start conditions for Conway, draw some initial points manually.
  // The screen dimensions are 128 x 64
  
  u8g2.clearBuffer();	

  // Make a glider near the middle ish of the screen.
  // Draw the values
  u8g2.drawBox(56, 30, 1, 1);
  u8g2.drawBox(57, 30, 1, 1);
  u8g2.drawBox(58, 30, 1, 1);
  u8g2.drawBox(56, 31, 1, 1);
  u8g2.drawBox(57, 32, 1, 1);

  // Update the screen array.
  screen[56][30] = true;
  screen[57][30] = true;
  screen[58][30] = true;
  screen[56][31] = true;
  screen[57][32] = true;

  // Generate some random points at the start of every game.
  // This is inside of the outer edge, same as my iteration.
  for (int rand_starts = 0; rand_starts < 100; rand_starts++){
    // int rand_x = random(1, 127);
    // int rand_y = random(1, 63);
    int rand_x = random(1, 31);
    int rand_y = random(1, 15);
    u8g2.drawBox(rand_x, rand_y, 1, 1);
    screen[rand_x][rand_y] = true;
  }

  u8g2.sendBuffer();
}

void loop(void) {
  // Now that we have our screen defined, we can clear the display.
  u8g2.clearBuffer();	

  for (int i = 1; i < 31; i++) {
    for (int j = 1; j < 15; j++) {
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

      // Defining Conway's rules:
      // Rule 1: A live cell dies if it has fewer than two live neighbors.
      if (screen[i][j] && total < 2) {
        screen[i][j] = false;
      }
      // Rule 2: A live cell with more than three live neighbors dies.
      else if (screen[i][j] && total > 3) {
        screen[i][j] = false;
      }
      // Rule 3: A live cell with two or three live neighbors lives on to the next generation.
      else if (screen[i][j] && (total == 2 || total == 3)) {
        screen[i][j] = true;
      }
      // Rule 4: A dead cell will be brought back to live if it has exactly three live neighbors.
      else if (screen[i][j] == false && total == 3) {
        screen[i][j] = true;
      }
      
      if (screen[i][j]){
        u8g2.drawPixel(i, j); // Draw a pixel if the corresponding 'screen' array value is true
      }
    }
    u8g2.sendBuffer();
  }
  delay(1000);
}
