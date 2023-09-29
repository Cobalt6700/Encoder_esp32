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
// #define Encoder_esp32_DEFAULT_BUT_PIN 15
// #define Encoder_esp32_DEFAULT_VCC_PIN -1
#define Encoder_esp32_DEFAULT_STEPS 1

// typedef enum
// {
// 	BUT_DOWN = 0,
// 	BUT_PUSHED = 1,
// 	BUT_UP = 2,
// 	BUT_RELEASED = 3,
// 	BUT_DISABLED = 99,
// } ButtonState;

class Encoder_esp32
{

private:
#if defined(ESP8266)
#else
	portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
	portMUX_TYPE buttonMux = portMUX_INITIALIZER_UNLOCKED;
#endif
	volatile long encoder0Pos = 0;

	// volatile int8_t lastMovementDirection = 0; // 1 right; -1 left
	// volatile unsigned long lastMovementAt = 0;
	// unsigned long rotaryAccelerationCoef = 150;

	bool _circleValues = false;
	bool isEnabled = true;

	uint8_t encoderAPin = Encoder_esp32_DEFAULT_A_PIN;
	uint8_t encoderBPin = Encoder_esp32_DEFAULT_B_PIN;
	// int encoderButtonPin = Encoder_esp32_DEFAULT_BUT_PIN;
	// int encoderVccPin = Encoder_esp32_DEFAULT_VCC_PIN;
	long encoderSteps = Encoder_esp32_DEFAULT_STEPS;	

	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wshift-negative-value"

	long _minEncoderValue = -1 << 15;
	long _maxEncoderValue = 1 << 15;

	#pragma GCC diagnostic pop

	int8_t old_AB;
	long lastReadEncoder0Pos;
	// bool previous_butt_state;

	// ButtonState buttonState;

	int8_t enc_states[16] = {0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0};
	void (*ISR_callback)();
	// void (*ISR_button)();

public:
	// Encoder_esp32(
	// 	uint8_t encoderAPin = Encoder_esp32_DEFAULT_A_PIN,
	// 	uint8_t encoderBPin = Encoder_esp32_DEFAULT_B_PIN,
	// 	int encoderButtonPin = Encoder_esp32_DEFAULT_BUT_PIN,
	// 	int encoderVccPin = Encoder_esp32_DEFAULT_VCC_PIN,
	// 	uint8_t encoderSteps = Encoder_esp32_DEFAULT_STEPS);

	Encoder_esp32(
		uint8_t encoderAPin = Encoder_esp32_DEFAULT_A_PIN,
		uint8_t encoderBPin = Encoder_esp32_DEFAULT_B_PIN
		);
	void setBoundaries(long minValue = -100, long maxValue = 100, bool circleValues = false);
	int correctionOffset=2;
	// bool isButtonPulldown = false;
	bool areEncoderPinsPulldownforEsp32 = true;
#if defined(ESP8266)
	ICACHE_RAM_ATTR void readEncoder_ISR();
	ICACHE_RAM_ATTR void readButton_ISR();
#else
	void IRAM_ATTR readEncoder_ISR();
	void IRAM_ATTR readButton_ISR();

#endif

	void setup(void (*ISR_callback)(void));
	// void setup(void (*ISR_callback)(void), void (*ISR_button)(void));
	void begin();
	void reset(long newValue = 0);
	int32_t readAndReset();
	void enable();
	void disable();
	long readEncoder();
	void setEncoderValue(long newValue);
	// long encoderChanged();
	// ButtonState currentButtonState();
	// ButtonState readButtonState();
	// unsigned long getAcceleration() { return this->rotaryAccelerationCoef; }
	// void setAcceleration(unsigned long acceleration) { this->rotaryAccelerationCoef = acceleration; }
	// void disableAcceleration() { setAcceleration(0); }

	// bool isEncoderButtonClicked(unsigned long maximumWaitMilliseconds = 300);
	// bool isEncoderButtonDown();
};
#endif
