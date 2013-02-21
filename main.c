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
#include <stdint.h>

/*Global Variables*/
uint8_t tapState = 0; /**<A varaible that represents the current tap state*/

/*!
 @brief Thread to perform menial tasks such as switching LEDs
 @param argument Unused
 */
void thread(void const * argument);

//! Thread structure for above thread
osThreadDef(thread, osPriorityNormal, 1, 0);

/*!
 @brief Program entry point
 */
int main (void) {
	// ID for thread
	osThreadId tid_thread1;
	
	initIO();
	initTempADC();
	initTIM3();
	initACC();
	initEXTIACC();

	// Start thread
	tid_thread1 = osThreadCreate(osThread(thread), NULL);

	// The below doesn't really need to be in a loop
	while(1){
		osDelay(osWaitForever);
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
*@brief An interrupt handler for TIM3
*@retval None
*/
void EXTI0_IRQHandler(void)
{
	tapState = 1 - tapState;	//Change the current tap state
	EXTI_ClearITPendingBit(EXTI_Line0);	//Clear the EXTI0 interrupt flag
}
