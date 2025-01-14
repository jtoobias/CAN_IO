/*
 * can_manager.c
 *
 *  Created on: Jan 15, 2025
 *      Author: jonathan
 */


#include "stm32f1xx_hal.h"


// Example code for now
#define CAN_TX_BUFFER_SIZE 16 // Adjust as needed

typedef struct {
    CAN_TxHeaderTypeDef header;
    uint8_t data[8]; // CAN data payload
} CAN_Packet;

typedef struct {
    CAN_Packet buffer[CAN_TX_BUFFER_SIZE];
    volatile uint16_t head;
    volatile uint16_t tail;
} CircularBuffer;

// Initialize the circular buffer
CircularBuffer tx_buffer = { .head = 0, .tail = 0 };

// Check if the buffer is full
int is_buffer_full() {
    return ((tx_buffer.head + 1) % CAN_TX_BUFFER_SIZE) == tx_buffer.tail;
}

// Check if the buffer is empty
int is_buffer_empty() {
    return tx_buffer.head == tx_buffer.tail;
}

// Add a packet to the buffer
int enqueue_packet(CAN_Packet *packet) {
    if (is_buffer_full()) {
        return -1; // Buffer full, drop the packet
    }
    tx_buffer.buffer[tx_buffer.head] = *packet;
    tx_buffer.head = (tx_buffer.head + 1) % CAN_TX_BUFFER_SIZE;
    return 0;
}

// Retrieve a packet from the buffer
int dequeue_packet(CAN_Packet *packet) {
    if (is_buffer_empty()) {
        return -1; // Buffer empty
    }
    *packet = tx_buffer.buffer[tx_buffer.tail];
    tx_buffer.tail = (tx_buffer.tail + 1) % CAN_TX_BUFFER_SIZE;
    return 0;
}

// Transmit packets from the buffer
void CAN_TxHandler(CAN_HandleTypeDef *hcan) {
    CAN_Packet packet;

    // Try to transmit while mailboxes are available and buffer is not empty
    while (!is_buffer_empty()) {
        uint32_t free_mailbox;
        if (HAL_CAN_GetTxMailboxesFreeLevel(hcan) > 0 &&
            HAL_CAN_IsTxMessagePending(hcan) == 0 &&
            dequeue_packet(&packet) == 0) {
            // Attempt to transmit the dequeued packet
            if (HAL_CAN_AddTxMessage(hcan, &packet.header, packet.data, &free_mailbox) != HAL_OK) {
                // Transmission failed, re-enqueue the packet
                enqueue_packet(&packet);
                break;
            }
        }
    }
}

// Example usage in main or interrupt
void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan) {
    // Called when a mailbox is free; send the next packet
    CAN_TxHandler(hcan);
}

// Function to queue a packet for transmission
int CAN_SendPacket(CAN_HandleTypeDef *hcan, CAN_TxHeaderTypeDef *header, uint8_t *data) {
    CAN_Packet packet;
    packet.header = *header;
    memcpy(packet.data, data, 8);

    // Enqueue the packet
    if (enqueue_packet(&packet) != 0) {
        return -1; // Buffer full
    }

    // Trigger transmission if a mailbox is available
    CAN_TxHandler(hcan);
    return 0;
}
