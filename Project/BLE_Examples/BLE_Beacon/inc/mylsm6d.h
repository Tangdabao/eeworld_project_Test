#ifndef __MYLSM6D_H
#define __MYLSM6D_H
#include <stdio.h>
#include "lsm6ds3.h"

typedef struct {
	int8_t  Name_LSD6;
  int32_t AXIS_X;
  int32_t AXIS_Y;
  int32_t AXIS_Z;
} AxesRaw_t;


void LSM6D_Configuration(void);
void Init_Pressure_Temperature_Sensor(void);
IMU_6AXES_StatusTypeDef GetAccAxesRaw(AxesRaw_t * acceleration_data);
IMU_6AXES_StatusTypeDef GetRead_ID(AxesRaw_t * Device_Name_data);
void Acc_Update(AxesRaw_t *data);
void Press_Update(void);
#endif
