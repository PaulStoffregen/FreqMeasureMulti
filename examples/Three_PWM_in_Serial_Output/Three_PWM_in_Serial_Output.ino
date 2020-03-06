/* FreqMeasureMulti - Example with serial output
 * http://www.pjrc.com/teensy/td_libs_FreqMeasure.html
 *
 * This example code is in the public domain.
 */
#include <FreqMeasureMulti.h>

// Measure 3 frequencies at the same time! :-)
FreqMeasureMulti freq1;
FreqMeasureMulti freq2;
FreqMeasureMulti freq3;

void setup() {
  Serial.begin(57600);
  while (!Serial) ; // wait for Arduino Serial Monitor
  analogWriteResolution(10);
  analogWriteFrequency(10, 60);
  analogWrite(10, 50);
  analogWriteFrequency(11, 100);
  analogWrite(11, 50);
  analogWriteFrequency(12, 200);
  analogWrite(12, 50);
  
  delay(10);
  Serial.println("FreqMeasureMulti Begin");
  delay(10);
  freq1.begin(7);
  freq2.begin(8);
  freq3.begin(0);
}

float sum1=0, sum2=0, sum3=0;
int count1=0, count2=0, count3=0;
elapsedMillis timeout;

void loop() {
  if (Serial.available()) {
    uint16_t analog_write_val=0;
    uint16_t val = 0;
    int ch;
    while ((ch = Serial.read()) != -1) {
      if((ch == ' ') || (ch == ',')) {
        analog_write_val= val;
        val = 0;        
      } else if ((ch >= '0') && (ch <= '9')){
        val = val *10 + ch - '0';
      }
    }
    // Yah quick and dirty
    analogWriteFrequency(10, val);
    if (analog_write_val) {
      analogWrite(10, analog_write_val);
    }   
  }
  if (freq1.available()) {
    sum1 = sum1 + freq1.read();
    count1 = count1 + 1;
  }
  if (freq2.available()) {
    sum2 = sum2 + freq2.read();
    count2 = count2 + 1;
  }
  if (freq3.available()) {
    sum3 = sum3 + freq3.read();
    count3 = count3 + 1;
  }
  // print results every half second
  if (timeout > 500) {
    if (count1 > 0) {
      Serial.print(freq1.countToFrequency(sum1 / count1));
    } else {
      Serial.print("(no pulses)");
    }
    Serial.print(",  ");

    if (count2 > 0) {
      Serial.print(freq2.countToFrequency(sum2 / count2));
    } else {
      Serial.print("(no pulses)");
    }
    Serial.print(",  ");
    if (count3 > 0) {
      Serial.print(freq3.countToFrequency(sum3 / count3));
    } else {
      Serial.print("(no pulses)");
    }
    Serial.println();
    sum1 = 0;
    sum2 = 0;
    sum3 = 0;
    count1 = 0;
    count2 = 0;
    count3 = 0;
    timeout = 0;
  }
}
