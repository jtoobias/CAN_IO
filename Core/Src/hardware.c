/*
 * hardware.c
 *
 *  Created on: Nov 14, 2024
 *      Author: Jonathan
 */
#include "hardware.h"
// Variables
// Define the global instance of hardware_t
hardware_t hardware;

// Functions

void hardware_init(void){
	setup_CAN_cfg();
}

void hardware_monitoring(void){
	// Read inputs
    hardware.input.bit0 = (HAL_GPIO_ReadPin(OUT0_GPIO_Port, OUT0_Pin) == GPIO_PIN_SET);
    hardware.input.bit1 = (HAL_GPIO_ReadPin(OUT1_GPIO_Port, OUT1_Pin) == GPIO_PIN_SET);
    hardware.input.bit2 = (HAL_GPIO_ReadPin(OUT2_GPIO_Port, OUT2_Pin) == GPIO_PIN_SET);
    hardware.input.bit3 = (HAL_GPIO_ReadPin(OUT3_GPIO_Port, OUT3_Pin) == GPIO_PIN_SET);
    hardware.input.bit4 = (HAL_GPIO_ReadPin(OUT4_GPIO_Port, OUT4_Pin) == GPIO_PIN_SET);
    hardware.input.bit5 = (HAL_GPIO_ReadPin(OUT5_GPIO_Port, OUT5_Pin) == GPIO_PIN_SET);
    hardware.input.bit6 = (HAL_GPIO_ReadPin(OUT6_GPIO_Port, OUT6_Pin) == GPIO_PIN_SET);
    hardware.input.bit7 = (HAL_GPIO_ReadPin(OUT7_GPIO_Port, OUT7_Pin) == GPIO_PIN_SET);
}

// Functions
void setup_CAN_cfg(void){

	// Read inputs
    hardware.can.id.bit0 = (HAL_GPIO_ReadPin(CAN_CFG1_GPIO_Port, CAN_CFG1_Pin) == GPIO_PIN_SET);
    hardware.can.id.bit1 = (HAL_GPIO_ReadPin(CAN_CFG2_GPIO_Port, CAN_CFG2_Pin) == GPIO_PIN_SET);
    hardware.can.id.bit2 = (HAL_GPIO_ReadPin(CAN_CFG3_GPIO_Port, CAN_CFG3_Pin) == GPIO_PIN_SET);


    HAL_CAN_Start(&hardware.can.hcan);
    HAL_CAN_ActivateNotification(&hardware.can.hcan,CAN_IT_RX_FIFO0_MSG_PENDING);
}


void send_can_packet(int func_code,int8_t *data,int data_len){

	  hardware.can.tx_header.DLC = data_len;
	  hardware.can.tx_header.ExtId = 0;
	  hardware.can.tx_header.IDE = CAN_ID_STD;
	  hardware.can.tx_header.RTR = CAN_RTR_DATA;
	  hardware.can.tx_header.StdId = func_code << 7 | hardware.can.id_value;
	  hardware.can.tx_header.TransmitGlobalTime = DISABLE;
	  for (int i = 0; i < data_len; i++){
		  hardware.can.tx_data_buffer[i] = *(data + i);
	  }
	  HAL_CAN_AddTxMessage(&hardware.can.hcan,&hardware.can.tx_header,
			  hardware.can.tx_data_buffer,&hardware.can.tx_mail_box);
}


void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan){
//	Manage CAN messages here, build packet handler
	HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &hardware.can.rx_header, hardware.can.rx_data_buffer);
}
