/******************** (C) COPYRIGHT 2016 STMicroelectronics ********************
* File Name          : BLE_Beacon_main.c
* Author             : AMS - VMA RF Application Team
* Version            : V1.1.0
* Date               : 20-May-2016
* Description        : BlueNRG-1 main file for beacon device
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/
/** @addtogroup BlueNRG1_demonstrations_applications
 *  BlueNRG-1 Beacon demo \see BLE_Beacon_main.c for documentation.
 *
 *@{
 */

/** @} */
/** \cond DOXYGEN_SHOULD_SKIP_THIS
 */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "BlueNRG1_it.h"
#include "BlueNRG1_conf.h"
#include "ble_const.h"
#include "bluenrg1_stack.h"
#include "sleep.h"
#include "SDK_EVAL_Config.h"
#include "Beacon_config.h"
#include "OTA_btl.h"
//Private  Include//
#include "BlueNRG1_adc.h"
#include "myadc.h"
#include "mylsm6d.h"
#include "lsm6ds3.h"
#include "lsm6ds3_hal.h"
/* Private typedef -----------------------------------------------------------*/
AxesRaw_t acc_data;

//extern PRESSURE_DrvTypeDef* xLPS25HBDrv;
extern IMU_6AXES_DrvTypeDef *Imu6AxesDrv;
extern LSM6DS3_DrvExtTypeDef *Imu6AxesDrvExt;

/* Private define ------------------------------------------------------------*/
#define BLE_BEACON_VERSION_STRING "1.1.0"

/* Set to 1 for enabling Flags AD Type position at the beginning
   of the advertising packet */
#define ENABLE_FLAGS_AD_TYPE_AT_BEGINNING 1
//Test Define
#define ADC_Test   0
#define ACC_Test   1
#define Press_Test 0
#define VTimer_Num 0
#define WAKEUP_TIMEOUT 20000 //20S


#define H16(x)   (((int) (x)) >> 8)
#define L16(x)   (((int) (x)))

#define H32(x)   (((int) (x)) >> 24)
#define MH32(x)  (((int) (x)) >> 16)
#define L32(x)   (((int) (x)) >> 8)
#define XL32(x)  (((int) (x)))


/*****************************/
float vlotage = 3.3;//V
uint32_t time = 2500;//ms
float pressure = 101.5;//hPa
float temperature = 26.2;//degC
float angleX = 10.2;//deg
float angleY = 0.3;//deg
/*****************************/


/* Private macro -------------------------------------------------------------*/
extern ADC_InitType xADC_InitType;

/* Private variables ---------------------------------------------------------*/
volatile uint32_t lSystickCounter = 0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void DelayMs(volatile uint32_t lTimeMs);
void sleep_timer(void);
void Display(void);

void Device_Init(void)
{
    uint8_t ret;
    uint16_t service_handle;
    uint16_t dev_name_char_handle;
    uint16_t appearance_char_handle;
    ret = aci_hal_set_tx_power_level(0, 3);
    if(ret != 0)
    {
        printf ("Error in aci_hal_set_tx_power_level() 0x%04xr\n", ret);
        NVIC_SystemReset();
    }
    ret = aci_gatt_init();
    if (ret != 0)
        printf ("Error in aci_gatt_init() 0x%04xr\n", ret);
    else
        printf ("aci_gatt_init() --> SUCCESS\r\n");

    /* Init the GAP */
    ret = aci_gap_init(0x01, 0x00, 0x08, &service_handle,
                       &dev_name_char_handle, &appearance_char_handle);
    if (ret != 0)
        printf ("Error in aci_gap_init() 0x%04x\r\n", ret);
    else
        printf ("aci_gap_init() --> SUCCESS\r\n");
}


/**
* @brief  Start beaconing
* @param  None
* @retval None
*/
static void Start_Beaconing(void)
{
    uint8_t ret = BLE_STATUS_SUCCESS;
    uint8_t adv_data[] =
    {
        0x02,
        0x01,
        0x06,
        2,
        AD_TYPE_MANUFACTURER_SPECIFIC_DATA,  //manufacturer type
        0x30
    };
    ret = hci_le_set_scan_response_data(0, NULL);
    if (ret != BLE_STATUS_SUCCESS)
    {
        printf ("Error in hci_le_set_scan_resp_data() 0x%04x\r\n", ret);
    }
    else
        printf ("hci_le_set_scan_resp_data() --> SUCCESS\r\n");

    ret = aci_gap_set_discoverable(ADV_NONCONN_IND, 0x4000, 0x4000, PUBLIC_ADDR, NO_WHITE_LIST_USE,
                                   0, NULL, 0, NULL, 0, 0);
    if (ret != BLE_STATUS_SUCCESS)
    {
        printf ("Error in aci_gap_set_discoverable() 0x%04x\r\n", ret);
        NVIC_SystemReset();
    }
    else
        printf ("aci_gap_set_discoverable() --> SUCCESS\r\n");

    /* Set the  ADV data with the Flags AD Type at beginning of the
       advertsing packet,  followed by the beacon manufacturer specific data */
    ret = hci_le_set_advertising_data (sizeof(adv_data), adv_data);
    if (ret != BLE_STATUS_SUCCESS)
    {
        printf ("Error in hci_le_set_advertising_data() 0x%04x\r\n", ret);
        return;
    }
    else
        printf ("hci_le_set_advertising_data() --> SUCCESS\r\n");
}



static void Update_Beaconing(void)
{
    static uint8_t Count = 0;
    
	  Count++;
    printf("Update_Beaconing :%d \r\n", Count);

    /* Set AD Type Flags at beginning on Advertising packet  */
    uint8_t adv_data[] =
    {
        /* Advertising data: Flags AD Type */
        0x02,
        0x01,
        0x06,
        15,
        AD_TYPE_MANUFACTURER_SPECIFIC_DATA,
        H16(vlotage * 10),
        L16(vlotage * 10),
        H32(time),
        MH32(time),
        L32(time),
        XL32(time),
        H16(pressure * 10),
        L16(pressure * 10),
        H16(temperature * 10),
        L16(temperature * 10),
        H16(angleX * 10),
        L16(angleX * 10),
        H16(angleY * 10),
        L16(angleY * 10),

    };
    /* Update the ADV data with the BEACON manufacturing data */
    hci_le_set_advertising_data (sizeof(adv_data), adv_data);
}

void sleep_timer(void)
{
    uint8_t ret;
    printf("\r\nEnter to Sleep - SLEEPMODE_WAKETIMER\r\n");

    ret = HAL_VTimerStart_ms(VTimer_Num, WAKEUP_TIMEOUT);
    if (ret != BLE_STATUS_SUCCESS)
    {
        printf("HAL_VTimerStart_ms() error 0x%02x\r\n", ret);
    }
    BTLE_StackTick();

    ret = BlueNRG_Sleep(SLEEPMODE_WAKETIMER, 0, 0);
    if (ret != BLE_STATUS_SUCCESS)
    {
        printf("BlueNRG_Sleep() error 0x%02x\r\n", ret);
    }

}

void Display(void)
{
    //************ADC*******************//
    if( ADC_GetFlagStatus(ADC_FLAG_EOC))
    {
        vlotage = ADC_GetConvertedData(xADC_InitType.ADC_Input, xADC_InitType.ADC_ReferenceVoltage);
        //vlotage = adc_value / 1000;

        if(vlotage < 0)
            vlotage = 0;
        printf("ADC value: %.0f V\r\n", vlotage);
        ADC_Cmd(ENABLE);
    }
    //************ACC*******************//
    if (GetAccAxesRaw(&acc_data) == IMU_6AXES_OK)
    {
        Acc_Update(&acc_data);
    }
    //************Press*******************//
    Press_Update();
		
		//
		Update_Beaconing();
}

int main(void)
{
    uint8_t ret;

    float adc_value;
    SystemInit();
    SdkEvalIdentification();
    SdkEvalComUartInit(UART_BAUDRATE);
    printf("This is Test Demo \r\n");

    //*********Start BLE**********//
    ret = BlueNRG_Stack_Initialization(&BlueNRG_Stack_Init_params);
    if (ret != BLE_STATUS_SUCCESS)
    {
        printf("Error in BlueNRG_Stack_Initialization() 0x%02x\r\n", ret);
        while(1);
    }
    Device_Init();
    Start_Beaconing();
    //*********Start ADC**********//
    ADC_Configuration();
    ADC_Cmd(ENABLE);
    //*********Start LSM6D**********//
    LSM6D_Configuration();
    //*********Start LSM6D**********//
    Init_Pressure_Temperature_Sensor();

    while(1)
    {
        //Display();
        //Update_Beaconing();
        /* BlueNRG-1 stack tick */
        BTLE_StackTick();
        sleep_timer();
    }
}


/***************************************************************************************/
void DelayMs(volatile uint32_t lTimeMs)
{
    uint32_t i = 0;
    while(lTimeMs--)
    {
        i = 4000;
        while(i--) ;
    }

}
/***************************************************************************************/
void HAL_VTimerTimeoutCallback(uint8_t timerNum)
{
    HAL_VTimer_Stop(VTimer_Num);
    if(timerNum == VTimer_Num)
    {
        //Update_Beaconing();
        Display();
			  printf(" This is  HAL_VTimerTimeoutCallback OK\r\n");
        sleep_timer();
    }
}
#ifdef  USE_FULL_ASSERT

/**
* @brief  Reports the name of the source file and the source line number
*         where the assert_param error has occurred.
* @param  file: pointer to the source file name
* @param  line: assert_param error line source number
*/
void assert_failed(uint8_t *file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    while (1)
    {
    }
}

#endif

/******************* (C) COPYRIGHT 2015 STMicroelectronics *****END OF FILE****/
/** \endcond
 */
