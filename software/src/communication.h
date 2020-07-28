/* arinc429-bricklet
 * Copyright (C) 2020 Olaf Lüke <olaf@tinkerforge.com>
 *
 * communication.h: TFP protocol message handling
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <stdint.h>
#include <stdbool.h>

#include "bricklib2/protocols/tfp/tfp.h"
#include "bricklib2/bootloader/bootloader.h"

// Default functions
BootloaderHandleMessageResponse handle_message(const void *data, void *response);
void communication_tick(void);
void communication_init(void);

// Constants

#define ARINC429_RW_ERROR_OK 0
#define ARINC429_RW_ERROR_NO_WRITE 1
#define ARINC429_RW_ERROR_NO_READ 2
#define ARINC429_RW_ERROR_INVALID_OP_CODE 3
#define ARINC429_RW_ERROR_INVALID_LENGTH 4
#define ARINC429_RW_ERROR_SPI 5

#define ARINC429_BOOTLOADER_MODE_BOOTLOADER 0
#define ARINC429_BOOTLOADER_MODE_FIRMWARE 1
#define ARINC429_BOOTLOADER_MODE_BOOTLOADER_WAIT_FOR_REBOOT 2
#define ARINC429_BOOTLOADER_MODE_FIRMWARE_WAIT_FOR_REBOOT 3
#define ARINC429_BOOTLOADER_MODE_FIRMWARE_WAIT_FOR_ERASE_AND_REBOOT 4

#define ARINC429_BOOTLOADER_STATUS_OK 0
#define ARINC429_BOOTLOADER_STATUS_INVALID_MODE 1
#define ARINC429_BOOTLOADER_STATUS_NO_CHANGE 2
#define ARINC429_BOOTLOADER_STATUS_ENTRY_FUNCTION_NOT_PRESENT 3
#define ARINC429_BOOTLOADER_STATUS_DEVICE_IDENTIFIER_INCORRECT 4
#define ARINC429_BOOTLOADER_STATUS_CRC_MISMATCH 5

#define ARINC429_STATUS_LED_CONFIG_OFF 0
#define ARINC429_STATUS_LED_CONFIG_ON 1
#define ARINC429_STATUS_LED_CONFIG_SHOW_HEARTBEAT 2
#define ARINC429_STATUS_LED_CONFIG_SHOW_STATUS 3

// Function and callback IDs and structs
#define FID_DEBUG_GET_DISCRETES 1
#define FID_DEBUG_READ_REGISTER_LOW_LEVEL 2
#define FID_DEBUG_WRITE_REGISTER_LOW_LEVEL 3


typedef struct {
	TFPMessageHeader header;
} __attribute__((__packed__)) DebugGetDiscretes;

typedef struct {
	TFPMessageHeader header;
	uint16_t rx_discretes;
	uint16_t tx_discretes;
} __attribute__((__packed__)) DebugGetDiscretes_Response;

typedef struct {
	TFPMessageHeader header;
	uint8_t op_code;
} __attribute__((__packed__)) DebugReadRegisterLowLevel;

typedef struct {
	TFPMessageHeader header;
	uint8_t value_length;
	uint8_t value_data[32];
	uint8_t rw_error;
} __attribute__((__packed__)) DebugReadRegisterLowLevel_Response;

typedef struct {
	TFPMessageHeader header;
	uint8_t op_code;
	uint8_t value_length;
	uint8_t value_data[32];
} __attribute__((__packed__)) DebugWriteRegisterLowLevel;

typedef struct {
	TFPMessageHeader header;
	uint8_t rw_error;
} __attribute__((__packed__)) DebugWriteRegisterLowLevel_Response;


// Function prototypes
BootloaderHandleMessageResponse debug_get_discretes(const DebugGetDiscretes *data, DebugGetDiscretes_Response *response);
BootloaderHandleMessageResponse debug_read_register_low_level(const DebugReadRegisterLowLevel *data, DebugReadRegisterLowLevel_Response *response);
BootloaderHandleMessageResponse debug_write_register_low_level(const DebugWriteRegisterLowLevel *data, DebugWriteRegisterLowLevel_Response *response);

// Callbacks


#define COMMUNICATION_CALLBACK_TICK_WAIT_MS 1
#define COMMUNICATION_CALLBACK_HANDLER_NUM 0
#define COMMUNICATION_CALLBACK_LIST_INIT \


#endif
