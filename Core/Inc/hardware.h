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
#include "can_manager.h"
#include "ICM42670P.h"

#define TRUE 	1
#define FALSE	0
#define UNKNOWN	-1


typedef struct{

		// This holds the current input values
	union {
		struct{
			volatile uint16_t bit0		:1;
			volatile uint16_t bit1		:1;
			volatile uint16_t bit2		:1;
			volatile uint16_t bit3		:1;
			volatile uint16_t bit4		:1;
			volatile uint16_t bit5		:1;
			volatile uint16_t bit6		:1;
			volatile uint16_t bit7		:1;
		}input;
		volatile uint16_t input_value;
	};

	union {
		struct{
			volatile uint16_t bit0		:1;
			volatile uint16_t bit1		:1;
			volatile uint16_t bit2		:1;
			volatile uint16_t bit3		:1;
			volatile uint16_t bit4		:1;
			volatile uint16_t bit5		:1;
			volatile uint16_t bit6		:1;
			volatile uint16_t bit7		:1;
		}output;
		volatile uint16_t output_value;
	};

	uint32_t last_can_state_check_time;
	uint32_t last_can_msg_time;

}hardware_t;

// Declare the global instance of hardware_t
extern hardware_t hardware;

//Functions
void hardware_init(CAN_HandleTypeDef *can_handler,SPI_HandleTypeDef *hSPI);
void hardware_monitoring(void);

uint32_t diff_ms_timer(uint32_t last_period);
uint32_t get_ms_timer(void);
void set_outputs(void);
void read_inputs(void);
void reboot_device(void);
#endif /* INC_HARDWARE_H_ */
