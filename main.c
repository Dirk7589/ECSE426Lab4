/**
*@file main.c
*@author Dirk Dubois, Alain Slak
*@date February 21th, 2013
*@brief 
*
*/

/*Includes*/
#include "arm_math.h"
#include "stm32f4xx.h"
#include "cmsis_os.h"
#include "init.h"
#include "initACC.h"
#include "moving_average.h"
#include "temp.h"
#include <stdint.h>

/*Global Variables*/
uint8_t tapState = 0; /**<A varaible that represents the current tap state*/
uint8_t sampleACCFlag = 0; /**<A flag variable for sampling, restricted to a value of 0 or 1*/
uint8_t sampleTempCounter = 0; /**<A counter variable for sampling the temperature sensor */
uint8_t sampleTempFlag = 0;

/*Defines */
#define MAX_COUNTER_VALUE 5; //Maximum value for the temperature sensor to sample at 20Hz
#define USER_BTN 0x0001 /*!<Defines the bit location of the user button*/

/*!
 @brief Thread to perform menial tasks such as switching LEDs
 @param argument Unused
 */
void temperatureThread(void const * argument);

//! Thread structure for above thread
osThreadDef(temperatureThread, osPriorityNormal, 1, 0);

/*!
 @brief Program entry point
 */
int main (void) {
	// ID for thread
	osThreadId tThread;
	
	initIO();
	initTempADC();
	initTim3();
	initACC();
	initEXTIACC();

	// Start thread
	tThread = osThreadCreate(osThread(temperatureThread), NULL);

	// The below doesn't really need to be in a loop
	while(1){
		osDelay(osWaitForever);
	}
}

void temperatureThread(void const *argument){
	uint16_t adcValue = 0;
	float temperature = 0;
	uint8_t buttonState = 0; /**<A variable that represents the current state of the button*/
	uint8_t LEDState = 0; /**<A variable that sets the led state*/
	
	AVERAGE_DATA_TYPEDEF temperatureData;
	
	movingAverageInit(&temperatureData);
	
	while(1){
		//check current state of button
		if(GPIOA->IDR & USER_BTN){
			buttonState = 1 - buttonState; //toggle button state
			
			//debounce delay
			osDelay(300);
			
			switch(buttonState){
				case 0:
					//toggle delay
					osDelay(500);
				
					//toggle LEDs
					LEDState = LEDToggle(LEDState);
				
					break;
				
				case 1:
					//check sample flag. This ensures the proper 20Hz sampling rate
					if(sampleTempFlag == 1){
						sampleTempFlag = 0; //reset the flag
						
						GPIOD->ODR = 0; //Turn off the LEDs
						
						ADC_SoftwareStartConv(ADC1); //Start conversion
						
						while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET); //wait for end of conversion
						ADC_ClearFlag(ADC1, ADC_FLAG_EOC); //clear flag
					
						adcValue = ADC1->DR; //Retrieve ADC value in bits
						
						temperature = toDegreeC(adcValue); //Determine the temperature in Celcius
						temperature = movingAverage(temperature, &temperatureData);
						displayTemperature(temperature);
					}
					break;
			}
		}
	}	
}

void thread (void const *argument) {
	while(1){
		osDelay(1000);
		GPIOD->BSRRL = GPIO_Pin_12;
		osDelay(1000);
		GPIOD->BSRRH = GPIO_Pin_12;
	}
}

/**
*@brief An interrupt handler for EXTI0
*@retval None
*/
void EXTI0_IRQHandler(void)
{
	tapState = 1 - tapState;	//Change the current tap state
	EXTI_ClearITPendingBit(EXTI_Line0);	//Clear the EXTI0 interrupt flag
}

/**
*@brief An interrupt handler for Tim3
*@retval None
*/
void TIM3_IRQHandler(void)
{
	sampleACCFlag = 1;													//Set flag for accelerometer sampling
	
	if(sampleTempFlag == 0){
		if(sampleTempCounter == 5){
			sampleTempFlag = 1;
			sampleTempCounter = 0;
		}
		else{
			sampleTempCounter++;												//Set flag for temperature sampling
		}
	}
	
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update); //Clear the TIM3 interupt bit
}

