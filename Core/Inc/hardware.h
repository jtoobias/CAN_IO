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
#include "ee.h"


#define TRUE 	1
#define FALSE	0
#define UNKNOWN	-1

#define EEPROM_ADDR_TPDO1	0
#define EEPROM_ADDR_TPDO2	1
#define EEPROM_ADDR_TPDO3	2
#define EEPROM_ADDR_TPDO4	3
#define EEPROM_ADDR_RPDO1	4
#define EEPROM_ADDR_RPDO2	5
#define EEPROM_ADDR_RPDO3	6
#define EEPROM_ADDR_RPDO4	7

#define CEIL_DIV(x, y) (((x) + (y) - 1) / (y))


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

typedef struct{
	pdo_config_t tpdo1;						// This holds the information for the Transmit pdo
	pdo_config_t tpdo2;						// This holds the information for the Transmit pdo
	pdo_config_t tpdo3;						// This holds the information for the Transmit pdo
	pdo_config_t tpdo4;						// This holds the information for the Transmit pdo

	pdo_config_t rpdo1;						// This holds the information for the Receive pdo
	pdo_config_t rpdo2;						// This holds the information for the Receive pdo
	pdo_config_t rpdo3;						// This holds the information for the Receive pdo
	pdo_config_t rpdo4;						// This holds the information for the Receive pdo

}eeprom_storage_t;

/*
 With extern:
	Using extern declares the variable without defining it.
	This tells the compiler that the variable exists,
	but its definition (memory allocation and initialization) is elsewhere,
	usually in a different source file. This is useful for sharing variables across multiple files.
*/
extern hardware_t hardware;
extern eeprom_storage_t eeprom_storage;

//Functions
void hardware_init(CAN_HandleTypeDef *can_handler,SPI_HandleTypeDef *hSPI);
void hardware_monitoring(void);
void update_can_ee_pdo_config(pdo_config_t *ee_pdo,pdo_config_t *pdo,int pdo_size);

uint32_t diff_ms_timer(uint32_t last_period);
uint32_t get_ms_timer(void);
void set_outputs(void);
void read_inputs(void);
void reboot_device(void);
#endif /* INC_HARDWARE_H_ */
