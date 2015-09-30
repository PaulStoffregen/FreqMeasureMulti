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
  delay(10);
  Serial.println("FreqMeasureMulti Begin");
  delay(10);
  freq1.begin(6);
  freq2.begin(9);
  freq3.begin(10);
}

float sum1=0, sum2=0, sum3=0;
int count1=0, count2=0, count3=0;
elapsedMillis timeout;

void loop() {
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

