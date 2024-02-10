# conway
Using my own implementation of Conway's game of life to learn more about embedded software.

## Goal/Overview

- The PCB itself is supposed to just be a nice bit of desk decoration. It can all be powered
from a USB cable, connected to the STM. 
- Started out with a basic design to perpetually render Conway's game of life on an LCD
screen. Naturally scope creep ensued:
    - Made a custom PCB for mounting the screen and connecting to it.
    - Added buttons and LEDs for different rules to use.
    - Figured keyboard keys would work quite well for this. Updated PCB accordingly
- Wound up with a relatively useful shield that can be repurposed for future projects.

![Screenshot](img/conway_proj.jpg)

First version:
<details>
<summary> Initial breadboard layout</summary>

![Screenshot](img/arduino_snapshot.jpg)

</details>


### Arduino support
The standard Arduino Uno does not have enough on-board memory to support a 128x64 pixel display.
In my case, pictured above, I wanted to release a version of this code that works easily for the average user.
Hence reducing the screen size to 64x36 is the largest section that we can run with a simple approach to Conway's game of life.

### Python demo
The software for Conway's game of life, for a basic python version and Arduino can be found in `/software`.

### LCD Screen config
The U8G2 Arduino library has a large amount of support for LCD screens, available [here](https://github.com/olikraus/u8g2/wiki/u8g2setupcpp). I used the `ST7920_128X64` module.

Refer to the LCD datasheet in `/circuitry` for wiring.

### STM Nucleo
On an STM32 NULCEO-F411RE, with more memory, the full 128x64 LCD screen is easily rendered.
I have made a shield that the screen can connect to directly using kicad 7. 
These files can be found under `/circuitry/kicad`. 

<details>
<summary> First, basic layout</summary>

![Screenshot](img/conway_board_3d.png)

Example wiring.
Barebones only display updating is in `conway_stm64.ino` for full display and a counter of generations.

![Screenshot](img/full_conway.jpeg)
![Screenshot](img/stm_wiring.jpeg)


</details>

#### Adding buttons

Additionally, buttons and LEDs were added so the user can choose different rulesets to use
for Conway's game of life. Pictured below is the "white" LED shining from selecting the
Morley's miracle rules, by clicking the white button.

![Screenshot](img/morley_running.jpg)

We now also have a more elegant PCB with support for Cherry MX keys:
I found [these](https://www.digikey.co.nz/en/products/detail/cherry-americas-llc/MX1A-E1NW/20180) 
on Digikey that had nice 3D files. This yielded the screenshot in the Overview section.

I plan to also make a 3D-printed case for this, done on onshape, the .stl will be added in `/enclosure`.

Reference file structure:
```
.
├── circuitry
├── enclosure
├── img
└── software
```
