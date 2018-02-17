////////////////////////////////////////////////////////////
//-----------------  MR_SLIDER V 1.1 ---------------------//
// autors: Maksim Rezenov, Radoslav Dodek                 //
//                                                        //
////////////////////////////////////////////////////////////
//TO DO: time to end measurement in timelapse mode, correct annotation

#include <Arduino.h>
#include <math.h>
#include <SPI.h>
#include <dogm_7036.h>
#include <TimerOne.h>
#include <MenuSystem.h>
#include <stdio.h>
#include <EEPROMex.h>    // NEEDED corrections in library: EEPROMex.cpp 40: _allowedWrites(100000), 265: int EEPROMClassEx::_memSize= 1024;

//
// Constants
//
#define BUTTON_PIN A0
#define LED_PIN 10
#define Motor_PIN 3
#define DIR_PIN 2
#define drvEN_PIN 4
#define M0_PIN A4
#define M1_PIN A3
#define M2_PIN A5

#define Sensor_PIN 5
#define Shutter_PIN 13

//EEPROM addresses
#define EE_SIZE 1024
#define EE_ADDR_TIMELAPSE_STEP_TIME      0    // 2byte
#define EE_ADDR_TIMELAPSE_STEP_LENGTH    2    // 2byte
#define EE_ADDR_TIMELAPSE_SHUTTER        4    // 2byte
#define EE_ADDR_TIMELAPSE_HD            10   // 1 byte
#define EE_ADDR_TIMELAPSE_RTS            11   // 1 byte
#define EE_ADDR_SETTINGS_RAIL_LENGTH     12   // 2byte
#define EE_ADDR_SETTINGS_WHEEL_DIAMETER  14   // 2byte
#define EE_ADDR_SETTINGS_MOTOR_SPR       16   // 2byte      
#define EE_ADDR_STEPLESSMOTION_SPEED     18   // 2byte
#define EE_ADDR_INTERNAL_MOTOR_STEPS     20   // 2byte     

#define UNIT_STEPLESSMOTION_SPEED            "*0.5 mm/s"
#define UNIT_TIMELAPSE_STEP_TIME         "sec"
#define UNIT_TIMELAPSE_SHUTTER_TIME      "sec"
#define UNIT_TIMELAPSE_STEP_LENGTH       "mm"
#define UNIT_SETTINGS_RAIL_LENGTH        "mm"
#define UNIT_SETTINGS_WHEEL_DIAMETER     "mm"
#define UNIT_SETTINGS_MOTOR_SPR          "s/r"

#define TEXT_ON                          "On"
#define TEXT_OFF                         "Off"
#define TEXT_RIGHT                       "Right"
#define TEXT_LEFT                        "Left"
#define TEXT_YES                         "Yes"
#define TEXT_NO                          "No"


// Configuration values
unsigned int steplessmotion_speed;
unsigned int timelapse_step_time;
unsigned int timelapse_step_length;
unsigned int timelapse_shutter;
bool timelapse_hd;
bool timelapse_rts;
unsigned int settings_rail_length;
unsigned int settings_wheel_diameter;
unsigned int settings_motor_spr;
unsigned int settings_steplessmotion_speed;
unsigned long internal_motor_steps;



volatile bool break_once_flag = false, break_flag = false, sensor_break_flag = false;

volatile short Sensor_PIN_state_prev, Sensor_PIN_state;
volatile int shutter_PIN_state = LOW;
volatile int motor_PIN_state = LOW;
volatile int DIR_PIN_state = LOW;
volatile unsigned long steps_FN_total = 0;
volatile unsigned long time_FN_total = 0;
volatile unsigned long internal_steps_FN_total = 0;
volatile int delay_button_counter = 0;
volatile int delay_sensor_counter = 0;
volatile unsigned long SH_time;     //variable for setting timer interrupt
volatile bool shutter_EN = false;       //variable for enable shutter ISR
volatile bool motor_EN = false;         //variable for enable motor ISR
volatile bool IsrFlag = false;          //ISR happend signal variable
volatile bool allow_break_by_button = false;
volatile bool Sensor_right = false;
volatile bool Sensor_left = false;

dogm_7036 LCD;

byte symbol_enter[] = {0x01, 0x01, 0x05, 0x09, 0x1F, 0x08, 0x04, 0x00}; // definicia symbolu ↵
byte symbol_plus_minus[] = {0x04, 0x04, 0x1F, 0x04, 0x04, 0x00, 0x1f, 0x00};

//
// Menu items
//

void on_steplessmotion_start(MenuItem* p_menu_item);
void on_steplessmotion_speed(MenuItem* p_menu_item);


void on_timelapse_start(MenuItem* p_menu_item);
void on_timelapse_step_time(MenuItem* p_menu_item);
void on_timelapse_step_length(MenuItem* p_menu_item);
void on_timelapse_shutter_1(MenuItem* p_menu_item);

void on_timelapse_hard_duty(MenuItem* p_menu_item);
void on_timelapse_return_to_start(MenuItem* p_menu_item);
void on_settings_rail_length(MenuItem* p_menu_item);
void on_settings_wheel_diameter(MenuItem* p_menu_item);
void on_settings_motor_spr(MenuItem* p_menu_item);

void on_memory_save(MenuItem* p_menu_item);
void on_memory_clear(MenuItem* p_menu_item);
void on_exit(MenuItem* p_menu_item);

// Live motion
const Menu mu_steplessmotion("STEPLESS MOTION");
const MenuItem mi_steplessmotion_start("Start", &on_steplessmotion_start);
const MenuItem mi_steplessmotion_speed("Speed", &on_steplessmotion_speed);  //dorobit!!!!!!!
const MenuItem mi_steplessmotion_exit("Exit", &on_exit);

// Time lapse
const Menu mu_timelapse("TIME LAPSE");
const MenuItem mi_timelapse_start("Start", &on_timelapse_start);
const MenuItem mi_timelapse_step_time("Step time", &on_timelapse_step_time);
const MenuItem mi_timelapse_step_length("Step length", &on_timelapse_step_length);
const MenuItem mi_timelapse_shutter("Shutter", &on_timelapse_shutter_1);
const MenuItem mi_timelapse_hard_duty("Hard duty", &on_timelapse_hard_duty);
const MenuItem mi_timelapse_return_to_start("Return to start", &on_timelapse_return_to_start);
const MenuItem mi_timelapse_exit("Exit", &on_exit);

// settings
const Menu mu_settings("SETTINGS");
const MenuItem mi_settings_rail_length("Rail length", &on_settings_rail_length);
const MenuItem mi_settings_wheel_diameter("Wheel diameter", &on_settings_wheel_diameter);  //!!!
const MenuItem mi_settings_motor_spr("Motor SpR", &on_settings_motor_spr);  //!!!!
const MenuItem mi_settings_exit("Exit", &on_exit);

// Memmory
const Menu mu_memory("MEMORY");
const MenuItem mi_memory_save("Save settings", &on_memory_save);
const MenuItem mi_memory_clear("Clear settings", &on_memory_clear);
const MenuItem mi_memory_exit("Exit", &on_exit);

void display_menu(Menu const& menu);

int read_steplessmotion_automation();
void util_int2digits(int *result, int size, int number);
void lcd_write_digits(int cursor_start, int length, int *digits);
void lcd_write_number(int cursor_start, int length, int number);
int editNumericValue(int cursorStart, int value, int digits, char *unit);


class MyRenderer : public MenuComponentRenderer {
  public:
    virtual void render(Menu const& menu) const {
      display_menu(menu);
    }
    virtual void render_menu_item(MenuItem const& menu_item) const {
    }
    virtual void render_back_menu_item(BackMenuItem const& menu_item) const {
    }
    virtual void render_numeric_menu_item(NumericMenuItem const& menu_item) const {
    }
    virtual void render_menu(Menu const& menu) const {
    }
};
MyRenderer my_renderer;
MenuSystem ms(my_renderer);

void setup() {
  pinMode (M0_PIN, OUTPUT);
  pinMode (M1_PIN, OUTPUT);
  pinMode (M2_PIN, OUTPUT);


  pinMode(Shutter_PIN, OUTPUT);

  pinMode(Sensor_PIN, INPUT);
  digitalWrite(Sensor_PIN, HIGH);

  pinMode(LED_PIN, OUTPUT);

  pinMode(drvEN_PIN, OUTPUT);
  digitalWrite(drvEN_PIN, HIGH);


  initialize_display();
  delay(100);

  initialize_menu();
  delay(100);

  main_initialize_memory();
  delay(100);

  ADCSRA &= ~(1 << 7);    //shutdown ADC
  delay(1000);

  ms.display();

  delay(2000);
  //---------------INITIAL CALIBRATION SEQUENCE---------------------
  Timer1.initialize(1000);
  Timer1.attachInterrupt(timerISR);

  microstepping(16);

  digitalWrite(drvEN_PIN, LOW);
  allow_break_by_button = false;
  motor_DRV_B(100, 100, 100, 100, 1);
  delay(1000);
  if (sensor_break_flag == false) {
    DIR_PIN_state = DIR_PIN_state ^ 1;
  }

  microstepping(8);
  motor_DRV_B(100000, 1000, 100, 1000, 1);


  digitalWrite(drvEN_PIN, HIGH);
  //------------------------------------------------------
}

void loop() {
  button_read_menu();
  delay(200);
}




