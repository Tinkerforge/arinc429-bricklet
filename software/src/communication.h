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


/****************************************************************************/
/* PROTOTYPES                                                               */
/****************************************************************************/

BootloaderHandleMessageResponse  handle_message(const void *data, void *response);

void communication_tick(void);
void communication_init(void);


/****************************************************************************/
/* DEFINES                                                                  */
/****************************************************************************/

// return / error codes

#define ARINC429_RW_ERROR_OK               0  // operation executed successfully
#define ARINC429_RW_ERROR_NO_WRITE         1  // error: opcode invalid for write access
#define ARINC429_RW_ERROR_NO_READ          2  // error: opcode invalid for read  access
#define ARINC429_RW_ERROR_INVALID_OP_CODE  3  // error: opcode invalid (undefined)
#define ARINC429_RW_ERROR_INVALID_LENGTH   4  // error: invalid number of data bytes
#define ARINC429_RW_ERROR_SPI              5  // error: SPI communication failed


// user parameter encodings

#define ARINC429_CHANNEL_TX                0  // all TX channels
#define ARINC429_CHANNEL_TX1               1  // TX channel #1
#define ARINC429_CHANNEL_TX2               2  // not used
#define ARINC429_CHANNEL_TX3               3  // ...
#define ARINC429_CHANNEL_TX4               4  // ...
#define ARINC429_CHANNEL_TX5               5  // ...
#define ARINC429_CHANNEL_TX6               6  // ...
#define ARINC429_CHANNEL_TX7               7  // ...
#define ARINC429_CHANNEL_TX8               8  // ...
#define ARINC429_CHANNEL_TX9               9  // ...
#define ARINC429_CHANNEL_TX10             10  // ...
#define ARINC429_CHANNEL_TX11             11  // ...
#define ARINC429_CHANNEL_TX12             12  // ...
#define ARINC429_CHANNEL_RX               32  // all TX channels
#define ARINC429_CHANNEL_RX1              33  // RX channel #1
#define ARINC429_CHANNEL_RX2              34  // RX channel #2
#define ARINC429_CHANNEL_RX3              35  // not used
#define ARINC429_CHANNEL_RX4              36  // ...
#define ARINC429_CHANNEL_RX5              37  // ...
#define ARINC429_CHANNEL_RX6              38  // ...
#define ARINC429_CHANNEL_RX7              39  // ...
#define ARINC429_CHANNEL_RX8              40  // ...
#define ARINC429_CHANNEL_RX9              41  // ...
#define ARINC429_CHANNEL_RX10             42  // ...
#define ARINC429_CHANNEL_RX11             43  // ...
#define ARINC429_CHANNEL_RX12             44  // ...

#define ARINC429_SDI0                      0  // SDI bits used for address extension, SDI 0
#define ARINC429_SDI1                      1  // SDI bits used for address extension, SDI 1
#define ARINC429_SDI2                      2  // SDI bits used for address extension, SDI 2
#define ARINC429_SDI3                      3  // SDI bits used for address extension, SDI 3
#define ARINC429_SDI_DATA                  4  // SDI bits used for data

#define ARINC429_PARITY_DATA               0  // parity bit is used for data or parity provided by user
#define ARINC429_PARITY_AUTO               1  // parity bit is set automatically

#define ARINC429_SPEED_HS                  0  // wire speed is high-speed
#define ARINC429_SPEED_LS                  1  // wire speed is  low-speed

#define ARINC429_CHANNEL_MODE_PASSIVE      0  // initialized, but inactive (output stage of TX channels in HI-Z)
#define ARINC429_CHANNEL_MODE_ACTIVE       1  // initialized, ready to receive (RX channels) / ready for direct transmit (TX channels)
#define ARINC429_CHANNEL_MODE_RUN          2  // TX channels only: active and scheduler running

#define ARINC429_PRIORITY_DISABLED         0  // RX priority buffers disabled
#define ARINC429_PRIORITY_ENABLED          1  // RX priority buffers enabled

#define ARINC429_FRAME_STATUS_UPDATE       0  // new or updated frame received
#define ARINC429_FRAME_STATUS_TIMEOUT      1  // frame is overdue (frame data are last data received)

#define ARINC429_SCHEDULER_JOB_SKIP        0  // scheduler job code for 'skip'  : no transmit, no   dwell
#define ARINC429_SCHEDULER_JOB_DWELL       1  // scheduler job code for 'dwell' : no transmit, only dwell
#define ARINC429_SCHEDULER_JOB_SINGLE      2  // scheduler job code for 'single': send frame once       and dwell
#define ARINC429_SCHEDULER_JOB_CYCLIC      3  // scheduler job code for 'cyclic': send frame repeatedly and dwell
#define ARINC429_SCHEDULER_JOB_RETRANS_RX1 4 // scheduler job code for retransmission of a a frame received on RX1
#define ARINC429_SCHEDULER_JOB_RETRANS_RX2 5 // scheduler job code for retransmission of a a frame received on RX2

#define ARINC429_A429_MODE_NORMAL          0  // high-level A429 operations are executed
#define ARINC429_A429_MODE_DEBUG           1  // high-level A429 operations are stopped for low-level debug access

#define ARINC429_CALLBACK_JOB_NONE         0  // callback job code for 'nothing to do'
#define ARINC429_CALLBACK_JOB_HEARTBEAT    1  // callback job code for 'heartbeat'
#define ARINC429_CALLBACK_JOB_FRAME_RX1    2  // callback job code for 'new frame', bit 0 = 0 -> channel 1
#define ARINC429_CALLBACK_JOB_FRAME_RX2    3  // callback job code for 'new frame', bit 0 = 1 -> channel 2
#define ARINC429_CALLBACK_JOB_TIMEOUT_RX1  4  // callback job code for 'timeout',   bit 0 = 0 -> channel 1
#define ARINC429_CALLBACK_JOB_TIMEOUT_RX2  5  // callback job code for 'timeout',   bit 0 = 0 -> channel 2

#define ARINC429_CALLBACK_OFF              0  // callback disabled
#define ARINC429_CALLBACK_ON               1  // callback enabled
#define ARINC429_CALLBACK_ON_CHANGE        2  // callback enabled, on change only


// system parameter encodings

#define ARINC429_BOOTLOADER_MODE_BOOTLOADER                         0
#define ARINC429_BOOTLOADER_MODE_FIRMWARE                           1
#define ARINC429_BOOTLOADER_MODE_BOOTLOADER_WAIT_FOR_REBOOT         2
#define ARINC429_BOOTLOADER_MODE_FIRMWARE_WAIT_FOR_REBOOT           3
#define ARINC429_BOOTLOADER_MODE_FIRMWARE_WAIT_FOR_ERASE_AND_REBOOT 4

#define ARINC429_BOOTLOADER_STATUS_OK                               0
#define ARINC429_BOOTLOADER_STATUS_INVALID_MODE                     1
#define ARINC429_BOOTLOADER_STATUS_NO_CHANGE                        2
#define ARINC429_BOOTLOADER_STATUS_ENTRY_FUNCTION_NOT_PRESENT       3
#define ARINC429_BOOTLOADER_STATUS_DEVICE_IDENTIFIER_INCORRECT      4
#define ARINC429_BOOTLOADER_STATUS_CRC_MISMATCH                     5

#define ARINC429_STATUS_LED_CONFIG_OFF                              0
#define ARINC429_STATUS_LED_CONFIG_ON                               1
#define ARINC429_STATUS_LED_CONFIG_SHOW_HEARTBEAT                   2
#define ARINC429_STATUS_LED_CONFIG_SHOW_STATUS                      3


// function and callback IDs

#define FID_DEBUG_GET_DISCRETES                       1
#define FID_DEBUG_READ_REGISTER_LOW_LEVEL             2
#define FID_DEBUG_WRITE_REGISTER_LOW_LEVEL            3

#define FID_GET_CAPABILITIES                          4
#define FID_SET_HEARTBEAT_CALLBACK_CONFIGURATION      5
#define FID_GET_HEARTBEAT_CALLBACK_CONFIGURATION      6
#define FID_CALLBACK_HEARTBEAT                        7
#define FID_SET_CHANNEL_CONFIGURATION                 8
#define FID_GET_CHANNEL_CONFIGURATION                 9
#define FID_SET_CHANNEL_MODE                         10
#define FID_GET_CHANNEL_MODE                         11
#define FID_CLEAR_ALL_RX_FILTERS                     12
#define FID_CLEAR_RX_FILTER                          13
#define FID_SET_RX_STANDARD_FILTERS                  14
#define FID_SET_RX_FILTER                            15
#define FID_GET_RX_FILTER                            16
#define FID_READ_FRAME                               17
#define FID_SET_RECEIVE_CALLBACK_CONFIGURATION       18
#define FID_GET_RECEIVE_CALLBACK_CONFIGURATION       19
#define FID_CALLBACK_FRAME_MESSAGE                   20
#define FID_WRITE_FRAME_DIRECT                       21
#define FID_WRITE_FRAME_SCHEDULED                    22
#define FID_CLEAR_SCHEDULE_ENTRIES                   23
#define FID_SET_SCHEDULE_ENTRY                       24
#define FID_GET_SCHEDULE_ENTRY                       25
#define FID_RESET_A429                               26


/****************************************************************************/
/* DATA STRUCTURES                                                          */
/****************************************************************************/

/*** input & output data structures - debug level functions ***/

// debug_get_discretes()
typedef struct {
	TFPMessageHeader  header;                 // message header
} __attribute__((__packed__)) DebugGetDiscretes;

typedef struct {
	TFPMessageHeader  header;                 // message header
	uint16_t          rx_discretes;           // RX related discretes
	uint16_t          tx_discretes;           // TX related discretes
} __attribute__((__packed__)) DebugGetDiscretes_Response;


// debug_read_register_low_level()
typedef struct {
	TFPMessageHeader  header;                 // message header
	uint8_t           op_code;                // SPI command opcode
} __attribute__((__packed__)) DebugReadRegisterLowLevel;

typedef struct {
	TFPMessageHeader  header;                 // message header
	uint8_t           value_length;           // number of bytes in value_data[]
	uint8_t           value_data[32];         // bytes read from A429 chip
	uint8_t           rw_error;               // SPI return / error code
} __attribute__((__packed__)) DebugReadRegisterLowLevel_Response;


// debug_write_register_low_level()
typedef struct {
	TFPMessageHeader  header;                 // message header
	uint8_t           op_code;                // SPI command code
	uint8_t           value_length;           // number of bytes to write
	uint8_t           value_data[32];         // bytes to write
} __attribute__((__packed__)) DebugWriteRegisterLowLevel;

typedef struct {
	TFPMessageHeader  header;                 // message header
	uint8_t           rw_error;               // SPI return / error code
} __attribute__((__packed__)) DebugWriteRegisterLowLevel_Response;


/*** input & output data structures - user level functions ***/

// get_capabilities()
typedef struct {
	TFPMessageHeader  header;                 // message header
} __attribute__((__packed__)) GetCapabilities;

typedef struct {
	TFPMessageHeader  header;                   // message header
	uint16_t          tx_total_scheduler_tasks; // total number of  TX scheduler task entries
	uint16_t          tx_used_scheduler_tasks;  // number of unused TX scheduler task entries
	uint16_t          rx_total_frame_filters;   // total number of  RX frame buffers
	uint16_t          rx_used_frame_filters[2]; // number of unused RX frame buffers
} __attribute__((__packed__)) GetCapabilities_Response;


// set_heartbeat_callback_configuration()
typedef struct {
	TFPMessageHeader  header;                 // message header
	uint8_t           period;                 // heartbeat period
	bool              value_has_to_change;    // delay heartbeat in case values have not changed
} __attribute__((__packed__)) SetHeartbeatCallbackConfiguration;


// get_heartbeat_callback_configuration()
typedef struct {
	TFPMessageHeader  header;                 // message header
} __attribute__((__packed__)) GetHeartbeatCallbackConfiguration;

typedef struct {
	TFPMessageHeader  header;                 // message header
	uint8_t           period;                 // heartbeat period
	bool              value_has_to_change;    // heartbeat is delayed in case values have not changed
} __attribute__((__packed__)) GetHeartbeatCallbackConfiguration_Response;


// set_channel_configuration()
typedef struct {
	TFPMessageHeader  header;                 // message header
	uint8_t           channel;                // selected channel
	uint8_t           parity;                 // parity mode
	uint8_t           speed;                  // speed  mode
} __attribute__((__packed__)) SetChannelConfiguration;


// get_channel_configuration()
typedef struct {
	TFPMessageHeader  header;                 // message header
	uint8_t           channel;                // selected channel
} __attribute__((__packed__)) GetChannelConfiguration;

typedef struct {
	TFPMessageHeader  header;                 // message header
	uint8_t           parity;                 // parity mode
	uint8_t           speed;                  // speed  mode
} __attribute__((__packed__)) GetChannelConfiguration_Response;


// set_channel_mode()
typedef struct {
	TFPMessageHeader  header;                 // message header
	uint8_t           channel;                // selected channel
	uint8_t           mode;                   // channel mode
} __attribute__((__packed__)) SetChannelMode;


// get_channel_mode()
typedef struct {
	TFPMessageHeader  header;                 // message header
	uint8_t           channel;                // selected channel
} __attribute__((__packed__)) GetChannelMode;

typedef struct {
	TFPMessageHeader  header;                 // message header
	uint8_t           mode;                   // channel mode
} __attribute__((__packed__)) GetChannelMode_Response;


// clear_all_rx_filters()
typedef struct {
	TFPMessageHeader  header;                 // message header
	uint8_t           channel;                // selected channel
} __attribute__((__packed__)) ClearAllRXLabelFilters;


// clear_rx_filter()
typedef struct {
	TFPMessageHeader  header;                 // message header
	uint8_t           channel;                // selected channel
	uint8_t           label;                  // label code
	uint8_t           sdi;                    // use of SDI bits
} __attribute__((__packed__)) ClearRXLabelFilter;

typedef struct {
	TFPMessageHeader  header;                 // message header
	bool              success;                // filter removed true/false
} __attribute__((__packed__)) ClearRXLabelFilter_Response;


// set_rx_standard_filters()
typedef struct {
	TFPMessageHeader  header;                 // message header
	uint8_t           channel;                // selected channel
} __attribute__((__packed__)) SetRXStandardFilters;


// set_rx_filter()
typedef struct {
	TFPMessageHeader  header;                 // message header
	uint8_t           channel;                // selected channel
	uint8_t           label;                  // label code
	uint8_t           sdi;                    // use of SDI bits
} __attribute__((__packed__)) SetRXFilter;

typedef struct {
	TFPMessageHeader  header;                 // message header
	bool              success;                // filter created true/false
} __attribute__((__packed__)) SetRXFilter_Response;


// get_rx_filter()
typedef struct {
	TFPMessageHeader  header;                 // message header
	uint8_t           channel;                // selected channel
	uint8_t           label;                  // label code
	uint8_t           sdi;                    // use of SDI bits
} __attribute__((__packed__)) GetRXFilter;

typedef struct {
	TFPMessageHeader  header;                 // message header
	bool              configured;             // filter configured true/false
} __attribute__((__packed__)) GetRXFilter_Response;


// read_frame()
typedef struct {
	TFPMessageHeader  header;                 // message header
	uint8_t           channel;                // selected channel
	uint8_t           label;                  // label code
	uint8_t           sdi;                    // use of SDI bits
} __attribute__((__packed__)) ReadFrame;

typedef struct {
	TFPMessageHeader  header;                 // message header
	bool              status;                 // frame available
	uint32_t          frame;                  // frame data in case status == TRUE
	uint16_t          age;                    // frame age [ms]
} __attribute__((__packed__)) ReadFrame_Response;


// set_rx_callback_configuration()
typedef struct {
	TFPMessageHeader  header;                 // message header
	uint8_t           channel;                // selected channel
	bool              enabled;                // callback enabled / disabled
	bool              value_has_to_change;    // callback on changed frame data only
	uint16_t          timeout;                // timeout setting
} __attribute__((__packed__)) SetRXCallbackConfiguration;


// get_rx_callback_configuration()
typedef struct {
	TFPMessageHeader  header;                 // message header
	uint8_t           channel;                // selected channel
} __attribute__((__packed__)) GetRXCallbackConfiguration;

typedef struct {
	TFPMessageHeader  header;                 // message header
	bool              enabled;                // callback enabled / disabled
	bool              value_has_to_change;    // callback on changed frame data only
	uint16_t          timeout;                // timeout setting
} __attribute__((__packed__)) GetRXCallbackConfiguration_Response;


// write_frame_direct()
typedef struct {
	TFPMessageHeader  header;                 // message header
	uint8_t           channel;                // selected channel
	uint32_t          frame;                  // complete A429 frame (data and label)
} __attribute__((__packed__)) WriteFrameDirect;


// write_frame_scheduled()
typedef struct {
	TFPMessageHeader  header;                 // message header
	uint8_t           channel;                // selected channel
	uint16_t          frame_index;            // index position referenced in scheduler job entry
	uint32_t          frame;                  // complete A429 frame (data and label)
} __attribute__((__packed__)) WriteFrameScheduled;


// set_schedule_entry()
typedef struct {
	TFPMessageHeader  header;                 // message header
	uint8_t           channel;                // selected channel
	uint16_t          task_index;             // index number in job table
	uint8_t           job;                    // assigned job
	uint16_t          frame_index;            // index number in frame table selecting frame to send
	uint8_t           dwell_time;             // time in ms to wait before executing the next job
} __attribute__((__packed__)) SetScheduleEntry;


// clear_schedule_entries()
typedef struct {
	TFPMessageHeader  header;                 // message header
	uint8_t           channel;                // selected channel
	uint16_t          task_index_first;       // first schedule entry by index number to be cleared
	uint16_t          task_index_last;        // last  schedule entry by index number to be cleared
} __attribute__((__packed__)) ClearScheduleEntries;


// get_schedule_entry()
typedef struct {
	TFPMessageHeader  header;                 // message header
	uint8_t           channel;                // selected channel: ARINC429_CHANNEL_TX1
	uint16_t          task_index;             // index number in job table
} __attribute__((__packed__)) GetScheduleEntry;

typedef struct {
	TFPMessageHeader  header;                 // message header
	uint8_t           job;                    // assigned job
	uint16_t          frame_index;            // index number in frame table selecting frame to send
	uint32_t          frame;                  // complete A429 frame sent (data and label)
	uint8_t           dwell_time;             // time in ms waited before executing the next job
} __attribute__((__packed__)) GetScheduleEntry_Response;


// reset()
typedef struct {
	TFPMessageHeader  header;                 // message header
	uint8_t           mode;                   // A429 operations mode
} __attribute__((__packed__)) Reset_A429;


/*** output data structures - callbacks ***/

// bricklet heartbeat callback
#define TCN           3                       // total number of channels - copy of ARINC429_CHANNEL_TOTAL_NUM from arinc429.h
typedef struct {
	TFPMessageHeader  header;                 // message header
	uint8_t           seq_number;             // sequence number of the heartbeat message
	uint16_t          timestamp;              // time of message creation
	uint16_t          frames_processed[TCN];  // statistics counter - processed frames
	uint16_t          frames_lost[TCN];       // statistics counter - lost      frames
} __attribute__((__packed__)) Heartbeat_Callback;


// frame message callback
typedef struct {
	TFPMessageHeader  header;                 // message header
	uint8_t           channel;                // channel on which the frame was received
	uint8_t           seq_number;             // sequence number of the rx callback message
	uint16_t          timestamp;              // time of message creation
	uint8_t           frame_status;           // reason for callback: ARINC429_FRAME_STATUS_TIMEOUT or ARINC429_FRAME_STATUS_UPDATE
	uint32_t          frame;                  // complete A429 frame received (data and label) 
	uint16_t          age;                    // time elapsed since last reception of a frame with this label and SDI, in [ms]
} __attribute__((__packed__)) Frame_Callback;


/****************************************************************************/
/* PROTOTYPES                                                               */
/****************************************************************************/

/*** function prototypes - internal functions ***/

bool check_sw_filter_map(uint8_t channel_index, uint16_t ext_label);
bool enqueue_message    (uint8_t message_type,  uint16_t timestamp, uint8_t buffer);


/*** function prototypes - debug level ***/

BootloaderHandleMessageResponse debug_get_discretes                 (const DebugGetDiscretes                 *data, DebugGetDiscretes_Response                 *response);
BootloaderHandleMessageResponse debug_read_register_low_level       (const DebugReadRegisterLowLevel         *data, DebugReadRegisterLowLevel_Response         *response);
BootloaderHandleMessageResponse debug_write_register_low_level      (const DebugWriteRegisterLowLevel        *data, DebugWriteRegisterLowLevel_Response        *response);


/*** function prototypes - user level ***/

BootloaderHandleMessageResponse get_capabilities                    (const GetCapabilities                   *data, GetCapabilities_Response                   *response);

BootloaderHandleMessageResponse set_heartbeat_callback_configuration(const SetHeartbeatCallbackConfiguration *data                                                      );
BootloaderHandleMessageResponse get_heartbeat_callback_configuration(const GetHeartbeatCallbackConfiguration *data, GetHeartbeatCallbackConfiguration_Response *response);

BootloaderHandleMessageResponse set_channel_configuration           (const SetChannelConfiguration           *data                                                      );
BootloaderHandleMessageResponse get_channel_configuration           (const GetChannelConfiguration           *data, GetChannelConfiguration_Response           *response);

BootloaderHandleMessageResponse set_channel_mode                    (const SetChannelMode                    *data                                                      );
BootloaderHandleMessageResponse get_channel_mode                    (const GetChannelMode                    *data, GetChannelMode_Response                    *response);

BootloaderHandleMessageResponse clear_all_rx_filters                (const ClearAllRXLabelFilters            *data                                                      );
BootloaderHandleMessageResponse clear_rx_filter                     (const ClearRXLabelFilter                *data, ClearRXLabelFilter_Response                *response);
BootloaderHandleMessageResponse set_rx_standard_filters             (const SetRXStandardFilters              *data                                                      );
BootloaderHandleMessageResponse set_rx_filter                       (const SetRXFilter                       *data, SetRXFilter_Response                       *response);
BootloaderHandleMessageResponse get_rx_filter                       (const GetRXFilter                       *data, GetRXFilter_Response                       *response);

BootloaderHandleMessageResponse read_frame                          (const ReadFrame                         *data, ReadFrame_Response                         *response);

BootloaderHandleMessageResponse set_rx_callback_configuration       (const SetRXCallbackConfiguration        *data                                                      );
BootloaderHandleMessageResponse get_rx_callback_configuration       (const GetRXCallbackConfiguration        *data, GetRXCallbackConfiguration_Response        *response);

BootloaderHandleMessageResponse write_frame_direct                  (const WriteFrameDirect                  *data                                                      );
BootloaderHandleMessageResponse write_frame_scheduled               (const WriteFrameScheduled               *data                                                      );

BootloaderHandleMessageResponse clear_schedule_entries              (const ClearScheduleEntries              *data                                                      );
BootloaderHandleMessageResponse set_schedule_entry                  (const SetScheduleEntry                  *data                                                      );
BootloaderHandleMessageResponse get_schedule_entry                  (const GetScheduleEntry                  *data, GetScheduleEntry_Response                  *response);

BootloaderHandleMessageResponse reset_a429                          (const Reset_A429                        *data                                                      );


/*** function prototypes - callbacks ***/

bool handle_callbacks(void);

#define COMMUNICATION_CALLBACK_TICK_WAIT_MS  1
#define COMMUNICATION_CALLBACK_HANDLER_NUM   1

#define COMMUNICATION_CALLBACK_LIST_INIT \
	handle_callbacks, \


#endif  // COMMUNICATION_H

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
