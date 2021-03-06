/******************** (C) COPYRIGHT 2015 STMicroelectronics ********************
* File Name          : UART/Polling/main.c 
* Author             : RF Application Team
* Version            : V1.0.0
* Date               : September-2015
* Description        : Code demostrating the UART functionality
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/


/* Includes ------------------------------------------------------------------*/
#include "BlueNRG_x_device.h"
#include "BlueNRG1_conf.h"
#include "SDK_EVAL_Config.h"


/** @addtogroup BlueNRG1_StdPeriph_Examples BlueNRG1 Standard Peripheral Examples
  * @{
  */


/** @addtogroup UART_Examples UART Examples
  * @{
  */

/** @addtogroup UART_Polling UART Polling Example
  * @{
  */
    
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
* @brief  Main program code
* @param  None
* @retval None
*/
int main(void)
{
  uint8_t tmp;

  /* System initialization function */
  SystemInit();
  
  /* Identify BlueNRG1 platform */
  SdkEvalIdentification();
  
  /* UART initialization */
  SdkEvalComUartInit(UART_BAUDRATE);
    
  /* Infinite loop */
  while(1) {
    /* Loop until the UART Receive Data Register is not empty */
    while (UART_GetFlagStatus(UART_FLAG_RXFE) == SET);
    
    /* Store the received byte in RxBuffer */
    tmp = UART_ReceiveData();

    /* Loop until UART DR register is empty */
    while (UART_GetFlagStatus(UART_FLAG_TXFE) == RESET);
    
    /* Send byte through UART */
    UART_SendData(tmp);
    
  }
  
}


#ifdef  USE_FULL_ASSERT

/**
* @brief  Reports the name of the source file and the source line number
*         where the assert_param error has occurred.
* @param  file: pointer to the source file name
* @param  line: assert_param error line source number
*/
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
  ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  
  /* Infinite loop */
  while (1)
  {
  }
}

#endif

/**
* @}
*/

/**
* @}
*/

/**
* @}
*/

/******************* (C) COPYRIGHT 2015 STMicroelectronics *****END OF FILE****/
