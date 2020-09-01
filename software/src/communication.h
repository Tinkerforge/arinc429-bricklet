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

#define ARINC429_CHANNEL_TX 0
#define ARINC429_CHANNEL_TX1 1
#define ARINC429_CHANNEL_TX2 2
#define ARINC429_CHANNEL_TX3 3
#define ARINC429_CHANNEL_TX4 4
#define ARINC429_CHANNEL_TX5 5
#define ARINC429_CHANNEL_TX6 6
#define ARINC429_CHANNEL_TX7 7
#define ARINC429_CHANNEL_TX8 8
#define ARINC429_CHANNEL_TX9 9
#define ARINC429_CHANNEL_TX10 10
#define ARINC429_CHANNEL_TX11 11
#define ARINC429_CHANNEL_TX12 12
#define ARINC429_CHANNEL_RX 32
#define ARINC429_CHANNEL_RX1 33
#define ARINC429_CHANNEL_RX2 34
#define ARINC429_CHANNEL_RX3 35
#define ARINC429_CHANNEL_RX4 36
#define ARINC429_CHANNEL_RX5 37
#define ARINC429_CHANNEL_RX6 38
#define ARINC429_CHANNEL_RX7 39
#define ARINC429_CHANNEL_RX8 40
#define ARINC429_CHANNEL_RX9 41
#define ARINC429_CHANNEL_RX10 42
#define ARINC429_CHANNEL_RX11 43
#define ARINC429_CHANNEL_RX12 44

#define ARINC429_BUFFER_ANYTHING 0
#define ARINC429_BUFFER_PRIO1 1
#define ARINC429_BUFFER_PRIO2 2
#define ARINC429_BUFFER_PRIO3 3
#define ARINC429_BUFFER_FIFO 4

#define ARINC429_SDI_DATA 0
#define ARINC429_SDI_ADDRESS 1

#define ARINC429_PARITY_TRANSPARENT 0
#define ARINC429_PARITY_PARITY 1

#define ARINC429_SPEED_HS 0
#define ARINC429_SPEED_LS 1

#define ARINC429_CHANNEL_MODE_UNINIT 0
#define ARINC429_CHANNEL_MODE_PASSIVE 1
#define ARINC429_CHANNEL_MODE_ACTIVE 2
#define ARINC429_CHANNEL_MODE_FILTER 3
#define ARINC429_CHANNEL_MODE_RUNNING 4

#define ARINC429_FRAME_STATUS_TIMEOUT 0
#define ARINC429_FRAME_STATUS_UPDATE 1

#define ARINC429_SCHEDULER_JOB_MUTE 0
#define ARINC429_SCHEDULER_JOB_SINGLE 1
#define ARINC429_SCHEDULER_JOB_CYCLIC 2

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
#define FID_GET_CAPABILITIES 4
#define FID_SET_HEARTBEAT_CALLBACK_CONFIGURATION 5
#define FID_GET_HEARTBEAT_CALLBACK_CONFIGURATION 6
#define FID_SET_CHANNEL_CONFIGURATION 8
#define FID_GET_CHANNEL_CONFIGURATION 9
#define FID_SET_CHANNEL_MODE 10
#define FID_GET_CHANNEL_MODE 11
#define FID_CLEAR_PRIO_LABELS 12
#define FID_SET_PRIO_LABELS 13
#define FID_GET_PRIO_LABELS 14
#define FID_CLEAR_RX_LABELS 15
#define FID_SET_RX_LABEL_CONFIGURATION 16
#define FID_GET_RX_LABEL_CONFIGURATION 17
#define FID_READ_NEXT_FRAME 18
#define FID_SET_RECEIVE_FRAME_CALLBACK_CONFIGURATION 19
#define FID_GET_RECEIVE_FRAME_CALLBACK_CONFIGURATION 20
#define FID_WRITE_FRAME_DIRECT 22
#define FID_WRITE_FRAME_SCHEDULED 23
#define FID_SET_SCHEDULE_ENTRY 24
#define FID_GET_SCHEDULE_ENTRY 25
#define FID_CLEAR_SCHEDULE_ENTRIES 26

#define FID_CALLBACK_HEARTBEAT 7
#define FID_CALLBACK_RECEIVE_FRAME 21

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

typedef struct {
	TFPMessageHeader header;
} __attribute__((__packed__)) GetCapabilities;

typedef struct {
	TFPMessageHeader header;
	uint8_t rx_channels;
	uint16_t rx_filter_frames;
	uint8_t tx_channels;
	uint16_t tx_schedule_slots;
	uint16_t tx_schedule_frames;
} __attribute__((__packed__)) GetCapabilities_Response;

typedef struct {
	TFPMessageHeader header;
	uint8_t channel;
	uint8_t period;
	bool value_has_to_change;
} __attribute__((__packed__)) SetHeartbeatCallbackConfiguration;

typedef struct {
	TFPMessageHeader header;
	uint8_t channel;
} __attribute__((__packed__)) GetHeartbeatCallbackConfiguration;

typedef struct {
	TFPMessageHeader header;
	uint8_t period;
	bool value_has_to_change;
} __attribute__((__packed__)) GetHeartbeatCallbackConfiguration_Response;

typedef struct {
	TFPMessageHeader header;
	uint8_t channel;
	uint8_t sequence_number;
	uint8_t channel_mode;
	uint16_t frames_processed;
	uint16_t frames_lost;
} __attribute__((__packed__)) Heartbeat_Callback;

typedef struct {
	TFPMessageHeader header;
	uint8_t channel;
	uint8_t parity;
	uint8_t speed;
} __attribute__((__packed__)) SetChannelConfiguration;

typedef struct {
	TFPMessageHeader header;
	uint8_t channel;
} __attribute__((__packed__)) GetChannelConfiguration;

typedef struct {
	TFPMessageHeader header;
	uint8_t parity;
	uint8_t speed;
} __attribute__((__packed__)) GetChannelConfiguration_Response;

typedef struct {
	TFPMessageHeader header;
	uint8_t channel;
	uint8_t mode;
} __attribute__((__packed__)) SetChannelMode;

typedef struct {
	TFPMessageHeader header;
	uint8_t channel;
} __attribute__((__packed__)) GetChannelMode;

typedef struct {
	TFPMessageHeader header;
	uint8_t mode;
} __attribute__((__packed__)) GetChannelMode_Response;

typedef struct {
	TFPMessageHeader header;
	uint8_t channel;
} __attribute__((__packed__)) ClearPrioLabels;

typedef struct {
	TFPMessageHeader header;
	uint8_t channel;
	uint8_t label[3];
} __attribute__((__packed__)) SetPrioLabels;

typedef struct {
	TFPMessageHeader header;
	uint8_t channel;
} __attribute__((__packed__)) GetPrioLabels;

typedef struct {
	TFPMessageHeader header;
	bool prio_enabled;
	uint8_t label[3];
} __attribute__((__packed__)) GetPrioLabels_Response;

typedef struct {
	TFPMessageHeader header;
	uint8_t channel;
} __attribute__((__packed__)) ClearRXLabels;

typedef struct {
	TFPMessageHeader header;
	uint8_t channel;
	uint8_t label;
	uint8_t sdi;
	uint16_t timeout;
} __attribute__((__packed__)) SetRXLabelConfiguration;

typedef struct {
	TFPMessageHeader header;
	uint8_t channel;
	uint8_t label;
} __attribute__((__packed__)) GetRXLabelConfiguration;

typedef struct {
	TFPMessageHeader header;
	uint8_t sdi;
	uint16_t timeout;
} __attribute__((__packed__)) GetRXLabelConfiguration_Response;

typedef struct {
	TFPMessageHeader header;
	uint8_t channel;
	uint8_t buffer;
} __attribute__((__packed__)) ReadNextFrame;

typedef struct {
	TFPMessageHeader header;
	bool status;
	uint32_t frame;
} __attribute__((__packed__)) ReadNextFrame_Response;

typedef struct {
	TFPMessageHeader header;
	uint8_t channel;
	uint8_t period;
	bool value_has_to_change;
} __attribute__((__packed__)) SetReceiveFrameCallbackConfiguration;

typedef struct {
	TFPMessageHeader header;
	uint8_t channel;
} __attribute__((__packed__)) GetReceiveFrameCallbackConfiguration;

typedef struct {
	TFPMessageHeader header;
	uint8_t period;
	bool value_has_to_change;
} __attribute__((__packed__)) GetReceiveFrameCallbackConfiguration_Response;

typedef struct {
	TFPMessageHeader header;
	uint8_t channel;
	uint8_t buffer;
	uint8_t frame_status;
	uint32_t frame;
	uint8_t age;
} __attribute__((__packed__)) ReceiveFrame_Callback;

typedef struct {
	TFPMessageHeader header;
	uint8_t channel;
	uint32_t frame;
} __attribute__((__packed__)) WriteFrameDirect;

typedef struct {
	TFPMessageHeader header;
	uint16_t frame_index;
	uint32_t frame;
} __attribute__((__packed__)) WriteFrameScheduled;

typedef struct {
	TFPMessageHeader header;
	uint8_t channel;
	uint16_t slot_index;
	uint8_t job;
	uint16_t frame_index;
	uint8_t dwell_time;
} __attribute__((__packed__)) SetScheduleEntry;

typedef struct {
	TFPMessageHeader header;
	uint8_t channel;
	uint16_t slot_index;
} __attribute__((__packed__)) GetScheduleEntry;

typedef struct {
	TFPMessageHeader header;
	uint8_t job;
	uint16_t frame_index;
	uint32_t frame;
	uint8_t dwell_time;
} __attribute__((__packed__)) GetScheduleEntry_Response;

typedef struct {
	TFPMessageHeader header;
	uint8_t channel;
	uint16_t slot_index_first;
	uint16_t slot_index_last;
} __attribute__((__packed__)) ClearScheduleEntries;


// Function prototypes
BootloaderHandleMessageResponse debug_get_discretes(const DebugGetDiscretes *data, DebugGetDiscretes_Response *response);
BootloaderHandleMessageResponse debug_read_register_low_level(const DebugReadRegisterLowLevel *data, DebugReadRegisterLowLevel_Response *response);
BootloaderHandleMessageResponse debug_write_register_low_level(const DebugWriteRegisterLowLevel *data, DebugWriteRegisterLowLevel_Response *response);
BootloaderHandleMessageResponse get_capabilities(const GetCapabilities *data, GetCapabilities_Response *response);
BootloaderHandleMessageResponse set_heartbeat_callback_configuration(const SetHeartbeatCallbackConfiguration *data);
BootloaderHandleMessageResponse get_heartbeat_callback_configuration(const GetHeartbeatCallbackConfiguration *data, GetHeartbeatCallbackConfiguration_Response *response);
BootloaderHandleMessageResponse set_channel_configuration(const SetChannelConfiguration *data);
BootloaderHandleMessageResponse get_channel_configuration(const GetChannelConfiguration *data, GetChannelConfiguration_Response *response);
BootloaderHandleMessageResponse set_channel_mode(const SetChannelMode *data);
BootloaderHandleMessageResponse get_channel_mode(const GetChannelMode *data, GetChannelMode_Response *response);
BootloaderHandleMessageResponse clear_prio_labels(const ClearPrioLabels *data);
BootloaderHandleMessageResponse set_prio_labels(const SetPrioLabels *data);
BootloaderHandleMessageResponse get_prio_labels(const GetPrioLabels *data, GetPrioLabels_Response *response);
BootloaderHandleMessageResponse clear_rx_labels(const ClearRXLabels *data);
BootloaderHandleMessageResponse set_rx_label_configuration(const SetRXLabelConfiguration *data);
BootloaderHandleMessageResponse get_rx_label_configuration(const GetRXLabelConfiguration *data, GetRXLabelConfiguration_Response *response);
BootloaderHandleMessageResponse read_next_frame(const ReadNextFrame *data, ReadNextFrame_Response *response);
BootloaderHandleMessageResponse set_receive_frame_callback_configuration(const SetReceiveFrameCallbackConfiguration *data);
BootloaderHandleMessageResponse get_receive_frame_callback_configuration(const GetReceiveFrameCallbackConfiguration *data, GetReceiveFrameCallbackConfiguration_Response *response);
BootloaderHandleMessageResponse write_frame_direct(const WriteFrameDirect *data);
BootloaderHandleMessageResponse write_frame_scheduled(const WriteFrameScheduled *data);
BootloaderHandleMessageResponse set_schedule_entry(const SetScheduleEntry *data);
BootloaderHandleMessageResponse get_schedule_entry(const GetScheduleEntry *data, GetScheduleEntry_Response *response);
BootloaderHandleMessageResponse clear_schedule_entries(const ClearScheduleEntries *data);

// Callbacks
bool handle_heartbeat_callback(void);
bool handle_receive_frame_callback(void);

#define COMMUNICATION_CALLBACK_TICK_WAIT_MS 1
#define COMMUNICATION_CALLBACK_HANDLER_NUM 2
#define COMMUNICATION_CALLBACK_LIST_INIT \
	handle_heartbeat_callback, \
	handle_receive_frame_callback, \


#endif
