/**
   Supports values 0 - 9 (only one digit)
*/
int editNumberEnumValue(int cursorStart, int value, int *valuesArray, int size) {
  // Display value
  lcd_write_number(cursorStart, 1, value);

  // "Enter" symbol
  int enterPosition = cursorStart + 2;
  LCD.position(enterPosition, 2);
  LCD.ascii(0);

  LCD.position(cursorStart, 2);

  // Find selected value
  int position = 0;
  for (int i = 0; i < size; i++) {
    if (valuesArray[i] == value) {
      position = i;
      break;
    }
  }

  int button;
  do {                              // caka sa kym sa neuvolni tlacitko 2 po zvoleni polozky v menu
    button = button_read_digits();
    delay(100);
  }
  while (button == 2);

  int exitFlag = 0;
  do {
    //    LCD.command(0x0E);            // nastavenie kursoru typu bar. Vid EADOGM162 datasheet.

    button = button_read_digits();
    delay(100);
    // posuv pozicie kurzoru

    switch (button) {
      case 1: // Up
        position++;
        if(position >= size) {
          position = 0; 
        }
        value = valuesArray[position];
        lcd_write_number(cursorStart, 1, value);
        break;
      case 3: // Down
        position--;
        if(position < 0) {
          position = size - 1; 
        }
        value = valuesArray[position];
        lcd_write_number(cursorStart, 1, value);
        break;
      case 2:
        // Turn off Enter symbol
        LCD.ascii(0);
        exitFlag = 1;
        break;
    }
  }
  while (exitFlag != 1);

  //Serial.println(value);
  return value;
}

