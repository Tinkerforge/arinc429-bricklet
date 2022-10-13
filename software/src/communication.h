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

#define ARINC429_CHANNEL_RX               32  // all RX channels
#define ARINC429_CHANNEL_RX1              33  // RX channel #1
#define ARINC429_CHANNEL_RX2              34  // RX channel #2

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

#define ARINC429_STATUS_NEW                0  // new     frame received (or 1st frame after a timeout)
#define ARINC429_STATUS_UPDATE             1  // updated frame received
#define ARINC429_STATUS_TIMEOUT            2  // frame is overdue (frame data are last data received)
#define ARINC429_STATUS_SCHEDULER          3  // scheduler message
#define ARINC429_STATUS_STATISTICS         4  // scheduler message

#define ARINC429_SCHEDULER_JOB_SKIP        0  // scheduler job code for for an unused task table entry
#define ARINC429_SCHEDULER_JOB_CALLBACK    1  // scheduler job code for sending a callback
#define ARINC429_SCHEDULER_JOB_STOP        2  // scheduler job code for stopping the scheduler
#define ARINC429_SCHEDULER_JOB_JUMP        3  // scheduler job code for jumping to a given job index position
#define ARINC429_SCHEDULER_JOB_RETURN      4  // scheduler job code for returning to the job index position following the last jump command
#define ARINC429_SCHEDULER_JOB_DWELL       5  // scheduler job code for 'dwell' : no transmit, only dwell
#define ARINC429_SCHEDULER_JOB_SINGLE      6  // scheduler job code for 'single': send frame once       and dwell
#define ARINC429_SCHEDULER_JOB_CYCLIC      7  // scheduler job code for 'cyclic': send frame repeatedly and dwell
#define ARINC429_SCHEDULER_JOB_RETRANS_RX1 8  // scheduler job code for retransmission of a a frame received on RX1
#define ARINC429_SCHEDULER_JOB_RETRANS_RX2 9  // scheduler job code for retransmission of a a frame received on RX2

#define ARINC429_TX_MODE_TRANSMIT          0  // transmit the frame / trigger a new single transmit | keep in line with ARINC429_SET   (enable  TX)
#define ARINC429_TX_MODE_MUTE              1  // do not transmit the frame                          | keep in line with ARINC429_CLEAR (disable TX)


// internal parameters encoding

#define ARINC429_A429_MODE_NORMAL          0  // high-level A429 operations are executed
#define ARINC429_A429_MODE_DEBUG           1  // high-level A429 operations are stopped for low-level debug access

#define ARINC429_CALLBACK_JOB_NONE         0  // callback job code for 'nothing to do'
#define ARINC429_CALLBACK_JOB_STATS_TX1    1  // callback job code for a TX statistics event
#define ARINC429_CALLBACK_JOB_STATS_RX1    2  // callback job code for a RX statistics event, bit 0 = 0 -> RX channel 1
#define ARINC429_CALLBACK_JOB_STATS_RX2    3  // callback job code for a RX statistics event, bit 0 = 1 -> RX channel 2
#define ARINC429_CALLBACK_JOB_NEW_RX1      4  // callback job code for a RX new frame  event, bit 0 = 0 -> RX channel 1
#define ARINC429_CALLBACK_JOB_NEW_RX2      5  // callback job code for a RX new frame  event, bit 0 = 1 -> RX channel 2
#define ARINC429_CALLBACK_JOB_FRAME_RX1    6  // callback job code for a RX new frame  event, bit 0 = 0 -> RX channel 1
#define ARINC429_CALLBACK_JOB_FRAME_RX2    7  // callback job code for a RX new frame  event, bit 0 = 1 -> RX channel 2
#define ARINC429_CALLBACK_JOB_TIMEOUT_RX1  8  // callback job code for a RX timeout    event, bit 0 = 0 -> RX channel 1
#define ARINC429_CALLBACK_JOB_TIMEOUT_RX2  9  // callback job code for a RX timeout    event, bit 0 = 1 -> RX channel 2
#define ARINC429_CALLBACK_JOB_SCHEDULER_CB 10 // callback job code for a TX scheduler  event

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

#define FID_GET_CAPABILITIES                          1
#define FID_SET_HEARTBEAT_CALLBACK_CONFIGURATION      2
#define FID_GET_HEARTBEAT_CALLBACK_CONFIGURATION      3
#define FID_CALLBACK_HEARTBEAT                        4
#define FID_SET_CHANNEL_CONFIGURATION                 5
#define FID_GET_CHANNEL_CONFIGURATION                 6
#define FID_SET_CHANNEL_MODE                          7
#define FID_GET_CHANNEL_MODE                          8
#define FID_CLEAR_ALL_RX_FILTERS                      9
#define FID_CLEAR_RX_FILTER                          10
#define FID_SET_RX_STANDARD_FILTERS                  11
#define FID_SET_RX_FILTER                            12
#define FID_GET_RX_FILTER                            13
#define FID_READ_FRAME                               14
#define FID_SET_RECEIVE_CALLBACK_CONFIGURATION       15
#define FID_GET_RECEIVE_CALLBACK_CONFIGURATION       16
#define FID_CALLBACK_FRAME_MESSAGE                   17
#define FID_WRITE_FRAME_DIRECT                       18
#define FID_WRITE_FRAME_SCHEDULED                    19
#define FID_CLEAR_SCHEDULE_ENTRIES                   20
#define FID_SET_SCHEDULE_ENTRY                       21
#define FID_GET_SCHEDULE_ENTRY                       22
#define FID_RESTART                                  23
#define FID_CALLBACK_SCHEDULER_MESSAGE               24
#define FID_SET_FRAME_MODE                           25


/****************************************************************************/
/* DATA STRUCTURES                                                          */
/****************************************************************************/

/*** input & output data structures ***/

// get_capabilities()
typedef struct {
	TFPMessageHeader  header;                   // message header
} __attribute__((__packed__)) GetCapabilities;

typedef struct {
	TFPMessageHeader  header;                   // message header
	uint16_t          tx_total_scheduler_jobs;  // total number of  TX scheduler job entries
	uint16_t          tx_used_scheduler_jobs;   // number of unused TX scheduler job entries
	uint16_t          rx_total_frame_filters;   // total number of  RX frame buffers
	uint16_t          rx_used_frame_filters[2]; // number of unused RX frame buffers
} __attribute__((__packed__)) GetCapabilities_Response;


// set_heartbeat_callback_configuration()
typedef struct {
	TFPMessageHeader  header;                 // message header
	uint8_t           channel;                // selected channel
	bool              enabled;                // callback enabled / disabled
	bool              value_has_to_change;    // callback on changed frame data only
	uint16_t          period;                 // period setting
} __attribute__((__packed__)) SetHeartbeatCallbackConfiguration;


// get_heartbeat_callback_configuration()
typedef struct {
	TFPMessageHeader  header;                 // message header
	uint8_t           channel;                // selected channel
} __attribute__((__packed__)) GetHeartbeatCallbackConfiguration;

typedef struct {
	TFPMessageHeader  header;                 // message header
	bool              enabled;                // callback enabled / disabled
	bool              value_has_to_change;    // callback on changed frame data only
	uint16_t          period;                 // period setting
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
	uint16_t          job_index;              // index number in job table
	uint8_t           job;                    // assigned job
	uint16_t          frame_index;            // index number in frame table selecting frame to send
	uint8_t           dwell_time;             // time in ms to wait before executing the next job
} __attribute__((__packed__)) SetScheduleEntry;


// clear_schedule_entries()
typedef struct {
	TFPMessageHeader  header;                 // message header
	uint8_t           channel;                // selected channel
	uint16_t          job_index_first;        // first schedule entry by index number to be cleared
	uint16_t          job_index_last;         // last  schedule entry by index number to be cleared
} __attribute__((__packed__)) ClearScheduleEntries;


// get_schedule_entry()
typedef struct {
	TFPMessageHeader  header;                 // message header
	uint8_t           channel;                // selected channel: ARINC429_CHANNEL_TX1
	uint16_t          job_index;              // index number in job table
} __attribute__((__packed__)) GetScheduleEntry;

typedef struct {
	TFPMessageHeader  header;                 // message header
	uint8_t           job;                    // assigned job
	uint16_t          frame_index;            // index number in frame table selecting frame to send
	uint32_t          frame;                  // complete A429 frame sent (data and label)
	uint8_t           dwell_time;             // time in ms waited before executing the next job
} __attribute__((__packed__)) GetScheduleEntry_Response;


// restart()
typedef struct {
	TFPMessageHeader  header;                 // message header
} __attribute__((__packed__)) Restart;


// set_frame_mode()
typedef struct {
	TFPMessageHeader  header;                 // message header
	uint8_t           channel;                // selected channel: ARINC429_CHANNEL_TX1
	uint16_t          frame_index;            // index number in frame table selecting frame to send
	uint8_t           mode;                   // selected TX mode
} __attribute__((__packed__)) SetFrameMode;


/*** output data structures - callbacks ***/

// bricklet heartbeat callback
typedef struct {
	TFPMessageHeader  header;                 // message header
	uint8_t           channel;                // channel this heartbeat is valid for
	uint8_t           status;                 // reason for the callback: ARINC429_STATUS_STATISTICS
	uint8_t           seq_number;             // sequence number of the heartbeat message
	uint16_t          timestamp;              // time of message creation
	uint16_t          frames_processed;       // statistics counter - processed frames
	uint16_t          frames_lost;            // statistics counter - lost      frames
} __attribute__((__packed__)) Heartbeat_Callback;


// frame message callback
typedef struct {
	TFPMessageHeader  header;                 // message header
	uint8_t           channel;                // channel on which the frame was received
	uint8_t           status;                 // reason for the callback: ARINC429_STATUS_NEW, _UPDATE, _TIMEOUT
	uint8_t           seq_number;             // sequence number of the rx callback message
	uint16_t          timestamp;              // time of message creation
	uint32_t          frame;                  // complete A429 frame received (data and label) 
	uint16_t          age;                    // time elapsed since last reception of a frame with this label and SDI, in [ms]
} __attribute__((__packed__)) Frame_Callback;


// scheduler message callback
typedef struct {
	TFPMessageHeader  header;                 // message header
	uint8_t           channel;                // channel on which the callback was generated
	uint8_t           status;                 // reason for the callback: ARINC429_STATUS_SCHEDULER
	uint8_t           seq_number;             // sequence number of the rx callback message
	uint16_t          timestamp;              // time of message creation
	uint8_t           userdata;               // user data from scheduler callback job
} __attribute__((__packed__)) Scheduler_Callback;


/****************************************************************************/
/* PROTOTYPES                                                               */
/****************************************************************************/

/*** function prototypes - internal functions ***/

bool check_sw_filter_map(uint8_t channel_index, uint16_t ext_label);
bool enqueue_message    (uint8_t message_type,  uint16_t timestamp, uint32_t frame, uint16_t age_token);


/*** function prototypes - API ***/

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

BootloaderHandleMessageResponse restart                             (const Restart                           *data                                                      );

BootloaderHandleMessageResponse set_frame_mode                      (const SetFrameMode                      *data                                                      );


/*** function prototypes - callbacks ***/

bool handle_callbacks(void);

#define COMMUNICATION_CALLBACK_TICK_WAIT_MS  1
#define COMMUNICATION_CALLBACK_HANDLER_NUM   1

#define COMMUNICATION_CALLBACK_LIST_INIT \
	handle_callbacks, \


#endif  // COMMUNICATION_H

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
