#FreqMeasureMulti Library#

FreqMeasureMulti basically measures the elapsed time during each cycle of an input frequency, that means the number of timer ticks between a raising ramp of the input signal and the next. Extended capture modes which allow a more detailed analysis of the input signal have been added in v0.2 (see below).

Up to 8 frequencies can be measured simultaneously.

    Board            Supported Pins
    -----            --------------
    Teensy LC        6, 9, 10, 20, 22, 23
    Teensy 3.1       5, 6, 9, 10, 20, 21, 22, 23
    Teensy 3.2       5, 6, 9, 10, 20, 21, 22, 23

FreqMeasureMulti uses the same functions as FreqMeasure, except begin() must specify a pin number.  Because more than 1 input can be measured, you must create FreqMeasureMulti instances.  See the Serial_Output example.

Extended in v0.22 :

As in the v0.1, an instance can be started with begin(pin). More capture modes are available through begin(pin, mode) by using the following constants for mode as follows :
* FREQMEASUREMULTI_RAISING -> A new value becomes available at each rising ramp and contains the time since the previous rising ramp (default value when mode is not given in begin(pin))
* FREQMEASUREMULTI_FALLING ->  A new value becomes available at each falling ramp and contains the time since the previous falling ramp.
* FREQMEASUREMULTI_INTERLEAVE -> A new value becomes available at each rising and each falling ramp and contains the time since the previous similar (rising or falling) ramp. That virtually doubles the actualization rate and allows quicker tracking of variations in the signal frequency.
* FREQMEASUREMULTI\_SPACE\_ONLY -> A new value becomes available at each rising ramp and contains the time since the previous falling ramp. Thus, only the "low time" of the signal period is returned
* FREQMEASUREMULTI\_MARK\_ONLY -> A new value becomes available at each falling ramp and contains the time since the previous rising ramp. Thus, only the "high time" of the signal period is returned
* FREQMEASUREMULTI_ALTERNATE -> A new value becomes available at each rising and each falling ramp and contains the time since the previous ramp. Thus, the high and the low time of the signal period are returned alternating.

Since depending on the selected capture mode it is not always clear which part of the signal has just been measured, extended information can additionally be obtained with the function readLevel() after invoking read().
Possible return values are:
* LEVEL\_SPACE\_ONLY (binary 0b00) -> the read() value contains only the duration of the "low time"
* LEVEL\_SPACE\_MARK (binary 0b01) -> the read() value contains the duration of a full signal period, starting with "low" and followed by "high"
* LEVEL\_MARK\_SPACE (binary 0b10) -> the read() value contains the duration of a full signal period, starting with "high" and followed by "low" (default when mode is not given in begin(pin))
* LEVEL\_MARK\_ONLY (binary 0b11) -> the read() value contains only the duration of the "high time"

Finally, since not everything is about frequency but sometimes about time, a class member function countToNanoseconds(uint32\_t count) has been added, similar to countToFrequency(uint32\_t count), but it does return the corresponding time in nanoseconds.

http://www.pjrc.com/teensy/td_libs_FreqMeasure.html

![FreqMeasure and Test Equipment](http://www.pjrc.com/teensy/td_libs_FreqMeasure_1.jpg)
