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
//0x1000	0x00	Device Type	The type of the device	uint32_t	Read-only
//0x1001	0x00	Device Name	The name of the device	string	Read-only
//0x1017	0x00	Heartbeat	Heartbeat object for device status	uint8_t	Read/Write
//0x1200	0x00	SDO Communication	Service Data Object for configuration	SDO	Read/Write
//0x2000	0x00	Inputs state	uint8_t	Read-only
//0x2001	0x00	Outputs state (can write to this)	uint8_t	Write-only
//0x1018	0x00	Serial Number	Serial number of the STM32 device	string	Read-only

//OD
#define DEVICE_TYPE_INDEX_OD	0x1000
#define DEVICE_TYPE_SUBINDEX_OD	0x00
#define DEVICE_NAME_INDEX_OD	0x1001
#define DEVICE_NAME_SUBINDEX_OD	0x00
#define HEARTBEAT_INDEX_OD		0x1017
#define HEARTBEAT_SUBINDEX_OD	0x00

#define INPUT_INDEX_OD			0x2000
#define INPUT_SUBINDEX_OD		0x00
#define OUTPUT_INDEX_OD			0x2001
#define OUTPUT_SUBINDEX_OD		0x00


//Transmit PDO1 (TPDO1)	0x4	0x180 + Node ID
//Transmit PDO2 (TPDO2)	0x5	0x280 + Node ID
//Transmit PDO3 (TPDO3)	0x6	0x380 + Node ID
//Transmit PDO4 (TPDO4)	0x7	0x480 + Node ID
//Receive PDO1 (RPDO1)	0x8	0x200 + Node ID
//Receive PDO2 (RPDO2)	0x9	0x300 + Node ID
//Receive PDO3 (RPDO3)	0xA	0x400 + Node ID
//Receive PDO4 (RPDO4)	0xB	0x500 + Node ID
//5. Service Data Object (SDO) - Function Codes 0x10 to 0x1F
//SDO Type	Function Code (Hex)	COB-ID Calculation
//Request (Client->Server)	0x10	0x600 + Node ID
//Response (Server->Client)	0x11	0x580 + Node ID

// Defines
// PDO's
		// Transmitted by the CAN IO board
#define TPDO1			0x180
#define TPDO2			0x280
#define TPDO3			0x380
#define TPDO4			0x480
		// Received by the CAN IO board
#define RPDO1			0x200
#define RPDO2			0x300
#define RPDO3			0x400
#define RPDO4			0x500
// SDO's
#define TSDO			0x580		// Transmitted by the CAN IO board
#define RSDO			0x600		// Received by the CAN IO board

// SDO write commands (respond with success)
#define WR_CLIENT_REQUEST_CMD_1_BYTE	0x2F
#define WR_CLIENT_REQUEST_CMD_2_BYTE	0x2B
#define WR_CLIENT_REQUEST_CMD_3_BYTE	0x27
#define WR_CLIENT_REQUEST_CMD_4_BYTE	0x23

// SDO read commands
#define RD_CLIENT_REQUEST_CMD_BYTE		0x40
#define RD_SERVER_RESPONSE_CMD_1_BYTE	0x4F
#define RD_SERVER_RESPONSE_CMD_2_BYTE	0x4B
#define RD_SERVER_RESPONSE_CMD_3_BYTE	0x47
#define RD_SERVER_RESPONSE_CMD_4_BYTE	0x43

#define CAN_SERVER_RESPONSE_SUCCESS			0x60
#define CAN_SERVER_RESPONSE_ERROR			0x80

//CMD determines the direction of data transfer and the size of the data object:
//• 0x23 Sending of 4-byte data (bytes 5 - 8 contains a 32-bit value).
//• 0x2B Sending of 2-byte data (bytes 5, 6 contains a 16-bit value.
//• 0x2F Sending of 1-byte data (byte 5 contains an 8-bit value).
//The Slave answers:
//• 60 hex Data sent successfully.
//• 80 hex Error.

// Variables
typedef struct{
	 // COB-ID is made from 11 bits ( 4 for the function code, 7 for the CAN_ID)
		// This is set the current CAN ID at start up
	union {
		struct{
			uint8_t bit0		:1;
			uint8_t bit1		:1;
			uint8_t bit2		:1;
			uint8_t bit3		:1;
			uint8_t bit4		:1;
			uint8_t bit5		:1;
			uint8_t bit6		:1;
			uint8_t bit7		:1;
		}id;
		uint8_t id_value;
	}id_u;


	CAN_TxHeaderTypeDef tx_header;
	CAN_RxHeaderTypeDef rx_header;
	CAN_FilterTypeDef	filter;

	uint32_t tx_mail_box;
	uint8_t tx_data_buffer[8];
	uint8_t rx_data_buffer[8];
}can_t;

typedef struct{

		// This holds the current input values
	union {
		struct{
			uint8_t bit0		:1;
			uint8_t bit1		:1;
			uint8_t bit2		:1;
			uint8_t bit3		:1;
			uint8_t bit4		:1;
			uint8_t bit5		:1;
			uint8_t bit6		:1;
			uint8_t bit7		:1;
		}input;
		uint8_t input_value;
	}in_u;

	union {
		struct{
			uint8_t bit0		:1;
			uint8_t bit1		:1;
			uint8_t bit2		:1;
			uint8_t bit3		:1;
			uint8_t bit4		:1;
			uint8_t bit5		:1;
			uint8_t bit6		:1;
			uint8_t bit7		:1;
		}output;
		uint8_t output_value;
	}out_u;
	can_t can;
}hardware_t;

// Declare the global instance of hardware_t
extern hardware_t hardware;
//Functions
void hardware_init(void);
void hardware_monitoring(void);
void setup_CAN_cfg(CAN_HandleTypeDef *hcan);
void send_can_packet(CAN_HandleTypeDef *hcan,int func_code,int8_t *data,int data_len);
void set_outputs(void);
void read_input(void);
#endif /* INC_HARDWARE_H_ */
