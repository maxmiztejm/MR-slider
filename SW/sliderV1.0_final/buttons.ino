
/////////////////////////////////////////////////////////////////////
//funkcia nacitania tlacidiel pre pohyb v menu
/////////////////////////////////////////////////////////////////////
int button_read_menu() {
  ADCSRA |=  1<<7;                                  //enable ADC
  char text4 [10];
  static int AnRead;
  static int val;
  AnRead = analogRead(BUTTON_PIN);
  Menu const* cp_menu = ms.get_current_menu();

  if (AnRead <= 200 && val != 1) {
    val = 1;
    ms.prev();
    ms.display();
    ADCSRA &= ~(1<<7);                               //disable ADC                       
    return 1;
  }
  if (AnRead > 200 && AnRead <= 400 && val != 2) {
    val = 2;
    ms.select();
    ms.display();
    ADCSRA &= ~(1<<7);                              //disable ADC
    return 2;
  }
  if (AnRead > 400 && AnRead <= 800 && val != 3) {
    val = 3;
    ms.next();
    ms.display();
    ADCSRA &= ~(1<<7);                             //disable ADC
    return 3;
  }
  if (AnRead > 800 and AnRead <= 1024) {
    val = 0;
    ADCSRA &= ~(1<<7);                             //disable ADC
    return 0;
  }
  ADCSRA &= ~(1<<7);                               //disable ADC
  return 0;
}

////////////////////////////////////////////////////////////
//funkcia nacitania tlacidiel pre nastavenia cisiel
///////////////////////////////////////////////////////////
int button_read_digits() {
  ADCSRA |=  1<<7;                                //enable ADC
  char text4 [10];
  static int AnRead;
  static int val;
  AnRead = analogRead(BUTTON_PIN);

  if (AnRead <= 200 && val != 1) {                 //200
    val = 1;
    ADCSRA &= ~(1<<7);                             //disable ADC
    return 1;
  }
  if (AnRead > 250 && AnRead <= 450 && val != 2) {  //200-400
    val = 2;
    ADCSRA &= ~(1<<7);                             //disable ADC
    return 2;
  }
  if (AnRead > 550 && AnRead <= 800 && val != 3) { //400-800
    val = 3;
    ADCSRA &= ~(1<<7);                            //disable ADC
    return 3;
  }
  if (AnRead > 800 and AnRead <= 1024) {          //800-1024
    val = 0;
    ADCSRA &= ~(1<<7);                           //disable ADC
    return 0;
  }
  ADCSRA &= ~(1<<7);                             //disable ADC
  return 0;
}

