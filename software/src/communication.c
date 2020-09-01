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
#include "arinc429.h"

extern const int8_t opcode_length[256];

BootloaderHandleMessageResponse handle_message(const void *message, void *response) {
	switch(tfp_get_fid_from_message(message)) {
		case FID_DEBUG_GET_DISCRETES: return debug_get_discretes(message, response);
		case FID_DEBUG_READ_REGISTER_LOW_LEVEL: return debug_read_register_low_level(message, response);
		case FID_DEBUG_WRITE_REGISTER_LOW_LEVEL: return debug_write_register_low_level(message, response);
		case FID_GET_CAPABILITIES: return get_capabilities(message, response);
		case FID_SET_HEARTBEAT_CALLBACK_CONFIGURATION: return set_heartbeat_callback_configuration(message);
		case FID_GET_HEARTBEAT_CALLBACK_CONFIGURATION: return get_heartbeat_callback_configuration(message, response);
		case FID_SET_CHANNEL_CONFIGURATION: return set_channel_configuration(message);
		case FID_GET_CHANNEL_CONFIGURATION: return get_channel_configuration(message, response);
		case FID_SET_CHANNEL_MODE: return set_channel_mode(message);
		case FID_GET_CHANNEL_MODE: return get_channel_mode(message, response);
		case FID_CLEAR_PRIO_LABELS: return clear_prio_labels(message);
		case FID_SET_PRIO_LABELS: return set_prio_labels(message);
		case FID_GET_PRIO_LABELS: return get_prio_labels(message, response);
		case FID_CLEAR_RX_LABELS: return clear_rx_labels(message);
		case FID_SET_RX_LABEL_CONFIGURATION: return set_rx_label_configuration(message);
		case FID_GET_RX_LABEL_CONFIGURATION: return get_rx_label_configuration(message, response);
		case FID_READ_NEXT_FRAME: return read_next_frame(message, response);
		case FID_SET_RECEIVE_FRAME_CALLBACK_CONFIGURATION: return set_receive_frame_callback_configuration(message);
		case FID_GET_RECEIVE_FRAME_CALLBACK_CONFIGURATION: return get_receive_frame_callback_configuration(message, response);
		case FID_WRITE_FRAME_DIRECT: return write_frame_direct(message);
		case FID_WRITE_FRAME_SCHEDULED: return write_frame_scheduled(message);
		case FID_SET_SCHEDULE_ENTRY: return set_schedule_entry(message);
		case FID_GET_SCHEDULE_ENTRY: return get_schedule_entry(message, response);
		case FID_CLEAR_SCHEDULE_ENTRIES: return clear_schedule_entries(message);
		default: return HANDLE_MESSAGE_RESPONSE_NOT_SUPPORTED;
	}
}

// Assumes one TX und two RX channels
static bool check_channel(const uint8_t channel, const bool is_setter) {
	// Allow TX1, RX1 and RX2
	if((channel == 1) || (channel == 33) || (channel == 34)) {
		return true;
	}

	// Allow "all RX/TX channel" for setter, but not for getter
	if(is_setter && ((channel == 0) || (channel == 32))) {
		return true;
	}

	return false;
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

BootloaderHandleMessageResponse get_capabilities(const GetCapabilities *data, GetCapabilities_Response *response) {
	response->header.length      = sizeof(GetCapabilities_Response);
	response->rx_channels        = ARINC429_CHANNEL_RX_NUM;
	response->rx_filter_frames   = ARINC429_RX_FRAME_NUM;
	response->tx_channels        = ARINC429_CHANNEL_TX_NUM;
	response->tx_schedule_slots  = ARINC429_TX_SLOTS_NUM;
	response->tx_schedule_frames = ARINC429_TX_FRAME_NUM;

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse set_heartbeat_callback_configuration(const SetHeartbeatCallbackConfiguration *data) {
	if(!check_channel(data->channel, true)) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse get_heartbeat_callback_configuration(const GetHeartbeatCallbackConfiguration *data, GetHeartbeatCallbackConfiguration_Response *response) {
	response->header.length = sizeof(GetHeartbeatCallbackConfiguration_Response);

	if(!check_channel(data->channel, false)) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse set_channel_configuration(const SetChannelConfiguration *data) {
	if(!check_channel(data->channel, true)) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	if(data->parity > ARINC429_PARITY_PARITY) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	if(data->speed > ARINC429_SPEED_LS) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	if((data->channel == ARINC429_CHANNEL_TX) || (data->channel == ARINC429_CHANNEL_TX1)) {
		arinc429.tx_channel[0].common.config_parity = data->parity;
		arinc429.tx_channel[0].common.config_speed  = data->speed;
		arinc429.tx_channel[0].common.config_new    = true;
	}

	if((data->channel == ARINC429_CHANNEL_RX) || (data->channel == ARINC429_CHANNEL_RX1)) {
		arinc429.rx_channel[0].common.config_parity = data->parity;
		arinc429.rx_channel[0].common.config_speed  = data->speed;
		arinc429.rx_channel[0].common.config_new    = true;
	}

	if((data->channel == ARINC429_CHANNEL_RX) || (data->channel == ARINC429_CHANNEL_RX2)) {
		arinc429.rx_channel[1].common.config_parity = data->parity;
		arinc429.rx_channel[1].common.config_speed  = data->speed;
		arinc429.rx_channel[1].common.config_new    = true;
	}

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse get_channel_configuration(const GetChannelConfiguration *data, GetChannelConfiguration_Response *response) {
	response->header.length = sizeof(GetChannelConfiguration_Response);

	if(!check_channel(data->channel, false)) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	switch(data->channel) {
		case ARINC429_CHANNEL_TX1: {
			response->parity = arinc429.tx_channel[0].common.config_parity;
			response->speed  = arinc429.tx_channel[0].common.config_speed;
			break;
		}

		case ARINC429_CHANNEL_RX1: {
			response->parity = arinc429.rx_channel[0].common.config_parity;
			response->speed  = arinc429.rx_channel[0].common.config_speed;
			break;
		}

		case ARINC429_CHANNEL_RX2: {
			response->parity = arinc429.rx_channel[1].common.config_parity;
			response->speed  = arinc429.rx_channel[1].common.config_speed;
			break;
		}

		default: {
			return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
		}
	}		

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse set_channel_mode(const SetChannelMode *data) {
	if(!check_channel(data->channel, true)) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse get_channel_mode(const GetChannelMode *data, GetChannelMode_Response *response) {
	response->header.length = sizeof(GetChannelMode_Response);

	if(!check_channel(data->channel, false)) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse clear_prio_labels(const ClearPrioLabels *data) {
	if(!check_channel(data->channel, true)) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse set_prio_labels(const SetPrioLabels *data) {
	if(!check_channel(data->channel, true)) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse get_prio_labels(const GetPrioLabels *data, GetPrioLabels_Response *response) {
	response->header.length = sizeof(GetPrioLabels_Response);

	if(!check_channel(data->channel, false)) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse clear_rx_labels(const ClearRXLabels *data) {
	if(!check_channel(data->channel, true)) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse set_rx_label_configuration(const SetRXLabelConfiguration *data) {
	if(!check_channel(data->channel, true)) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse get_rx_label_configuration(const GetRXLabelConfiguration *data, GetRXLabelConfiguration_Response *response) {
	response->header.length = sizeof(GetRXLabelConfiguration_Response);

	if(!check_channel(data->channel, false)) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse read_next_frame(const ReadNextFrame *data, ReadNextFrame_Response *response) {
	response->header.length = sizeof(ReadNextFrame_Response);

	if(!check_channel(data->channel, false)) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse set_receive_frame_callback_configuration(const SetReceiveFrameCallbackConfiguration *data) {
	if(!check_channel(data->channel, true)) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse get_receive_frame_callback_configuration(const GetReceiveFrameCallbackConfiguration *data, GetReceiveFrameCallbackConfiguration_Response *response) {
	response->header.length = sizeof(GetReceiveFrameCallbackConfiguration_Response);

	if(!check_channel(data->channel, false)) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse write_frame_direct(const WriteFrameDirect *data) {
	if(!check_channel(data->channel, true)) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse write_frame_scheduled(const WriteFrameScheduled *data) {

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse set_schedule_entry(const SetScheduleEntry *data) {
	if(!check_channel(data->channel, true)) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse get_schedule_entry(const GetScheduleEntry *data, GetScheduleEntry_Response *response) {
	response->header.length = sizeof(GetScheduleEntry_Response);

	if(!check_channel(data->channel, false)) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse clear_schedule_entries(const ClearScheduleEntries *data) {
	if(!check_channel(data->channel, true)) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}




bool handle_heartbeat_callback(void) {
	static bool is_buffered = false;
	static Heartbeat_Callback cb;

	if(!is_buffered) {
		tfp_make_default_header(&cb.header, bootloader_get_uid(), sizeof(Heartbeat_Callback), FID_CALLBACK_HEARTBEAT);
		// TODO: Implement Heartbeat callback handling

		return false;
	}

	if(bootloader_spitfp_is_send_possible(&bootloader_status.st)) {
		bootloader_spitfp_send_ack_and_message(&bootloader_status, (uint8_t*)&cb, sizeof(Heartbeat_Callback));
		is_buffered = false;
		return true;
	} else {
		is_buffered = true;
	}

	return false;
}

bool handle_receive_frame_callback(void) {
	static bool is_buffered = false;
	static ReceiveFrame_Callback cb;

	if(!is_buffered) {
		tfp_make_default_header(&cb.header, bootloader_get_uid(), sizeof(ReceiveFrame_Callback), FID_CALLBACK_RECEIVE_FRAME);
		// TODO: Implement ReceiveFrame callback handling

		return false;
	}

	if(bootloader_spitfp_is_send_possible(&bootloader_status.st)) {
		bootloader_spitfp_send_ack_and_message(&bootloader_status, (uint8_t*)&cb, sizeof(ReceiveFrame_Callback));
		is_buffered = false;
		return true;
	} else {
		is_buffered = true;
	}

	return false;
}

void communication_tick(void) {
//	communication_callback_tick();
}

void communication_init(void) {
//	communication_callback_init();
}