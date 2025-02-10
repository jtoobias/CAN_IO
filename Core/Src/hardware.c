/*
 * hardware.c
 *
 *  Created on: Nov 14, 2024
 *      Author: Jonathan
 */
#include "hardware.h"
#include "main.h"
#include <string.h>
// Variables
// Define the global instance of hardware_t
hardware_t hardware;
eeprom_storage_t eeprom_storage;
volatile uint32_t ms_timer = 0;

// Functions

// used to handle any hardware inits
void hardware_init(CAN_HandleTypeDef *can_handler,SPI_HandleTypeDef *hSPI){
	can_init(can_handler);
	ICM42670P_init(hSPI);
    EE_Init(&eeprom_storage,sizeof(eeprom_storage));
    EE_Read();
    	// If it is first boot clear all of the eeprom storage area and set the init flag
    if (eeprom_storage.init_flag != 1){
    	clear_pdo_config(&can.tpdo1);
    	clear_pdo_config(&can.tpdo2);
    	clear_pdo_config(&can.tpdo3);
    	clear_pdo_config(&can.tpdo4);

    	clear_pdo_config(&can.rpdo1);
    	clear_pdo_config(&can.rpdo2);
    	clear_pdo_config(&can.rpdo3);
    	clear_pdo_config(&can.rpdo4);
    	eeprom_storage.init_flag = 1;
    	can.save_settings = 1;
    	save_configuration();
    }
}

// This requires the EE_Write() to be called still
void update_can_ee_pdo_config(pdo_config_t *ee_pdo,pdo_config_t *pdo,int pdo_size){
	memcpy(ee_pdo,pdo,pdo_size);
}

// This requires the EE_Write() to be called still
void load_can_ee_pdo_config(pdo_config_t *pdo,pdo_config_t *ee_pdo,int pdo_size){
	memcpy(pdo,ee_pdo,pdo_size);
		// Have to manually set the function pointers.
	if (pdo->cb_function_id == 1){
		pdo->call_back = set_outputs;
	}
}
// Used to poll hardware
void hardware_monitoring(void){
//	read_inputs();
}

int8_t save_configuration(void){
	int8_t result = 0;
	if (can.save_settings == 1){
		update_can_ee_pdo_config(&eeprom_storage.rpdo1,&can.rpdo1.config,sizeof(can.rpdo1.config));
		update_can_ee_pdo_config(&eeprom_storage.rpdo2,&can.rpdo2.config,sizeof(can.rpdo2.config));
		update_can_ee_pdo_config(&eeprom_storage.rpdo3,&can.rpdo3.config,sizeof(can.rpdo3.config));
		update_can_ee_pdo_config(&eeprom_storage.rpdo4,&can.rpdo4.config,sizeof(can.rpdo4.config));

		update_can_ee_pdo_config(&eeprom_storage.tpdo1,&can.tpdo1.config,sizeof(can.tpdo1.config));
		update_can_ee_pdo_config(&eeprom_storage.tpdo2,&can.tpdo2.config,sizeof(can.tpdo2.config));
		update_can_ee_pdo_config(&eeprom_storage.tpdo3,&can.tpdo3.config,sizeof(can.tpdo3.config));
		update_can_ee_pdo_config(&eeprom_storage.tpdo4,&can.tpdo4.config,sizeof(can.tpdo4.config));
		result = EE_Write();
	}else if (can.save_settings == 2){
		eeprom_storage.accel_x_offset = ICM42670P.accel_x_offset;
		eeprom_storage.accel_y_offset = ICM42670P.accel_y_offset;
		eeprom_storage.accel_z_offset = ICM42670P.accel_z_offset;

		eeprom_storage.gyro_x_offset = ICM42670P.gyro_x_offset;
		eeprom_storage.gyro_y_offset = ICM42670P.gyro_y_offset;
		eeprom_storage.gyro_z_offset = ICM42670P.gyro_z_offset;

		eeprom_storage.temperature_offset = ICM42670P.temperature_offset;
		result = EE_Write();
	}else if (can.save_settings == 255){
			// Clear PDOs and save it to eeprom
    	clear_pdo_config(&can.tpdo1);
    	clear_pdo_config(&can.tpdo2);
    	clear_pdo_config(&can.tpdo3);
    	clear_pdo_config(&can.tpdo4);

    	clear_pdo_config(&can.rpdo1);
    	clear_pdo_config(&can.rpdo2);
    	clear_pdo_config(&can.rpdo3);
    	clear_pdo_config(&can.rpdo4);
    	eeprom_storage.init_flag = 1;
    	can.save_settings = 1;
    	result = save_configuration();
	}
	can.save_settings = 0;
	return result;
}


int8_t load_configuration(void){
	EE_Read();
	load_can_ee_pdo_config(&can.rpdo1.config,&eeprom_storage.rpdo1,sizeof(can.rpdo1.config));
	load_can_ee_pdo_config(&can.rpdo2.config,&eeprom_storage.rpdo2,sizeof(can.rpdo2.config));
	load_can_ee_pdo_config(&can.rpdo3.config,&eeprom_storage.rpdo3,sizeof(can.rpdo3.config));
	load_can_ee_pdo_config(&can.rpdo4.config,&eeprom_storage.rpdo4,sizeof(can.rpdo4.config));

	load_can_ee_pdo_config(&can.tpdo1.config,&eeprom_storage.tpdo1,sizeof(can.tpdo1.config));
	load_can_ee_pdo_config(&can.tpdo2.config,&eeprom_storage.tpdo2,sizeof(can.tpdo2.config));
	load_can_ee_pdo_config(&can.tpdo3.config,&eeprom_storage.tpdo3,sizeof(can.tpdo3.config));
	load_can_ee_pdo_config(&can.tpdo4.config,&eeprom_storage.tpdo4,sizeof(can.tpdo4.config));

	memset(can.rpdo1.config.number_of_bytes, 0, sizeof(can.rpdo1.config.number_of_bytes)); // Set all elements to 0
	memset(can.rpdo2.config.number_of_bytes, 0, sizeof(can.rpdo2.config.number_of_bytes)); // Set all elements to 0
	memset(can.rpdo3.config.number_of_bytes, 0, sizeof(can.rpdo3.config.number_of_bytes)); // Set all elements to 0
	memset(can.rpdo4.config.number_of_bytes, 0, sizeof(can.rpdo4.config.number_of_bytes)); // Set all elements to 0
	can.rpdo1.config.total_number_of_bytes = 0;
	can.rpdo2.config.total_number_of_bytes = 0;
	can.rpdo3.config.total_number_of_bytes = 0;
	can.rpdo4.config.total_number_of_bytes = 0;

	memset(can.tpdo1.config.number_of_bytes, 0, sizeof(can.tpdo1.config.number_of_bytes)); // Set all elements to 0
	memset(can.tpdo2.config.number_of_bytes, 0, sizeof(can.tpdo2.config.number_of_bytes)); // Set all elements to 0
	memset(can.tpdo3.config.number_of_bytes, 0, sizeof(can.tpdo3.config.number_of_bytes)); // Set all elements to 0
	memset(can.tpdo4.config.number_of_bytes, 0, sizeof(can.tpdo4.config.number_of_bytes)); // Set all elements to 0
	can.tpdo1.config.total_number_of_bytes = 0;
	can.tpdo2.config.total_number_of_bytes = 0;
	can.tpdo3.config.total_number_of_bytes = 0;
	can.tpdo4.config.total_number_of_bytes = 0;


		// Re-map variables for the payloads
	for (int i = 0; i < sizeof(can.rpdo1.config.mapped_indices);i++){
		if (can.rpdo1.config.mapped_indices[i] != 0) update_pdo_payload(&can.rpdo1,i);
		if (can.rpdo2.config.mapped_indices[i] != 0) update_pdo_payload(&can.rpdo2,i);
		if (can.rpdo3.config.mapped_indices[i] != 0) update_pdo_payload(&can.rpdo3,i);
		if (can.rpdo4.config.mapped_indices[i] != 0) update_pdo_payload(&can.rpdo4,i);
		if (can.tpdo1.config.mapped_indices[i] != 0) update_pdo_payload(&can.tpdo1,i);
		if (can.tpdo2.config.mapped_indices[i] != 0) update_pdo_payload(&can.tpdo2,i);
		if (can.tpdo3.config.mapped_indices[i] != 0) update_pdo_payload(&can.tpdo3,i);
		if (can.tpdo4.config.mapped_indices[i] != 0) update_pdo_payload(&can.tpdo4,i);
	}

	ICM42670P.accel_x_offset = eeprom_storage.accel_x_offset;
	ICM42670P.accel_y_offset = eeprom_storage.accel_y_offset;
	ICM42670P.accel_z_offset = eeprom_storage.accel_z_offset;

	ICM42670P.gyro_x_offset = eeprom_storage.gyro_x_offset;
	ICM42670P.gyro_y_offset = eeprom_storage.gyro_y_offset;
	ICM42670P.gyro_z_offset = eeprom_storage.gyro_z_offset;

	ICM42670P.temperature_offset = eeprom_storage.temperature_offset;
	return 1;
}

void reboot_device(void) {
	// Make sure to perform any save before reset

    NVIC_SystemReset(); // Triggers a system reset
}

uint32_t diff_ms_timer(uint32_t last_period){
	uint32_t diff = ms_timer-last_period;
	return diff;
}

uint32_t get_ms_timer(void){
	return ms_timer;
}

void set_outputs(void){
	if (hardware.wanted_output.bit0)HAL_GPIO_WritePin(OUT0_GPIO_Port, OUT0_Pin, GPIO_PIN_SET);
	else HAL_GPIO_WritePin(OUT0_GPIO_Port, OUT0_Pin, GPIO_PIN_RESET);

	if (hardware.wanted_output.bit1)HAL_GPIO_WritePin(OUT1_GPIO_Port, OUT1_Pin, GPIO_PIN_SET);
	else HAL_GPIO_WritePin(OUT1_GPIO_Port, OUT1_Pin, GPIO_PIN_RESET);

	if (hardware.wanted_output.bit2)HAL_GPIO_WritePin(OUT2_GPIO_Port, OUT2_Pin, GPIO_PIN_SET);
	else HAL_GPIO_WritePin(OUT2_GPIO_Port, OUT2_Pin, GPIO_PIN_RESET);

	if (hardware.wanted_output.bit3)HAL_GPIO_WritePin(OUT3_GPIO_Port, OUT3_Pin, GPIO_PIN_SET);
	else HAL_GPIO_WritePin(OUT3_GPIO_Port, OUT3_Pin, GPIO_PIN_RESET);

	if (hardware.wanted_output.bit4)HAL_GPIO_WritePin(OUT4_GPIO_Port, OUT4_Pin, GPIO_PIN_SET);
	else HAL_GPIO_WritePin(OUT4_GPIO_Port, OUT4_Pin, GPIO_PIN_RESET);

	if (hardware.wanted_output.bit5)HAL_GPIO_WritePin(OUT5_GPIO_Port, OUT5_Pin, GPIO_PIN_SET);
	else HAL_GPIO_WritePin(OUT5_GPIO_Port, OUT5_Pin, GPIO_PIN_RESET);

	if (hardware.wanted_output.bit6)HAL_GPIO_WritePin(OUT6_GPIO_Port, OUT6_Pin, GPIO_PIN_SET);
	else HAL_GPIO_WritePin(OUT6_GPIO_Port, OUT6_Pin, GPIO_PIN_RESET);

	if (hardware.wanted_output.bit7)HAL_GPIO_WritePin(OUT7_GPIO_Port, OUT7_Pin, GPIO_PIN_SET);
	else HAL_GPIO_WritePin(OUT7_GPIO_Port, OUT7_Pin, GPIO_PIN_RESET);

	hardware.output_value = hardware.target_output_value;
}

void read_inputs(void){
	uint8_t tmp_val = 0;
	// Read inputs
    tmp_val = (HAL_GPIO_ReadPin(IN0_GPIO_Port, IN0_Pin) != GPIO_PIN_SET);
    if (hardware.input.bit0 != tmp_val){
    	pdo_input_check_wrapper(0,tmp_val);
    	// finally update the variable
    	hardware.input.bit0 = tmp_val;
    }
    tmp_val = (HAL_GPIO_ReadPin(IN1_GPIO_Port, IN1_Pin) != GPIO_PIN_SET);
    if (hardware.input.bit1 != tmp_val){
    	// First check if a PDO is triggered from this input
    	pdo_input_check_wrapper(1,tmp_val);
    	// finally update the variable
    	hardware.input.bit1 = tmp_val;
    }
    tmp_val = (HAL_GPIO_ReadPin(IN2_GPIO_Port, IN2_Pin) != GPIO_PIN_SET);
    if (hardware.input.bit2 != tmp_val){
    	// First check if a PDO is triggered from this input
    	pdo_input_check_wrapper(2,tmp_val);
    	// finally update the variable
    	hardware.input.bit2 = tmp_val;
    }
    tmp_val = (HAL_GPIO_ReadPin(IN3_GPIO_Port, IN3_Pin) != GPIO_PIN_SET);
    if (hardware.input.bit3 != tmp_val){
    	// First check if a PDO is triggered from this input
    	pdo_input_check_wrapper(3,tmp_val);
    	// finally update the variable
    	hardware.input.bit3 = tmp_val;
    }
    tmp_val = (HAL_GPIO_ReadPin(IN4_GPIO_Port, IN4_Pin) != GPIO_PIN_SET);
    if (hardware.input.bit4 != tmp_val){
    	// First check if a PDO is triggered from this input
    	pdo_input_check_wrapper(4,tmp_val);
    	// finally update the variable
    	hardware.input.bit4 = tmp_val;
    }
    tmp_val = (HAL_GPIO_ReadPin(IN5_GPIO_Port, IN5_Pin) != GPIO_PIN_SET);
    if (hardware.input.bit5 != tmp_val){
    	// First check if a PDO is triggered from this input
    	pdo_input_check_wrapper(5,tmp_val);
    	// finally update the variable
    	hardware.input.bit5 = tmp_val;
    }
    tmp_val = (HAL_GPIO_ReadPin(IN6_GPIO_Port, IN6_Pin) != GPIO_PIN_SET);
    if (hardware.input.bit6 != tmp_val){
    	// First check if a PDO is triggered from this input
    	pdo_input_check_wrapper(6,tmp_val);
    	// finally update the variable
    	hardware.input.bit6 = tmp_val;
    }
    tmp_val = (HAL_GPIO_ReadPin(IN7_GPIO_Port, IN7_Pin) != GPIO_PIN_SET);
    if (hardware.input.bit7 != tmp_val){
    	// First check if a PDO is triggered from this input
    	pdo_input_check_wrapper(7,tmp_val);
    	// finally update the variable
    	hardware.input.bit7 = tmp_val;
    }
}

// This is used to override the weak HAL interrupt function
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == IMU_INT_Pin){
		// Handler IMU interrupt
		if (HAL_GPIO_ReadPin(IMU_INT_GPIO_Port, IMU_INT_Pin) == GPIO_PIN_SET){
			// call read DMA function
			ICM42670P_read_imu_DMA();
		}
	} else {
		read_inputs();
	}
}

// This is used to override the weak HAL interrupt function
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if (hspi->Instance == SPI1){
		ICM42670P_read_imu_DMA_complete();
		// Check CAN PDO thresholds, and update the last values
		check_all_tpdos_for_analog_trigger(&can.tpdo1,&can,&ICM42670P);
		check_all_tpdos_for_analog_trigger(&can.tpdo2,&can,&ICM42670P);
		check_all_tpdos_for_analog_trigger(&can.tpdo3,&can,&ICM42670P);
		check_all_tpdos_for_analog_trigger(&can.tpdo4,&can,&ICM42670P);

		check_all_rpdos_for_analog_trigger(&can.tpdo1,&can,&ICM42670P);
		check_all_rpdos_for_analog_trigger(&can.tpdo2,&can,&ICM42670P);
		check_all_rpdos_for_analog_trigger(&can.tpdo3,&can,&ICM42670P);
		check_all_rpdos_for_analog_trigger(&can.tpdo4,&can,&ICM42670P);

	}
}


// INTERRUPTS

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM2) {
        // Custom logic for 1 ms timer
    	ms_timer++;
    }
}
//void USB_LP_CAN1_RX0_IRQHandler(void)
//{
//    HAL_PCD_IRQHandler(&hpcd_USB_FS);
//}

//uint8_t CDC_Receive_FS(uint8_t* Buf, uint32_t *Len)
//{
//    // `Buf` contains the incoming data
//    // `Len` contains the length of the data
//    // Process the incoming data
//
//    // Example: echo data back to the host
//    CDC_Transmit_FS(Buf, *Len);
//
//    return USBD_OK;
//}
