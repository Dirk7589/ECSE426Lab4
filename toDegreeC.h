/**
*@file toDegreeC.h
*@author Dirk Dubois, Alain Slak
*@date February 5th, 2013
*@brief A function that converts the adc value of the temperature sensor to celcius
*
*/

/*Defines*/
/**@defgroup Temperature_Scalers
* @{
*/

#define AVG_SLOPE 2.5 /**< avg_slope defines the avg_slope*/

#define AVG_SLOPE_INVERSE 1/ AVG_SLOPE /**< avg_slope_inverse defines the slope inverse*/

#define V25 760 /**< V25 defines the voltage at 25 degrees C*/

#define SCALE 0.7324 /**< SCALE defines the conversion factor from bits to miliVolts. */

/**
  * @}
  */
	
/**
* @brief Conversion function for ADC value to degrees Celcius.
* The following function connverts an ADC value from the temperature
* sensor to a float value representing the temperature in degrees Celcius.
* The following formula is used: Temperature (in °C) = {(VSENSE – V25) / Avg_Slope} + 25
* @param[in] vSense the value returned by the ADC connected to the temperature sensor in bits
* @retval The temperature in degree C.
* @note The value for avg_slope is 2.5mV/C and v25 is 0.76V
* @warning vSense is internally converted to a value in miliVolts. Be sure to set SCALE to the appropriate value.
*/
float toDegreeC(uint16_t vSense)
{
	vSense = vSense * SCALE; //Convert from bits to miliVolts
	float temperature = (((vSense - V25) * AVG_SLOPE_INVERSE ) + 25 ); //Apply formula to convert to temperature
	return temperature;
}
