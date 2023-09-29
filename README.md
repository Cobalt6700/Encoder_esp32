# Intro

This library is used by EncoderButton to allow **rotary encoder** to work with **ESP32** or **ESP8266**.

It uses stripped down code from https://github.com/igorantolic/ai-esp32-rotary-encoder due to the fact that I do 
not know enough to write my own rotuines. 

I have not tested this with an **ESP8266**!

I take no credit for this rework. 

I hope it does a thing for you. 

# Arduino Esp32 Encoder Library

Rotary encoder main interrupt code is extracted from (https://github.com/marcmerlin/IoTuz) and some additional features are included here.

## Description

This library enables  easy implementation of rotary encoder functionality in your application for ESP32, ESP8266 (or similar microcontroller).
-
## Installing

The downloaded code can be included as a new library into the IDE selecting the menu:

     Sketch / include Library / Add .Zip library


## Connecting Rotary Encoder with Switch (no pcb version. The encoder is like [this](https://www.seeedstudio.com/Rotary-Encoder-with-Switch-p-667.html)) to your microcontroller ESP32, ESP8266 (or similar)

There is no need for external resistors, you can use only the encoder.

**You have to set INPUT_PULLUP of Right and Left pins with `pinMode(ROTARY_ENCODER_A_PIN, INPUT_PULLUP);` and `pinMode(ROTARY_ENCODER_B_PIN, INPUT_PULLUP);`**
Look example Esp32RotaryEncoderTheShortestExampleNoResistors.



There is a new option areEncoderPinsPulldownforEsp32. By default it is true, but in case you use ESP32 you can set it to true. 
It doesn't make any change for ESP8266, only ESP32.

```
rotaryEncoder.areEncoderPinsPulldownforEsp32 = false;
```

There is a new option isButtonPulldown. By default it is false, but in case you use ESP32 you can set it to true. See Multi-select example.
It doesn't make any change for ESP8266, only ESP32.

```
rotaryEncoder.isButtonPulldown = true;
```




