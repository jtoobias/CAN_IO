/*
 * hardware.c
 *
 *  Created on: Nov 14, 2024
 *      Author: Jonathan
 */
#include "hardware.h"
#include "main.h"
//#include <string.h>
// Variables
// Define the global instance of hardware_t
hardware_t hardware;
volatile uint32_t ms_timer = 0;

// Functions

// used to handle any hardware inits
void hardware_init(CAN_HandleTypeDef *can_handler,SPI_HandleTypeDef *hSPI){
	can_init(can_handler);
	ICM42670P_init(hSPI);
}

// Used to poll hardware
void hardware_monitoring(void){
	read_inputs();
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
	if (hardware.output.bit0)HAL_GPIO_WritePin(OUT0_GPIO_Port, OUT0_Pin, GPIO_PIN_SET);
	else HAL_GPIO_WritePin(OUT0_GPIO_Port, OUT0_Pin, GPIO_PIN_RESET);

	if (hardware.output.bit1)HAL_GPIO_WritePin(OUT1_GPIO_Port, OUT1_Pin, GPIO_PIN_SET);
	else HAL_GPIO_WritePin(OUT1_GPIO_Port, OUT1_Pin, GPIO_PIN_RESET);

	if (hardware.output.bit2)HAL_GPIO_WritePin(OUT2_GPIO_Port, OUT2_Pin, GPIO_PIN_SET);
	else HAL_GPIO_WritePin(OUT2_GPIO_Port, OUT2_Pin, GPIO_PIN_RESET);

	if (hardware.output.bit3)HAL_GPIO_WritePin(OUT3_GPIO_Port, OUT3_Pin, GPIO_PIN_SET);
	else HAL_GPIO_WritePin(OUT3_GPIO_Port, OUT3_Pin, GPIO_PIN_RESET);

	if (hardware.output.bit4)HAL_GPIO_WritePin(OUT4_GPIO_Port, OUT4_Pin, GPIO_PIN_SET);
	else HAL_GPIO_WritePin(OUT4_GPIO_Port, OUT4_Pin, GPIO_PIN_RESET);

	if (hardware.output.bit5)HAL_GPIO_WritePin(OUT5_GPIO_Port, OUT5_Pin, GPIO_PIN_SET);
	else HAL_GPIO_WritePin(OUT5_GPIO_Port, OUT5_Pin, GPIO_PIN_RESET);

	if (hardware.output.bit6)HAL_GPIO_WritePin(OUT6_GPIO_Port, OUT6_Pin, GPIO_PIN_SET);
	else HAL_GPIO_WritePin(OUT6_GPIO_Port, OUT6_Pin, GPIO_PIN_RESET);

	if (hardware.output.bit7)HAL_GPIO_WritePin(OUT7_GPIO_Port, OUT7_Pin, GPIO_PIN_SET);
	else HAL_GPIO_WritePin(OUT7_GPIO_Port, OUT7_Pin, GPIO_PIN_RESET);
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
