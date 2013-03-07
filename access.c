/**
*@file access.c
*@author Dirk Dubois, Alain Slak
*@date March 6th, 2013
*@brief A set of functions that provide access to global variables shared accross multiple threads
*
*/

/*Includes*/
#include "common.h"

/**
*@brief A function that safely access's the corrected values of the accelerometer
*@param[inout] accValues A pointer to the new location in memory that data is copied to
*@retval None
*/
void getACCValues(float* accValues)
{
	
	int i = 0;
	for(i = 0; i < 3; i++)
	//STILL NEED SEMAPHORE TO PROTECT ACCESS
		accValues[i] = accCorrectedValues[i];
}

/**
*@brief A function that safely access's the temperature value of the temperature sensor
*@param[in] temp A the new location in memory for temperature
*@retval temperature
*/
float getTemperature(float temp)
{
	//STILL NEED SEMAPHORE TO PROTECT ACCESS
	return temp = temperature;
}