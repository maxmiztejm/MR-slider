/////////////////////////////////////////////////////////
// funkcia nastavenie cislice
/////////////////////////////////////////////////////////
int digit_set(int pozicia, int digit, int digitmin, int digitmax)
{
  int button;

  LCD.cursor_onoff(1);
  //
  do {
    LCD.position(pozicia, 2);
    button = button_read_digits();
    delay(100);
    if (button == 1)
      if (digit < digitmax)
        digit++;
      else
        digit = digitmin;
    if (button == 3)
      if (digit > digitmin)
        digit--;
      else
        digit = digitmax;
    lcd_write_digit(digit);
  }
  while (button != 2);
  return digit;
}

