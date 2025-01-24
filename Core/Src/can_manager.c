/*
 * can_manager.c
 *
 *  Created on: Jan 15, 2025
 *      Author: Jonathan
 */
#include "can_manager.h"
#include <string.h>
#include "main.h"
#include "hardware.h"


can_t can;

// Functions
// used to handle any hardware inits
void can_init(CAN_HandleTypeDef *can_handler){
	//
	can.handler = can_handler;
	can.state = CAN_STATE_INIT;
	can.target_state = CAN_STATE_PREOPERATIONAL;
}

  // Call before starting CAN
void setup_CAN_cfg(void){

	if (HAL_CAN_DeInit(can.handler) != HAL_OK) {
        Error_Handler(); // Replace with appropriate error handling
    }
	// Read inputs
    can.id_u.id.bit0 = (HAL_GPIO_ReadPin(CAN_CFG1_GPIO_Port, CAN_CFG1_Pin) == GPIO_PIN_SET);
    can.id_u.id.bit1 = (HAL_GPIO_ReadPin(CAN_CFG2_GPIO_Port, CAN_CFG2_Pin) == GPIO_PIN_SET);
    can.id_u.id.bit2 = (HAL_GPIO_ReadPin(CAN_CFG3_GPIO_Port, CAN_CFG3_Pin) == GPIO_PIN_SET);
    can.id_u.id.bit3 = (HAL_GPIO_ReadPin(CAN_CFG4_GPIO_Port, CAN_CFG4_Pin) == GPIO_PIN_SET);

    can.buad_u.buad.bit0 = (HAL_GPIO_ReadPin(CAN_Baud1_GPIO_Port, CAN_Baud1_Pin) == GPIO_PIN_SET);
    can.buad_u.buad.bit1 = (HAL_GPIO_ReadPin(CAN_Baud2_GPIO_Port, CAN_Baud2_Pin) == GPIO_PIN_SET);
    can.buad_u.buad.bit2 = (HAL_GPIO_ReadPin(CAN_Baud3_GPIO_Port, CAN_Baud3_Pin) == GPIO_PIN_SET);

	// 0 is eeprom value, 1 = 125 kbps, 2 = 250 kbps, 3=500 kbps, 4 = 1 kbps, higher defaults to 500 kbps
	if (can.buad_u.value == 0);
	else if (can.buad_u.value == 1) can.handler->Init.Prescaler = CAN_BUAD_RATE_PRESCALER_125;
	else if (can.buad_u.value == 2) can.handler->Init.Prescaler = CAN_BUAD_RATE_PRESCALER_250;
	else if (can.buad_u.value == 3) can.handler->Init.Prescaler = CAN_BUAD_RATE_PRESCALER_500;
	else if (can.buad_u.value == 4) can.handler->Init.Prescaler = CAN_BUAD_RATE_PRESCALER_1000;
	else can.handler->Init.Prescaler = CAN_BUAD_RATE_PRESCALER_500;


	//init
	can.handler->Instance = CAN1;
	can.handler->Init.Mode = CAN_MODE_NORMAL;
	can.handler->Init.SyncJumpWidth = CAN_SJW_1TQ;
	can.handler->Init.TimeSeg1 = CAN_BS1_5TQ;
	can.handler->Init.TimeSeg2 = CAN_BS2_3TQ;
	can.handler->Init.TimeTriggeredMode = DISABLE;
	can.handler->Init.AutoBusOff = DISABLE;
	can.handler->Init.AutoWakeUp = DISABLE;
	can.handler->Init.AutoRetransmission = DISABLE;
	can.handler->Init.ReceiveFifoLocked = DISABLE;
	can.handler->Init.TransmitFifoPriority = DISABLE;
	  if (HAL_CAN_Init(can.handler) != HAL_OK)
	  {
	    Error_Handler();
	  }

//  can.filter.FilterIdHigh = (0x000 << 5);  // Base ID for global messages like NMT/SYNC
//  can.filter.FilterIdLow = (0x580 << 5);  // Base ID for SDO messages
//  can.filter.FilterMaskIdHigh = (0x780 << 5);  // Accept NMT, SYNC, SDOs, and IDs in Node group
//  can.filter.FilterMaskIdLow = (0x780 << 5);  // Same mask for low filter
    can.filter.FilterIdHigh = 0x0000;//(can.id_u.id_value << 5);
    can.filter.FilterIdLow = 0x0000;//(0x580 << 5);  // Base ID for SDO messages
    can.filter.FilterMaskIdHigh = 0x0000;//(0x780 << 5);  // Accept NMT, SYNC, SDOs, and IDs in Node group
    can.filter.FilterMaskIdLow = 0x0000;//(0x780 << 5);  // Same mask for low filter
    can.filter.FilterFIFOAssignment = CAN_RX_FIFO1;
    can.filter.FilterBank = 0;
    can.filter.FilterMode = CAN_FILTERMODE_IDMASK;
    can.filter.FilterScale = CAN_FILTERSCALE_16BIT;
    can.filter.FilterActivation = CAN_FILTER_ENABLE;
//    can.filter.SlaveStartFilterBank = ; //    For single CAN instances, this parameter is meaningless.
    // Apply filter configuration and handle errors
	if (HAL_CAN_ConfigFilter(can.handler, &can.filter) != HAL_OK) {
	  Error_Handler(); // Replace with appropriate error handling
	}
	EE_Read();
	init_pdo(TPDO1,&can.tpdo1,&eeprom_storage.tpdo1);
//	init_pdo(&can.tpdo2);
//	init_pdo(&can.tpdo3);
//	init_pdo(&can.tpdo4);
//
//	init_pdo(&can.rpdo1);
//	init_pdo(&can.rpdo2);
//	init_pdo(&can.rpdo3);
//	init_pdo(&can.rpdo4);

		// Clear the transmit buffer
	can.tx_buffer.head = 0;
	can.tx_buffer.tail = 0;
}

// Place holder. This will likely be filled in my eeprom variables
void init_pdo(uint32_t func_code,pdo_t *pdo,pdo_t *ee_pdo){

	pdo->config._cob_id = func_code | can.id_u.id_value;
	memset(pdo->config.number_of_bytes, 0, sizeof(pdo->config.number_of_bytes)); // Set all elements to 0
	pdo->config.inhibit_time = 1500;
	pdo->config.last_send_time = get_ms_timer();
	pdo->config.transmit_period = 1000;
	memset(pdo->config.mode, DISABLE, sizeof(pdo->config.mode));

	pdo->config.cyclic_mode = DISABLE;
	pdo->config.sync_mode_val = ENABLE;

	memset(pdo->config.triggers, DISABLE, sizeof(pdo->config.triggers)); // Set all elements to 0
	memset(pdo->config.analog_thresholds, 0, sizeof(pdo->config.analog_thresholds)); // Set all elements to 0


	pdo->payload[0] = (int16_t*)&ICM42670P.accel_x_conv;
	pdo->payload[1] = (int16_t*)&ICM42670P.accel_y_conv;
	pdo->payload[2] = (int16_t*)&ICM42670P.accel_z_conv;
	pdo->config.number_of_bytes[0] = PDO_NUMBER_OF_BYTES_2;
	pdo->config.number_of_bytes[1] = PDO_NUMBER_OF_BYTES_2;
	pdo->config.number_of_bytes[2] = PDO_NUMBER_OF_BYTES_2;
}


int8_t pdo_input_check_wrapper(uint8_t input_index, uint8_t new_val){
	int8_t debug_status = FALSE;
	// First check if a PDO is triggered from this input
	debug_status = pdo_input_check_and_send(&can.tpdo1,input_index,new_val);
	if (debug_status == UNKNOWN) return UNKNOWN;
	debug_status = pdo_input_check_and_send(&can.tpdo2,input_index,new_val);
	if (debug_status == UNKNOWN) return UNKNOWN;
	debug_status = pdo_input_check_and_send(&can.tpdo3,input_index,new_val);
	if (debug_status == UNKNOWN) return UNKNOWN;
	debug_status = pdo_input_check_and_send(&can.tpdo4,input_index,new_val);
	if (debug_status == UNKNOWN) return UNKNOWN;
	return TRUE;
}

// Check if the input triggers a PDO send and for what Input transition
int8_t pdo_input_check_and_send(pdo_t* tpdo,uint8_t input_index,uint8_t new_val){

			// Check if there are mapped registers enabled
	if (tpdo->config.event_mode_val == DISABLE){
		return FALSE;
	} else {
		if (tpdo->config.triggers[input_index] == PDO_TRIGGER_HIGH_TO_LOW){
				// High -> Low
			if ((hardware.input_value & (1<<input_index)) > (new_val)){
				// Send PDO
				send_pdo(tpdo);
				return TRUE;
			}
		} else if (tpdo->config.triggers[input_index] == PDO_TRIGGER_LOW_TO_HIGH){
				// Low -> High
			if ((hardware.input_value & (1<<input_index)) < (new_val)){
				// Send PDO
				send_pdo(tpdo);
				return TRUE;
			}
		} else if (tpdo->config.triggers[input_index] == PDO_TRIGGER_BOTH){
			// Send PDO
			send_pdo(tpdo);
			return TRUE;
		}
	}
	return FALSE;
}



void can_state_manager(void){
// NODE CONNECTIONS
// init -> preoperational
// preoperational -> operational
// Preoerational -> Stopped
// Preoperational -> Init (two ways (reset, and comms reset)
// Operational -> stopped
// Operational -> preoperational
// Operational -> Init (two ways (reset, and comms reset)

	// Check current states and see requierd action to get to the next state
	switch (can.state){
		case CAN_STATE_INIT:

			can.state = CAN_STATE_PREOPERATIONAL;
			can.target_state = CAN_STATE_PREOPERATIONAL;
			HAL_CAN_Stop(can.handler);
			setup_CAN_cfg();
			HAL_CAN_Start(can.handler);
			HAL_CAN_ActivateNotification(can.handler,CAN_IT_RX_FIFO1_MSG_PENDING);

			uint8_t tmp_arr = 0x00;
			send_can_packet(HEARTBEAT,&tmp_arr,1);
			break;
		case CAN_STATE_PREOPERATIONAL:
			if (can.target_state == CAN_STATE_OPERATIONAL) can.state = CAN_STATE_OPERATIONAL;
		case CAN_STATE_OPERATIONAL:
			if (can.target_state == CAN_STATE_PREOPERATIONAL &&
				can.state == CAN_STATE_OPERATIONAL) {
				can.state = CAN_STATE_PREOPERATIONAL;

			} else if (can.target_state == CAN_STATE_INIT) {
				can.state = CAN_STATE_INIT;
			} else if (can.target_state == CAN_STATE_STOPPED) {
				can.state = CAN_STATE_STOPPED;
			}
			break;
		case CAN_STATE_STOPPED:
			if (can.target_state == CAN_STATE_PREOPERATIONAL) {
				can.state = CAN_STATE_PREOPERATIONAL;
			} else if (can.target_state == CAN_STATE_OPERATIONAL) {
				can.state = CAN_STATE_OPERATIONAL;
			} else if (can.target_state == CAN_STATE_INIT) {
				can.state = CAN_STATE_INIT;
			}
			break;
		default:
			//problem
			break;
	}
}


//void update outputs from CAN PDO sync
int8_t pdo_sync_output_check_and_update(pdo_t* rpdo){
	// For output based RPDOs first set the outputs before updating the variables.
	// This makes sure all devices are sync'd on the following sync call
	//check if sync mode is active
	if (rpdo->config.sync_mode_val == FALSE){
		return FALSE;
	} else {
		// check mapping for output control
		for (int i = 0; i<sizeof(rpdo->config.mapped_indices); i++){
			if (rpdo->config.mapped_indices[i] == (OUTPUT_INDEX_OD<<8|OUTPUT_SUBINDEX_OD)){
				// Output is controlled by this pdo
				set_outputs();
				switch (rpdo->config.number_of_bytes[i]){
					case PDO_NUMBER_OF_BYTES_1:
						// update output variables from the payload
						hardware.output_value = (*(uint8_t*)rpdo->payload[i] & 0x00FF);
						break;
					case PDO_NUMBER_OF_BYTES_2:
						// update output variables from the payload
						hardware.output_value = (*(uint16_t*)rpdo->payload[i] & 0xFFFF);
						break;
					default:
						// Problems wrong type
						return UNKNOWN;
						break;

				}
				return TRUE;
			}
		}
	}
	return FALSE;
}

// Function to insert a value into a sorted array
void insert_sorted(uint8_t *array, uint8_t *current_size, uint8_t arr_size, uint8_t value) {
    // If the array is full, truncate the last element
    if (*current_size >= arr_size) {
        (*current_size)--; // Reduce size before shifting
    }

    // Find the correct position to insert the new value
    int i = *current_size - 1;
    while (i >= 0 && array[i] > value) {
        array[i + 1] = array[i]; // Shift element to the right
        i--;
    }

    // Insert the new value at the correct position
    array[i + 1] = value;

    // Increment the current size of the array
    (*current_size)++;
}



/*************************
 * CAN Send packet management
 *************************/
// Check if the buffer is full
int is_buffer_full() {
    return ((can.tx_buffer.head + 1) % CAN_TX_BUFFER_SIZE) == can.tx_buffer.tail;
}

// Check if the buffer is empty
int is_buffer_empty() {
    return can.tx_buffer.head == can.tx_buffer.tail;
}

// Add a packet to the buffer
int enqueue_packet(CAN_Packet *packet) {
    if (is_buffer_full()) {
        return FALSE; // Buffer full, drop the packet
    }
    	// Add to queue
    can.tx_buffer.buffer[can.tx_buffer.head] = *packet;
    	// increment head index and manage wrap around
    can.tx_buffer.head = (can.tx_buffer.head + 1) % CAN_TX_BUFFER_SIZE;
    return TRUE;
}

// Add a packet to the buffer
int add_packet_to_front_of_queue(CAN_Packet *packet) {
    if (is_buffer_full()) {
        return FALSE; // Buffer full, drop the packet
    }

	// Increment and manager wrap around of buffer
    can.tx_buffer.tail = (can.tx_buffer.tail - 1 + CAN_TX_BUFFER_SIZE) % CAN_TX_BUFFER_SIZE;
    	// Add to
    can.tx_buffer.buffer[can.tx_buffer.tail] = *packet;
    return TRUE;
}

// Retrieve a packet from the buffer
int dequeue_packet(CAN_Packet *packet) {
    if (is_buffer_empty()) {
        return FALSE; // Buffer empty
    }
    *packet = can.tx_buffer.buffer[can.tx_buffer.tail];
    	// Increment and manager wrap around of buffer
    can.tx_buffer.tail = (can.tx_buffer.tail + 1) % CAN_TX_BUFFER_SIZE;
    return TRUE;
}

// Transmit packets from the buffer
void CAN_TxHandler(CAN_HandleTypeDef *hcan) {
    CAN_Packet packet;

    // Try to transmit while mailboxes are available and buffer is not empty
    while (!is_buffer_empty()) {
        uint32_t free_mailbox;
        // Mailbox free? and message being sent? and are there packets to send?
        if (HAL_CAN_GetTxMailboxesFreeLevel(hcan) > 0 &&
            dequeue_packet(&packet) == TRUE) {
            // Attempt to transmit the dequeued packet
            if (HAL_CAN_AddTxMessage(hcan, &packet.header, packet.data, &free_mailbox) != HAL_OK) {
                // Transmission failed, re-enqueue the packet
            	add_packet_to_front_of_queue(&packet);
                break;
            }
        } else {
        	// no mail box free/no packets
        	add_packet_to_front_of_queue(&packet);
            break;
        }
    }
}

// Handle Mailbox 0 Complete
void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan) {
    CAN_TxHandler(hcan);
}

// Handle Mailbox 1 Complete
void HAL_CAN_TxMailbox1CompleteCallback(CAN_HandleTypeDef *hcan) {
    CAN_TxHandler(hcan);
}

// Handle Mailbox 2 Complete
void HAL_CAN_TxMailbox2CompleteCallback(CAN_HandleTypeDef *hcan) {
    CAN_TxHandler(hcan);
}

// Function to queue a packet for transmission
int8_t send_can_packet(uint32_t cob_id,uint8_t *data,uint32_t data_len){

    CAN_Packet packet;
    packet.header.DLC = data_len;
	packet.header.ExtId = 0;
	packet.header.IDE = CAN_ID_STD;
	packet.header.RTR = CAN_RTR_DATA;
	packet.header.StdId = cob_id; // func_code | can.id_u.id_value
	packet.header.TransmitGlobalTime = DISABLE;

	// Copy over data
    memcpy(packet.data, data, data_len);

    // Enqueue the packet
    if (enqueue_packet(&packet) == FALSE) {
        return FALSE; // Buffer full
    }
    // Trigger transmission if a mailbox is available
    CAN_TxHandler(can.handler);
    return TRUE;
}

void send_pdo(pdo_t *pdo){
	uint8_t tx_buffer[8] = {0};
	uint8_t i = 0, payload_size = 0;
		// Don't send if the time since last send is less than the inhibit period
	if (diff_ms_timer(pdo->config.last_send_time) < pdo->config.inhibit_time)return;
	// loop through the bytes
	for (i = 0; i < sizeof(pdo->config.number_of_bytes);i++){
		// exit if variable is not setup
		if (pdo->config.number_of_bytes[i] == 0) break;
			// Add variable to the array
		switch (pdo->config.number_of_bytes[i]){
			case 1:
				tx_buffer[payload_size] = *(int8_t*)pdo->payload[i];
				payload_size ++;
				break;
			case 2:
				*(int16_t*)&tx_buffer[payload_size] = *(int16_t*)pdo->payload[i];
				payload_size += 2;
				break;
			case 3:
				*(int32_t*)&tx_buffer[payload_size] = *(int32_t*)pdo->payload[i];
				payload_size += 4;
				break;
			default:
				// incorrect type
				break;
		}
	}
	send_can_packet(pdo->config._cob_id,&tx_buffer[0],payload_size);
	pdo->config.last_send_time = get_ms_timer();
}

	// Given look at all pdo's, check if interval period is set and send if required
void send_cyclic_pdo(pdo_t* pdo){
		// Enabled?
	if (pdo->config.transmit_period > 0 && pdo->config.cyclic_mode == TRUE){
			// Check if ready to send
		if (diff_ms_timer(pdo->config.last_send_time) >= pdo->config.transmit_period){
			send_pdo(pdo);
		}
	}
}


/*****************************
 * PACKET HANDLERS
 *****************************/

//	Manage CAN messages here, build packet handler
void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *can_handler){
//    HAL_GPIO_TogglePin(OUT0_GPIO_Port, OUT0_Pin); // Example debug pin
    if (HAL_CAN_GetRxMessage(can_handler, CAN_RX_FIFO1, &can.rx_header, can.rx_data_buffer) == HAL_OK) {
        // Process the received message
    	int can_id = (can.rx_header.StdId & 0x07F);
		// Check COB-ID
    	int cob_id = (can.rx_header.StdId & 0x780);

    	if (cob_id == NMT_COB_ID) {
    		nmt_handler(can.rx_data_buffer,can.rx_header.DLC);
    		return;
    	} else if (can_id != can.id_u.id_value){
    		if (cob_id == SYNC_COB_ID && can.state == CAN_STATE_OPERATIONAL) sync_handler();
    		return;
    	}
    	if (can_id != can.id_u.id_value) return;
    	if (can.state != CAN_STATE_PREOPERATIONAL &&
    		can.state != CAN_STATE_OPERATIONAL) return;
//    	can.rx_header.DLC
    	switch (cob_id){
			case RPDO1:
					// PDO's only work in Operational state
		    	if (can.state != CAN_STATE_OPERATIONAL) break;

				break;

			case RPDO2:
		    	if (can.state != CAN_STATE_OPERATIONAL) break;

				break;

			case RPDO3:
		    	if (can.state != CAN_STATE_OPERATIONAL) break;

				break;

			case RPDO4:
		    	if (can.state != CAN_STATE_OPERATIONAL) break;

				break;

    		case RSDO:
    			sdo_handler(can.rx_data_buffer,can.rx_header.DLC);
    			break;

    		case HEARTBEAT:
    			// nothing
    			break;

    		default:
//    			printf("[ERROR] Unknown COB-ID %d",cob_id);
    			break;
    	}
    }
}

void nmt_handler(uint8_t *packet,uint32_t len){
	// Handle device state
		// make sure the nmt is for us
	if (*(packet+1) != can.id_u.id_value) return;
	uint8_t cmd = *packet;

	// Actual transitions are handled in another state machine to make sure the correct steps are executed
	switch (cmd){
		case NMT_START:
			can.target_state = CAN_STATE_OPERATIONAL;
			break;
		case NMT_STOP:
			can.target_state = CAN_STATE_STOPPED;
			break;
		case NMT_PREOPERATIONAL:
			can.target_state = CAN_STATE_PREOPERATIONAL;
			break;
		case NMT_RESET_NODE:
			reboot_device();
			break;
		case NMT_RESET_COMMS:
			// Call Can setup and update PDO's from eeprom
			can.target_state = CAN_STATE_INIT;
			break;
	}
}

// Likely need to handle multiple sends (likely require a global transmit buffer that feeds the CAN FIFO when there is space)
void sync_handler(void){
	// Place holder for now (if PDO is configured for Sync it will trigger sends and loads here)
	if (can.tpdo1.config.sync_mode_val == TRUE){
		send_pdo(&can.tpdo1);
	}

	if (can.tpdo2.config.sync_mode_val == TRUE){
		send_pdo(&can.tpdo2);
	}

	if (can.tpdo3.config.sync_mode_val == TRUE){
		send_pdo(&can.tpdo3);
	}

	if (can.tpdo4.config.sync_mode_val == TRUE){
		send_pdo(&can.tpdo4);
	}

	// Handle RPDOs:
	pdo_sync_output_check_and_update(&can.rpdo1);
	pdo_sync_output_check_and_update(&can.rpdo2);
	pdo_sync_output_check_and_update(&can.rpdo3);
	pdo_sync_output_check_and_update(&can.rpdo4);
}

void sdo_handler(uint8_t *packet,uint32_t len){
	int cmd = *packet;
		// Go from little-edian to big
	int16_t index = (*(int16_t*)(packet+1));
	int8_t sub_index = *(packet+3);
	int32_t data = 0;
	switch (len){
		case 1:
				//  Handle different data lengths
			data = *(int8_t*)(packet+4);
			break;
		case 2:
				//  Handle different data lengths
			data = *(int16_t*)(packet+4);
			break;
		case 4:
				//  Handle different data lengths
			data = *(int32_t*)(packet+4);
			break;
	}

	uint8_t response[8];
	uint32_t response_len = 0;
	pdo_config_t *pdo_config;

		// Check for a write operation
	if (cmd & 0x20){
		// Use data length to get data out (set objects)
		switch (index){

			case RPDO1_MAPPING_PARAM:
			case RPDO2_MAPPING_PARAM:
			case RPDO3_MAPPING_PARAM:
			case RPDO4_MAPPING_PARAM:
			case TPDO1_MAPPING_PARAM:
			case TPDO2_MAPPING_PARAM:
			case TPDO3_MAPPING_PARAM:
			case TPDO4_MAPPING_PARAM:
				response[0] = CAN_SERVER_RESPONSE_SUCCESS;
				*(int16_t*)&response[1] = index;
				if (index == RPDO1_MAPPING_PARAM) pdo_config = &can.rpdo1.config;
				else if (index == RPDO2_MAPPING_PARAM) pdo_config = &can.rpdo2.config;
				else if (index == RPDO3_MAPPING_PARAM) pdo_config = &can.rpdo3.config;
				else if (index == RPDO4_MAPPING_PARAM) pdo_config = &can.rpdo4.config;
				else if (index == TPDO1_MAPPING_PARAM) pdo_config = &can.tpdo1.config;
				else if (index == TPDO2_MAPPING_PARAM) pdo_config = &can.tpdo2.config;
				else if (index == TPDO3_MAPPING_PARAM) pdo_config = &can.tpdo3.config;
				else if (index == TPDO4_MAPPING_PARAM) pdo_config = &can.tpdo4.config;
				else break;	// Shouldn't happen

				response[3] = sub_index;
				response_len = 4;
				switch (sub_index){
					case PDO_MAPPING_PARAM_NUM_ENTRIES:
						// If this is greater than zero, COS is enabled, and is based on the mapped indices
						if (data > 0) {
							uint16_t index = 0;
							uint8_t sub_index = 0;
							uint8_t new_triggers[8] = {0};
							uint8_t trigger_index = -1, trigger_count = 0;
							uint8_t i = 0;
								// Go through mapped registers check which are on and off, disabled un-mapped and enable mapped through triggers
							for (i = 0; i < sizeof(pdo_config->mapped_indices);i++){
								// get index and sub index
								index = (uint16_t)((pdo_config->mapped_indices[i] >> 16) & 0xFFFF);
								sub_index = (uint8_t)((pdo_config->mapped_indices[i] >> 8) & 0x0000FF);
								if (index != 0){
									switch (index){
										case INPUT_INDEX_OD:
											// check the sub_index for specific input
											if (sub_index == INPUT_1_SUBINDEX_OD) trigger_index = TRIGGER_INDEX_DIN_1;
											else if (sub_index == INPUT_2_SUBINDEX_OD) trigger_index = TRIGGER_INDEX_DIN_2;
											else if (sub_index == INPUT_3_SUBINDEX_OD) trigger_index = TRIGGER_INDEX_DIN_3;
											else if (sub_index == INPUT_4_SUBINDEX_OD) trigger_index = TRIGGER_INDEX_DIN_4;
											else if (sub_index == INPUT_5_SUBINDEX_OD) trigger_index = TRIGGER_INDEX_DIN_5;
											else if (sub_index == INPUT_6_SUBINDEX_OD) trigger_index = TRIGGER_INDEX_DIN_6;
											else if (sub_index == INPUT_7_SUBINDEX_OD) trigger_index = TRIGGER_INDEX_DIN_7;
											else if (sub_index == INPUT_8_SUBINDEX_OD) trigger_index = TRIGGER_INDEX_DIN_8;
											break;
										case GYRO_INDEX_OD:
											// check the sub_index for specific input
											if (sub_index == GYRO_X_SUBINDEX_OD) trigger_index = TRIGGER_INDEX_GYRO_X;
											else if (sub_index == GYRO_Y_SUBINDEX_OD) trigger_index = TRIGGER_INDEX_GYRO_Y;
											else if (sub_index == GYRO_Z_SUBINDEX_OD) trigger_index = TRIGGER_INDEX_GYRO_Z;
											break;
										case ACCEL_INDEX_OD:
											// check the sub_index for specific input
											if (sub_index == ACCEL_X_SUBINDEX_OD) trigger_index = TRIGGER_INDEX_ACCEL_X;
											else if (sub_index == ACCEL_Y_SUBINDEX_OD) trigger_index = TRIGGER_INDEX_ACCEL_Y;
											else if (sub_index == ACCEL_Z_SUBINDEX_OD) trigger_index = TRIGGER_INDEX_ACCEL_Z;
											break;
									}
									// Save the trigger indices for the current mapped registers. This will be used to clear the rest
									if (trigger_index != -1) {
										// Keep a sorted array by ascending order
										insert_sorted(new_triggers, &trigger_count, sizeof(new_triggers), trigger_index);
									}
								}
							}

							// Loop through the trigger indices. Disable old
							for (i = 0;i<sizeof(pdo_config->triggers);i++){
								trigger_index = 0;
								if (i != new_triggers[trigger_index]){
									pdo_config->triggers[i] = DISABLE;
								}
							}
							// finally show it is on
							pdo_config->event_mode_val = ENABLE;
						}
						else {
							pdo_config->event_mode_val = DISABLE;
						}
						break;
					case PDO_MAPPING_PARAM_ENTRY_1:
					case PDO_MAPPING_PARAM_ENTRY_2:
					case PDO_MAPPING_PARAM_ENTRY_3:
					case PDO_MAPPING_PARAM_ENTRY_4:
					case PDO_MAPPING_PARAM_ENTRY_5:
					case PDO_MAPPING_PARAM_ENTRY_6:
					case PDO_MAPPING_PARAM_ENTRY_7:
					case PDO_MAPPING_PARAM_ENTRY_8:
						// Sub-index used to index the array
						pdo_config->mapped_indices[(sub_index-1)] = data;
						break;
					default:
						// shouldn't come here
						break;
				}
				break;
			case RPDO1_COMM_PARAM:
			case RPDO2_COMM_PARAM:
			case RPDO3_COMM_PARAM:
			case RPDO4_COMM_PARAM:
			case TPDO1_COMM_PARAM:
			case TPDO2_COMM_PARAM:
			case TPDO3_COMM_PARAM:
			case TPDO4_COMM_PARAM:
				response[0] = CAN_SERVER_RESPONSE_SUCCESS;
				*(int16_t*)&response[1] = index;
				if (index == RPDO1_COMM_PARAM) pdo_config = &can.rpdo1.config;
				else if (index == RPDO2_COMM_PARAM) pdo_config = &can.rpdo2.config;
				else if (index == RPDO3_COMM_PARAM) pdo_config = &can.rpdo3.config;
				else if (index == RPDO4_COMM_PARAM) pdo_config = &can.rpdo4.config;
				else if (index == TPDO1_COMM_PARAM) pdo_config = &can.rpdo2.config;
				else if (index == TPDO2_COMM_PARAM) pdo_config = &can.rpdo2.config;
				else if (index == TPDO3_COMM_PARAM) pdo_config = &can.rpdo3.config;
				else if (index == TPDO4_COMM_PARAM) pdo_config = &can.rpdo4.config;
				else break;	// Shouldn't happen

				response_len = 4;
				response[3] = sub_index;
				switch (sub_index){
					case PDO_COMM_PARAM_COB_ID_SUBINDEX:
						// Store the cob-id for the PDO to be sent with
						pdo_config->_cob_id = data;
						break;
					case PDO_COMM_PARAM_TRANS_TYPE_SUBINDEX:
						// Synchronous or asynchronouss (value from 0->255) 255 == Async and 0->254 is the amount of Syncs before send
						if (data == 0xFF) {
							pdo_config->sync_mode_val = DISABLE;
							pdo_config->cyclic_mode = ENABLE;
						} else if (data > 0){
							pdo_config->sync_mode_val = data;
						}
						// Need to check the data to see what modes are enabled
						break;
					case PDO_COMM_PARAM_INHIBIT_TIME_SUBINDEX:
						// Store the inhibit time
						pdo_config->inhibit_time = data;
						break;
//					case PDO_COMM_PARAM_RESERVED_SUBINDEX:
//						break;
					case PDO_COMM_PARAM_EVENT_TIMER_SUBINDEX:
						// Store the update period
						pdo_config->transmit_period = data;
						break;
					default:
						// shouldn't come here
						break;
				}
				break;
			case OUTPUT_INDEX_OD:
				hardware.output_value = data;
				set_outputs();
				response[0] = CAN_SERVER_RESPONSE_SUCCESS;
				*(int16_t*)&response[1] = index;
				response[3] = OUTPUT_SUBINDEX_OD;
				response[4] = hardware.output_value;
				response_len = 5;
				break;
			default:
				response[0] = CAN_SERVER_RESPONSE_ERROR;
				*(int16_t*)&response[1] = index;
				response[3] = sub_index;
				response_len = 4;
//				sprintf("[ERROR] CAN Write index not found %d",index);
				break;
		}
	} else if (cmd & RD_CLIENT_REQUEST_CMD_BYTE){

		*(int16_t*)&response[1] = index;
		response[3] = sub_index;
		// Use data length to get data out (set objects)
		switch (index){
			case DEVICE_TYPE_INDEX_OD:
				response[0] = RD_SERVER_RESPONSE_CMD_4_BYTE;
				// device type is  I/O device
				response[4] = 0x92;
				response[5] = 0x01;
				response[6] = 0x02;
				response[7] = 0x00;
				response_len = 8;
				break;

			case ID_OBJECT_OD:
				response[0] = RD_SERVER_RESPONSE_CMD_4_BYTE;
				switch (sub_index){
					case ID_OBJECT_NUM_ENTRIES_OD:
						response[4] = 0x04;
						response[5] = 0x00;
						break;
					case ID_OBJECT_VENDOR_ID_OD:
						response[4] = 0xD2;
						response[5] = 0x04;
						break;
					case ID_OBJECT_PRODUCT_CODE_OD:
						response[4] = 0x2E;
						response[5] = 0x16;
						break;
					case ID_OBJECT_REVISION_NUM_OD:
						response[4] = 0x01;
						response[5] = 0x00;
						break;
					default:
						// shouldn't come here
						break;
				}

				response[6] = 0x00;
				response[7] = 0x00;
				response_len = 8;
				break;

			case OUTPUT_INDEX_OD:
				response[0] = RD_SERVER_RESPONSE_CMD_1_BYTE;
				response[4] = hardware.output_value;
				response_len = 5;
				break;

			case INPUT_INDEX_OD:
				response[0] = RD_SERVER_RESPONSE_CMD_1_BYTE;
				switch (sub_index){
					case INPUT_ALL_SUBINDEX_OD:
						response[4] = hardware.input_value;
						break;
					case INPUT_1_SUBINDEX_OD:
					case INPUT_2_SUBINDEX_OD:
					case INPUT_3_SUBINDEX_OD:
					case INPUT_4_SUBINDEX_OD:
					case INPUT_5_SUBINDEX_OD:
					case INPUT_6_SUBINDEX_OD:
					case INPUT_7_SUBINDEX_OD:
					case INPUT_8_SUBINDEX_OD:
						response[4] = ((hardware.input_value >> (sub_index-1)) & 1);
						break;
				}
				response_len = 5;
//				send_can_packet((TSDO|can.id_u.id_value),response,response_len);
				break;
			case TEMP_INDEX_OD:
				response[0] = RD_SERVER_RESPONSE_CMD_2_BYTE;
				*(int16_t*)&response[1] = index;
				response[3] = TEMP_SUBINDEX_OD;
				*(int16_t*)&response[4] = __REV16(ICM42670P.temperature);
				response_len = 6;
//				send_can_packet((TSDO|can.id_u.id_value),response,response_len);
				break;

			case ACCEL_INDEX_OD:
				response[0] = RD_SERVER_RESPONSE_CMD_2_BYTE;
				*(int16_t*)&response[1] = index;

				switch (sub_index){
					case ACCEL_X_SUBINDEX_OD:
						response[3] = ACCEL_X_SUBINDEX_OD;
						*(int16_t*)&response[4] = __REV16(ICM42670P.accel_x_conv);
						break;
					case ACCEL_Y_SUBINDEX_OD:
						response[3] = ACCEL_Y_SUBINDEX_OD;
						*(int16_t*)&response[4] = __REV16(ICM42670P.accel_y_conv);
						break;
					case ACCEL_Z_SUBINDEX_OD:
						response[3] = ACCEL_Z_SUBINDEX_OD;
						*(int16_t*)&response[4] = __REV16(ICM42670P.accel_z_conv);
						break;
					default:
						// shouldn't come here
						break;
				}
				response_len = 6;
//				send_can_packet((TSDO|can.id_u.id_value),response,response_len);
				break;

				case GYRO_INDEX_OD:
					response[0] = RD_SERVER_RESPONSE_CMD_2_BYTE;
					*(int16_t*)&response[1] = index;

					switch (sub_index){
						case GYRO_X_SUBINDEX_OD:
							response[3] = GYRO_X_SUBINDEX_OD;
							*(int16_t*)&response[4] = __REV16(ICM42670P.gyro_x_conv);
							break;
						case GYRO_Y_SUBINDEX_OD:
							response[3] = GYRO_Y_SUBINDEX_OD;
							*(int16_t*)&response[4] = __REV16(ICM42670P.gyro_y_conv);
							break;
						case GYRO_Z_SUBINDEX_OD:
							response[3] = GYRO_Z_SUBINDEX_OD;
							*(int16_t*)&response[4] = __REV16(ICM42670P.gyro_z_conv);
							break;
						default:
							// shouldn't come here
							break;
					}
					response_len = 6;
//					send_can_packet((TSDO|can.id_u.id_value),response,response_len);
					break;

			default:
				response[0] = CAN_SERVER_RESPONSE_ERROR;
				*(int16_t*)&response[1] = index;
				response[3] = sub_index;
				response_len = 4;
//				printf("[ERROR] CAN Read index not found %d",index);
				break;
		}

	} else {
		response[0] = CAN_SERVER_RESPONSE_ERROR;
		*(int16_t*)&response[1] = 0x0000;
		response[3] = 0x00;
		response_len = 4;
		// No a known command
//		printf("[ERROR] CAN CMD not found %d",cmd);
	}

	// Send response if required
	if (response_len != 0) send_can_packet((TSDO|can.id_u.id_value),response,response_len);
}
