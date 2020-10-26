# Example code for the EVE on a ESP 32 with the WEMOS D1 mini

![Weather Screen](/images/Demo1.jpg)

- Designed for Matrix Orbital EVE3-43 SPI TFT Displays

  https://www.matrixorbital.com/ftdi-eve

- This code makes use of the Matrix Orbital EVE Library found here: 

  https://github.com/MatrixOrbital/EVE2-Library

  - While a copy of the library files (Eve2_81x.c and Eve2_81x.h) is included here, you may look for updated
    files if you wish.  This is optional, but the Eve2-Library is likely to contain an extension of what you
    have here in case you wish to make some more advanced screens.

- Matrix Orbital EVE SPI TFT display information can be found at: https://www.matrixorbital.com/ftdi-eve

# Requirements 

- WEMOS D1 Mini (Most models of the ESP32/ESP8266 should work, but are untested)
- A Matrix Orbital [EVE3-43 display](https://www.matrixorbital.com/eve3-43a)
- A Matrix Orbital [EVE2-SPI2BBC Breakout board](https://www.matrixorbital.com/EVE2-SPI2BBC)
- A Matrix Orbital [EVE2-USB2SPI-KIT-A USB to SPI Adapter board](https://www.matrixorbital.com/eve2-usb2spi-kit-a)


# 1. Display setup

You need to upload the background image to the display
Connect the EVE display to the USB2SPI board, be careful that the FFC is properly inserted in both headers
Plug-in the USB2SPI to a USB cable and to your computer

![DM](/images/USB2SPI_Device_Manager.png)

To verify you can run the test program, you should see the Matrix Orbital logo

[jpeg_demo_43_EVE3_TPN.exe](https://github.com/MatrixOrbital/ESP-EVE3-Weather-Demo/raw/main/jpeg_demo_43_EVE3_TPN.exe)


Download and install the EVE Asset Builder

https://www.matrixorbital.com/index.php?route=download/download/get&did=1695

With the EVE Asset Builder you will need to upload the images to the display

![EAB1](/images/EAB-1.png)

* a. Select Flash Utilities
* b. Select Flash Programmer
* c. Select MPSEE
* d. Click Detect

The software will connect to the display and verify the on-board memory

![EAB2](/images/EAB-2.png)

* e. Select the flash.bin file in the flash folder
* f. Click Update&Verify
* g. Verify the upload is successful


# 2. Wiring the ESP32 to the EVE Display

Display---SPI2BBC---ESP32

![EAB2](/images/Connection.jpg)

|ESP|Breakout Board|
|-|-|
|GND|GND|
|3.3V|3.3V|
|D5|SCK|
|D6|MISO|
|D7|MOSI|
|D8|CS|
|D0|RST|

# 3. Skeptch/Code setup

You will require the Arduino IDE

You may require to rename the folder to 'ESP-EVE3-Weather-Demo'

Load 'ESP-EVE3-Weather-Demo.ino'

Install the ESP8266 Board Manager

https://averagemaker.com/2018/03/wemos-d1-mini-setup.html

Configure the IDE by selecting the proper Board and COM Port.

## Libraries required

to install `Sketch > Include Library > Manage Libraries...` and installing the library

- `ESP8266 Weather Station` library by ThingPulse, version 2.0.1 was used during development. 
- `Json Streaming Parser` library by Daniel Eichhorn, version 1.0.5 was used during development. 

## openweathermap.org API Key
The datasource for this demo is the OpenWeatherMap API, befoure you can use this API you'll need to obtain a free API key. 

- Go to https://openweathermap.org/appid/ and sign up for the free account. 
- Obtain your API key from https://home.openweathermap.org/api_keys

## openweathermap.org Location Code

- Go to https://openweathermap.org/find and search for the city of your choice.
- Click the link of the city of your choice and it will navigate to the weather page for this city. The last part of the URL will be the location code.

ie: If you Search for Calgary the page will be `https://openweathermap.org/city/5913490` in this example `5913490` will be the location code. 

# Code Changes

Before you can run the code a few edits will need to be made in `esp_weather_demo.ino`

|Variable|Purpose|
|-|-|
|WIFI_SSID|The name of the WIFI access point you would like to use|
|WIFI_PWD|The password required for the WIFI access point|
|OPEN_WEATHER_MAP_APP_ID|The openweathermap.org API key obtained above|
|OPEN_WEATHER_MAP_LOCATION|The Location Code obtained above|
|UTC_OFFSET|The time offset from DST|

# Compile and Run

![Loading](/images/Demo2.jpg)
![Weather Screen](/images/Demo1.jpg)

# Support Forums
  
  http://www.lcdforums.com/forums/viewforum.php?f=45
