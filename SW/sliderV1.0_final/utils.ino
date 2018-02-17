/**
   @param result
   @param size    number of digits
   @param number  number which you want to split to digits
*/
void util_int2digits(int *result, int size, int number) {
  int i = size - 1;
  while ((number > 0) || (i >= 0)) {
    if ((number == 0) && (i >= 0)) {
      result[i--] = 0;
    }
    else {
      result[i--] = number % 10;
      number /= 10;
    }
  }
}

/*
   Convert digits for integer value
*/
unsigned int util_digits2int(int size, int* digits) {
  unsigned long result = 0;
  result = *(digits + 0); // position 0
  for (int i = 0; i < size - 1; i++) {
    result = 10 * result + *(digits + i + 1);
  }
  return result;
}

