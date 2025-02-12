/*
 * can_manager.h
 *
 *  Created on: Jan 15, 2025
 *      Author: jonathan
 */

#ifndef INC_CAN_MANAGER_H_
#define INC_CAN_MANAGER_H_
#include "stm32f1xx_hal.h"
#include "ICM42670P.h"

#define CAN_TX_BUFFER_SIZE 12 // Adjust as needed

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
#define SAVE_CONFIG_INDEX_OD	0x1010

//Bit 0 (Generic Error) 				→ Any unknown failure occurs.
//Bit 1 (Current Error) 				→ Overcurrent detected in an I/O channel.
//Bit 2 (Voltage Error) 				→ Power supply too high/low.
//Bit 3 (Temperature Error) 			→ Device overheats.
//Bit 4 (Communication Error) 			→ CAN bus off or bus error detected.
//Bit 5 (Profile-Specific Error) 		→ Invalid sensor reading or actuator failure.
//6	Reserved	Not used (reserved for future use).
//Bit 7 (Manufacturer-Specific Error) 	→ Custom issues like hardware faults.

#define ERROR_GENERIC				0x01
#define ERROR_CURRENT				0x02
#define ERROR_VOLTAGE				0x04
#define ERROR_TEMPERATURE			0x08
#define ERROR_COMMUNICATION			0x10
#define ERROR_SENSOR				0x20
#define ERROR_CUSTOM				0x80


#define ERROR_REG_INDEX_OD			0x1001
#define DERROR_REG_SUBINDEX_OD		0x00
#define HEARTBEAT_INDEX_OD			0x1017
#define HEARTBEAT_SUBINDEX_OD		0x00
#define ID_OBJECT_OD				0x1018
#define ID_OBJECT_NUM_ENTRIES_OD	0x00
#define ID_OBJECT_VENDOR_ID_OD		0x01
#define ID_OBJECT_PRODUCT_CODE_OD	0x02
#define ID_OBJECT_REVISION_NUM_OD	0x03

// RPDO Communication Parameters
#define RPDO1_COMM_PARAM_OD      0x1400
#define RPDO2_COMM_PARAM_OD      0x1401
#define RPDO3_COMM_PARAM_OD      0x1402
#define RPDO4_COMM_PARAM_OD      0x1403

// TPDO Communication Parameters
#define TPDO1_COMM_PARAM_OD      0x1800
#define TPDO2_COMM_PARAM_OD      0x1801
#define TPDO3_COMM_PARAM_OD      0x1802
#define TPDO4_COMM_PARAM_OD      0x1803

#define PDO_COMM_PARAM_COB_ID_SUBINDEX_OD   		0x01
#define PDO_COMM_PARAM_TRANS_TYPE_SUBINDEX_OD 		0x02
#define PDO_COMM_PARAM_INHIBIT_TIME_SUBINDEX_OD 	0x03
#define PDO_COMM_PARAM_RESERVED_SUBINDEX_OD 		0x04
#define PDO_COMM_PARAM_EVENT_TIMER_SUBINDEX_OD	 	0x05

// RPDO Mapping Parameters
#define RPDO1_MAPPING_PARAM_OD   0x1600
#define RPDO2_MAPPING_PARAM_OD   0x1601
#define RPDO3_MAPPING_PARAM_OD   0x1602
#define RPDO4_MAPPING_PARAM_OD   0x1603

// TPDO Mapping Parameters
#define TPDO1_MAPPING_PARAM_OD   0x1A00
#define TPDO2_MAPPING_PARAM_OD   0x1A01
#define TPDO3_MAPPING_PARAM_OD   0x1A02
#define TPDO4_MAPPING_PARAM_OD   0x1A03

#define PDO_MAPPING_SUBINDEX_NUM_ENTRIES_OD 0x00 // Subindex for number of entries
#define PDO_MAPPING_SUBINDEX_ENTRY_1_OD     0x01 // Subindex for first entry
#define PDO_MAPPING_SUBINDEX_ENTRY_2_OD     0x02 // Subindex for second entry
#define PDO_MAPPING_SUBINDEX_ENTRY_3_OD     0x03 // Subindex for third entry
#define PDO_MAPPING_SUBINDEX_ENTRY_4_OD     0x04 // Subindex for fourth entry
#define PDO_MAPPING_SUBINDEX_ENTRY_5_OD     0x05 // Subindex for fifth entry
#define PDO_MAPPING_SUBINDEX_ENTRY_6_OD     0x06 // Subindex for sixth entry
#define PDO_MAPPING_SUBINDEX_ENTRY_7_OD     0x07 // Subindex for seventh entry
#define PDO_MAPPING_SUBINDEX_ENTRY_8_OD     0x08 // Subindex for eighth entry


// TPDO1 Analog Thresholds and Trigger Directions
#define TPDO1_ANALOG_THRESHOLDS_OD    	0x2000
#define TPDO1_TRIGGER_DIRS_OD  			0x2001
#define RPDO1_ANALOG_THRESHOLDS_OD    	0x2002
#define RPDO1_TRIGGER_DIRS_OD  			0x2003

// TPDO2 Analog Thresholds and Trigger Directions
#define TPDO2_ANALOG_THRESHOLDS_OD    	0x2010
#define TPDO2_TRIGGER_DIRS_OD  			0x2011
#define RPDO2_ANALOG_THRESHOLDS_OD    	0x2012
#define RPDO2_TRIGGER_DIRS_OD  			0x2013

// TPDO3 Analog Thresholds and Trigger Directions
#define TPDO3_ANALOG_THRESHOLDS_OD   	0x2020
#define TPDO3_TRIGGER_DIRS_OD  			0x2021
#define RPDO3_ANALOG_THRESHOLDS_OD    	0x2022
#define RPDO3_TRIGGER_DIRS_OD  			0x2023

// TPDO4 Analog Thresholds and Trigger Directions
#define TPDO4_ANALOG_THRESHOLDS_OD    	0x2030
#define TPDO4_TRIGGER_DIRS_OD  			0x2031
#define RPDO4_ANALOG_THRESHOLDS_OD    	0x2032
#define RPDO4_TRIGGER_DIRS_OD  			0x2033

// Trigger sub_indices
#define TRIGGER_IN_1_SUBINDEX_OD		0x01
#define TRIGGER_IN_2_SUBINDEX_OD		0x02
#define TRIGGER_IN_3_SUBINDEX_OD		0x03
#define TRIGGER_IN_4_SUBINDEX_OD		0x04
#define TRIGGER_IN_5_SUBINDEX_OD		0x05
#define TRIGGER_IN_6_SUBINDEX_OD		0x06
#define TRIGGER_IN_7_SUBINDEX_OD		0x07
#define TRIGGER_IN_8_SUBINDEX_OD		0x08
#define TRIGGER_GYRO_X_SUBINDEX_OD		0x09
#define TRIGGER_GYRO_Y_SUBINDEX_OD		0x0A
#define TRIGGER_GYRO_Z_SUBINDEX_OD		0x0B
#define TRIGGER_ACCEL_X_SUBINDEX_OD		0x0C
#define TRIGGER_ACCEL_Y_SUBINDEX_OD		0x0D
#define TRIGGER_ACCEL_Z_SUBINDEX_OD		0x0E

//#define ANALOG_NUMBER_SUBINDEX_OD     0x00
//#define ANALOG_INPUT_1_SUBINDEX_OD    0x01
//#define ANALOG_INPUT_2_SUBINDEX_OD    0x02
//#define ANALOG_INPUT_3_SUBINDEX_OD    0x03
//#define ANALOG_INPUT_4_SUBINDEX_OD    0x04
//#define ANALOG_INPUT_5_SUBINDEX_OD    0x05
//#define ANALOG_INPUT_6_SUBINDEX_OD    0x06


#define INPUT_INDEX_OD			0x6000
#define INPUT_ALL_SUBINDEX_OD	0x00
#define INPUT_1_SUBINDEX_OD		0x01
#define INPUT_2_SUBINDEX_OD		0x02
#define INPUT_3_SUBINDEX_OD		0x03
#define INPUT_4_SUBINDEX_OD		0x04
#define INPUT_5_SUBINDEX_OD		0x05
#define INPUT_6_SUBINDEX_OD		0x06
#define INPUT_7_SUBINDEX_OD		0x07
#define INPUT_8_SUBINDEX_OD		0x08
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

// Accelerometer Offset (Three Axes)
#define ACCEL_OFFSET_OD        0x3000
#define ACCEL_OFFSET_X_SUB     0x01
#define ACCEL_OFFSET_Y_SUB     0x02
#define ACCEL_OFFSET_Z_SUB     0x03

// Gyroscope Offset (Three Axes)
#define GYRO_OFFSET_OD         0x3001
#define GYRO_OFFSET_X_SUB      0x01
#define GYRO_OFFSET_Y_SUB      0x02
#define GYRO_OFFSET_Z_SUB      0x03

// Temperature Offset
#define TEMP_OFFSET_OD         0x3002
#define TEMP_OFFSET_SUB        0x00

// Device State
#define DEVICE_STATE_OD        0x7000


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


#define PDO_DISABLED_COB_ID 0x80000000

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

#define PDO_MODE_SYNC_INDEX			0x00
#define PDO_MODE_EVENT_INDEX		0x01
#define PDO_MODE_CYCLIC_INDEX		0x02
#define PDO_MODE_RTR_INDEX			0x03

/*
	Transmission Type	Mode	When is PDO Sent?
	0	Acyclic Synchronous	First SYNC after an event
	1-240	Cyclic Synchronous	Every X SYNCs (fixed rate)
	255	Asynchronous (Event-Driven)	Immediately when event occurs
 */
#define TRANSMISSION_TYPE_MODE_ACYCLIC	0
#define TRANSMISSION_TYPE_MODE_SYNC_LOW	1
#define TRANSMISSION_TYPE_MODE_SYNC_UP	240
#define TRANSMISSION_TYPE_MODE_EVENT	255


#define PDO_TRIGGER_LOW_TO_HIGH		0x01
#define PDO_TRIGGER_HIGH_TO_LOW		0x02
#define PDO_TRIGGER_BOTH			0x03

#define PDO_NUMBER_OF_ANALOG_TRIGGERS		6		// 6 analog
#define PDO_NUMBER_OF_DIGITAL_TRIGGERS		8		// 6 analog
#define PDO_NUMBER_OF_TRIGGERS		PDO_NUMBER_OF_ANALOG_TRIGGERS+PDO_NUMBER_OF_DIGITAL_TRIGGERS		// 8 digital i/o , 6 analog

#define TRIGGER_INDEX_DIN_1			0
#define TRIGGER_INDEX_DIN_2			1
#define TRIGGER_INDEX_DIN_3			2
#define TRIGGER_INDEX_DIN_4			3
#define TRIGGER_INDEX_DIN_5			4
#define TRIGGER_INDEX_DIN_6			5
#define TRIGGER_INDEX_DIN_7			6
#define TRIGGER_INDEX_DIN_8			7
#define TRIGGER_INDEX_GYRO_X		8
#define TRIGGER_INDEX_GYRO_Y		9
#define TRIGGER_INDEX_GYRO_Z		10
#define TRIGGER_INDEX_ACCEL_X		11
#define TRIGGER_INDEX_ACCEL_Y		12
#define TRIGGER_INDEX_ACCEL_Z		13

#define ANALOG_THRESHOLD_INDEX_GYRO_X		0
#define ANALOG_THRESHOLD_INDEX_GYRO_Y		1
#define ANALOG_THRESHOLD_INDEX_GYRO_Z		2
#define ANALOG_THRESHOLD_INDEX_ACCEL_X		3
#define ANALOG_THRESHOLD_INDEX_ACCEL_Y		4
#define ANALOG_THRESHOLD_INDEX_ACCEL_Z		5

#define PDO_NUMBER_OF_BYTES_0		0x00
#define PDO_NUMBER_OF_BYTES_1		0x01
#define PDO_NUMBER_OF_BYTES_2		0x02
#define PDO_NUMBER_OF_BYTES_4		0x03


typedef struct {
    CAN_TxHeaderTypeDef header;
    uint8_t data[8]; // CAN data payload
} CAN_Packet;

typedef struct {
    CAN_Packet buffer[CAN_TX_BUFFER_SIZE];
    volatile uint16_t head;
    volatile uint16_t tail;
} CircularBuffer;


// 56 Bytes
typedef struct{
	uint32_t _cob_id;						// Don't edit this
	uint32_t inhibit_time;					// time in ms: This block sends of the pdo until the time has elapsed
	uint32_t last_send_time;				// time in ms: The last time the pdo was sent
	uint8_t number_of_bytes[8]; 			// This holds the number of bytes for each of the payload variables
											// (0=undefined,1=int8,2=int16,3 or 4=int32)
	uint8_t total_number_of_bytes;			// Used to know if this pdo is valid or is going to overflow
	uint32_t mapped_indices[8]; 			// This holds the OD indices of the mapped variables: Used to know the contents of the payload

	union{
		struct{
			uint8_t transmission_type;		// Stores the amount of syncs before a send is required and if its in a
			uint8_t event_mode_val;			// True or False for if there are mapped indices.
			uint8_t cyclic_mode;			// This is meant to be part of event mode, but having its own flag makes it easier
			uint8_t request_mode_val;
		};
		uint8_t mode[4];							// PDO mode (sync[0], event[1],cyclic[2], Request[3])
	};

	uint32_t transmit_period;					// If event mode and a transmit period is set: (0 is OFF)

	uint8_t triggers[PDO_NUMBER_OF_TRIGGERS];	// Represents a specific input (8 Digital I/O, 6 Analog = 3 Gyro,3 Accel)
												// 0=Off, 1=low->high, 2=high->low, 3=both, (This array is in order. DIO 1->8,xyzGyro,xyzAccel)

	int32_t analog_thresholds[PDO_NUMBER_OF_ANALOG_TRIGGERS];				// Thresholds for analog trigger


    // Function pointer for a function taking (void) and returning void
    void (*call_back)(void);
    uint8_t cb_function_id;

}pdo_config_t;

// PDO mapping can be done by receiving indices which will be used to map the variables to the correct pdo
typedef struct {


	void* payload[8];						// This points the the variables that are sent or received in the pdo
	int8_t cos_occured;
	pdo_config_t config;
	uint8_t sync_count;
	uint16_t target_action_value;			// This is used to set the wanted action from the rpdo, e.g. outputs

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

//	CAN_TxHeaderTypeDef tx_header;
	CAN_RxHeaderTypeDef rx_header;
	CAN_FilterTypeDef	filter;

	uint32_t tx_mail_box;
//	uint8_t tx_data_buffer[8];
	CircularBuffer tx_buffer;
	uint8_t rx_data_buffer[8];

	uint8_t state;
	uint8_t target_state;

	volatile uint8_t error;
	volatile uint8_t send_fail_count;

	pdo_t tpdo1;						// This holds the information for the Transmit pdo
	pdo_t tpdo2;						// This holds the information for the Transmit pdo
	pdo_t tpdo3;						// This holds the information for the Transmit pdo
	pdo_t tpdo4;						// This holds the information for the Transmit pdo

	pdo_t rpdo1;						// This holds the information for the Receive pdo
	pdo_t rpdo2;						// This holds the information for the Receive pdo
	pdo_t rpdo3;						// This holds the information for the Receive pdo
	pdo_t rpdo4;						// This holds the information for the Receive pdo

	// Used to see if the threshold is crossed for PDO sends on COS (make sure to set on init to the first readings)
	int16_t last_gyro_x;
	int16_t last_gyro_y;
	int16_t last_gyro_z;

	int16_t last_accel_x;
	int16_t last_accel_y;
	int16_t last_accel_z;

	uint8_t save_settings;

}can_t;
// Declare the global instance of hardware_t
extern can_t can;

void can_init(CAN_HandleTypeDef *can_handler);
void setup_CAN_cfg(void);
void insert_sorted(uint8_t *array, uint8_t *current_size, uint8_t arr_size, uint8_t value);
void init_pdo(uint32_t func_code,pdo_t *pdo,pdo_config_t *ee_pdo);
void clear_pdo_config(pdo_t *pdo);
void send_pdo(pdo_t *pdo);
void send_cyclic_pdo(pdo_t* pdo);
//int8_t pdo_sync_output_check_and_update(pdo_t* rpdo);
int8_t pdo_input_check_wrapper(uint8_t input_index, uint8_t new_val);
int8_t tpdo_input_check_and_send(pdo_t* tpdo,uint8_t input_index,uint8_t new_val);
int8_t rpdo_input_check_and_send(pdo_t* rpdo,uint8_t input_index,uint8_t new_val);
int8_t check_analog_low_to_high_trigger(pdo_config_t* tpdo_config,uint8_t trigger_index, uint8_t analog_threshold_index,int16_t new_val,int16_t last_val);
int8_t check_analog_high_to_low_trigger(pdo_config_t* tpdo_config,uint8_t trigger_index, uint8_t analog_threshold_index,int16_t new_val,int16_t last_val);
int8_t check_rpdo_triggered_by_analog(pdo_t* rpdo,uint8_t trigger_index, uint8_t analog_threshold_index,int16_t new_val,int16_t last_val);
int8_t check_tpdo_triggered_by_analog(pdo_t* tpdo,uint8_t trigger_index, uint8_t analog_threshold_index,int16_t new_val,int16_t last_val);
void check_all_tpdos_for_analog_trigger(pdo_t* pdo,can_t* can_ptr,ICM42670P_t *imu_ptr);
void check_all_rpdos_for_analog_trigger(pdo_t* rpdo,can_t* can_ptr,ICM42670P_t *imu_ptr);
void rpdo_handler(pdo_t *rpdo_config,uint8_t *packet);
void update_pdo_payload(pdo_t *pdo, uint8_t arr_index);
void sdo_handler(uint8_t *packet,uint32_t len);
void nmt_handler(uint8_t *packet,uint32_t len);
void sync_handler(void);
void can_state_manager(void);

void tpdo_sync_checks(pdo_t *rpdo);
void rpdo_sync_checks(pdo_t *rpdo);

int is_buffer_full();
int is_buffer_empty();
int enqueue_packet(CAN_Packet *packet);
int add_packet_to_front_of_queue(CAN_Packet *packet);
int dequeue_packet(CAN_Packet *packet);
void CAN_TxHandler(CAN_HandleTypeDef *hcan);
int8_t send_can_packet(uint32_t cob_id,uint8_t *data,uint32_t data_len);

#endif /* INC_CAN_MANAGER_H_ */
