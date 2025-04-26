
# ESP-Audio-Level

This project performs vizualization of audio on SSD1306 display, connected to ESP32
OLED display shows a bar of size, proportional to current average level of signal.

## Pinout

SSD1306 display is connected to default ESP-Wroom-32 I2C pins, that is:
 
```
 GND     ->    GND
 VCC     ->    VCC
 I2C SDA ->    GPIO 21
 I2C SCL ->    GPIO 22
```
 
A led on GPIO 2 indicates connected/disconnected state of board.

## Usage 

Power on the board and connect it to your audio source. 

You would normally want the sound not to be consumed by the board, but also played on your host machine.
On Linux you can execute:

`pactl load-module module-combine-sink`

And select `combined` device in your audio settings.
