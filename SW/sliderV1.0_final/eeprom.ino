//
// Functions for reading and writing from/to EEPROM
//

/**
   Write all configuration values to EEPROM
*/
void EEPROMstore() {
  EEPROM.updateBlock(EE_ADDR_TIMELAPSE_STEP_TIME, timelapse_step_time);
  EEPROM.updateBlock(EE_ADDR_TIMELAPSE_STEP_LENGTH, timelapse_step_length);
  EEPROM.updateBlock(EE_ADDR_TIMELAPSE_SHUTTER, timelapse_shutter);
  EEPROM.updateBlock(EE_ADDR_TIMELAPSE_HD, timelapse_hd);
  EEPROM.updateBlock(EE_ADDR_TIMELAPSE_RTS, timelapse_rts);
  EEPROM.updateBlock(EE_ADDR_SETTINGS_RAIL_LENGTH, settings_rail_length);
  EEPROM.updateBlock(EE_ADDR_SETTINGS_WHEEL_DIAMETER, settings_wheel_diameter);
  EEPROM.updateBlock(EE_ADDR_SETTINGS_MOTOR_SPR, settings_motor_spr);
  EEPROM.updateBlock(EE_ADDR_LIVEMOTION_SPEED, livemotion_speed);
  EEPROM.updateBlock(EE_ADDR_INTERNAL_MOTOR_STEPS, internal_motor_steps);

  LCD.position(1, 2);
  LCD.string("Settings saved");
  delay(1000);
  LCD.position(1, 2);
  LCD.string("                ");
}

/**
   Read all configuration values from EEPROM
*/
void EEPROMread() {
  EEPROM.readBlock(EE_ADDR_TIMELAPSE_STEP_TIME, timelapse_step_time);
  EEPROM.readBlock(EE_ADDR_TIMELAPSE_STEP_LENGTH, timelapse_step_length);
  EEPROM.readBlock(EE_ADDR_TIMELAPSE_SHUTTER, timelapse_shutter);
  EEPROM.readBlock(EE_ADDR_TIMELAPSE_HD, timelapse_hd);
  EEPROM.readBlock(EE_ADDR_TIMELAPSE_RTS, timelapse_rts);
  EEPROM.readBlock(EE_ADDR_SETTINGS_RAIL_LENGTH, settings_rail_length); 
  EEPROM.readBlock(EE_ADDR_SETTINGS_WHEEL_DIAMETER, settings_wheel_diameter);
  EEPROM.readBlock(EE_ADDR_SETTINGS_MOTOR_SPR, settings_motor_spr);
  EEPROM.readBlock(EE_ADDR_LIVEMOTION_SPEED, livemotion_speed);
  EEPROM.readBlock(EE_ADDR_INTERNAL_MOTOR_STEPS, internal_motor_steps);
}

void EEPROMclear() {
  LCD.position(1, 2);
  LCD.string("wait...");
  for (int i = 0 ; i < EE_SIZE; i++) {
    EEPROM.updateByte(i, 0);
  }
  LCD.position(1, 2);
  LCD.string("Settings cleared");
  delay(1000);
  LCD.position(1, 2);
  LCD.string("                ");
}
/*
   Initialize variables holding the configuration settings
*/

void main_initialize_memory() {
  // Read all configuration values from EEPROM here:
  EEPROM.setMaxAllowedWrites(10000);
  EEPROMread();
}
