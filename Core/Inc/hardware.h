/*
 * hardware.h
 *
 *  Created on: Nov 14, 2024
 *      Author: Jonathan
 */

#ifndef INC_HARDWARE_H_
#define INC_HARDWARE_H_
#include "stm32f1xx_hal.h"
#include "main.h"

// Variables
typedef struct{
	 // COB-ID is made from 11 bits ( 4 for the function code, 7 for the CAN_ID)
		// This is set the current CAN ID at start up
	union canId{
		struct{
			int8_t bit0		:1;
			int8_t bit1		:1;
			int8_t bit2		:1;
			int8_t bit3		:1;
			int8_t bit4		:1;
			int8_t bit5		:1;
			int8_t bit6		:1;
			int8_t bit7		:1;
		}id;
		int8_t id_value;
	};


	CAN_TxHeaderTypeDef tx_header;
	CAN_RxHeaderTypeDef rx_header;

	CAN_HandleTypeDef hcan;

	uint32_t tx_mail_box;
	uint8_t tx_data_buffer[8];
	uint8_t rx_data_buffer[8];
}can_t;

typedef struct{

		// This holds the current input values
	union Inputs{
		struct{
			int8_t bit0		:1;
			int8_t bit1		:1;
			int8_t bit2		:1;
			int8_t bit3		:1;
			int8_t bit4		:1;
			int8_t bit5		:1;
			int8_t bit6		:1;
			int8_t bit7		:1;
		}input;
		int8_t input_value;
	};
	can_t can;
}hardware_t;

// Declare the global instance of hardware_t
extern hardware_t hardware;
//Functions
void hardware_init(void);
void hardware_monitoring(void);
void setup_CAN_cfg(void);
void send_can_packet(int func_code,int8_t *data,int data_len);
#endif /* INC_HARDWARE_H_ */
