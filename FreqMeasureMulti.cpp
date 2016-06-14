/* FreqMeasure Library, for measuring relatively low frequencies
 * http://www.pjrc.com/teensy/td_libs_FreqMeasure.html
 * Copyright (c) 2015 PJRC.COM, LLC - Paul Stoffregen <paul@pjrc.com>
 * Extended 2016 by Thierry Frenkel <theremingenieur@gmail.com>
 *
 * Version 0.22
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "FreqMeasureMulti.h"

#define FTM_SC_VALUE (FTM_SC_TOIE | FTM_SC_CLKS(1) | FTM_SC_PS(0))
#define FTM_CSC_RAISING (FTM_CSC_CHIE | FTM_CSC_ELSA)
#define FTM_CSC_FALLING (FTM_CSC_CHIE | FTM_CSC_ELSB)
#define UPDATE_ON_RISING 1
#define UPDATE_ON_FALLING 2
#define UPDATE_DIFFERENCE 4

static uint8_t channelmask[4] = {0, 0, 0, 0};
static uint16_t capture_msw[4] = {0, 0, 0, 0};
static FreqMeasureMulti * list[4][8];

bool FreqMeasureMulti::begin(uint32_t pin) {
	return begin(pin, FREQMEASUREMULTI_RISING);
}

bool FreqMeasureMulti::begin(uint32_t pin, uint8_t mode)
{
	uint8_t capture_mode;
	switch (pin) {
		case 22: channel = 0; ftm=0; CORE_PIN22_CONFIG = PORT_PCR_MUX(4); break;
		case 23: channel = 1; ftm=0; CORE_PIN23_CONFIG = PORT_PCR_MUX(4); break;
		case  9: channel = 2; ftm=0; CORE_PIN9_CONFIG  = PORT_PCR_MUX(4); break;
		case 10: channel = 3; ftm=0; CORE_PIN10_CONFIG = PORT_PCR_MUX(4); break;
		case  6: channel = 4; ftm=0; CORE_PIN6_CONFIG  = PORT_PCR_MUX(4); break;
		case 20: channel = 5; ftm=0; CORE_PIN20_CONFIG = PORT_PCR_MUX(4); break;
		case  3: channel = 0; ftm=1; CORE_PIN3_CONFIG = PORT_PCR_MUX(3); break;
		case  4: channel = 1; ftm=1; CORE_PIN4_CONFIG = PORT_PCR_MUX(3); break;
#if defined(KINETISK)
		case 21: channel = 6; ftm=0; CORE_PIN21_CONFIG = PORT_PCR_MUX(4); break;
		case  5: channel = 7; ftm=0; CORE_PIN5_CONFIG  = PORT_PCR_MUX(4); break;
		case 32: channel = 0; ftm=2; CORE_PIN32_CONFIG  = PORT_PCR_MUX(3); break;
		case 25: channel = 1; ftm=2; CORE_PIN25_CONFIG = PORT_PCR_MUX(3); break;
#endif
		default:
			channel = 8;
			return false;
	}
	switch (mode) {
		case FREQMEASUREMULTI_FALLING:
		case FREQMEASUREMULTI_INTERLEAVE:
		case FREQMEASUREMULTI_MARK_ONLY:
		case FREQMEASUREMULTI_SPACE_ONLY:
		case FREQMEASUREMULTI_ALTERNATE:
			capture_mode = mode;
			break;
		default:
			capture_mode = FREQMEASUREMULTI_RISING;
	}
	act_on_rise = (capture_mode & UPDATE_ON_RISING);
	act_on_fall = (capture_mode & UPDATE_ON_FALLING);
	read_diff = (capture_mode & UPDATE_DIFFERENCE);

	switch(ftm) {
	/* To do: add MK6x FTM3 support */
	case 0:
		NVIC_DISABLE_IRQ(IRQ_FTM0);
		if (FTM0_MOD != 0xFFFF || (FTM0_SC & 0x7F) != FTM_SC_VALUE) {
			FTM0_SC = 0;
			FTM0_CNT = 0;
			FTM0_MOD = 0xFFFF;
			FTM0_SC = FTM_SC_VALUE;
			#ifdef KINETISK
			FTM0_MODE = FTM_MODE_WPDIS; //allow reconfiguring the CSC on the fly
			#endif
		}
		csc = &FTM0_C0SC + channel * 2;
		#if defined(KINETISL)
		*csc = 0;
		delayMicroseconds(1);
		#endif
		attachInterruptVector(IRQ_FTM0, myFtm0isr);
		NVIC_SET_PRIORITY(IRQ_FTM0, 48);
		break;
	case 1:
		NVIC_DISABLE_IRQ(IRQ_FTM1);
		if (FTM1_MOD != 0xFFFF || (FTM1_SC & 0x7F) != FTM_SC_VALUE) {
			FTM1_SC = 0;
			FTM1_CNT = 0;
			FTM1_MOD = 0xFFFF;
			FTM1_SC = FTM_SC_VALUE;
			#ifdef KINETISK
			FTM1_MODE = FTM_MODE_WPDIS; //allow reconfiguring the CSC on the fly
			#endif
		}
		csc = &FTM1_C0SC + channel * 2;
		#if defined(KINETISL)
		*csc = 0;
		delayMicroseconds(1);
		#endif
		attachInterruptVector(IRQ_FTM1, myFtm1isr);
		NVIC_SET_PRIORITY(IRQ_FTM1, 48);
		break;
	case 2:
		NVIC_DISABLE_IRQ(IRQ_FTM2);
		if (FTM2_MOD != 0xFFFF || (FTM2_SC & 0x7F) != FTM_SC_VALUE) {
			FTM2_SC = 0;
			FTM2_CNT = 0;
			FTM2_MOD = 0xFFFF;
			FTM2_SC = FTM_SC_VALUE;
			FTM2_MODE = FTM_MODE_WPDIS; //allow reconfiguring the CSC on the fly
		}
		csc = &FTM2_C0SC + channel * 2;
		attachInterruptVector(IRQ_FTM2, myFtm2isr);
		NVIC_SET_PRIORITY(IRQ_FTM2, 48);
		break;
	}
	capture_msw[ftm] = 0;
	risecap_previous = 0;
	fallcap_previous = 0;
	next_is_falling = false;
	buffer_head = 0;
	buffer_tail = 0;

	*csc = FTM_CSC_RAISING; // first capture is always rising

	list[ftm][channel] = this;
	channelmask[ftm] |= (1 << channel);
	switch (ftm) {
	case 0:
		NVIC_ENABLE_IRQ(IRQ_FTM0); break;
	case 1:
		NVIC_ENABLE_IRQ(IRQ_FTM1); break;
	case 2:
		NVIC_ENABLE_IRQ(IRQ_FTM2); break;
/* To do: add MK6x FTM3 support */
	}
	return true;
}

uint32_t FreqMeasureMulti::available(void)
{
	uint32_t head = buffer_head;
	uint32_t tail = buffer_tail;
	if (head >= tail) return head - tail;
	return FREQMEASUREMULTI_BUFFER_LEN + head - tail;
}

uint32_t FreqMeasureMulti::read(void)
{
	uint32_t head = buffer_head;
	uint32_t tail = buffer_tail;
	if (head == tail) return 0xFFFFFFFF;
	tail = tail + 1;
	if (tail >= FREQMEASUREMULTI_BUFFER_LEN) tail = 0;
	uint32_t value = buffer_value[tail].count;
	last_read_level = buffer_value[tail].level;
	buffer_tail = tail;
	return value;
}

uint8_t FreqMeasureMulti::readLevel(void)
{
	return last_read_level;
}

float FreqMeasureMulti::countToFrequency(uint32_t count)
{
#if defined(__arm__) && defined(TEENSYDUINO) && defined(KINETISK)
	return (float)F_BUS / (float)count;
#elif defined(__arm__) && defined(TEENSYDUINO) && defined(KINETISL)
	return (float)(F_PLL/2) / (float)count;
#else
	return 0.0;
#endif
}

float FreqMeasureMulti::countToNanoseconds(uint32_t count)
{
#if defined(__arm__) && defined(TEENSYDUINO) && defined(KINETISK)
	return (float)(count * 1000) / (float)(F_BUS / 1000000);
#elif defined(__arm__) && defined(TEENSYDUINO) && defined(KINETISL)
	return (float)(count * 1000) / (float)(F_PLL / 2000000);
#else
	return 0.0;
#endif
}

void FreqMeasureMulti::end(void)
{
	switch (ftm) {
		case 0:
			switch (channel) {
			case 0: CORE_PIN22_CONFIG = 0; break;
			case 1: CORE_PIN23_CONFIG = 0; break;
			case 2: CORE_PIN9_CONFIG  = 0; break;
			case 3: CORE_PIN10_CONFIG = 0; break;
			case 4: CORE_PIN6_CONFIG  = 0; break;
			case 5: CORE_PIN20_CONFIG = 0; break;
#if defined(KINETISK)
			case 6: CORE_PIN21_CONFIG = 0; break;
			case 7: CORE_PIN5_CONFIG  = 0; break;
#endif
			default: return;
			} break;
		case 1:
			switch (channel) {
			case 0: CORE_PIN3_CONFIG = 0; break;
			case 1: CORE_PIN4_CONFIG = 0; break;
			default: return;
			} break;
#if defined(KINETISK)
		case 2:
			switch (channel) {
			case 0: CORE_PIN32_CONFIG = 0; break;
			case 1: CORE_PIN25_CONFIG = 0; break;
			default: return;
			} break;
#endif
		}
	channelmask[ftm] &= ~(1 << channel);
	if(channelmask[ftm] == 0) { //check if all channels are free, then restore interrupt vectors
		switch(ftm) {
		case 0: attachInterruptVector(IRQ_FTM0, ftm0_isr); break;
		case 1: attachInterruptVector(IRQ_FTM1, ftm1_isr); break;
		case 2: attachInterruptVector(IRQ_FTM2, ftm2_isr); break;
		}

	}
	*csc = 0;
}


void FreqMeasureMulti::isr(bool inc)
{
	uint32_t period = 0;
	uint8_t level = LEVEL_UNDEFINED;
	uint32_t capture = csc[1];
	next_is_falling = !next_is_falling; // toggle capture mode
	#if defined(KINETISK)
	csc[0] = (next_is_falling ? FTM_CSC_FALLING : FTM_CSC_RAISING);
	#elif defined(KINETISL)
	csc[0] = (next_is_falling ? FTM_CSC_FALLING : FTM_CSC_RAISING) | FTM_CSC_CHF;
	#endif
	if (capture <= 0xE000 || !inc) {
		capture |= (capture_msw[ftm] << 16);
	} else {
		capture |= ((capture_msw[ftm] - 1) << 16);
	}
	// compute the waveform period
	if (next_is_falling) {
		if (act_on_rise) period = capture - (read_diff ? fallcap_previous : risecap_previous);
		risecap_previous = capture;
		level = (read_diff ? LEVEL_SPACE_ONLY : LEVEL_MARK_SPACE);
	} else if (!next_is_falling ) {
		if (act_on_fall) period = capture - (read_diff ? risecap_previous : fallcap_previous);
		fallcap_previous = capture;
		level = (read_diff ? LEVEL_MARK_ONLY : LEVEL_SPACE_MARK);
	}
	if (period != 0) {
		uint32_t i = buffer_head + 1;
		if (i >= FREQMEASUREMULTI_BUFFER_LEN) i = 0;
		if (i != buffer_tail) {
			buffer_value[i].level = level;
			buffer_value[i].count = period;
			buffer_head = i;
		}
	}
}

#ifdef __cplusplus
extern "C" {
#endif

void myFtm0isr(void)
{
	bool inc = false;
	if (FTM0_SC & FTM_SC_TOF) {
		#if defined(KINETISK)
		FTM0_SC = FTM_SC_VALUE;
		#elif defined(KINETISL)
		FTM0_SC = FTM_SC_VALUE | FTM_SC_TOF;
		#endif
		capture_msw[0]++;
		inc = true;
	}
	uint8_t mask = FTM0_STATUS & channelmask[0];
	if ((mask & 0x01)) list[0][0]->isr(inc);
	if ((mask & 0x02)) list[0][1]->isr(inc);
	if ((mask & 0x04)) list[0][2]->isr(inc);
	if ((mask & 0x08)) list[0][3]->isr(inc);
	if ((mask & 0x10)) list[0][4]->isr(inc);
	if ((mask & 0x20)) list[0][5]->isr(inc);
	#if defined(KINETISK)
	if ((mask & 0x40)) list[0][6]->isr(inc);
	if ((mask & 0x80)) list[0][7]->isr(inc);
	#endif
}

void myFtm1isr(void)
{
	bool inc = false;
	if (FTM1_SC & FTM_SC_TOF) {
		#if defined(KINETISK)
		FTM1_SC = FTM_SC_VALUE;
		#elif defined(KINETISL)
		FTM1_SC = FTM_SC_VALUE | FTM_SC_TOF;
		#endif
		capture_msw[1]++;
		inc = true;
	}
	uint8_t mask = FTM1_STATUS & channelmask[1];
//	Serial.println(FTM1_STATUS);//
	if ((mask & 0x01)) list[1][0]->isr(inc);
	if ((mask & 0x02)) list[1][1]->isr(inc);
}

void myFtm2isr(void)
{
	bool inc = false;
	if (FTM2_SC & FTM_SC_TOF) {
		FTM2_SC = FTM_SC_VALUE;
		capture_msw[2]++;
		inc = true;
	}
	uint8_t mask = FTM2_STATUS & channelmask[2];
	if ((mask & 0x01)) list[2][0]->isr(inc);
	if ((mask & 0x02)) list[2][1]->isr(inc);
}

#ifdef __cplusplus
}
#endif
