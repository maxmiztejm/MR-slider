void initialize_display() {
  LCD.initialize(9, 7, 6, 8, 0, 1, 2); //SS = 10, 0,0= use Hardware SPI, 9 = RS, 4= RESET, 1 = 5V, EA LCDM162-A (=2 lines)
  LCD.displ_onoff(true);          //turn Display on
  LCD.cursor_onoff(false);         //turn Curosor blinking off

  pinMode(BUTTON_PIN, INPUT);
  analogReference(EXTERNAL);
//  Serial.begin(9600);           // for debug purpose

  LCD.clear_display();            //Clear the whole content
  LCD.define_char(0, symbol_enter); //ulozenie symbolu ↵ do pamate displeja na poziciu 0
  LCD.define_char(1,symbol_plus_minus);
  LCD.position(1, 1);             //set Position: first line, first character
  LCD.string(" MR_Slider V1.1");
  LCD.contrast(225); /////////////////////////////////
  backlight(255);
  delay(100);
//  ms.display();
 
}

void backlight(byte brightness)  //  backlight declaration
{
  analogWrite(LED_PIN, brightness);
}

/**
   Write one digit
*/
void lcd_write_digit(int digit) {
  LCD.ascii(digit + 48);
}

/**
   Write more digits
*/
void lcd_write_digits(int cursor_start, int length, int *digits) {
  for (int i = 0; i < length; i++) {
    LCD.position(cursor_start + i, 2);
    lcd_write_digit(digits[i]);
  }
}

void lcd_write_number(int cursor_start, int length, int number) {
  char val_str[length];
  sprintf(val_str, "%d", number);
  int actual_length = strlen(val_str);
  int cursor = cursor_start;
  for (int i = 0; i < (length - actual_length); i++) {
    LCD.position(cursor++, 2);
    LCD.string("0");
  }
  LCD.position(cursor, 2);
  LCD.string(val_str);
}

