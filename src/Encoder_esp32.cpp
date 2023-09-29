// based on https://github.com/marcmerlin/IoTuz code - extracted and modified Encoder code
//
//


/*

24/09/23

Lib re-written to work with EncoderButton

*/

#if defined(ESP8266)
#else
#include "esp_log.h"
#define LOG_TAG "Encoder_esp32"
#endif

#include "Encoder_esp32.h"

#if defined(ESP8266)
ICACHE_RAM_ATTR void Encoder_esp32::readEncoder_ISR()
#else
void IRAM_ATTR Encoder_esp32::readEncoder_ISR()
#endif
{

	// unsigned long now = millis();
#if defined(ESP8266)
#else
	portENTER_CRITICAL_ISR(&(this->mux));
#endif
	if (this->isEnabled)
	{
		// code from https://www.circuitsathome.com/mcu/reading-rotary-encoder-on-arduino/
		/**/
		this->old_AB <<= 2; // remember previous state

		int8_t ENC_PORT = ((digitalRead(this->encoderBPin)) ? (1 << 1) : 0) | ((digitalRead(this->encoderAPin)) ? (1 << 0) : 0);

		this->old_AB |= (ENC_PORT & 0x03); // add current state

		this->encoder0Pos += ( this->enc_states[( this->old_AB & 0x0f )]);
		// int8_t currentDirection = (this->enc_states[(this->old_AB & 0x0f)]); //-1,0 or 1

		// if (currentDirection != 0)
		// {
		// 	// bool ignoreCorrection = false;
		// 	// if (this->encoder0Pos > this->_maxEncoderValue) ignoreCorrection = true;
		// 	// if (this->encoder0Pos < this->_minEncoderValue) ignoreCorrection = true;
		// 	long prevRotaryPosition = this->encoder0Pos / this->encoderSteps;
		// 	this->encoder0Pos += currentDirection;
		// 	long newRotaryPosition = this->encoder0Pos / this->encoderSteps;

		// 	if (newRotaryPosition != prevRotaryPosition && rotaryAccelerationCoef > 1)
		// 	{
		// 		// additional movements cause acceleration?
		// 		//  at X ms, there should be no acceleration.
		// 		unsigned long accelerationLongCutoffMillis = 200;
		// 		// at Y ms, we want to have maximum acceleration
		// 		unsigned long accelerationShortCutffMillis = 4;

		// 		// compute linear acceleration
		// 		if (currentDirection == lastMovementDirection &&
		// 			currentDirection != 0 &&
		// 			lastMovementDirection != 0)
		// 		{
		// 			// ... but only of the direction of rotation matched and there
		// 			// actually was a previous rotation.
		// 			unsigned long millisAfterLastMotion = now - lastMovementAt;

		// 			if (millisAfterLastMotion < accelerationLongCutoffMillis)
		// 			{
		// 				if (millisAfterLastMotion < accelerationShortCutffMillis)
		// 				{
		// 					millisAfterLastMotion = accelerationShortCutffMillis; // limit to maximum acceleration
		// 				}
		// 				if (currentDirection > 0)
		// 				{
		// 					this->encoder0Pos += rotaryAccelerationCoef / millisAfterLastMotion;
		// 				}
		// 				else
		// 				{
		// 					this->encoder0Pos -= rotaryAccelerationCoef / millisAfterLastMotion;
		// 				}
		// 			}
		// 		}
		// 		this->lastMovementAt = now;
		// 		this->lastMovementDirection = currentDirection;
		// 	}

						//https://github.com/igorantolic/ai-esp32-rotary-encoder/issues/40
						/*
						when circling there is an issue since encoderSteps is tipically 4
						that means 4 changes for a single roary movement (step)
						so if maximum is 4 that means _maxEncoderValue is 4*4=16
						when we detact 18 we cannot go to zero since next 2 will make it wild
						Here we changed to 18 set not to 0 but to -2; 17 to -3...
						Now it seems better however that -3 divided with 4 will give -1 which is not regular -> also readEncoder() is changed to give allowed values
						It is not yet perfect for cycling options but it is much better than before

						optimistic view was that most of the time encoder0Pos values will be near to N*encodersteps
						*/
			// respect limits
			if ((this->encoder0Pos/ this->encoderSteps) > (this->_maxEncoderValue/ this->encoderSteps)){
				// Serial.print("circle values limit HIGH");
				// Serial.print(this->encoder0Pos);
				//this->encoder0Pos = this->_circleValues ? this->_minEncoderValue : this->_maxEncoderValue;
				if (_circleValues){
					//if (!ignoreCorrection){
						int delta  =this->_maxEncoderValue + this->encoderSteps -this->encoder0Pos;
						this->encoder0Pos = this->_minEncoderValue-delta;
					//}
				} else {
					this->encoder0Pos = this->_maxEncoderValue;
				}
				//this->encoder0Pos = this->_circleValues ? (this->_minEncoderValue this->encoder0Pos-this->encoderSteps) : this->_maxEncoderValue;
				// Serial.print(" -> ");
				// Serial.println(this->encoder0Pos);
			} else if ((this->encoder0Pos/ this->encoderSteps) < (this->_minEncoderValue/ this->encoderSteps)){
				// Serial.print("circle values limit LOW");
				// Serial.print(this->encoder0Pos);
				//this->encoder0Pos = this->_circleValues ? this->_maxEncoderValue : this->_minEncoderValue;
				this->encoder0Pos = this->_circleValues ? this->_maxEncoderValue : this->_minEncoderValue;
				if (_circleValues){
					//if (!ignoreCorrection){
						int delta  =this->_minEncoderValue +this->encoderSteps +this->encoder0Pos;
						this->encoder0Pos = this->_maxEncoderValue+delta;
					//}
				} else {
					this->encoder0Pos = this->_minEncoderValue;
				}
				
				// Serial.print(" -> ");
				// Serial.println(this->encoder0Pos);
			}else{
				// Serial.print("no circle values limit ");
				// Serial.println(this->encoder0Pos);
			}
 			//Serial.println(this->encoder0Pos);
		// }
	}
#if defined(ESP8266)
#else
	portEXIT_CRITICAL_ISR(&(this->mux));
#endif
}


// #if defined(ESP8266)
// ICACHE_RAM_ATTR void Encoder_esp32::readButton_ISR()
// #else
// void IRAM_ATTR Encoder_esp32::readButton_ISR()
// #endif
// {
// #if defined(ESP8266)
// #else
// 	portENTER_CRITICAL_ISR(&(this->buttonMux));
// #endif

// 	uint8_t butt_state = !digitalRead(this->encoderButtonPin);

// 	if (!this->isEnabled)
// 	{
// 		buttonState = BUT_DISABLED;
// 	}
// 	else if (butt_state && !this->previous_butt_state)
// 	{
// 		this->previous_butt_state = true;
// 		Serial.println("Button Pushed");
// 		buttonState = BUT_PUSHED;
// 	}
// 	else if (!butt_state && this->previous_butt_state)
// 	{
// 		this->previous_butt_state = false;
// 		Serial.println("Button Released");
// 		buttonState = BUT_RELEASED;
// 	}
// 	else
// 	{
// 		buttonState = (butt_state ? BUT_DOWN : BUT_UP);
// 		Serial.println(butt_state ? "BUT_DOWN" : "BUT_UP");
// 	}

// #if defined(ESP8266)
// #else
// 	portEXIT_CRITICAL_ISR(&(this->buttonMux));
// #endif
// }

// Encoder_esp32::Encoder_esp32(uint8_t encoder_APin, uint8_t encoder_BPin, int encoder_ButtonPin, int encoder_VccPin, uint8_t encoderSteps)
// {
// 	this->old_AB = 0;

// 	this->encoderAPin = encoder_APin;
// 	this->encoderBPin = encoder_BPin;
// 	this->encoderButtonPin = encoder_ButtonPin;
// 	this->encoderVccPin = encoder_VccPin;
// 	this->encoderSteps = encoderSteps;

// #if defined(ESP8266)
// 	pinMode(this->encoderAPin, INPUT_PULLUP);
// 	pinMode(this->encoderBPin, INPUT_PULLUP);
// #else
// 	pinMode(this->encoderAPin, (areEncoderPinsPulldownforEsp32? INPUT_PULLDOWN:INPUT_PULLUP));
// 	pinMode(this->encoderBPin, (areEncoderPinsPulldownforEsp32? INPUT_PULLDOWN:INPUT_PULLUP));
// #endif
// }

// REWRITTEN Button and vcc to be removed. 
Encoder_esp32::Encoder_esp32(uint8_t encoder_APin, uint8_t encoder_BPin)
{
	this->old_AB = 0;

	this->encoderAPin = encoder_APin;
	this->encoderBPin = encoder_BPin;
	// this->encoderButtonPin = -1;
	// this->encoderVccPin = -1;
	this->encoderSteps = 1;

#if defined(ESP8266)
	pinMode(this->encoderAPin, INPUT_PULLUP);
	pinMode(this->encoderBPin, INPUT_PULLUP);
#else
	pinMode(this->encoderAPin, (areEncoderPinsPulldownforEsp32? INPUT_PULLDOWN:INPUT_PULLUP));
	pinMode(this->encoderBPin, (areEncoderPinsPulldownforEsp32? INPUT_PULLDOWN:INPUT_PULLUP));
#endif
}

// Currently not USED
void Encoder_esp32::setBoundaries(long minEncoderValue, long maxEncoderValue, bool circleValues)
{
	this->_minEncoderValue = minEncoderValue * this->encoderSteps;
	this->_maxEncoderValue = maxEncoderValue * this->encoderSteps;

	this->_circleValues = circleValues;
}

// USED
long Encoder_esp32::readEncoder()
{
	//return (this->encoder0Pos / this->encoderSteps);
	if(this->encoderSteps == 1){
		if (this->encoder0Pos > this->_maxEncoderValue)
			return this->_maxEncoderValue;
		if (this->encoder0Pos < this->_minEncoderValue)
			return this->_minEncoderValue;
		return this->encoder0Pos;
	}
	else {
		if ((this->encoder0Pos/ this->encoderSteps) > (this->_maxEncoderValue/ this->encoderSteps))
			return this->_maxEncoderValue/ this->encoderSteps;
		if ((this->encoder0Pos/ this->encoderSteps) < (this->_minEncoderValue/ this->encoderSteps))
			return this->_minEncoderValue/ this->encoderSteps;
		return (this->encoder0Pos / this->encoderSteps);
	}
}

// USED
void Encoder_esp32::setEncoderValue(long newValue)
{
	reset(newValue);
}

// Not USED - EB_LIB covers
// long Encoder_esp32::encoderChanged()
// {
// 	long _encoder0Pos = readEncoder();
// 	long encoder0Diff = _encoder0Pos - this->lastReadEncoder0Pos;

// 	this->lastReadEncoder0Pos = _encoder0Pos;

// 	return encoder0Diff;
// }

// USED
// void Encoder_esp32::setup(void (*ISR_callback)(void), void (*ISR_button)(void))
// {
// 	attachInterrupt(digitalPinToInterrupt(this->encoderAPin), ISR_callback, CHANGE);
// 	attachInterrupt(digitalPinToInterrupt(this->encoderBPin), ISR_callback, CHANGE);
// 	// if (this->encoderButtonPin >= 0)
// 	// 	attachInterrupt(digitalPinToInterrupt(this->encoderButtonPin), ISR_button, RISING);
// 	// attachInterrupt(digitalPinToInterrupt(this->encoderButtonPin), ISR_button, CHANGE);
// }

// USED
void Encoder_esp32::setup(void (*ISR_callback)(void))
{
	attachInterrupt(digitalPinToInterrupt(this->encoderAPin), ISR_callback, CHANGE);
	attachInterrupt(digitalPinToInterrupt(this->encoderBPin), ISR_callback, CHANGE);
}

// USED
void Encoder_esp32::begin()
{
	this->lastReadEncoder0Pos = 0;
	// if (this->encoderVccPin >= 0)
	// {
	// 	pinMode(this->encoderVccPin, OUTPUT);
	// 	digitalWrite(this->encoderVccPin, 1); // Vcc for encoder
	// }

	// // Initialize rotary encoder reading and decoding
	// this->previous_butt_state = 0;
	// if (this->encoderButtonPin >= 0)
	// {
		
	// #if defined(ESP8266)
	// 	pinMode(this->encoderButtonPin, INPUT_PULLUP);
	// #else
	// 	pinMode(this->encoderButtonPin,isButtonPulldown?INPUT_PULLDOWN: INPUT_PULLUP);
	// #endif
		
	// }
}

// NOT USED - TO BE REMOVED
// ButtonState Encoder_esp32::currentButtonState()
// {
// 	return buttonState;
// }

// NOT USED - TO BE REMOVED
// ButtonState Encoder_esp32::readButtonState()
// {
// 	// ButtonState _buttonState = buttonState;
// 	// buttonState =
// 	return buttonState;
// }

// USED
void Encoder_esp32::reset(long newValue_)
{
	newValue_ = newValue_ * this->encoderSteps;
	this->encoder0Pos = newValue_ +this->correctionOffset;
	this->lastReadEncoder0Pos = this->encoder0Pos;
	if (this->encoder0Pos > this->_maxEncoderValue)
		this->encoder0Pos = this->_circleValues ? this->_minEncoderValue : this->_maxEncoderValue;
	if (this->encoder0Pos < this->_minEncoderValue)
		this->encoder0Pos = this->_circleValues ? this->_maxEncoderValue : this->_minEncoderValue;

	this->lastReadEncoder0Pos = this->readEncoder();
}

// NEW FUNCTION
int32_t Encoder_esp32::readAndReset() 
{
	static long newValue_ = 0;
	//newValue_ = newValue_ * this->encoderSteps;
	int32_t returnPos = this->encoder0Pos;
	this->encoder0Pos = newValue_ +this->correctionOffset;
	this->lastReadEncoder0Pos = this->encoder0Pos;
	if (this->encoder0Pos > this->_maxEncoderValue)
		this->encoder0Pos = this->_circleValues ? this->_minEncoderValue : this->_maxEncoderValue;
	if (this->encoder0Pos < this->_minEncoderValue)
		this->encoder0Pos = this->_circleValues ? this->_maxEncoderValue : this->_minEncoderValue;

	this->lastReadEncoder0Pos = this->readEncoder();

	return returnPos;
}

// NOT CURRENTLY USED
void Encoder_esp32::enable()
{
	this->isEnabled = true;
}
// NOT CURRENTLY USED
void Encoder_esp32::disable()
{
	this->isEnabled = false;
}

// NOT USED - TO BE REMOVED
// bool Encoder_esp32::isEncoderButtonClicked(unsigned long maximumWaitMilliseconds)
// {
// 	static bool wasTimeouted = false;
// 	int button = 1 - digitalRead(encoderButtonPin);
// 	if (!button)
// 	{
// 		if (wasTimeouted)
// 		{
// 			wasTimeouted = false;
// 			return true;
// 		}
// 		return false;
// 	}
// 	delay(30); // debounce
// 	button = 1 - digitalRead(encoderButtonPin);
// 	if (!button)
// 	{
// 		return false;
// 	}

// 	// wait release of button but only maximumWaitMilliseconds
// 	wasTimeouted = false;
// 	unsigned long waitUntil = millis() + maximumWaitMilliseconds;
// 	while (1 - digitalRead(encoderButtonPin))
// 	{
// 		if (millis() > waitUntil)
// 		{
// 			// button not released until timeout
// 			wasTimeouted = true;
// 			return false;
// 		}
// 	}

// 	return true;
// }

// NOT USED - TO BE REMOVED
// bool Encoder_esp32::isEncoderButtonDown()
// {
// 	return digitalRead(encoderButtonPin) ? false : true;
// }