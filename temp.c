#include "temp.h"

void displayTemperature(float temperature){
    if(temperature < 30){
        GPIOD->ODR =0;
        GPIOD->BSRRL = BLUE_LED;
    }
    
    temperature = (int)fmodf(temperature, 8);
    
    if( temperature == 0 || temperature == 7){
        GPIOD->ODR =0;
        GPIOD->BSRRL = BLUE_LED; //Max value of 32
    }
    
    if( temperature == 2 || temperature == 1 ){
        GPIOD->ODR = 0;
        GPIOD->BSRRL = BLUE_LED; //Max value of 34
        GPIOD->BSRRL = GREEN_LED;
    }
    
    if(temperature == 4 || temperature == 3 ){
        GPIOD->ODR = 0;
        GPIOD->BSRRL = BLUE_LED; //Max value of 36
        GPIOD->BSRRL = GREEN_LED;
        GPIOD->BSRRL = ORANGE_LED;
    }
    
    if(temperature == 6 || temperature == 5 ){
        GPIOD->ODR = 0;
        GPIOD->BSRRL = BLUE_LED; //Max value of 38
        GPIOD->BSRRL = GREEN_LED;
        GPIOD->BSRRL = ORANGE_LED;
        GPIOD->BSRRL = RED_LED;
    }
}

uint8_t LEDToggle(uint8_t LEDState){
    if(!LEDState){
        LEDState = 1; //update state
        //Turn on LEDs
        GPIOD->BSRRL = GREEN_LED;
        GPIOD->BSRRL = ORANGE_LED;
        GPIOD->BSRRL = RED_LED;
        GPIOD->BSRRL = BLUE_LED;
    }
    else{
        LEDState = 0; //update state
        GPIOD->ODR = 1; //Turn off leds
    }
		
		return LEDState;
}

float toDegreeC(uint16_t vSense)
{
	vSense = vSense * SCALE; //Convert from bits to miliVolts
	float temperature = (((vSense - V25) * AVG_SLOPE_INVERSE ) + 25 ); //Apply formula to convert to temperature
	return temperature;
}
