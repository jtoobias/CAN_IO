/*
 * ICM42670P.h
 *
 *  Created on: Dec 26, 2024
 *      Author: rustu
 */

#ifndef INC_ICM42670P_H_
#define INC_ICM42670P_H_
// Includes
#include "stm32f1xx_hal.h"
#include "main.h"
#include "ICM42670P.h"

// Defines
	// REGISTERS

#define ICM42670_WHOAMI_REG		0x75
#define ICM42670_PWRMGMT0_REG	0x1F

#define ICM42670_INT_CONFIG		0x06
#define ICM42670_INT_SOURCE0	0x2B
#define ICM42670_INT_SOURCE1	0x2C
#define ICM42670_INT_SOURCE3	0x2D
#define ICM42670_INT_SOURCE4	0x2E

#define ICM42670_TEMP_DATA1		0x09
#define ICM42670_TEMP_DATA0		0x0A

#define ICM42670_ACCEL_DATA_X1	0x0B
#define ICM42670_ACCEL_DATA_X0	0x0C
#define ICM42670_ACCEL_DATA_Y1	0x0D
#define ICM42670_ACCEL_DATA_Y0	0x0E
#define ICM42670_ACCEL_DATA_Z1	0x0F
#define ICM42670_ACCEL_DATA_Z0	0x10

#define ICM42670_GYRO_DATA_X1	0x11
#define ICM42670_GYRO_DATA_X0	0x12
#define ICM42670_GYRO_DATA_Y1	0x13
#define ICM42670_GYRO_DATA_Y0	0x14
#define ICM42670_GYRO_DATA_Z1	0x15
#define ICM42670_GYRO_DATA_Z0	0x16


#define ICM42670_GYRO_CONFIG0	0x20
#define ICM42670_ACCEL_CONFIG0	0x21
#define ICM42670_GYRO_CONFIG1	0x23
#define ICM42670_ACCEL_CONFIG1	0x24
	// Configuration

#define GYRO_UI_FS_SEL1_RES_X10	164		// (LSB*10/(º/s)) for UI_FS_SEL = 1
#define GYRO_UI_FS_SEL2_RES_X10	328		// (LSB*10/(º/s)) for UI_FS_SEL = 2
#define GYRO_UI_FS_SEL3_RES_X10	655		// (LSB*10/(º/s)) for UI_FS_SEL = 3
#define GYRO_UI_FS_SEL4_RES_X10	1310	// (LSB*10/(º/s)) for UI_FS_SEL = 4

#define ACCEL_UI_FS_SEL1_RES	2048	// (LSB/g) for UI_FS_SEL = 1
#define ACCEL_UI_FS_SEL2_RES	4096	// (LSB/g) for UI_FS_SEL = 2
#define ACCEL_UI_FS_SEL3_RES	8192	// (LSB/g) for UI_FS_SEL = 3
#define ACCEL_UI_FS_SEL4_RES	16384	// (LSB/g) for UI_FS_SEL = 4

#define INV_GYRO_UI_FS_SEL1_RES_X10	1/GYRO_UI_FS_SEL1_RES_X10		// (LSB*10/(º/s)) for UI_FS_SEL = 1
#define INV_GYRO_UI_FS_SEL2_RES_X10	1/GYRO_UI_FS_SEL2_RES_X10		// (LSB*10/(º/s)) for UI_FS_SEL = 2
#define INV_GYRO_UI_FS_SEL3_RES_X10	1/GYRO_UI_FS_SEL3_RES_X10		// (LSB*10/(º/s)) for UI_FS_SEL = 3
#define INV_GYRO_UI_FS_SEL4_RES_X10	1/GYRO_UI_FS_SEL4_RES_X10	// (LSB*10/(º/s)) for UI_FS_SEL = 4

#define INV_ACCEL_UI_FS_SEL1_RES	1/ACCEL_UI_FS_SEL1_RES	// (LSB/g) for UI_FS_SEL = 1
#define INV_ACCEL_UI_FS_SEL2_RES	1/ACCEL_UI_FS_SEL2_RES	// (LSB/g) for UI_FS_SEL = 2
#define INV_ACCEL_UI_FS_SEL3_RES	1/ACCEL_UI_FS_SEL3_RES	// (LSB/g) for UI_FS_SEL = 3
#define INV_ACCEL_UI_FS_SEL4_RES	1/ACCEL_UI_FS_SEL4_RES	// (LSB/g) for UI_FS_SEL = 4

#define ACCELERATION_G_EARTH_X100 	981	// (m/s/s)*100/g

// Variables declarations

typedef struct {

	SPI_HandleTypeDef *SPI_handler;
	int16_t gyro_x_offset;
	int16_t gyro_y_offset;
	int16_t gyro_z_offset;

	int16_t accel_x_offset;
	int16_t accel_y_offset;
	int16_t accel_z_offset;

	uint8_t data_rx_buffer[15];

	int16_t accel_x;
	int16_t accel_y;
	int16_t accel_z;

	int16_t accel_x_conv;		// m*100/s/s
	int16_t accel_y_conv;		// m*100/s/s
	int16_t accel_z_conv;		// m*100/s/s

	int16_t gyro_x;
	int16_t gyro_y;
	int16_t gyro_z;

	int16_t gyro_x_conv;		// deg*100/s
	int16_t gyro_y_conv;		// deg*100/s
	int16_t gyro_z_conv;		// deg*100/s


	int32_t temperature;	// degrees*10
	int32_t temperature_offset;


}ICM42670P_t;

extern ICM42670P_t ICM42670P;
// Functions
uint8_t ICM42670P_init (SPI_HandleTypeDef *hSPI);
uint8_t ICM42670P_read_reg (uint8_t addr, uint8_t* read_data);
uint8_t ICM42670P_write_reg (uint8_t addr, uint8_t write_data);
uint8_t ICM42670P_upate_temperature (void);
uint8_t ICM42670P_update_accel_data (void);
uint8_t ICM42670P_update_gyro_data (void);
uint8_t ICM42670P_read_imu_DMA (void);
void ICM42670P_read_imu_DMA_complete (void);
#endif /* INC_ICM42670P_H_ */
