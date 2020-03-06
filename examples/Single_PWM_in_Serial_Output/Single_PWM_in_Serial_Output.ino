/* FreqMeasureMulti - Example with serial output
   http://www.pjrc.com/teensy/td_libs_FreqMeasure.html

   This example code is in the public domain.
*/
#include <FreqMeasureMulti.h>

// Measure 3 frequencies at the same time! :-)
FreqMeasureMulti freq1;

// Try different values for which type of measure:
#define MEASURE_PIN 5
#define MEASURE_TYPE FREQMEASUREMULTI_RAISING
#if defined(__IMXRT1062__)
#define PWM_OUT_PIN 10   // not a flexPWM pin 
#else
#define PWM_OUT_PIN 7  // not an FTM0 pin... 
#endif
//    FREQMEASUREMULTI_RAISING 
// T3.6: 60.00(2:1000000:16.67 2:1000000:16.67 2:1000000:16.67 2:1000000:16.67 )
// T4:   60.02(2:2490368:16.60 2:2490368:16.60 2:2490368:16.60 2:2490368:16.60 )
//    FREQMEASUREMULTI_FALLING 
// T3.6:  60.0(1:1000000:16.67 1:1000000:16.67 1:1000000:16.67 1:1000000:16.67 )
// T4:   60.02(1:2490368:16.60 1:2490368:16.60 1:2490368:16.60 1:2490368:16.60 )
//    FREQMEASUREMULTI_INTERLEAVE 
// T3.6: 60.00(2:1000000:16.67 1:1000000:16.67 2:1000000:16.67 1:1000000:16.67 )
// T4   60.02(1:2490368:16.60 2:2490368:16.60 1:2490368:16.60 2:2490368:16.60 )
//    FREQMEASUREMULTI_SPACE_ONLY 
// T3.6:63.08(0:951184:15.85 0:951184:15.85 0:951184:15.85 0:951184:15.85 )
// T4:  63.05(0:2359296:15.73 0:2359296:15.73 0:2424832:16.17 0:2424832:16.17 )
//    FREQMEASUREMULTI_MARK_ONLY 
// T3.6: 1229.11(3:48816:0.81 3:48816:0.81 3:48816:0.81 3:48816:0.81 )
// T4: 1226.16(3:65536:0.44 3:131072:0.87 3:131072:0.87 3:131072:0.87 )
//    FREQMEASUREMULTI_ALTERNATE 
// T3.6: 120.00(3:48816:0.81 0:951184:15.85 3:48816:0.81 0:951184:15.85 )
// T4  : 120.04(3:131072:0.87 0:2359296:15.73 3:131072:0.87 0:2359296:15.73 )

void setup() {
  Serial.begin(57600);
  while (!Serial) ; // wait for Arduino Serial Monitor
  analogWriteResolution(10);
  analogWriteFrequency(PWM_OUT_PIN, 60);
  analogWrite(PWM_OUT_PIN, 50);

  delay(10);
  Serial.println("FreqMeasureMulti Begin");
  delay(10);
  freq1.begin(MEASURE_PIN, MEASURE_TYPE);
}

float sum1 = 0;
int count1 = 0;
int freq1Reads[4];
uint8_t levels[4];
elapsedMillis timeout;

void loop() {
  if (Serial.available()) {
    uint16_t analog_write_val = 0;
    uint16_t val = 0;
    int ch;
    while ((ch = Serial.read()) != -1) {
      if ((ch == ' ') || (ch == ',')) {
        analog_write_val = val;
        val = 0;
      } else if ((ch >= '0') && (ch <= '9')) {
        val = val * 10 + ch - '0';
      }
    }
    // Yah quick and dirty
    analogWriteFrequency(PWM_OUT_PIN, val);
    if (analog_write_val) {
      analogWrite(PWM_OUT_PIN, analog_write_val);
    }
  }
  if (freq1.available()) {
    for (uint8_t i = 3; i > 0; i--) {
      freq1Reads[i] = freq1Reads[i - 1];
      levels[i] = levels[i - 1];
    }
    freq1Reads[0] = freq1.read();
    levels[0] = freq1.readLevel();
    sum1 = sum1 + freq1Reads[0];
    count1 = count1 + 1;
  }
  // print results every half second
  if (timeout > 500) {
    if (count1 > 0) {
      Serial.print(freq1.countToFrequency(sum1 / count1));
      Serial.print("(");
      for (uint8_t i = 0; i < 4; i++)
        Serial.printf("%u:%u:%.2f ", levels[i], freq1Reads[i], freq1.countToNanoseconds(freq1Reads[i])/1000000.0);
      Serial.print(")");
    } else {
      Serial.print("(no pulses)");
    }
    Serial.println();
    sum1 = 0;
    count1 = 0;
    timeout = 0;
  }
}
