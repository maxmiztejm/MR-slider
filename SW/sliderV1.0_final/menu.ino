/*
   Initialize menu structure
*/
void initialize_menu() {
  // Livemotion
  mu_livemotion.add_item(&mi_livemotion_start);
  mu_livemotion.add_item(&mi_livemotion_speed);
  mu_livemotion.add_item(&mi_livemotion_exit);

  // Timelapse
  mu_timelapse.add_item(&mi_timelapse_start);
  mu_timelapse.add_item(&mi_timelapse_step_time);
  mu_timelapse.add_item(&mi_timelapse_step_length);
  mu_timelapse.add_item(&mi_timelapse_shutter);
  mu_timelapse.add_item(&mi_timelapse_hard_duty);
  mu_timelapse.add_item(&mi_timelapse_return_to_start);
  mu_timelapse.add_item(&mi_timelapse_exit);

  // settings
  mu_settings.add_item(&mi_settings_rail_length);
  mu_settings.add_item(&mi_settings_wheel_diameter);
  mu_settings.add_item(&mi_settings_motor_spr);
  mu_settings.add_item(&mi_settings_exit);

  //Memmory
  mu_memory.add_item(&mi_memory_save);
  mu_memory.add_item(&mi_memory_clear);
  mu_memory.add_item(&mi_memory_exit);

  // Root
  ms.get_root_menu().add_menu(&mu_livemotion);
  ms.get_root_menu().add_menu(&mu_timelapse);
  ms.get_root_menu().add_menu(&mu_settings);
  ms.get_root_menu().add_menu(&mu_memory);
}

//
// Menu actions
//
//-------------------------------------------------------------------
//-------------------------LIVE MOTION-------------------------------
//-------------------------------------------------------------------
//
void on_livemotion_start(MenuItem* p_menu_item)    // dorobit shutter
{ short microstepping_mode = 32;
  unsigned long steps_by_rail;
  unsigned long speed_constant;
  unsigned long motor_spr = settings_motor_spr;
  unsigned long rail_length = settings_rail_length;
  unsigned long wheel_diameter = settings_wheel_diameter;
  unsigned long lm_speed = livemotion_speed;

  LCD.position(2, 2);
  LCD.string("WAIT!           ");

  if (livemotion_speed < 10) {
    LCD.position(2, 2);
    LCD.string("speed to low    ");
    delay(1000);
  }
  else {
    if (livemotion_speed >= 10 and livemotion_speed < 50) {
      microstepping_mode = 32;
    }
    if (livemotion_speed >= 50 and livemotion_speed < 100) {
      microstepping_mode = 16;
    }
    if (livemotion_speed >= 100 and livemotion_speed < 200) {
      microstepping_mode = 8;
    }
    if (livemotion_speed >= 200 and livemotion_speed < 400) {
      microstepping_mode = 4;
    }
    if (livemotion_speed >= 400 and livemotion_speed < 800) {
      microstepping_mode = 2;
    }
    if (livemotion_speed >= 800 and livemotion_speed < 1000) {
      microstepping_mode = 1;
    }
    noInterrupts();
    speed_constant = 1000000 / ((microstepping_mode * lm_speed * motor_spr) / (3.14 * wheel_diameter));  //*2
    steps_by_rail =  (rail_length * microstepping_mode * motor_spr) / (3.14 * wheel_diameter);
    interrupts();

    microstepping(microstepping_mode);

    digitalWrite(drvEN_PIN, LOW);
    shutter_EN = true;

    if (lm_speed <= 100) {                                                        // forbid "stop by button", when speed is > 100mm/s
      allow_break_by_button = true;
    }
    else {
      allow_break_by_button = false;
    }

    motor_DRV_B(steps_by_rail, speed_constant * 5, speed_constant, speed_constant * 5, 2);

    shutter_EN = true;
    digitalWrite(drvEN_PIN, HIGH);
  }


  LCD.position(2, 2);
  LCD.string("DONE!           ");
  delay(1000);

}

void on_livemotion_speed(MenuItem* p_menu_item)
{
  livemotion_speed = editNumericValue(1, livemotion_speed, 3, UNIT_LIVEMOTION_SPEED);
}

//--------------------------------------------------------------------------------------------
//------------------------------------TIMELAPSE-----------------------------------------------
//--------------------------------------------------------------------------------------------
//
void on_timelapse_start(MenuItem* p_menu_item)
{ 
  short microstepping_mode = 8;
  unsigned long speed_constant = 100;
  short acceleration = 2;
  short acelleration_gradient = 10;
  unsigned long steps_by_rail;
  unsigned long steps_for_step;
  float motor_time;
  float pause_time;
  float pause_time_2 = 0.1;
  unsigned long shutter = timelapse_shutter;
  unsigned long step_time = timelapse_step_time;
  unsigned long step_length = timelapse_step_length;
  unsigned long motor_spr = settings_motor_spr;
  unsigned long rail_length = settings_rail_length;
  unsigned long wheel_diameter = settings_wheel_diameter;

  unsigned long index_a, index_b, index_c, index_d;     // variables for counting motor time


  microstepping(microstepping_mode);
  //--------------------------------------motor time counting-----------------------------------------------------------------------------------

  steps_for_step = (step_length * motor_spr * microstepping_mode) / (3.14 * wheel_diameter); // steps number of the step


//------------------  
  if (steps_for_step <= (((acelleration_gradient - 1) * speed_constant) / 2)  ) {
    
    motor_time = 0.0000025 * steps_for_step * (speed_constant * (acelleration_gradient) + (speed_constant * acelleration_gradient - (steps_for_step - 1) * acceleration));  
  }
//-----------------
  if (steps_for_step > (((acelleration_gradient - 1) * speed_constant) / 2) and steps_for_step <= ((acelleration_gradient - 1) * speed_constant)) {

    index_a = (steps_for_step/2) - ((((acelleration_gradient - 1) * speed_constant) / 2) - (steps_for_step/2));
    index_b = (speed_constant * acelleration_gradient - (index_a - 1)*acceleration);
    index_c = ((steps_for_step/2) - index_a)*(index_b); ///!!!
    index_d = (index_a/2)*(speed_constant * acelleration_gradient + index_b);
    motor_time = 0.000004 * (index_d+index_c);    
  }
//-----------------
  if (steps_for_step > ((acelleration_gradient - 1) * speed_constant)) {
    
    motor_time = 0.000002 * ((49.5 *  speed_constant * speed_constant) + (steps_for_step - 9 * speed_constant) * speed_constant); //time of the motor part of the step
  }

//------------------------------------------------------------------------------------------------------------------------------------------
  if (shutter == 0) {
    pause_time = step_time - motor_time - 0.2 - pause_time_2;
  }
  else {
    pause_time = step_time - motor_time - shutter - pause_time_2;
  }
//   Serial.println(steps_for_step);                       //for debug purpose
//   Serial.println(step_time);
//  Serial.println(motor_time);
//   Serial.println(shutter);
//   Serial.println(pause_time);
  
  

  if (pause_time >= 1 and steps_for_step > 0) {    // protection from wrong time constants and step_length = 0mm
    LCD.position(2, 2);
    LCD.string("WAIT!           ");   

    while (1) {
      digitalWrite(drvEN_PIN, LOW);
      allow_break_by_button = true;

      motor_DRV_B(steps_for_step, speed_constant * acelleration_gradient, speed_constant, speed_constant * acelleration_gradient, acceleration);

      if(timelapse_hd != true){                     // hard duty regime
        digitalWrite(drvEN_PIN, HIGH);
      }

      if (sensor_break_flag == true or break_flag == true) {
        sensor_break_flag = false;
        break_flag = false;
        break;
      }

      pause_DRV(pause_time);
      if (break_flag == true) {
        break_flag = false;
        break;
      }

      shutter_DRV(shutter);
      if (break_flag == true) {
        break_flag = false;
        break;
      }

      pause_DRV(pause_time_2);
      if (break_flag == true) {
        break_flag = false;
        break;
      }
    }
    
    digitalWrite(drvEN_PIN, HIGH);

    microstepping_mode = 8;
    microstepping(microstepping_mode);
    steps_by_rail =  (rail_length * microstepping_mode * motor_spr) / (3.14 * wheel_diameter);
    delay(500);

    if (timelapse_rts == true) {

      allow_break_by_button = false;
      
      digitalWrite(drvEN_PIN, LOW);
      motor_DRV_B(steps_by_rail, speed_constant * acelleration_gradient, speed_constant, speed_constant * acelleration_gradient, 1);
      digitalWrite(drvEN_PIN, HIGH);
    }

    LCD.position(2, 2);
    LCD.string("DONE!            ");
  }
  else {
    LCD.position(1, 1);
    LCD.string("     WRONG     ");
    LCD.position(1, 2);
    LCD.string("TIMELAPSE CONST.");
  }

  delay(1000);
}


//
void on_timelapse_step_time(MenuItem* p_menu_item)
{
  timelapse_step_time = editNumericValue(1, timelapse_step_time, 4, UNIT_TIMELAPSE_STEP_TIME);  //
}

//
void on_timelapse_step_length(MenuItem* p_menu_item)
{
  timelapse_step_length = editNumericValue(1, timelapse_step_length, 4, UNIT_TIMELAPSE_STEP_LENGTH);
}

//
void on_timelapse_shutter_1(MenuItem* p_menu_item)
{
  timelapse_shutter = editNumericValue(1, timelapse_shutter, 4, UNIT_TIMELAPSE_SHUTTER_TIME);
}

//

//
void on_timelapse_hard_duty(MenuItem* p_menu_item)
{
  timelapse_hd = editBoolValue(1, timelapse_hd, TEXT_YES, TEXT_NO);
}

//
void on_timelapse_return_to_start(MenuItem* p_menu_item)
{
  timelapse_rts = editBoolValue(1, timelapse_rts, TEXT_YES, TEXT_NO);
}

//

// settings
//

void on_settings_rail_length(MenuItem* p_menu_item)
{
  settings_rail_length = editNumericValue(1, settings_rail_length, 4, UNIT_SETTINGS_RAIL_LENGTH);
}

void on_settings_wheel_diameter(MenuItem* p_menu_item)
{
  settings_wheel_diameter = editNumericValue(1, settings_wheel_diameter, 3, UNIT_SETTINGS_WHEEL_DIAMETER);  //!!!!!
}

void on_settings_motor_spr(MenuItem* p_menu_item)
{
  settings_motor_spr = editNumericValue(1, settings_motor_spr, 3, UNIT_SETTINGS_MOTOR_SPR);
}

//MEMORY

void on_memory_save(MenuItem* p_menu_item)
{
  EEPROMstore();
}

void on_memory_clear(MenuItem* p_menu_item)
{
  EEPROMclear();
}

void on_exit(MenuItem* p_menu_item)
{
  ms.back();
}

//
// Menu renderer
//

void display_menu(Menu const& menu) {
  LCD.clear_display();
  LCD.position(1, 1);
  LCD.cursor_onoff(0);
  const char* name = menu.get_current_component()->get_name();
  LCD.string(name);

  // Display configuration values

  if (strcmp(name, mi_livemotion_speed.get_name()) == 0) {
    displayNumericValue(1, livemotion_speed, 3, UNIT_LIVEMOTION_SPEED);
  }

  if (strcmp(name, mi_timelapse_step_time.get_name()) == 0) {
    displayNumericValue(1, timelapse_step_time, 4, UNIT_TIMELAPSE_STEP_TIME);
  }
  if (strcmp(name, mi_timelapse_step_length.get_name()) == 0) {
    displayNumericValue(1, timelapse_step_length, 4, UNIT_TIMELAPSE_STEP_LENGTH);
  }
  if (strcmp(name, mi_timelapse_shutter.get_name()) == 0) {
    displayNumericValue(1, timelapse_shutter, 4, UNIT_TIMELAPSE_SHUTTER_TIME);
  }

  if (strcmp(name, mi_timelapse_return_to_start.get_name()) == 0) {
    displayBoolValue(1, timelapse_rts, TEXT_YES, TEXT_NO);
  }
  if (strcmp(name, mi_timelapse_hard_duty.get_name()) == 0) {
    displayBoolValue(1, timelapse_hd, TEXT_YES, TEXT_NO);
  }
  if (strcmp(name, mi_settings_rail_length.get_name()) == 0) {
    displayNumericValue(1, settings_rail_length, 4, UNIT_SETTINGS_RAIL_LENGTH);
  }
  if (strcmp(name, mi_settings_wheel_diameter.get_name()) == 0) {
    displayNumericValue(1, settings_wheel_diameter, 3, UNIT_SETTINGS_WHEEL_DIAMETER);
  }
  if (strcmp(name, mi_settings_motor_spr.get_name()) == 0) {
    displayNumericValue(1, settings_motor_spr, 3, UNIT_SETTINGS_MOTOR_SPR);
  }

}

