/**
*@file main.c
*@author Dirk Dubois, Alain Slak
*@date February 21th, 2013
*@brief 
*
*/

/*Includes*/
#include <stdint.h>
#include "arm_math.h"
#include "stm32f4xx.h"
#include "cmsis_os.h"
#include "init.h"
#include "initACC.h"
#include "moving_average.h"
#include "temp.h"
#include "access.h"
#include "common.h"


/*Global Variables*/
uint8_t tapState = 0; /**<A varaible that represents the current tap state*/
uint8_t sampleACCFlag = 0; /**<A flag variable for sampling, restricted to a value of 0 or 1*/
uint8_t sampleTempCounter = 0; /**<A counter variable for sampling the temperature sensor */
uint8_t sampleTempFlag = 0;
uint8_t buttonState = 1; /**<A variable that represents the current state of the button*/

/*Defines */
#define DEBUG 1

#define MAX_COUNTER_VALUE 5; //Maximum value for the temperature sensor to sample at 20Hz
#define USER_BTN 0x0001 /*!<Defines the bit location of the user button*/

/*!
 @brief Thread to perform menial tasks such as switching LEDs
 @param argument Unused
 */
void temperatureThread(void const * argument);

/*!
 @brief Thread to perform menial tasks such as switching LEDs
 @param argument Unused
 */
void accelerometerThread(void const * argument);

//! Thread structure for above thread
osThreadDef(temperatureThread, osPriorityNormal, 1, 0);
osThreadDef(accelerometerThread, osPriorityNormal, 1, 0);

/*!
 @brief Program entry point
 */
int main (void) {
	// ID for thread
	osThreadId tThread; //Tempearture thread ID
	osThreadId aThread; //Accelerometer thread ID
	
	initIO();
	initEXTIButton();
	initTempADC();
	initTim3();
	initACC();
	initEXTIACC();

	// Start thread
	//tThread = osThreadCreate(osThread(temperatureThread), NULL);
	aThread = osThreadCreate(osThread(accelerometerThread), NULL);

	// The below doesn't really need to be in a loop
	while(1){
		osDelay(osWaitForever);
	}
}

void temperatureThread(void const *argument){
	
	uint16_t adcValue = 0;
	uint8_t LEDState = 0; /**<A variable that sets the led state*/
	
	AVERAGE_DATA_TYPEDEF temperatureData; //Create temperature data structure
	
	movingAverageInit(&temperatureData); //Prepare filter
	
	while(1){
		//NO CHECK FOR TAP STATE?
		
		//check current state of button
// 		if(GPIOA->IDR & USER_BTN){
// 			buttonState = 1 - buttonState; //toggle button state
// 			
// 			//debounce delay
// 			osDelay(300);
// 		}
		
		switch(buttonState){
			case 0:

				//toggle LEDs
				LEDState = LEDToggle(LEDState);
				
				//toggle delay
				osDelay(500);
			
				break;
			
			case 1:
				//check sample flag. This ensures the proper 20Hz sampling rate
				if(sampleTempFlag == 1){
					
					GPIOD->ODR = 0; //Turn off the LEDs
					
					ADC_SoftwareStartConv(ADC1); //Start conversion
					
					while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET); //wait for end of conversion
					ADC_ClearFlag(ADC1, ADC_FLAG_EOC); //clear the ADC flag
				
					adcValue = ADC1->DR; //Retrieve ADC value in bits
					
					temperature = toDegreeC(adcValue); //Determine the temperature in Celcius
					temperature = movingAverage(temperature, &temperatureData); //Filter the temperature
					displayTemperature(temperature); //Display the temperature
					
					sampleTempFlag = 0; //reset the  sample flag
				}
				break;
			}
		}
}	

void accelerometerThread(void const * argument){

	int32_t accValues[3]; //To retrieve the accelerometer values
	
	//Create structures for moving average filter
	AVERAGE_DATA_TYPEDEF dataX;
	AVERAGE_DATA_TYPEDEF dataY;
	AVERAGE_DATA_TYPEDEF dataZ;
	
	//Intialize structures for moving average filter
	movingAverageInit(&dataX);
	movingAverageInit(&dataY);
	movingAverageInit(&dataZ);

	//Real-time processing of data
	while(1){
		
		LIS302DL_ReadACC(accValues); //Read from ACC			
		calibrateACC(accValues, accCorrectedValues); //Calibrate the accelerometer	
		
		//Filter ACC values
		accCorrectedValues[0] = movingAverage(accCorrectedValues[0], &dataX);
		accCorrectedValues[1] = movingAverage(accCorrectedValues[1], &dataY);
		accCorrectedValues[2] = movingAverage(accCorrectedValues[2], &dataZ);
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
*@brief An interrupt handler for EXTI1
*@retval None
*/
void EXTI1_IRQHandler(void)
{
	buttonState = 1 - buttonState;	//Change the current tap state
	EXTI_ClearITPendingBit(EXTI_Line1);	//Clear the EXTI0 interrupt flag
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

