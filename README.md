#FreqMeasureMulti Library#

FreqMeasureMulti measures the elapsed time during each cycle of an input frequency.

Up to 8 frequencies can be measured simultaneously.

    Board            Supported Pins
    -----            --------------
    Teensy LC        6, 9, 10, 20, 22, 23
    Teensy 3.1       5, 6, 9, 10, 20, 21, 22, 23
    Teensy 3.2       5, 6, 9, 10, 20, 21, 22, 23

FreqMeasureMulti uses the same functions as FreqMeasure, except begin() must specify a pin number.  Because more than 1 input can be measured, you must create FreqMeasureMulti instances.  See the Serial_Output example.

http://www.pjrc.com/teensy/td_libs_FreqMeasure.html

![FreqMeasure and Test Equipment](http://www.pjrc.com/teensy/td_libs_FreqMeasure_1.jpg)
