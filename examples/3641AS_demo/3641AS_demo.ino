#include <Geegrow_3641AS.h>

Geegrow_3641AS *display;

void setup() {
  Wire.begin();

//  display = new Geegrow_3641AS(0x21);
  display = new Geegrow_3641AS(0x21, 4);

//  display->setDevAddr(0x21);
  display->clear();

  // Show 24.91
  display->setDigit(0, 2, false);
  display->setDigit(1, 4, true);
  display->setDigit(2, 9, false);
  display->setDigit(3, 1, false);
  delay(1000);
  // Remove all dots
  display->resetDot(0);
  display->resetDot(1);
  display->resetDot(2);
  display->resetDot(3);
  // Show 1234
  display->setAllDigits(1, 2, 3, 4);
  delay(1000);
  // Show 2456
  uint8_t data[] = {2, 4, 5, 6};
  display->setAllDigits(data);
  delay(1000);
  // Show ints and floats
  display->setInt(-777);
  delay(1000);
  display->setInt(66);
  delay(1000);
  display->setInt(1234);
  delay(1000);
  display->setFloat(3.691, 1);
  delay(1000);
  display->setFloat(-3.691, 2);
  delay(1000);
  display->setInt(9999);
  delay(1000);
}

void loop() {
//  display->setDot(0);
//  display->setDot(1);
//  display->setDot(2);
//  display->setDot(3); 
//  delay(1000);
//  display->resetDot(0);
//  display->resetDot(1);
//  display->resetDot(2);
//  display->resetDot(3);
//  delay(1000);

  // Show increment by seconds
  static uint16_t counter = 0;
  display->setInt(counter++);
  if (counter == 10000)
    counter = 0;
  delay(1000);
}
