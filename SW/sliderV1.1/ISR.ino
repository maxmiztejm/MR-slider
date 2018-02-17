//--------------------------------------------------------------------
//----------INTERRUPTS FOR MOTOR, SHUTTER AND PAUSE CONTROLL----------
//--------------------------------------------------------------------

void timerISR(void) {
  noInterrupts();
  Timer1.setPeriod(SH_time);

  //-------------------------------------------------
  if (shutter_EN == true) {

    if (shutter_PIN_state == LOW) {
      shutter_PIN_state = HIGH;
    }
    else {
      shutter_PIN_state = LOW;
    }

    digitalWrite(Shutter_PIN, shutter_PIN_state);
    shutter_EN = false;
  }
  //--------------------------------------------------
  if (motor_EN == true) {

    if (motor_PIN_state == LOW) {
      motor_PIN_state = HIGH;
    }
    else {
      motor_PIN_state = LOW;
    }

    digitalWrite(Motor_PIN, motor_PIN_state);
    motor_EN = false;
  }
  //-------------------------------------------------
  if (break_once_flag == true){
  delay_button_counter++;}
  
  IsrFlag = true;
  interrupts();
}
