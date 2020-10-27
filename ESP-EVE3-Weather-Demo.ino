// ************************************************************************************
//  Matrix Orbital EVE3-43 and ESP32 Demo for displaying weather
//
//  sales: https://www.matrixorbital.com
//  support & feedback: https://www.lcdforums.com/forums/viewforum.php?f=45
//
//  EVE3-43A/G: https://www.matrixorbital.com/eve3-43g
// ************************************************************************************

#include <ESP8266WiFi.h>    // ESP library
#include <stdint.h>         // Find types like "uint8_t"
#include <stdbool.h>        // Find type "bool"
#include <SPI.h>            // Arduino SPI library
#include <NTPClient.h>      // Library for local time
#include <time.h>           // Time function

#include "MatrixEve2Conf.h" // Matrix Orbital EVE library
#include "hw_api.h"         // Matrix Orbital EVE library      
#include "Eve2_81x.h"       // Matrix Orbital EVE library      
#include "Arduino_AL.h"     // Hardware abstraction layer for Arduino
#include "process.h"        // More application level code
#include "OpenWeatherMapCurrent.h"  //Weather station library
#include "OpenWeatherMapForecast.h" //Weather station library

const char *WIFI_SSID = "xxxxxxxx";    //your WIFI SSID
const char *WIFI_PWD = "xxxxxxxx";     //your WIFI password

const boolean IS_METRIC = true;     // true = *C, false = *F
const String OPEN_WEATHER_MAP_APP_ID = "b02f9232814d92543b9b6b5ef35bb6ac";  //Obtain your API key from https://home.openweathermap.org/api_keys
const String OPEN_WEATHER_MAP_LOCATION = "5913490";     //your city ID from https://openweathermap.org/find
const String OPEN_WEATHER_MAP_LANGUAGE = "en";    //your language

#define UTC_OFFSET -7     // time zone offset
#define DST_OFFSET 1      // daylight savings
#define NTP_SERVERS "us.pool.ntp.org", "time.nist.gov", "pool.ntp.org"    //time servers
#define EPOCH_1970

time_t last_update;
OpenWeatherMapCurrentData currentWeatherData;
OpenWeatherMapCurrent currentWeatherClient;
uint32_t ptr;
uint32_t width;
uint32_t height;
int Reference=1;

extern "C" {
 void DBG(const char* data)
 {
  Serial.println(data);
 }
}

void MakeScreen_MatrixOrbital(uint8_t DotSize)
{
  Send_CMD(CMD_DLSTART);                   // Start a new display list
  Send_CMD(CLEAR_COLOR_RGB(0, 0, 0));      // Determine the clear screen color
  Send_CMD(CLEAR(1, 1, 1));                // Clear the screen and the curren display list
  Send_CMD(COLOR_RGB(26, 26, 192));        // change colour to blue
  Send_CMD(POINT_SIZE(DotSize * 16));      // set point size to DotSize pixels. Points = (pixels x 16)
  Send_CMD(BEGIN(POINTS));                 // start drawing point
  Send_CMD(TAG(1));                        // Tag the blue dot with a touch ID
  Send_CMD(VERTEX2II(Display_Width() / 2, (Display_Height() / 2)-60, 0, 0));     // place blue point
  Send_CMD(END());                         // end drawing point
  Send_CMD(COLOR_RGB(255, 255, 255));      // Change color to white for text
  Cmd_Text(Display_Width() / 2, (Display_Height() / 2)- 60, 30, OPT_CENTER, " MATRIX         ORBITAL"); //Write text in the center of the screen
  Cmd_Text(Display_Width() / 2, (Display_Height() / 2) , 28, OPT_CENTER, "Connecting to WIFI"); //Write text in the center of the screen
  Cmd_Spinner(Display_Width() / 2, 80 + (Display_Height() / 2), 0, 0);
  Send_CMD(DISPLAY());                     // End the display list
  Send_CMD(CMD_SWAP);                      // Swap commands into RAM
  UpdateFIFO();                            // Trigger the CoProcessor to start processing the FIFO
  Wait4CoProFIFOEmpty();                   // wait here until the coprocessor has read and executed every pending command.    
}


void LoadImage()
{
    Send_CMD(CMD_FLASHSOURCE);                   // Tell the next command where to get it's data from in flash
    Send_CMD(RAM_FLASH + 4096);                  // This is the address in Eve attached flash

    Send_CMD(CMD_LOADIMAGE);
    Send_CMD(RAM_G);                             // This is the address where decompressed data will go 
    Send_CMD(OPT_FLASH);                         // Send options , load image from flash
    UpdateFIFO();                                // Trigger the CoProcessor to start processing commands out of the FIFO
    Wait4CoProFIFOEmpty();                       // wait here until the coprocessor has read and executed every pending command.    

    int props_start_address = FifoWriteLocation; // the CMD_GETPROPS command will write the results into
                           // the fifo buffer, so we have to keep track what address we are currently at.

    Send_CMD(CMD_GETPROPS);                      // Tell the CoProcessor we would like to query the properties of the last compressed image. 

                           // Because the command will write into the fifo buffer we will have to reserve space for the output of all output variables
                           // from the manual section 5.53 CMD_GETPROPS
    Send_CMD(0);                                 // +4 ptr
    Send_CMD(0);                                 // +8 width
    Send_CMD(0);                                 // +12 height

    UpdateFIFO();                                // force run the GetProps command, so we can read back the results 
    Wait4CoProFIFOEmpty();                       // wait here until the coprocessor has read and executed every pending command.    
    ptr = rd32(props_start_address + RAM_CMD + 4);    // Read back the ptr variable 
    width = rd32(props_start_address + RAM_CMD + 8);  // Read back the width variable 
    height = rd32(props_start_address + RAM_CMD + 12);// Read back the height variable 
    printf("LoadImage addr:%.8x %dx%d\n", ptr, width, height);
      
}

void update_weather(bool force)
{
  time_t now = time(nullptr); 
  double seconds = difftime(now, last_update);
  if (seconds > (15*60) || force) {
    Serial.println("updating weather... ");
    currentWeatherClient.updateCurrentById(&currentWeatherData, OPEN_WEATHER_MAP_APP_ID, OPEN_WEATHER_MAP_LOCATION);  
    last_update=now;
  }
}
void DisplayWeather()
{
    char buf[16];
    time_t now = time(nullptr); 
    struct tm * timeinfo = localtime (&now); 
    static int last_sec = -1;
    update_weather(false);
    if (timeinfo->tm_sec != last_sec)
    { 
    Send_CMD(CMD_DLSTART);                      // Start a new display list. 
    Send_CMD(CLEAR_COLOR_RGB(0, 0, 0));         // Set the clear color to be white
    Send_CMD(CLEAR(1, 1, 1));                   // Clear the screen 

    Send_CMD(BITMAP_HANDLE(Reference));         // handle for this bitmap
    Cmd_SetBitmap(RAM_G + ptr, RGB565, width, height);   // Use the CoPro Command to fill in the bitmap parameters 
                                                         // Place the bitmap in the center of the screen
    int32_t left = (Display_Width() - width) / 2;
    int32_t top = (Display_Height() - height) / 2;

    Send_CMD(BEGIN(BITMAPS));                    // Begin bitmap placement
    Send_CMD(VERTEX2II(left, top, Reference, 0));// Define the placement position of the previously defined holding area.
    Send_CMD(END());                             // end placing bitmaps
      Send_CMD(COLOR_RGB(255, 255, 255));      //Change color to white for text
      Cmd_RomFont(1, 33);
      Cmd_RomFont(2, 29);
      Cmd_RomFont(3, 34);
      Cmd_Text(0, 0, 1,  0, currentWeatherData.cityName.c_str());       //Write text in the center of the screen
      Cmd_Text(8, 80, 2,  0, currentWeatherData.description.c_str());   //Write text in the center of the screen
      
      sprintf(buf,"%d%c",(int)(currentWeatherData.temp+0.5f), IS_METRIC ? 'C' : 'F');
      Cmd_Text(Display_Width()-strlen(buf)*55, Display_Height()-110, 3, 0, buf); //Write text in the center of the screen
      
      sprintf(buf,"%.2d:%.2d:%.2d",timeinfo->tm_hour,timeinfo->tm_min, timeinfo->tm_sec);
      Cmd_Text(4, Display_Height()-40 , 24,  0, buf); //Write text in the center of the screen
      
      Send_CMD(END());                          // end drawing point
      Send_CMD(DISPLAY());                      // End display list
      Send_CMD(CMD_SWAP);                       // Activate this display list
      UpdateFIFO();                             // force run the GetProps command, so we can read back the results 
      Wait4CoProFIFOEmpty();                    // wait here until the coprocessor has read and executed every pending command.    
      last_sec = timeinfo->tm_sec;
    }
}


void setup()
{
  Serial.begin(115200);
  while (! Serial);
  GlobalInit();
  Serial.println("ftinit..\n");
  FT81x_Init(DISPLAY_43, BOARD_EVE2, TOUCH_TPN);

if (FlashAttach())
  {
    uint32_t FlashSize = rd32(REG_FLASH_SIZE + RAM_REG);   // Flash size is reported in megabytes.
    Serial.printf("flash Size = %d mbytes (%d mbits)\n", FlashSize, FlashSize * 8);

    // You can only display images from flash if the flash is running in full speed mode
    // which you enter by the CMD_FLASHFAST command. 
    if (!FlashFast())
    {
      Serial.printf("Still unable to enter fast mode...please power cycle the unit\n");
    }

    uint8_t FlashStatus = rd8(REG_FLASH_STATUS + RAM_REG);
    Serial.printf("Flash Full Speed : %s\n", FlashStatus == FLASH_STATUS_FULL ? "yes" : "no");  
  }
  LoadImage();
  MakeScreen_MatrixOrbital(16);
  WiFi.begin(WIFI_SSID, WIFI_PWD);
  int counter = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP()); 
  Serial.println("setup done!..\n");
  currentWeatherClient.setMetric(IS_METRIC);
  currentWeatherClient.setLanguage(OPEN_WEATHER_MAP_LANGUAGE);
  Serial.println("fetching weather....\n");
  update_weather(true);
  Serial.println("fetching weather....done\n");
  Serial.println(currentWeatherData.cityName);
  Serial.println(currentWeatherData.main);  
  Serial.println(currentWeatherData.description);  
  Serial.println(currentWeatherData.main);  
  Serial.printf("Temp : %.2f\n", currentWeatherData.temp);  
  Serial.printf("Pressure : %d\n", currentWeatherData.pressure);  
  Serial.println("fetching weather....end\n");
  configTime((UTC_OFFSET * 3600), (DST_OFFSET * 3660), NTP_SERVERS);
  Serial.println("Syncing time...");
  while(time(nullptr) <= 10000) {
    Serial.print(".");
    delay(100);
  }
  Serial.println("Time sync'ed");
  LoadImage();
}

void loop()
{
  DisplayWeather();
}


// ************************************************************************************
// Following are wrapper functions for C++ Arduino functions so that they may be      *
// called from outside of C++ files.  These are also your opportunity to use a common *
// name for your hardware functions - no matter the hardware.  In Arduino-world you   *
// interact with hardware using Arduino built-in functions which are all C++ and so   *
// your "abstraction layer" must live in this xxx.ino file where C++ works.           *
//                                                                                    *
// Incidentally, this is also an alternative to ifdef-elif hell.  A different target  *
// processor or compiler will include different files for hardware abstraction, but   *
// the core "library" files remain unaltered - and clean.  Applications built on top  *
// of the libraries need not know which processor or compiler they are running /      *
// compiling on (in general and within reason)                                        *
// ************************************************************************************

void GlobalInit(void)
{
  Serial.println("Global init 1\n");
  //Serial.begin(115200);
  //while (!Serial) {;}                    // wait for serial port to connect.
  ESP.wdtFeed();
  // Keys Initialization
  //Init_Keys();
  Serial.println("Global init 2\n");
  // Matrix Orbital Eve display interface initialization
  pinMode(EvePDN_PIN, OUTPUT);            // Pin setup as output for Eve PDN pin.
  Serial.println("Global init 3\n");
  digitalWrite(EvePDN_PIN, LOW);          // Apply a resetish condition on Eve
  Serial.println("Global init 4\n");
  pinMode(EveChipSelect_PIN, OUTPUT);     // SPI CS Initialization
  Serial.println("Global init 5\n");
  digitalWrite(EveChipSelect_PIN, HIGH);  // Deselect Eve
  Serial.println("Global init 6\n");
  pinMode(EveAudioEnable_PIN, OUTPUT);    // Audio Enable PIN
  Serial.println("Global init 7\n");
  digitalWrite(EveAudioEnable_PIN, LOW);  // Disable Audio
  Serial.println("Global init 8\n");
  SPI.begin();                            // Enable SPI
  Serial.println("Global init done\n");
}


void DebugPrint(char *str)
{
  Serial.print(str);
}

// A millisecond delay wrapper for the Arduino function
void MyDelay(uint32_t DLY)
{
  uint32_t wait;
  wait = millis() + DLY; while(millis() < wait);
}

extern "C" 
{
// An abstracted pin write that may be called from outside this file.
void SetPin(uint8_t pin, bool state)
{
  digitalWrite(pin, state); 
}
}
// An abstracted pin read that may be called from outside this file.
#ifdef __cplusplus
extern "C" {
#endif
uint8_t ReadPin(uint8_t pin)
{
  return(digitalRead(pin));
}
#ifdef __cplusplus
}
#endif
