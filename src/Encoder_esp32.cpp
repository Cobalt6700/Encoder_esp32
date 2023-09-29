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
	}
#if defined(ESP8266)
#else
	portEXIT_CRITICAL_ISR(&(this->mux));
#endif
}

// REWRITTEN Button and vcc removed. 
Encoder_esp32::Encoder_esp32(uint8_t encoder_APin, uint8_t encoder_BPin)
{
	this->old_AB = 0;

	this->encoderAPin = encoder_APin;
	this->encoderBPin = encoder_BPin;
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
}

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
	int32_t returnPos = this->encoder0Pos;
	newValue_ = newValue_ * this->encoderSteps;
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