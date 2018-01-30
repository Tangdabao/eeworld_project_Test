#include <stdio.h>
#include <string.h>
#include "hw_config.h"
#include "BlueNRG1_it.h"
#include "BlueNRG1_conf.h"
#include "sleep.h"
#include "bluenrg1_stack.h"
#include "ble_status.h"
#include "SDK_EVAL_Led.h"
#include "osal.h"
#include "Micro_Sleep_Test_config.h"
#include "ble_const.h"
#include "SDK_EVAL_Config.h"



#define WAKEUP_TIMER 0 //Virtual Timer #0
#define WAKEUP_TIMEOUT 10000 // 10sec 
static void Update_Beaconing(void);

uint32_t Count_1,Count_old;
//BLE//

void Device_Init(void)
{
    uint8_t ret;
    uint16_t service_handle;
    uint16_t dev_name_char_handle;
    uint16_t appearance_char_handle;

    /* Set the TX Power to -2 dBm */
    ret = aci_hal_set_tx_power_level(1, 4);
    if(ret != 0)
    {
        printf ("Error in aci_hal_set_tx_power_level() 0x%04xr\n", ret);
        while(1);
    }

    /* Init the GATT */
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
	  uint8_t Num=0;
    uint8_t ret = BLE_STATUS_SUCCESS;
    /* Set AD Type Flags at beginning on Advertising packet  */
    uint8_t adv_data[] =
    {
        /* Advertising data: Flags AD Type */
        0x02,
        0x01,
        0x06,
        /* Advertising data: manufacturer specific data */
        26, //len
        AD_TYPE_MANUFACTURER_SPECIFIC_DATA,  //manufacturer type
        0x30, 0x00, //Company identifier code (Default is 0x0030 - STMicroelectronics: To be customized for specific identifier)
        0x02,       // ID
        0x15,       //Length of the remaining payload
        0xE2, 0x0A, 0x39, 0xF4, 0x73, 0xF5, 0x4B, 0xC4, //Location UUID
        0xA1, 0x2F, 0x17, 0xD1, 0xAD, 0x07, 0xA9, 0x61,
        0x00, 0x00, // Major number
        0x00, 0x00, // Minor number
        Num        //2's complement of the Tx power (-56dB)};
    };

    /* disable scan response */
    ret = hci_le_set_scan_response_data(0, NULL);
    if (ret != BLE_STATUS_SUCCESS)
    {
        printf ("Error in hci_le_set_scan_resp_data() 0x%04x\r\n", ret);
        return;
    }
    else
        printf ("hci_le_set_scan_resp_data() --> SUCCESS\r\n");

    /* put device in non connectable mode */
    ret = aci_gap_set_discoverable(ADV_NONCONN_IND, 0x4000, 0x4000, PUBLIC_ADDR, NO_WHITE_LIST_USE,
                                   0, NULL, 0, NULL, 0, 0);
    if (ret != BLE_STATUS_SUCCESS)
    {
        printf ("Error in aci_gap_set_discoverable() 0x%04x\r\n", ret);
        return;
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
  /* Set AD Type Flags at beginning on Advertising packet  */
  uint8_t adv_data[] = {
      /* Advertising data: Flags AD Type */
      0x02, 
      0x01, 
      0x06,
      10,
		  AD_TYPE_MANUFACTURER_SPECIFIC_DATA,
      0x11, 
      0x22, 
      0x33, 
      0x44,
      0x55,
      0x66,
      0x77,
      0x88,
      0x99,
  };
  /* Update the ADV data with the BEACON manufacturing data */
  hci_le_set_advertising_data (sizeof(adv_data), adv_data);
}


/* Enable the Sleep - SLEEPMODE_WAKETIMER */
void sleep_timer(void)
{
  uint8_t ret;  
  printf("\r\nEnter to Sleep - SLEEPMODE_WAKETIMER\r\n");
  ret = HAL_VTimerStart_ms(WAKEUP_TIMER, WAKEUP_TIMEOUT);
  if (ret != BLE_STATUS_SUCCESS) {
    printf("HAL_VTimerStart_ms() error 0x%02x\r\n", ret);
    while(1);
  }
  BTLE_StackTick(); 
	//Test
	//aci_gap_set_non_discoverable();
  ret = BlueNRG_Sleep(SLEEPMODE_WAKETIMER, 0, 0);
  if (ret != BLE_STATUS_SUCCESS) {
    printf("BlueNRG_Sleep() error 0x%02x\r\n", ret);
    while(1);
  }
}



int main(void)
{
  uint8_t  ret;
  SystemInit();
  SdkEvalIdentification();
  HW_Config();
  ret = BlueNRG_Stack_Initialization(&BlueNRG_Stack_Init_params);
  if (ret != BLE_STATUS_SUCCESS) {
    printf("Error during BlueNRG_Stack_Initialization() 0x%02x\r\n", ret);
    while(1);
  }
	
   Device_Init();
   printf("Sleep Test_Demo\r\n");  
   sleep_timer(); 
	
	while(1) {
		BTLE_StackTick();
		printf("Sleep Test\r\n");
		
  }
}

void HAL_VTimerTimeoutCallback(uint8_t timerNum)
{
	 HAL_VTimer_Stop(WAKEUP_TIMER);
   Start_Beaconing();
  /* Add app code to execute @ Sleep timeout */
	if (timerNum==WAKEUP_TIMER)
	{ 
		Count_old=Count_1;
		Count_1=HAL_VTimerGetCurrentTime_sysT32();
		printf("HAL_VTimerGetCurrentTime_sysT32 : %d\r\n",Count_1);
		printf("HAL_VTimerGetCurrentTime_sysT32 : %d\r\n",Count_old);
		printf("a-b : %d\r\n",Count_1-Count_old);
		printf("HAL_VTimerDiff_ms_sysT32 : %d\r\n",HAL_VTimerDiff_ms_sysT32(Count_1,Count_old));
		Update_Beaconing();
		printf("HAL_VTimerTimeoutCallback\r\n");
		aci_gap_set_non_discoverable();
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


