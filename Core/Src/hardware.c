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

// used to handle any hardware inits
void hardware_init(void){

}

// Used to poll hardware
void hardware_monitoring(void){
}

// Functions
void setup_CAN_cfg(CAN_HandleTypeDef *hcan){

	// Read inputs
    hardware.can.id_u.id.bit0 = (HAL_GPIO_ReadPin(CAN_CFG1_GPIO_Port, CAN_CFG1_Pin) == GPIO_PIN_SET);
    hardware.can.id_u.id.bit1 = (HAL_GPIO_ReadPin(CAN_CFG2_GPIO_Port, CAN_CFG2_Pin) == GPIO_PIN_SET);
    hardware.can.id_u.id.bit2 = (HAL_GPIO_ReadPin(CAN_CFG3_GPIO_Port, CAN_CFG3_Pin) == GPIO_PIN_SET);

    hardware.can.filter.FilterIdHigh = 0x00;
    hardware.can.filter.FilterIdLow = hardware.can.id_u.id_value;
    hardware.can.filter.FilterMaskIdHigh = 0x00;
    hardware.can.filter.FilterMaskIdLow = hardware.can.id_u.id_value;
    hardware.can.filter.FilterFIFOAssignment = CAN_RX_FIFO0;
    hardware.can.filter.FilterBank = 0;
    hardware.can.filter.FilterMode = CAN_FILTERMODE_IDMASK;
    hardware.can.filter.FilterScale = CAN_FILTERSCALE_16BIT;
    hardware.can.filter.FilterActivation = CAN_FILTER_ENABLE;
//    hardware.can.filter.SlaveStartFilterBank = ; //    For single CAN instances, this parameter is meaningless.
    HAL_CAN_ConfigFilter(hcan,&hardware.can.filter);

}

void send_can_packet(CAN_HandleTypeDef *hcan,int func_code,int8_t *data,int data_len){

	  hardware.can.tx_header.DLC = data_len;
	  hardware.can.tx_header.ExtId = 0;
	  hardware.can.tx_header.IDE = CAN_ID_STD;
	  hardware.can.tx_header.RTR = CAN_RTR_DATA;
	  hardware.can.tx_header.StdId = func_code << 7 | hardware.can.id_u.id_value;
	  hardware.can.tx_header.TransmitGlobalTime = DISABLE;
	  for (int i = 0; i < data_len; i++){
		  hardware.can.tx_data_buffer[i] = *(data + i);
	  }
	  HAL_CAN_AddTxMessage(hcan,&hardware.can.tx_header,
			  hardware.can.tx_data_buffer,&hardware.can.tx_mail_box);
}

void set_outputs(void){
	if (hardware.out_u.output.bit0)HAL_GPIO_WritePin(OUT0_GPIO_Port, OUT0_Pin, GPIO_PIN_SET);
	else HAL_GPIO_WritePin(OUT0_GPIO_Port, OUT0_Pin, GPIO_PIN_RESET);

	if (hardware.out_u.output.bit1)HAL_GPIO_WritePin(OUT1_GPIO_Port, OUT1_Pin, GPIO_PIN_SET);
	else HAL_GPIO_WritePin(OUT1_GPIO_Port, OUT1_Pin, GPIO_PIN_RESET);

	if (hardware.out_u.output.bit2)HAL_GPIO_WritePin(OUT2_GPIO_Port, OUT2_Pin, GPIO_PIN_SET);
	else HAL_GPIO_WritePin(OUT2_GPIO_Port, OUT2_Pin, GPIO_PIN_RESET);

	if (hardware.out_u.output.bit3)HAL_GPIO_WritePin(OUT3_GPIO_Port, OUT3_Pin, GPIO_PIN_SET);
	else HAL_GPIO_WritePin(OUT3_GPIO_Port, OUT3_Pin, GPIO_PIN_RESET);

	if (hardware.out_u.output.bit4)HAL_GPIO_WritePin(OUT4_GPIO_Port, OUT4_Pin, GPIO_PIN_SET);
	else HAL_GPIO_WritePin(OUT4_GPIO_Port, OUT4_Pin, GPIO_PIN_RESET);

	if (hardware.out_u.output.bit5)HAL_GPIO_WritePin(OUT5_GPIO_Port, OUT5_Pin, GPIO_PIN_SET);
	else HAL_GPIO_WritePin(OUT5_GPIO_Port, OUT5_Pin, GPIO_PIN_RESET);

	if (hardware.out_u.output.bit6)HAL_GPIO_WritePin(OUT6_GPIO_Port, OUT6_Pin, GPIO_PIN_SET);
	else HAL_GPIO_WritePin(OUT6_GPIO_Port, OUT6_Pin, GPIO_PIN_RESET);

	if (hardware.out_u.output.bit7)HAL_GPIO_WritePin(OUT7_GPIO_Port, OUT7_Pin, GPIO_PIN_SET);
	else HAL_GPIO_WritePin(OUT7_GPIO_Port, OUT7_Pin, GPIO_PIN_RESET);
}
void read_input(void){
	// Read inputs
    hardware.in_u.input.bit0 = (HAL_GPIO_ReadPin(IN0_GPIO_Port, IN0_Pin) != GPIO_PIN_SET);
    hardware.in_u.input.bit1 = (HAL_GPIO_ReadPin(IN1_GPIO_Port, IN1_Pin) != GPIO_PIN_SET);
    hardware.in_u.input.bit2 = (HAL_GPIO_ReadPin(IN2_GPIO_Port, IN2_Pin) != GPIO_PIN_SET);
    hardware.in_u.input.bit3 = (HAL_GPIO_ReadPin(IN3_GPIO_Port, IN3_Pin) != GPIO_PIN_SET);
    hardware.in_u.input.bit4 = (HAL_GPIO_ReadPin(IN4_GPIO_Port, IN4_Pin) != GPIO_PIN_SET);
    hardware.in_u.input.bit5 = (HAL_GPIO_ReadPin(IN5_GPIO_Port, IN5_Pin) != GPIO_PIN_SET);
    hardware.in_u.input.bit6 = (HAL_GPIO_ReadPin(IN6_GPIO_Port, IN6_Pin) != GPIO_PIN_SET);
    hardware.in_u.input.bit7 = (HAL_GPIO_ReadPin(IN7_GPIO_Port, IN7_Pin) != GPIO_PIN_SET);
}

void sdo_handler(CAN_HandleTypeDef *hcan, int8_t *packet,int len){
	int cmd = *packet;
		// Go from little-edian to big
	int16_t index = __REV16(*(int16_t*)(packet+1));
	int8_t sub_index = (packet+3);
	int data = *(int32_t*)(packet+(8-len));

	int8_t response[8];
	int response_len = 0;

	if (cmd & 0x20){
		// Use data length to get data out (set objects)
		switch (index){
			case OUTPUT_INDEX_OD:
				hardware.out_u.output_value = data;
				set_outputs();
				response[0] = CAN_SERVER_RESPONSE_SUCCESS;
				*(int16_t*)&response[1] = __REV16(index);
				response[3] = OUTPUT_SUBINDEX_OD;
				response[4] = hardware.out_u.output_value;
				response_len = 1;
				break;
			default:
				response[0] = CAN_SERVER_RESPONSE_ERROR;
				*(int16_t*)&response[1] = __REV16(index);
				response[3] = sub_index;
				response_len = 0;
				printf("[ERROR] CAN Write index not found %d",index);
				break;
		}
	} else if (cmd & RD_CLIENT_REQUEST_CMD_BYTE){
		// Use data length to get data out (set objects)
		switch (index){
			case OUTPUT_INDEX_OD:
				response[0] = RD_SERVER_RESPONSE_CMD_1_BYTE;
				*(int16_t*)&response[1] = __REV16(index);
				response[3] = OUTPUT_SUBINDEX_OD;
				response[4] = hardware.out_u.output_value;
				response_len = 1;
				break;
			case INPUT_INDEX_OD:
				response[0] = RD_SERVER_RESPONSE_CMD_1_BYTE;
				*(int16_t*)&response[1] = __REV16(index);
				response[3] = INPUT_SUBINDEX_OD;
				response[4] = hardware.in_u.input_value;
				response_len = 1;
				send_can_packet(hcan,TSDO,response,response_len);
				break;

			default:
				response[0] = CAN_SERVER_RESPONSE_ERROR;
				*(int16_t*)&response[1] = __REV16(index);
				response[3] = sub_index;
				response_len = 0;
				printf("[ERROR] CAN Read index not found %d",index);
				break;
		}

	} else {
		response[0] = CAN_SERVER_RESPONSE_ERROR;
		*(int16_t*)&response[1] = 0x0000;
		response[3] = 0x00;
		response_len = 0;
		// No a known command
		printf("[ERROR] CAN CMD not found %d",cmd);
	}

	send_can_packet(hcan,TSDO,response,response_len);
}


//	Manage CAN messages here, build packet handler
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan){

    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &hardware.can.rx_header, hardware.can.rx_data_buffer) == HAL_OK) {
        // Process the received message
    		// Check COB-ID
    	int cob_id = (hardware.can.rx_header.StdId & 0x78) >> 3;
//    	hardware.can.rx_header.DLC
    	switch (cob_id){
			case RPDO1:

				break;

			case RPDO2:

				break;

			case RPDO3:

				break;

			case RPDO4:

				break;

    		case RSDO:

    			break;
    	}
    }
}

// This is used to override the weak HAL interrupt function
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	read_inputs();
}
