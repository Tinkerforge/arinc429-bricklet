/* arinc429-bricklet
 * Copyright (C) 2020 Olaf Lüke <olaf@tinkerforge.com>
 *
 * communication.c: TFP protocol message handling
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

#include "communication.h"

#include "bricklib2/utility/communication_callback.h"
#include "bricklib2/protocols/tfp/tfp.h"
#include "bricklib2/logging/logging.h"

#include "xmc_gpio.h"
#include "hi3593.h"

extern const int8_t opcode_length[256];

BootloaderHandleMessageResponse handle_message(const void *message, void *response) {
	switch(tfp_get_fid_from_message(message)) {
		case FID_DEBUG_GET_DISCRETES: return debug_get_discretes(message, response);
		case FID_DEBUG_READ_REGISTER_LOW_LEVEL: return debug_read_register_low_level(message, response);
		case FID_DEBUG_WRITE_REGISTER_LOW_LEVEL: return debug_write_register_low_level(message, response);
		default: return HANDLE_MESSAGE_RESPONSE_NOT_SUPPORTED;
	}
}

BootloaderHandleMessageResponse debug_get_discretes(const DebugGetDiscretes *data, DebugGetDiscretes_Response *response) {
	response->header.length = sizeof(DebugGetDiscretes_Response);
	response->rx_discretes  = (XMC_GPIO_GetInput(hi3593_input_ports[HI3593_R1INT_INDEX],  hi3593_input_pins[HI3593_R1INT_INDEX])  << 0) |
	                          (XMC_GPIO_GetInput(hi3593_input_ports[HI3593_R1FLAG_INDEX], hi3593_input_pins[HI3593_R1FLAG_INDEX]) << 1) |
	                          (XMC_GPIO_GetInput(hi3593_input_ports[HI3593_MB13_INDEX],   hi3593_input_pins[HI3593_MB13_INDEX])   << 2) |
	                          (XMC_GPIO_GetInput(hi3593_input_ports[HI3593_MB12_INDEX],   hi3593_input_pins[HI3593_MB12_INDEX])   << 3) |
	                          (XMC_GPIO_GetInput(hi3593_input_ports[HI3593_MB11_INDEX],   hi3593_input_pins[HI3593_MB11_INDEX])   << 4) |
	                          (XMC_GPIO_GetInput(hi3593_input_ports[HI3593_R2INT_INDEX],  hi3593_input_pins[HI3593_R2INT_INDEX])  << 5) |
	                          (XMC_GPIO_GetInput(hi3593_input_ports[HI3593_R2FLAG_INDEX], hi3593_input_pins[HI3593_R2FLAG_INDEX]) << 6) |
	                          (XMC_GPIO_GetInput(hi3593_input_ports[HI3593_MB23_INDEX],   hi3593_input_pins[HI3593_MB23_INDEX])   << 7) |
	                          (XMC_GPIO_GetInput(hi3593_input_ports[HI3593_MB22_INDEX],   hi3593_input_pins[HI3593_MB22_INDEX])   << 8) |
	                          (XMC_GPIO_GetInput(hi3593_input_ports[HI3593_MB21_INDEX],   hi3593_input_pins[HI3593_MB21_INDEX])   << 9);

	response->tx_discretes  = (XMC_GPIO_GetInput(hi3593_input_ports[HI3593_TEMPTY_INDEX], hi3593_input_pins[HI3593_TEMPTY_INDEX]) << 0) |
	                          (XMC_GPIO_GetInput(hi3593_input_ports[HI3593_TFULL_INDEX],  hi3593_input_pins[HI3593_TFULL_INDEX])  << 1);

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse debug_read_register_low_level(const DebugReadRegisterLowLevel *data, DebugReadRegisterLowLevel_Response *response) {
	response->header.length = sizeof(DebugReadRegisterLowLevel_Response);
	if(opcode_length[data->op_code] < 0) {
		response->rw_error     = ARINC429_RW_ERROR_INVALID_OP_CODE;
		response->value_length = 0;
	} else if((data->op_code & (1 << 7)) == 0) {
		response->rw_error     = ARINC429_RW_ERROR_NO_READ;
		response->value_length = 0;
	} else {
		response->rw_error     = ARINC429_RW_ERROR_OK;
		response->value_length = opcode_length[data->op_code];
		uint32_t ret = hi3593_task_read_register(data->op_code, response->value_data, response->value_length);
		if(ret != 0) {
			response->rw_error     = ARINC429_RW_ERROR_SPI;
			response->value_length = 0;
		}
	}

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse debug_write_register_low_level(const DebugWriteRegisterLowLevel *data, DebugWriteRegisterLowLevel_Response *response) {
	if(data->value_length > 32) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	response->header.length = sizeof(DebugWriteRegisterLowLevel_Response);
	if(opcode_length[data->op_code] < 0) {
		response->rw_error = ARINC429_RW_ERROR_INVALID_OP_CODE;
	} else if(data->op_code & (1 << 7)) {
		response->rw_error = ARINC429_RW_ERROR_NO_WRITE;
	} else if(opcode_length[data->op_code] != data->value_length) {
		response->rw_error = ARINC429_RW_ERROR_INVALID_LENGTH;
	} else {
		response->rw_error = ARINC429_RW_ERROR_OK;
		uint32_t ret       = hi3593_task_write_register(data->op_code, data->value_data, data->value_length);
		if(ret != 0) {
			response->rw_error = ARINC429_RW_ERROR_SPI;
		}
	}

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}


void communication_tick(void) {
}

void communication_init(void) {
}
