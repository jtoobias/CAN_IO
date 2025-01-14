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

#define TRUE 	1
#define FALSE	0
#define UNKNOWN	-1
//0x1000	0x00	Device Type	The type of the device	uint32_t	Read-only
//0x1001	0x00	Device Name	The name of the device	string	Read-only
//0x1017	0x00	Heartbeat	Heartbeat object for device status	uint8_t	Read/Write
//0x1200	0x00	SDO Communication	Service Data Object for configuration	SDO	Read/Write
//0x2000	0x00	Inputs state	uint8_t	Read-only
//0x2001	0x00	Outputs state (can write to this)	uint8_t	Write-only
//0x1018	0x00	Serial Number	Serial number of the STM32 device	string	Read-only
// COBID
#define NMT_COB_ID 	0x00
#define SYNC_COB_ID	0x80
//OD
#define DEVICE_TYPE_INDEX_OD	0x1000
#define DEVICE_TYPE_SUBINDEX_OD	0x00

#define ERROR_REG_INDEX_OD			0x1001
#define DERROR_REG_SUBINDEX_OD		0x00
#define HEARTBEAT_INDEX_OD			0x1017
#define HEARTBEAT_SUBINDEX_OD		0x00
#define ID_OBJECT_OD				0x1018
#define ID_OBJECT_NUM_ENTRIES_OD	0x00
#define ID_OBJECT_VENDOR_ID_OD		0x01
#define ID_OBJECT_PRODUCT_CODE_OD	0x02
#define ID_OBJECT_REVISION_NUM_OD	0x03

#define INPUT_INDEX_OD			0x6000
#define INPUT_SUBINDEX_OD		0x00
#define OUTPUT_INDEX_OD			0x6200
#define OUTPUT_SUBINDEX_OD		0x00

#define TEMP_INDEX_OD			0x6002
#define TEMP_SUBINDEX_OD		0x00
#define ACCEL_INDEX_OD						0x6003
#define ACCEL_NUM_ENTRIES_SUBINDEX_OD		0x00
#define ACCEL_X_SUBINDEX_OD					0x01
#define ACCEL_Y_SUBINDEX_OD					0x02
#define ACCEL_Z_SUBINDEX_OD					0x03
#define GYRO_INDEX_OD						0x6004
#define GYRO_NUM_ENTRIES_SUBINDEX_OD		0x00
#define GYRO_X_SUBINDEX_OD					0x01
#define GYRO_Y_SUBINDEX_OD					0x02
#define GYRO_Z_SUBINDEX_OD					0x03


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
//heartbeat
#define HEARTBEAT		0x700

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

#define CAN_SERVER_RESPONSE_SUCCESS		0x60
#define CAN_SERVER_RESPONSE_ERROR		0x80

#define CAN_BUAD_RATE_PRESCALER_125		32
#define CAN_BUAD_RATE_PRESCALER_250		16
#define CAN_BUAD_RATE_PRESCALER_500		8
#define CAN_BUAD_RATE_PRESCALER_1000	4

// CAN STATE
#define CAN_STATE_OFF					0
#define CAN_STATE_INIT					1
#define CAN_STATE_PREOPERATIONAL		2
#define CAN_STATE_OPERATIONAL			3
#define CAN_STATE_STOPPED				4

// NMT COMMANDS
//Command Name			Command Byte (Hex)	Target State
//Start Remote Node		0x01				Operational
//Stop Remote Node		0x02				Stopped
//Enter Pre-operational	0x80				Pre-operational
//Reset Node			0x81				Reset Node (reboot)
//Reset Communication	0x82				Reset communication only
#define NMT_START					0x01
#define NMT_STOP					0x02
#define NMT_PREOPERATIONAL			0x80
#define NMT_RESET_NODE				0x81
#define NMT_RESET_COMMS				0x82
//CMD determines the direction of data transfer and the size of the data object:
//• 0x23 Sending of 4-byte data (bytes 5 - 8 contains a 32-bit value).
//• 0x2B Sending of 2-byte data (bytes 5, 6 contains a 16-bit value.
//• 0x2F Sending of 1-byte data (byte 5 contains an 8-bit value).
//The Slave answers:
//• 60 hex Data sent successfully.
//• 80 hex Error.

//#define PDO_MODE_OFF		0x00
//#define PDO_MODE_SYNC		0x01
//#define PDO_MODE_EVENT	0x02
//#define PDO_MODE_REQUEST	0x03

#define PDO_TRIGGER_OFF				0x00
#define PDO_TRIGGER_LOW_TO_HIGH		0x01
#define PDO_TRIGGER_HIGH_TO_LOW		0x02
#define PDO_TRIGGER_BOTH			0x03

#define PDO_NUMBER_OF_ANALOG_TRIGGERS		6		// 6 analog
#define PDO_NUMBER_OF_DIGITAL_TRIGGERS		8		// 6 analog
#define PDO_NUMBER_OF_TRIGGERS		PDO_NUMBER_OF_ANALOG_TRIGGERS+PDO_NUMBER_OF_DIGITAL_TRIGGERS		// 8 digital i/o , 6 analog

#define PDO_NUMBER_OF_BYTES_0		0x00
#define PDO_NUMBER_OF_BYTES_1		0x01
#define PDO_NUMBER_OF_BYTES_2		0x02
#define PDO_NUMBER_OF_BYTES_4		0x03



// PDO mapping can be done by receiving indices which will be used to map the variables to the correct pdo
typedef struct {
	uint16_t _func_code;					// Don't edit this
	uint32_t inhibit_time;					// time in ms: This block sends of the pdo until the time has elapsed
	uint32_t last_send_time;				// time in ms: The last time the pdo was sent

	void* payload[8];						// This points the the variables that are sent or received in the pdo
	uint8_t number_of_bytes[8]; 			// This holds the number of bytes for each of the payload variables
											// (0=undefined,1=int8,2=int16,3=int32)
	uint32_t mapped_indices[8]; 			// This holds the OD indices of the mapped variables: Used to know the contents of the payload

	union{
		struct{
			uint8_t sync_mode_val;
			union{
				struct{
					uint8_t bit0		:1;
					uint8_t bit1		:1;
					uint8_t bit2		:1;
					uint8_t bit3		:1;
					uint8_t bit4		:1;
					uint8_t bit5		:1;
					uint8_t bit6		:1;
					uint8_t bit7		:1;
				}event_mode;
				uint8_t event_mode_val;
			};
			uint8_t cyclic_mode;					// This is meant to be part of event mode, but having its own flag makes it easier
			uint8_t request_mode_val;
		};
		uint8_t mode[4];							// PDO mode (sync[0], event[1],cyclic[2], Request[3])
	};



	uint32_t transmit_period;					// If event mode and a transmit period is set: (0 is OFF)

	uint8_t triggers[PDO_NUMBER_OF_TRIGGERS];	// Represents a specific input (8 Digital I/O, 6 Analog = 3 Gyro,3 Accel)
												// 0=Off, 1=low->high, 2=high->low, 3=both, (This array is in order. DIO 1->8,xyzGyro,xyzAccel)

	int16_t analog_thresholds[PDO_NUMBER_OF_ANALOG_TRIGGERS];				// Thresholds for analog trigger

}pdo_t;


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
		}buad;
		uint8_t value;
	}buad_u;



	CAN_HandleTypeDef *handler;

	CAN_TxHeaderTypeDef tx_header;
	CAN_RxHeaderTypeDef rx_header;
	CAN_FilterTypeDef	filter;

	uint32_t tx_mail_box;
	uint8_t tx_data_buffer[8];
	uint8_t rx_data_buffer[8];

	uint8_t state;
	uint8_t target_state;

	pdo_t tpdo1;						// This holds the information for the Transmit pdo
	pdo_t tpdo2;						// This holds the information for the Transmit pdo
	pdo_t tpdo3;						// This holds the information for the Transmit pdo
	pdo_t tpdo4;						// This holds the information for the Transmit pdo

	pdo_t rpdo1;						// This holds the information for the Receive pdo
	pdo_t rpdo2;						// This holds the information for the Receive pdo
	pdo_t rpdo3;						// This holds the information for the Receive pdo
	pdo_t rpdo4;						// This holds the information for the Receive pdo


}can_t;

typedef struct{

		// This holds the current input values
	union {
		struct{
			uint16_t bit0		:1;
			uint16_t bit1		:1;
			uint16_t bit2		:1;
			uint16_t bit3		:1;
			uint16_t bit4		:1;
			uint16_t bit5		:1;
			uint16_t bit6		:1;
			uint16_t bit7		:1;
		}input;
		uint16_t input_value;
	};

	union {
		struct{
			uint16_t bit0		:1;
			uint16_t bit1		:1;
			uint16_t bit2		:1;
			uint16_t bit3		:1;
			uint16_t bit4		:1;
			uint16_t bit5		:1;
			uint16_t bit6		:1;
			uint16_t bit7		:1;
		}output;
		uint16_t output_value;
	};
	can_t can;

	uint32_t last_can_state_check_time;
	uint32_t last_can_msg_time;

}hardware_t;

// Declare the global instance of hardware_t
extern hardware_t hardware;

//Functions
void hardware_init(CAN_HandleTypeDef *can_handler);
void init_pdo(uint32_t func_code,pdo_t *pdo);
void hardware_monitoring(void);
void setup_CAN_cfg(void);
void send_can_packet(uint32_t func_code,uint8_t *data,uint32_t data_len);
void send_pdo(pdo_t *pdo);
void send_cyclic_pdo(pdo_t* pdo);
int8_t pdo_sync_output_check_and_update(pdo_t* rpdo);
int8_t pdo_input_check_wrapper(uint8_t input_index, uint8_t new_val);
int8_t pdo_input_check_and_send(pdo_t* tpdo,uint8_t input_index,uint8_t new_val);

uint32_t diff_ms_timer(uint32_t last_period);
uint32_t get_ms_timer(void);
void set_outputs(void);
void read_inputs(void);

void sdo_handler(uint8_t *packet,uint32_t len);
void nmt_handler(uint8_t *packet,uint32_t len);
void sync_handler(void);
void reboot_device(void);
void can_state_manager(void);
#endif /* INC_HARDWARE_H_ */
