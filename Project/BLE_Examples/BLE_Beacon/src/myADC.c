#include "myadc.h"
#include "BlueNRG1_adc.h"
#include "BlueNRG1_sysCtrl.h"

ADC_InitType xADC_InitType;

void ADC_Configuration(void)
{ 
  SysCtrl_PeripheralClockCmd(CLOCK_PERIPH_ADC, ENABLE);
  
  /* Configure ADC */
  /* ADC_Input_AdcPin1 == ADC1 */
  /* ADC_Input_AdcPin2 == ADC2 */
  /* ADC_Input_AdcPin12 == ADC1 - ADC2 */
  xADC_InitType.ADC_OSR = ADC_OSR_200;
  //ADC_Input_BattSensor; //ADC_Input_TempSensor;// ADC_Input_AdcPin1 // ADC_Input_AdcPin12 // ADC_Input_AdcPin2
  xADC_InitType.ADC_Input = ADC_Input_AdcPin1;//ADC_Input_AdcPin12; //ADC_Input_AdcPin12;
  xADC_InitType.ADC_ConversionMode = ADC_ConversionMode_Single;
  xADC_InitType.ADC_ReferenceVoltage = ADC_ReferenceVoltage_0V6;
  xADC_InitType.ADC_Attenuation = ADC_Attenuation_9dB54;
  
  ADC_Init(&xADC_InitType);
  
  /* Enable auto offset correction */
  ADC_Calibration(ENABLE);
  ADC_AutoOffsetUpdate(ENABLE);
}
