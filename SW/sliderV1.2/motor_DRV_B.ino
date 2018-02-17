//------------------------------------------------------------------------------------
//----------------------MOTOR DRIVER B------------------------------------------------
//------------------------------------------------------------------------------------
void motor_DRV_B(unsigned long steps, unsigned int start_duty, unsigned int main_duty, unsigned int end_duty, unsigned int speedy) {

  sensor_break_flag = false;
  steps_FN_total = 0;
  time_FN_total = 0;
  digitalWrite(DIR_PIN, DIR_PIN_state);


  int delay_sensor_counter = 0;
  int Sensor_PIN_state;
  int Sensor_PIN_state_prev;

  unsigned long stepsFN = 0;
  unsigned long step_time = 0;

  unsigned long duty = start_duty;
  unsigned long steps1 = round((start_duty - main_duty) / speedy);
  unsigned long steps2 = steps - round((end_duty - main_duty) / speedy);

  motor_EN = true;

  while (stepsFN != steps) {
    SH_time = duty;

    if (IsrFlag == true) {


      motor_EN = true;
      IsrFlag = false;
      if (motor_PIN_state == HIGH) {
        IsrFlag = false;
        steps_FN_total = stepsFN + 1;
        stepsFN = stepsFN + 1;
        time_FN_total = time_FN_total + duty;
        
      }
      else {
        
        
        
        if (stepsFN <= steps1 ) {
          duty = duty - speedy;
         // Serial.println(1);
        }
        if (stepsFN > steps1 and stepsFN <= steps2) {
          duty = duty;
         // Serial.println(2);
        }
        if (stepsFN > steps2 and stepsFN <= steps) {
          duty = duty + speedy;
         // Serial.println(3);
        }


      }
    }
    //-----------break by button routine------------------------------------------------------------
    if (stepsFN > 200 and allow_break_by_button == true) {    // protection from stopping imediatly after start motor routine
      break_by_button();
      if (break_flag == true) {
        break;
      }
    }
    //-----------break by sensor routine-------------------------------------------------------------
    break_by_sensor();
    if (sensor_break_flag == true) {
      break;
    }



    //------------------------------------------------------------------------

  }
  motor_EN = true;

  while (IsrFlag == false) {}

}
