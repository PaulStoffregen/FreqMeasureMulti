#ifndef FreqMeasureMulti_h
#define FreqMeasureMulti_h

#include <Arduino.h>

#define FREQMEASUREMULTI_BUFFER_LEN 24

class FreqMeasureMulti
{
public:
	bool begin(uint32_t pin);
	uint32_t available(void);
	uint32_t read(void);
	static float countToFrequency(uint32_t count);
	void end(void);
private:
	void isr(bool inc);
	friend void ftm0_isr(void);
	uint32_t buffer_value[FREQMEASUREMULTI_BUFFER_LEN];
	uint8_t buffer_head;
	uint8_t buffer_tail;
	uint8_t channel;
	uint32_t capture_previous;
};
	
#endif

