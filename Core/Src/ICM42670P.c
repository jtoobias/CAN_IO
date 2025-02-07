/*
 * ICM42670P.c
 *
 *  Created on: Dec 26, 2024
 *      Author: rustu
 */
#include "ICM42670P.h"


ICM42670P_t ICM42670P;
// SPI configuration
//SPI Operational Features
//1. Data is delivered MSB first and LSB last
//2. Data is latched on the rising edge of SCLK
//3. Data should be transitioned on the falling edge of SCLK
//4. The maximum frequency of SCLK is 24 MHz
//5. SPI read and write operations are completed in 16 or more clock cycles (two or more bytes). The first
//byte contains the Register Address, and the following byte(s) contain(s) the SPI data. The first bit of the
//first byte contains the Read/Write bit and indicates the Read (1) operation. The following 7 bits contain
//the Register Address. In cases of multiple-byte Reads, data is two or more bytes:
uint8_t ICM42670P_init (SPI_HandleTypeDef *hSPI){
	ICM42670P.SPI_handler = hSPI;
	uint8_t status = 0;
	//  Init for ICM42670
	status |= (ICM42670P_write_reg(ICM42670_PWRMGMT0_REG,0x0F) == HAL_OK);
	//   4+-g, 50 Hz (0x0B), 100Hz (0x0A), 400Hz (0x07)
	status |= (ICM42670P_write_reg(ICM42670_ACCEL_CONFIG0,0x2A) == HAL_OK);
	//  1000+-, 50 Hz(0x0B), 100Hz (0x0A), 400Hz (0x07)
	status |= (ICM42670P_write_reg(ICM42670_GYRO_CONFIG0,0x2A) == HAL_OK);
	// Int1 = active high, push pull, pulse 0x03
	status |= (ICM42670P_write_reg(ICM42670_INT_CONFIG,0x03) == HAL_OK);
	// Int1 trigger on ready data
	status |= (ICM42670P_write_reg(ICM42670_INT_SOURCE0,0x08) == HAL_OK);


	ICM42670P.accel_x_offset = 0;
	ICM42670P.accel_y_offset = 0;
	ICM42670P.accel_z_offset = 0;

	ICM42670P.gyro_x_offset = 0;
	ICM42670P.gyro_y_offset = 0;
	ICM42670P.gyro_z_offset = 0;

//	 Very rough guess
	ICM42670P.temperature_offset = 0;

	return status;
}

uint8_t ICM42670P_write_reg (uint8_t addr, uint8_t write_data){
	uint8_t tx_buff[2] = {addr,write_data};
	uint8_t rx_buff[2];

	//Select IMU: CS pin low for IMU
	HAL_GPIO_WritePin(IMU_CS_GPIO_Port,IMU_CS_Pin,GPIO_PIN_RESET);
	// Read register
	uint8_t status = (HAL_SPI_TransmitReceive(ICM42670P.SPI_handler,tx_buff,rx_buff,2,HAL_MAX_DELAY) == HAL_OK);
	//Select IMU: CS pin low for IMU
	HAL_GPIO_WritePin(IMU_CS_GPIO_Port,IMU_CS_Pin,GPIO_PIN_SET);

	return status;
}

uint8_t ICM42670_read_reg (uint8_t addr, uint8_t* read_data){
	uint8_t tx_buff[2] = {(0x80 | addr),0x00};
	uint8_t rx_buff[2] = {0,0};

	//Select IMU: CS pin low for IMU
	HAL_GPIO_WritePin(IMU_CS_GPIO_Port,IMU_CS_Pin,GPIO_PIN_RESET);
	// Read register
	uint8_t status = (HAL_SPI_TransmitReceive(ICM42670P.SPI_handler,tx_buff,rx_buff,2,HAL_MAX_DELAY) == HAL_OK);
	//Select IMU: CS pin low for IMU
	HAL_GPIO_WritePin(IMU_CS_GPIO_Port,IMU_CS_Pin,GPIO_PIN_SET);

		// Second byte has the data
	*read_data = rx_buff[1];

	return status;
}

uint8_t ICM42670P_update_temperature (void){
	uint8_t tx_buff[3] = {(0x80 | ICM42670_TEMP_DATA1),0,0};
	uint8_t rx_buff[3] = {0};
	int16_t tmp = 0;

	//Select IMU: CS pin low for IMU
	HAL_GPIO_WritePin(IMU_CS_GPIO_Port,IMU_CS_Pin,GPIO_PIN_RESET);
	// Read register
	uint8_t status = (HAL_SPI_TransmitReceive(ICM42670P.SPI_handler,tx_buff,rx_buff,sizeof(tx_buff),HAL_MAX_DELAY) == HAL_OK);
	HAL_GPIO_WritePin(IMU_CS_GPIO_Port,IMU_CS_Pin,GPIO_PIN_SET);
	tmp = (int16_t)(rx_buff[1] << 8 | rx_buff[2]);//Select IMU: CS pin low for IMU
	//	• Temperature in Degrees Centigrade = (TEMP_DATA / 128) + 25
	ICM42670P.temperature = ((tmp*10)/128) + (25*10) + ICM42670P.temperature_offset;
	return status;
}

// Check if union array can be used instead of rxbuff (change union arrays to uint8_t)
uint8_t ICM42670P_update_accel_data (void){

	uint8_t tx_buff[7] = {(0x80 | ICM42670_ACCEL_DATA_X1),0,0,0,0,0,0};
	uint8_t rx_buff[7] = {0};
	uint8_t status = 0;
	//Select IMU: CS pin low for IMU
	HAL_GPIO_WritePin(IMU_CS_GPIO_Port,IMU_CS_Pin,GPIO_PIN_RESET);
	// Read register
	status = (HAL_SPI_TransmitReceive(ICM42670P.SPI_handler,tx_buff,rx_buff,sizeof(tx_buff),HAL_MAX_DELAY) == HAL_OK);
	HAL_GPIO_WritePin(IMU_CS_GPIO_Port,IMU_CS_Pin,GPIO_PIN_SET);

    // Combine high and low bytes for each axis
	ICM42670P.accel_x = (int16_t)(rx_buff[1] << 8 | rx_buff[2]); // X-axis (X and Y mixed up)
	ICM42670P.accel_y = (int16_t)(rx_buff[3] << 8 | rx_buff[4]); // Y-axis
	ICM42670P.accel_z = (int16_t)(rx_buff[5] << 8 | rx_buff[6]); // Z-axis;

	return status;
}

uint8_t ICM42670P_update_gyro_data (void){

	uint8_t tx_buff[7] = {(0x80 | ICM42670_GYRO_DATA_X1),0,0,0,0,0,0};
	uint8_t rx_buff[7] = {0};
	uint8_t status = 0;
	//Select IMU: CS pin low for IMU
	HAL_GPIO_WritePin(IMU_CS_GPIO_Port,IMU_CS_Pin,GPIO_PIN_RESET);
	// Read register
	status = (HAL_SPI_TransmitReceive(ICM42670P.SPI_handler,tx_buff,rx_buff,sizeof(tx_buff),HAL_MAX_DELAY) == HAL_OK);
	HAL_GPIO_WritePin(IMU_CS_GPIO_Port,IMU_CS_Pin,GPIO_PIN_SET);

    // Combine high and low bytes for each axis
	ICM42670P.gyro_x = (int16_t)(rx_buff[1] << 8 | rx_buff[2]); // X-axis (X and Y mixed up)
	ICM42670P.gyro_y = (int16_t)(rx_buff[3] << 8 | rx_buff[4]); // Y-axis
	ICM42670P.gyro_z = (int16_t)(rx_buff[5] << 8 | rx_buff[6]); // Z-axis
	return status;

}


uint8_t ICM42670P_read_imu_DMA (void){

	uint8_t tx_buff[15] = {0};
	uint8_t status = 0;
	tx_buff[0] = (0x80 | ICM42670_TEMP_DATA1);
	//Select IMU: CS pin low for IMU
	HAL_GPIO_WritePin(IMU_CS_GPIO_Port,IMU_CS_Pin,GPIO_PIN_RESET);
	// Read register
	status = (HAL_SPI_TransmitReceive_DMA(ICM42670P.SPI_handler,tx_buff,ICM42670P.data_rx_buffer,sizeof(tx_buff)) == HAL_OK);

	if (status == 1){
		return 1;
	} else {
		// Read failed, de-select the chip
		HAL_GPIO_WritePin(IMU_CS_GPIO_Port,IMU_CS_Pin,GPIO_PIN_SET);
		return 0;
	}
}

// This whole function might take around 0.5us to complete (division is very costly x10 for than multiplication,
// but floating point is just as bad on the uC)
void ICM42670P_read_imu_DMA_complete (void){
	// Read completed, de-select the chip
	HAL_GPIO_WritePin(IMU_CS_GPIO_Port,IMU_CS_Pin,GPIO_PIN_SET);

	 // Temperature
	int16_t temp = (int16_t)(ICM42670P.data_rx_buffer[1] << 8 | ICM42670P.data_rx_buffer[2]);
	ICM42670P.temperature = ((temp*10)/128) + (25*10) + ICM42670P.temperature_offset;
    // Combine high and low bytes for each axis
	ICM42670P.accel_y = ((int16_t)(ICM42670P.data_rx_buffer[3] << 8 | ICM42670P.data_rx_buffer[4])); // Y-axis
	ICM42670P.accel_x = (int16_t)(ICM42670P.data_rx_buffer[5] << 8 | ICM42670P.data_rx_buffer[6]); // X-axis (X and Y mixed up)
	ICM42670P.accel_z = (int16_t)(ICM42670P.data_rx_buffer[7] << 8 | ICM42670P.data_rx_buffer[8]); // Z-axis
    // Combine high and low bytes for each axis
	ICM42670P.gyro_y = (int16_t)(ICM42670P.data_rx_buffer[9] << 8 | ICM42670P.data_rx_buffer[10]); // Y-axis
	ICM42670P.gyro_x = (int16_t)(ICM42670P.data_rx_buffer[11] << 8 | ICM42670P.data_rx_buffer[12]); // X-axis (X and Y mixed up)
	ICM42670P.gyro_z = (int16_t)(ICM42670P.data_rx_buffer[13] << 8 | ICM42670P.data_rx_buffer[14]); // Z-axis

	ICM42670P.accel_x_conv = (((int32_t)ICM42670P.accel_x*ACCELERATION_G_EARTH_X100)/ACCEL_UI_FS_SEL2_RES)+ICM42670P.accel_x_offset;
	ICM42670P.accel_y_conv = (((int32_t)ICM42670P.accel_y*ACCELERATION_G_EARTH_X100)/ACCEL_UI_FS_SEL2_RES)+ICM42670P.accel_y_offset;
	ICM42670P.accel_z_conv = (((int32_t)ICM42670P.accel_z*ACCELERATION_G_EARTH_X100)/ACCEL_UI_FS_SEL2_RES)+ICM42670P.accel_z_offset;

	ICM42670P.gyro_x_conv = ((int32_t)ICM42670P.gyro_x*1000)/GYRO_UI_FS_SEL2_RES_X10+ICM42670P.gyro_x_offset;
	ICM42670P.gyro_y_conv = ((int32_t)ICM42670P.gyro_y*1000)/GYRO_UI_FS_SEL2_RES_X10+ICM42670P.gyro_y_offset;
	ICM42670P.gyro_z_conv = ((int32_t)ICM42670P.gyro_z*1000)/GYRO_UI_FS_SEL2_RES_X10+ICM42670P.gyro_z_offset;
}
