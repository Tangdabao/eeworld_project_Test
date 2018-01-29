#include "mylsm6d.h"
#include "LPS25HB.h"
#include "lsm6ds3.h"
#include "lsm6ds3_hal.h"
#include "SDK_EVAL_Config.h"
#include <string.h>

PRESSURE_DrvTypeDef *xLPS25HBDrv = &LPS25HBDrv;
IMU_6AXES_DrvTypeDef *Imu6AxesDrv = NULL;
LSM6DS3_DrvExtTypeDef *Imu6AxesDrvExt = NULL;
static AxesRaw_t acceleration_data;

/*****************************/
extern float pressure;
extern float temperature;
extern float angleX;//deg
extern float angleY;//deg
/*****************************/

void LSM6D_Configuration(void)
{
    IMU_6AXES_InitTypeDef InitStructure;
    uint8_t tmp1 = 0x00;

    Imu6AxesDrv = &LSM6DS3Drv;
    Imu6AxesDrvExt = &LSM6DS3Drv_ext_internal;
    InitStructure.G_FullScale      = 125.0f;
    InitStructure.G_OutputDataRate = 13.0f;
    InitStructure.G_X_Axis         = 0; //1;
    InitStructure.G_Y_Axis         = 0;//1;
    InitStructure.G_Z_Axis         = 0; //1;
    InitStructure.X_FullScale      = 2.0f;
    InitStructure.X_OutputDataRate = 13.0f;
    InitStructure.X_X_Axis         = 1;
    InitStructure.X_Y_Axis         = 1;
    InitStructure.X_Z_Axis         = 1;
    Imu6AxesDrv->Init(&InitStructure);

    /* Disable all mems IRQs in order to enable free fall detection */ //TBR
    LSM6DS3_IO_Write(&tmp1, LSM6DS3_XG_MEMS_ADDRESS, LSM6DS3_XG_INT1_CTRL, 1);

    /* Clear first previous data */
    Imu6AxesDrv->Get_X_Axes((int32_t *)&acceleration_data);

    /* Enable Free fall detection */
    //Imu6AxesDrvExt->Enable_Free_Fall_Detection();
}


void Init_Pressure_Temperature_Sensor(void)
{
    /* LPS25HB initialization */

    PRESSURE_InitTypeDef InitStructure;
    InitStructure.OutputDataRate = LPS25HB_ODR_1Hz;
    InitStructure.BlockDataUpdate = LPS25HB_BDU_READ; //LPS25HB_BDU_READ LPS25HB_BDU_CONT
    InitStructure.DiffEnable = LPS25HB_DIFF_ENABLE;  // LPS25HB_DIFF_ENABLE
    InitStructure.SPIMode = LPS25HB_SPI_SIM_3W;  // LPS25HB_SPI_SIM_3W
    InitStructure.PressureResolution = LPS25HB_P_RES_AVG_32;
    InitStructure.TemperatureResolution = LPS25HB_T_RES_AVG_16;
    xLPS25HBDrv->Init(&InitStructure);
}


IMU_6AXES_StatusTypeDef GetAccAxesRaw(AxesRaw_t * acceleration_data)
{
  IMU_6AXES_StatusTypeDef status = IMU_6AXES_OK;
  status = Imu6AxesDrv->Get_X_Axes((int32_t *)acceleration_data);
	
  return status;
}



IMU_6AXES_StatusTypeDef GetRead_ID(AxesRaw_t * Device_Name_data)
{
  IMU_6AXES_StatusTypeDef status = IMU_6AXES_OK;
  status = Imu6AxesDrv->Read_XG_ID((uint8_t *)Device_Name_data);
  return status;
}


void Acc_Update(AxesRaw_t *data)
{  
  uint8_t buff[6];
	 //HOST_TO_LE_16(buff,  data->AXIS_X);
   //HOST_TO_LE_16(buff+2,data->AXIS_Y); 
   //HOST_TO_LE_16(buff+4,data->AXIS_Z);
   printf("/********ACC_Data*********/\r\n");
	 printf("ACC_X: %d \r\n",data->AXIS_X);
	 printf("ACC_Y: %d \r\n",data->AXIS_Y);
	 printf("ACC_Z: %d \r\n",data->AXIS_Z);
	 angleX=data->AXIS_X;
	 angleY=data->AXIS_Y;
	 //printf("GCC_X: %x \r\n",buff[3]);
	 //printf("GCC_Y: %x \r\n",buff[4]);
	 //printf("GCC_Z: %x \r\n",buff[5]);
	 printf("/********ACC_Data*********/\r\n");
    
}

void Press_Update(void)
{
    if (xLPS25HBDrv->GetPressure(&pressure) == 0)
    {
      //Press_Update((int32_t)(data *100)); 
			printf("LPS25HB_Press: %lf \r\n",pressure);
		}
		if(xLPS25HBDrv->GetTemperature(&temperature)==0)
    {
	  	printf("LPS25HB_Temperature : %lf \r\n",temperature);
	
		}
		
		

}
