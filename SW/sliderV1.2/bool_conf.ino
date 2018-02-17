int editBoolValue(int cursorStart, bool value, char *trueText, char *falseText) {
  // Display value
  displayBoolValue(cursorStart, value, trueText, falseText);

  // "Enter" symbol
  int enterPosition = cursorStart;
  int textLength;
  if (strlen(trueText) > strlen(falseText)) {
    textLength = strlen(trueText);
    enterPosition += textLength + 1;
  }
  else {
    textLength = strlen(falseText);
    enterPosition += textLength + 1;
  }
  LCD.position(enterPosition, 2);
  LCD.ascii(0);

  LCD.position(cursorStart, 2);

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
      case 1:
      case 3:
        value = !value;
        for(int i=1; i<=textLength; i++) {
          LCD.position(i, 2);
          LCD.string(" ");
        }
        displayBoolValue(cursorStart, value, trueText, falseText);
        break;
      case 2:
        // Turn off Enter symbol
        LCD.ascii(0);
        exitFlag = 1;
        break;
    }
  }
  while (exitFlag != 1);

 // Serial.println(value);
  return value;
}

/*
   Write shutter value to LCD
*/
void displayBoolValue(int cursorStart, bool value,  char *trueText, char *falseText) {
  LCD.position(cursorStart, 2);
  if (value) {
    LCD.string(trueText);
  }
  else {
    LCD.string(falseText);
  }
}

