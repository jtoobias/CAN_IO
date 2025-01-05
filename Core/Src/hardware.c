/*
 * hardware.c
 *
 *  Created on: Nov 14, 2024
 *      Author: Jonathan
 */
#include "hardware.h"
#include "ICM42670P.h"
// Variables
// Define the global instance of hardware_t
hardware_t hardware;

// Functions

// used to handle any hardware inits
void hardware_init(CAN_HandleTypeDef *can_handler){
	//
	hardware.can.handler = can_handler;
	hardware.can.state = CAN_STATE_INIT;
	hardware.can.target_state = CAN_STATE_PREOPERATIONAL;
}

// Used to poll hardware
void hardware_monitoring(void){
	read_inputs();

}

void reboot_device(void) {
	// Make sure to perform any save before reset

    NVIC_SystemReset(); // Triggers a system reset
}

// Functions
  // Call before starting CAN
void setup_CAN_cfg(void){

	if (HAL_CAN_DeInit(hardware.can.handler) != HAL_OK) {
        Error_Handler(); // Replace with appropriate error handling
    }
	// Read inputs
    hardware.can.id_u.id.bit0 = (HAL_GPIO_ReadPin(CAN_CFG1_GPIO_Port, CAN_CFG1_Pin) == GPIO_PIN_SET);
    hardware.can.id_u.id.bit1 = (HAL_GPIO_ReadPin(CAN_CFG2_GPIO_Port, CAN_CFG2_Pin) == GPIO_PIN_SET);
    hardware.can.id_u.id.bit2 = (HAL_GPIO_ReadPin(CAN_CFG3_GPIO_Port, CAN_CFG3_Pin) == GPIO_PIN_SET);
    hardware.can.id_u.id.bit3 = (HAL_GPIO_ReadPin(CAN_CFG4_GPIO_Port, CAN_CFG4_Pin) == GPIO_PIN_SET);

    hardware.can.buad_u.buad.bit0 = (HAL_GPIO_ReadPin(CAN_Baud1_GPIO_Port, CAN_Baud1_Pin) == GPIO_PIN_SET);
    hardware.can.buad_u.buad.bit1 = (HAL_GPIO_ReadPin(CAN_Baud2_GPIO_Port, CAN_Baud2_Pin) == GPIO_PIN_SET);
    hardware.can.buad_u.buad.bit2 = (HAL_GPIO_ReadPin(CAN_Baud3_GPIO_Port, CAN_Baud3_Pin) == GPIO_PIN_SET);

	// 0 is eeprom value, 1 = 125 kbps, 2 = 250 kbps, 3=500 kbps, 4 = 1 kbps, higher defaults to 500 kbps
	if (hardware.can.buad_u.value == 0);
	else if (hardware.can.buad_u.value == 1) hardware.can.handler->Init.Prescaler = CAN_BUAD_RATE_PRESCALER_125;
	else if (hardware.can.buad_u.value == 2) hardware.can.handler->Init.Prescaler = CAN_BUAD_RATE_PRESCALER_250;
	else if (hardware.can.buad_u.value == 3) hardware.can.handler->Init.Prescaler = CAN_BUAD_RATE_PRESCALER_500;
	else if (hardware.can.buad_u.value == 4) hardware.can.handler->Init.Prescaler = CAN_BUAD_RATE_PRESCALER_1000;
	else hardware.can.handler->Init.Prescaler = CAN_BUAD_RATE_PRESCALER_500;


	//init
	hardware.can.handler->Instance = CAN1;
	hardware.can.handler->Init.Mode = CAN_MODE_NORMAL;
	hardware.can.handler->Init.SyncJumpWidth = CAN_SJW_1TQ;
	hardware.can.handler->Init.TimeSeg1 = CAN_BS1_2TQ;
	hardware.can.handler->Init.TimeSeg2 = CAN_BS2_1TQ;
	hardware.can.handler->Init.TimeTriggeredMode = DISABLE;
	hardware.can.handler->Init.AutoBusOff = DISABLE;
	hardware.can.handler->Init.AutoWakeUp = DISABLE;
	hardware.can.handler->Init.AutoRetransmission = DISABLE;
	hardware.can.handler->Init.ReceiveFifoLocked = DISABLE;
	hardware.can.handler->Init.TransmitFifoPriority = DISABLE;
	  if (HAL_CAN_Init(hardware.can.handler) != HAL_OK)
	  {
	    Error_Handler();
	  }

    hardware.can.filter.FilterIdHigh = 0x00;
    hardware.can.filter.FilterIdLow = hardware.can.id_u.id_value;
    hardware.can.filter.FilterMaskIdHigh = 0x00;
    hardware.can.filter.FilterMaskIdLow = 0x00;
    hardware.can.filter.FilterFIFOAssignment = CAN_RX_FIFO1;
    hardware.can.filter.FilterBank = 0;
    hardware.can.filter.FilterMode = CAN_FILTERMODE_IDMASK;
    hardware.can.filter.FilterScale = CAN_FILTERSCALE_16BIT;
    hardware.can.filter.FilterActivation = CAN_FILTER_ENABLE;
//    hardware.can.filter.SlaveStartFilterBank = ; //    For single CAN instances, this parameter is meaningless.
    // Apply filter configuration and handle errors
	if (HAL_CAN_ConfigFilter(hardware.can.handler, &hardware.can.filter) != HAL_OK) {
	  Error_Handler(); // Replace with appropriate error handling
	}

}


void send_can_packet(int func_code,uint8_t *data,uint32_t data_len){

	  hardware.can.tx_header.DLC = data_len;
	  hardware.can.tx_header.ExtId = 0;
	  hardware.can.tx_header.IDE = CAN_ID_STD;
	  hardware.can.tx_header.RTR = CAN_RTR_DATA;
	  hardware.can.tx_header.StdId = func_code | hardware.can.id_u.id_value;
	  hardware.can.tx_header.TransmitGlobalTime = DISABLE;
	  for (int i = 0; i < data_len; i++){
		  hardware.can.tx_data_buffer[i] = *(data + i);
	  }
	  HAL_CAN_AddTxMessage(hardware.can.handler,&hardware.can.tx_header,
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
void read_inputs(void){
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

void sdo_handler(uint8_t *packet,uint32_t len){
	int cmd = *packet;
		// Go from little-edian to big
	int16_t index = (*(int16_t*)(packet+1));
	int8_t sub_index = *(packet+3);
	int data = *(int32_t*)(packet+4);

	uint8_t response[8];
	uint32_t response_len = 0;

		// Check for a write operation
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
				response_len = 5;
				break;
			default:
				response[0] = CAN_SERVER_RESPONSE_ERROR;
				*(int16_t*)&response[1] = __REV16(index);
				response[3] = sub_index;
				response_len = 4;
//				sprintf("[ERROR] CAN Write index not found %d",index);
				break;
		}
	} else if (cmd & RD_CLIENT_REQUEST_CMD_BYTE){
		// Use data length to get data out (set objects)
		switch (index){
			case DEVICE_TYPE_INDEX_OD:
				response[0] = RD_SERVER_RESPONSE_CMD_4_BYTE;
				*(int16_t*)&response[1] = __REV16(index);
				response[3] = DEVICE_TYPE_SUBINDEX_OD;
				// device type is  I/O device
				response[4] = 0x92;
				response[5] = 0x01;
				response[6] = 0x02;
				response[7] = 0x00;
				response_len = 8;
				break;

			case ID_OBJECT_OD:
				response[0] = RD_SERVER_RESPONSE_CMD_4_BYTE;
				*(int16_t*)&response[1] = __REV16(index);
				switch (sub_index){
					case ID_OBJECT_NUM_ENTRIES_OD:
						response[3] = ID_OBJECT_NUM_ENTRIES_OD;
						response[4] = 0x04;
						response[5] = 0x00;
						break;
					case ID_OBJECT_VENDOR_ID_OD:
						response[3] = ID_OBJECT_VENDOR_ID_OD;
						response[4] = 0xD2;
						response[5] = 0x04;
						break;
					case ID_OBJECT_PRODUCT_CODE_OD:
						response[3] = ID_OBJECT_PRODUCT_CODE_OD;
						response[4] = 0x2E;
						response[5] = 0x16;
						break;
					case ID_OBJECT_REVISION_NUM_OD:
						response[3] = ID_OBJECT_REVISION_NUM_OD;
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
				*(int16_t*)&response[1] = __REV16(index);
				response[3] = OUTPUT_SUBINDEX_OD;
				response[4] = hardware.out_u.output_value;
				response_len = 5;
				break;

			case INPUT_INDEX_OD:
				response[0] = RD_SERVER_RESPONSE_CMD_1_BYTE;
				*(int16_t*)&response[1] = __REV16(index);
				response[3] = INPUT_SUBINDEX_OD;
				response[4] = hardware.in_u.input_value;
				response_len = 5;
				send_can_packet(TSDO,response,response_len);
				break;

			case TEMP_INDEX_OD:
				response[0] = RD_SERVER_RESPONSE_CMD_2_BYTE;
				*(int16_t*)&response[1] = __REV16(index);
				response[3] = TEMP_SUBINDEX_OD;
				response[4] = __REV16(ICM42670P.temperature);
				response_len = 6;
				send_can_packet(TSDO,response,response_len);
				break;

//			case TEMP_INDEX_OD:
//				response[0] = RD_SERVER_RESPONSE_CMD_2_BYTE;
//				*(int16_t*)&response[1] = __REV16(index);
//				response[3] = TEMP_SUBINDEX_OD;
//				response[4] = __REV16(ICM42670P.temperature);
//				response_len = 6;
//				send_can_packet(TSDO,response,response_len);
//				break;

			default:
				response[0] = CAN_SERVER_RESPONSE_ERROR;
				*(int16_t*)&response[1] = __REV16(index);
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

	send_can_packet(TSDO,response,response_len);
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
	switch (hardware.can.state){
		case CAN_STATE_INIT:

			hardware.can.state = CAN_STATE_PREOPERATIONAL;
			hardware.can.target_state = CAN_STATE_PREOPERATIONAL;
			HAL_CAN_Stop(hardware.can.handler);
			setup_CAN_cfg();
			HAL_CAN_Start(hardware.can.handler);
			HAL_CAN_ActivateNotification(hardware.can.handler,CAN_IT_RX_FIFO1_MSG_PENDING);

			uint8_t tmp_arr = 0x00;
			send_can_packet(HEARTBEAT,&tmp_arr,1);
			break;
		case CAN_STATE_PREOPERATIONAL:
			if (hardware.can.target_state == CAN_STATE_OPERATIONAL) hardware.can.state = CAN_STATE_OPERATIONAL;
		case CAN_STATE_OPERATIONAL:
			if (hardware.can.target_state == CAN_STATE_PREOPERATIONAL &&
				hardware.can.state == CAN_STATE_OPERATIONAL) {
				hardware.can.state = CAN_STATE_PREOPERATIONAL;

			} else if (hardware.can.target_state == CAN_STATE_INIT) {
				hardware.can.state = CAN_STATE_INIT;
			} else if (hardware.can.target_state == CAN_STATE_STOPPED) {
				hardware.can.state = CAN_STATE_STOPPED;
			}
			break;
		case CAN_STATE_STOPPED:
			if (hardware.can.target_state == CAN_STATE_PREOPERATIONAL) {
				hardware.can.state = CAN_STATE_PREOPERATIONAL;
			} else if (hardware.can.target_state == CAN_STATE_OPERATIONAL) {
				hardware.can.state = CAN_STATE_OPERATIONAL;
			} else if (hardware.can.target_state == CAN_STATE_INIT) {
				hardware.can.state = CAN_STATE_INIT;
			}
			break;
		default:
			//problem
			break;
	}
}

void nmt_handler(uint8_t *packet,uint32_t len){
	// Handle device state
		// make sure the nmt is for us
	if (*(packet+1) != hardware.can.id_u.id_value) return;
	uint8_t cmd = *packet;

	// Actual transitions are handled in another state machine to make sure the correct steps are executed
	switch (cmd){
		case NMT_START:
			hardware.can.target_state = CAN_STATE_OPERATIONAL;
			break;
		case NMT_STOP:
			hardware.can.target_state = CAN_STATE_STOPPED;
			break;
		case NMT_PREOPERATIONAL:
			hardware.can.target_state = CAN_STATE_PREOPERATIONAL;
			break;
		case NMT_RESET_NODE:
			reboot_device();
			break;
		case NMT_RESET_COMMS:
			// Call Can setup and update PDO's from eeprom
			hardware.can.target_state = CAN_STATE_INIT;
			break;
	}
}

void sync_handler(void){
	// Place holder for now

}

//	Manage CAN messages here, build packet handler
void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *can_handler){

    if (HAL_CAN_GetRxMessage(can_handler, CAN_RX_FIFO1, &hardware.can.rx_header, hardware.can.rx_data_buffer) == HAL_OK) {
        // Process the received message
    	int can_id = (hardware.can.rx_header.StdId & 0x07F);

    	if (can_id == NMT_COB_ID) {
    		nmt_handler(hardware.can.rx_data_buffer,hardware.can.rx_header.DLC);
    		return;
    	} else if (can_id == SYNC_COB_ID) {
    		sync_handler();
    		return;
    	} else if (can_id != hardware.can.id_u.id_value) return;
    		// Check COB-ID
    	int cob_id = (hardware.can.rx_header.StdId & 0x780);

    	if (hardware.can.state != CAN_STATE_PREOPERATIONAL &&
    		hardware.can.state != CAN_STATE_OPERATIONAL) return;
//    	hardware.can.rx_header.DLC
    	switch (cob_id){
			case RPDO1:
					// PDO's only work in Operational state
		    	if (hardware.can.state != CAN_STATE_OPERATIONAL) break;

				break;

			case RPDO2:
		    	if (hardware.can.state != CAN_STATE_OPERATIONAL) break;

				break;

			case RPDO3:
		    	if (hardware.can.state != CAN_STATE_OPERATIONAL) break;

				break;

			case RPDO4:
		    	if (hardware.can.state != CAN_STATE_OPERATIONAL) break;

				break;

    		case RSDO:
    			sdo_handler(hardware.can.rx_data_buffer,hardware.can.rx_header.DLC);
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
