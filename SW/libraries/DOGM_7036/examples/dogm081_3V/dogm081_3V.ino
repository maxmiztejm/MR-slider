#include <Arduino.h>
#include <SPI.h>
#include <dogm_7036.h>
/*Available functions in dogm_7036 Libraray:
void initialize       (byte p_cs, byte p_si, byte p_clk, byte p_rs, byte p_res, boolean sup_5V, byte lines);
void string           (const char *str);
void ascii            (char character);
void position         (byte column, byte line);
void displ_onoff      (boolean on);
void cursor_onoff     (boolean on);
void define_char      (byte mem_adress, byte *dat);
void clear_display    (void);
void contrast         (byte contr);
*/

dogm_7036 DOG;
byte arrow_down[] = {0x04, 0x04, 0x04, 0x04, 0x15, 0x0E, 0x04, 0x00}; //pattern for own defined character

//the following port definitions are used by our demo board "EA PCBARDDOG7036"
int led       = 3;
int led_red   = 3;
int led_green = 5;
int led_blue  = 6;
void init_backlight(boolean mono);
void mono_backlight(byte brightness);
void rgb_backlight(byte red, byte green, byte blue);

void setup()
{
  init_backlight(true); //use monochrome backlight in this sample code. Please change it to your configuration
  DOG.initialize(10,0,0,9,4,0,DOGM081);   //SS = 10, 0,0= use Hardware SPI, 9 = RS, 4= RESET, 0 = 3.3V, EA DOGM081-A (=1 line)
  DOG.displ_onoff(true);          //turn Display on
  DOG.cursor_onoff(true);         //turn Curosor blinking on
  DOG.define_char(0, arrow_down); //define own char on memory adress 0
}

void loop()
{
  DOG.position(1,1);              //set Position: first line, first character
  DOG.string("DOGM081 ");          //show String
  mono_backlight(255);
  delay(2000);
  DOG.position(1,1);             
  DOG.string("OwnChar");
  DOG.ascii(0);                   //show own defined char (adress 0, see DOG.define_char())
  mono_backlight(128);
  delay(2000);
  DOG.position(1,1);              
  DOG.string("Arduino ");         
  mono_backlight(64);
  delay(2000);
  
  
}

//The following functions controll the backlight with a PWM. Not needed for the display content
void init_backlight(boolean mono)
{
  if(mono) //EA LED55X31-G, EA LED55X31-W, EA LED55X31-B, EA LED55X31-A, EA LED55X31-R
  {
    pinMode(led,  OUTPUT);
    mono_backlight(255);
  }
  else  //EA LED55X31-RGB
  {
    pinMode(led_blue,  OUTPUT);
    pinMode(led_green, OUTPUT);
    pinMode(led_red,   OUTPUT);
    rgb_backlight(255,0,0);
  }
}
//Use this funtion for monochrome backlight
void mono_backlight(byte brightness)
{
  analogWrite(led, brightness);  
}
//Use this function for RGB backlight
void rgb_backlight(byte red, byte green, byte blue)
{
  analogWrite(led_red,  red);  
  analogWrite(led_green, green);  
  analogWrite(led_blue,   blue);  
}
