//------------------------------------------------------------------------------------
//-----------------------------BREAK BY BUTTON ROUTINE--------------------------------   
//------------------------------------------------------------------------------------

void break_by_button(void) {
  int read_digit;
  break_flag = false;
  read_digit=button_read_digits();

  if (break_once_flag == true) {
 
  if (delay_button_counter > 10000) {           // time for canceling routine interruption by buttons
    delay_button_counter = 0;
    LCD.position(2, 2);
    LCD.string("                ");          

    break_once_flag = false;
    break_flag = false;
  }}

  if (read_digit == 2) {

    if (break_once_flag == true) {

      if (delay_button_counter > 1000) {           // function for the delay reaction to button event  1000
        LCD.position(2, 2);
        LCD.string("STOPED!         ");
        
        if (timelapse_rts == true) {               // switch direction when "return to start" setting is "YES" 
          DIR_PIN_state = DIR_PIN_state ^ 1;
        }
        
        delay_button_counter = 0;
        break_flag = true;
        break_once_flag = false;
      }

    }
    else {
      LCD.position(2, 2);
      LCD.string("STOP?           ");             
      break_once_flag = true;
      break_flag = false;
     
    }
  }
  else {
    break_flag = false;
  }
}

//------------------------------------------------------------------------------------
//-----------------------------BREAK BY SENSOR ROUTINE--------------------------------
//------------------------------------------------------------------------------------
void break_by_sensor(void) {

  Sensor_PIN_state_prev = Sensor_PIN_state;

  Sensor_PIN_state = digitalRead(Sensor_PIN);
  if (Sensor_PIN_state_prev == 1 and Sensor_PIN_state == 0) {

    if (DIR_PIN_state == HIGH) {
      Sensor_right = true;
      Sensor_left = false;
      sensor_break_flag = true;
    }

    else {
      Sensor_right = false;
      Sensor_left = true;
      sensor_break_flag = true;
    }
    DIR_PIN_state = DIR_PIN_state ^ 1;
  }


}



//------------------------------------------------------------------------------------
//--------------------SHUTTER DRIVER--------------------------------------------------
//------------------------------------------------------------------------------------
void shutter_DRV(unsigned long shutter_time) {
  int time_counter = 0;
  short konstant;

  if (shutter_time == 0) {
    shutter_time = 1;
    SH_time = 20000;
    konstant = 1;
  }
  else {
    SH_time = 1000;
    konstant = 1000;
  }

  shutter_EN = true;
  timerISR();

  while (time_counter <= shutter_time*konstant) {

    if (IsrFlag == true) {
     
      time_counter++;
      IsrFlag = false;

      if (time_counter == shutter_time*konstant) {
        SH_time = 2000;
        shutter_EN = true;
      }
    }
    //-----------------------------------------------------------------------
    break_by_button();
    if (break_flag == true) {

      break;
    }
    //------------------------------------------------------------------------

  }
  if (break_flag != true) {
    while (IsrFlag == false ) {}
  }
  else {
    shutter_EN = true;
    timerISR();
  }
}
//------------------------------------------------------------------------------------
//-----------------------PAUSE DRIVER-------------------------------------------------
//------------------------------------------------------------------------------------
void pause_DRV(float pause_time) {
  unsigned long time_counter = 0;


  SH_time = 1000;
   shutter_EN = false;       
  timerISR();


  while (time_counter < pause_time*1000) {

    if (IsrFlag == true) {

      time_counter = time_counter + 1;
      IsrFlag = false;


      if (time_counter == pause_time*1000) {  //pokus!
        SH_time = 100;
      
      }


    }
    //-----------------------------------------------------------------------
    break_by_button();
    if (break_flag == true) {

      break;
    }
    //------------------------------------------------------------------------

  }
  if (break_flag != true) {
    while (IsrFlag == false ) {}
  }
  
}

//------------------------------------------------------------------------------------
//----------------------MOTOR DRIVER--------------------------------------------------
//------------------------------------------------------------------------------------
void motor_DRV(unsigned long steps, short start_duty, short end_duty, short speedy) {
  
  sensor_break_flag = false;
  steps_FN_total = 0;
  digitalWrite(DIR_PIN, DIR_PIN_state);


  int delay_sensor_counter = 0;
  int Sensor_PIN_state;
  int Sensor_PIN_state_prev;

  unsigned long stepsFN = 0;
  unsigned long step_time = 0;

  unsigned int duty1;
  unsigned int duty2;
  unsigned int duty_old;

  bool Increment;
  bool Accel;

  duty1 = start_duty;
  duty2 = end_duty;

  //-----------------------------------------------
  if (start_duty < end_duty) {
    Increment = true;
    Accel = true;
  }
  else if (start_duty > end_duty) {
    Increment = false;
    Accel = true;
  }
  else {
    Increment = false;
    Accel = false;
  }
  //-----------------------------------------------

  motor_EN = true;

  while (stepsFN != steps) {
    SH_time = duty1;

    if (IsrFlag == true) {


      motor_EN = true;
      IsrFlag = false;
      if (motor_PIN_state == HIGH) {
        IsrFlag = false;
        steps_FN_total = stepsFN + 1;
        stepsFN++;

      }
      else {
        if (Accel == true and duty1 != duty2) {
          if (Increment == true) {
            duty1 = duty1 + speedy;
          }
          else {
            duty1 = duty1 - speedy;
          }
        }
      }
    }
    //-----------break by button routine------------------------------------------------------------
    if (stepsFN > 200) {                                                 // protection from stopping imediatly after start motor routine
      break_by_button();
      if (break_flag == true) {
        analogWrite(LED_PIN, 255);//////////////backlight pokus
        break;
      }
    }
    //-----------break by sensor routine-------------------------------------------------------------
    break_by_sensor();
    if (sensor_break_flag == true) {
      analogWrite(LED_PIN, 255);////////////////backlight pokus
      break;
    }



    //------------------------------------------------------------------------

  }
  motor_EN = true;

  while (IsrFlag == false) {}
analogWrite(LED_PIN, 255);///////////////backlight pokus
}

//------------------------------------------------------------------------------------
//----------------------MICROSTEP REGIME----------------------------------------------
//------------------------------------------------------------------------------------
void microstepping(short step_fraction) {

  switch (step_fraction){
  case 2:{
     digitalWrite(M0_PIN, HIGH);   
     digitalWrite(M1_PIN, LOW);
     digitalWrite(M2_PIN, LOW); 
    break;}  
  case 4:{                       
     digitalWrite(M0_PIN, LOW);   
     digitalWrite(M1_PIN, HIGH);
     digitalWrite(M2_PIN, LOW); 
    break;}  
  case 8:{                        
     digitalWrite(M0_PIN, HIGH);   
     digitalWrite(M1_PIN, HIGH);
     digitalWrite(M2_PIN, LOW); 
    break;}  
  case 16:{
     digitalWrite(M0_PIN, LOW);   
     digitalWrite(M1_PIN, LOW);
     digitalWrite(M2_PIN, HIGH); 
    break;}  
  case 32:{
     digitalWrite(M0_PIN, HIGH);   
     digitalWrite(M1_PIN, LOW);
     digitalWrite(M2_PIN, HIGH); 
    break;}  
  default :{
     digitalWrite(M0_PIN, LOW);   
     digitalWrite(M1_PIN, LOW);
     digitalWrite(M2_PIN, LOW); 
    break;}   
  }
  }
