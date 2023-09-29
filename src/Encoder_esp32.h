// Encoder_esp32.h
// based on https://github.com/marcmerlin/IoTuz code - extracted and modified Encoder code

#ifndef _Encoder_esp32_h
#define _Encoder_esp32_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

// Rotary Encocer
#define Encoder_esp32_DEFAULT_A_PIN 25
#define Encoder_esp32_DEFAULT_B_PIN 26
#define Encoder_esp32_DEFAULT_STEPS 1

class Encoder_esp32
{

private:
#if defined(ESP8266)
#else
	portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
	portMUX_TYPE buttonMux = portMUX_INITIALIZER_UNLOCKED;
#endif
	volatile long encoder0Pos = 0;

	bool _circleValues = false;
	bool isEnabled = true;
	uint8_t encoderAPin = Encoder_esp32_DEFAULT_A_PIN;
	uint8_t encoderBPin = Encoder_esp32_DEFAULT_B_PIN;
	long encoderSteps = Encoder_esp32_DEFAULT_STEPS;	

	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wshift-negative-value"

	long _minEncoderValue = -1 << 15;
	long _maxEncoderValue = 1 << 15;

	#pragma GCC diagnostic pop

	int8_t old_AB;
	long lastReadEncoder0Pos;

	int8_t enc_states[16] = {0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0};
	void (*ISR_callback)();

public:

	Encoder_esp32(
		uint8_t encoderAPin = Encoder_esp32_DEFAULT_A_PIN,
		uint8_t encoderBPin = Encoder_esp32_DEFAULT_B_PIN
		);
	void setBoundaries(long minValue = -100, long maxValue = 100, bool circleValues = false);
	int correctionOffset=2;
	bool areEncoderPinsPulldownforEsp32 = true;
#if defined(ESP8266)
	ICACHE_RAM_ATTR void readEncoder_ISR();
	ICACHE_RAM_ATTR void readButton_ISR();
#else
	void IRAM_ATTR readEncoder_ISR();
	void IRAM_ATTR readButton_ISR();

#endif

	void setup(void (*ISR_callback)(void));
	void begin();
	void reset(long newValue = 0);
	int32_t readAndReset();
	void enable();
	void disable();
	long readEncoder();
	void setEncoderValue(long newValue);
	
};
#endif
