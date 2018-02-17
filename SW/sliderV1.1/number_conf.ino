unsigned int editNumericValue(int cursorStart, unsigned int value, int digits, char *unit) {
  const int MAX_POSITION = digits;
  int position = 0; // Start with numerator

  // Write the shutter value
  displayNumericValue(cursorStart, value, digits, unit);

  // "Enter" symbol
  int enterPosition = cursorStart + digits + strlen(unit) + 2;
  LCD.position(enterPosition, 2);
  LCD.ascii(0);

  LCD.position(cursorStart, 2);

  // Convert values to single digits

  int digitsSettings[digits];

  util_int2digits(digitsSettings, digits, value);

  int button;
  do {                              // caka sa kym sa neuvolni tlacitko 2 po zvoleni polozky v menu
    button = button_read_digits();
    delay(100);
  }
  while (button == 2);

  int exitFlag = 0;
  do {
    LCD.command(0x0E);            // nastavenie kursoru typu bar. Vid EADOGM162 datasheet.
    LCD.position(getDisplayPosition(position, cursorStart, digits, enterPosition), 2);

    button = button_read_digits();
    // Serial.print(getDisplayPosition(position, cursorStart, digits, enterPosition));//
    delay(100);
    // posuv pozicie kurzoru

    switch (button) {

      case 1:
        if (position < MAX_POSITION)
          position++;
        else
          position = 0;
        break;
      case 2:
        if (position == MAX_POSITION) {
          LCD.ascii(0);
          exitFlag = 1;
        }
        else {

          if (position >= 0 && position < MAX_POSITION) {
            int digit = digit_set(getDisplayPosition(position, cursorStart, digits, enterPosition), digitsSettings[position], 0, 9);
            digitsSettings[position] = digit;
          }


        }
        break;
      case 3:
        if (position > 0)
          position--;
        else
          position = MAX_POSITION;
        break;
    }

  }
  while (exitFlag != 1);

  unsigned int result = util_digits2int(digits, digitsSettings);
  // Serial.println(result);
  return result;
}

/**
   @param position  position within the array (starts with 0)
*/
int getDisplayPosition(int position, int cursorStart, int digits, int enterPosition) {
  if (position < digits) {
    return cursorStart + position;
  }
  else {
    return enterPosition;
  }
}

/*
   Write numeric value to LCD
*/
void displayNumericValue(int cursorStart, unsigned int value, int digits, char *unit) {
  LCD.position(cursorStart, 2);
  char valueStr[digits];
  char format[4];
  strcpy(format, "%0");
  char num[2];
  sprintf(num, "%d", digits);
  strcat(format, num);
  strcat(format, "d");
  sprintf(valueStr, format, value);
  LCD.string(valueStr);                               // 0
  LCD.string(" ");
  LCD.string(unit);                                        // 1
}

