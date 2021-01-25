/* arinc429-bricklet
 * Copyright (C) 2020 Olaf Lüke <olaf@tinkerforge.com>
 *
 * arinc429.h: State Machine for ARINC429 
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

#ifndef ARINC429_H
#define ARINC429_H

#include <stdint.h>
#include <stdbool.h>

#define PACKED __attribute__((__packed__))


/****************************************************************************/
/* DEFINES                                                                  */
/****************************************************************************/

// bricklet capabilities
#define ARINC429_TX_CHANNELS_NUM         1                  // number of TX channels                                      ** given by hardware **
#define ARINC429_RX_CHANNELS_NUM         2                  // number of RX channels                                      ** given by hardware **
#define ARINC429_RX_FIFO_BUFFER_NUM     32                  // number of buffers in receive FIFO                          ** given by hardware **
#define ARINC429_CHANNEL_TOTAL_NUM      (ARINC429_TX_CHANNELS_NUM + ARINC429_RX_CHANNELS_NUM)

// bricklet setup
#define LED_PULSE_TIME                  10                  // pulse time of RX & TX LED                                  ##  customizable

// A429 chip setup
#define ARINC429_FLIP                    1                  // reverse the bit order of the first 8 bits of each frame    ** given by application design  **

// RX filter
#define ARINC429_RX_FILTERS_NUM          1024               // number of extended labels (label + SDI)                    ** given by application design  **
#define ARINC429_RX_BUFFER_NUM           256                // number of frame buffers                                    ## customizable, max 256        ##
#define ARINC429_RX_BUFFER_NEW           0xFFFC             // value in frame_buffer[].frame_age for a frame after timeout** given by application design  **
#define ARINC429_RX_BUFFER_TIMEOUT       0xFFFD             // value in frame_buffer[].frame_age for a timeout            ** given by application design  **
#define ARINC429_RX_BUFFER_EMPTY         0xFFFE             // value in frame_buffer[].frame_age for empty  buffers       ** given by application design  **
#define ARINC429_RX_BUFFER_UNUSED        0xFFFF             // value in frame_buffer[].frame_age for unused buffers       ** given by application design  **
#define ARINC429_RX_FRAME_LABEL_MASK     0x000000FF         // mask for frame label                                       ** given by A429 standard       **
#define ARINC429_RX_FRAME_EXT_LABEL_MASK 0x000003FF         // mask for frame label including SDI ("extended label")      ** given by A429 standard       **
#define ARINC429_RX_TIMEOUT_SCAN_PERIOD  100                // period of RX buffer scans for timeouts [ms]                ## customizable, min 2 and even ##
#define ARINC429_RX_FRAME_BUDGET         5                  // max number of frames read per channel in one tick          ## fudge factor for performance tuning (good value:  5)
#define ARINC429_TIMEOUT_CHECK_BUDGET    10                 // number of frame buffers checked for timeout in one tick    ## fudge factor for performance tuning (goof value:  ?)

// TX scheduler
#define ARINC429_TX_JOBS_NUM             1000               // number of TX jobs                                          ## customizable, max 4096       ##
#define ARINC429_TX_BUFFER_NUM           256                // number of TX frame buffers                                 ## customizable, max 4096       ##
#define ARINC429_TX_JOB_JOBCODE_MASK     0xF000             // mask for job   code                                        ** given by application design  **
#define ARINC429_TX_JOB_INDEX_MASK       0x0FFF             // mask for frame index                                       ** given by application design  **
#define ARINC429_TX_JOB_JOBCODE_POS      12                 // LSB position of job   code                                 ** given by application design  **
#define ARINC429_TX_JOB_INDEX_POS        0                  // LSB position of frame index                                ** given by application design  **
#define ARINC429_TX_ZERO_DWELL_BUDGET    4                  // number of successive zero dwell time jobs done in one tick ## fudge factor for performance tuning (good value:  4)

// callback queue
#define ARINC429_CB_QUEUE_SIZE           300                // number of entries in the callback queue                    ## customizable, max 2^16 ##

// immediate transmit queue
#define ARINC429_TX_QUEUE_SIZE           16                 // number of entries in the immediate transmit queue          ## customizable, max 2^8  ##

// requests - system level
#define ARINC429_SYSTEM_RESET_XMC_DATA   (1 << 0)           // request reset  of the XMC  data structure
#define ARINC429_SYSTEM_RESET_XMC_CHIP   (1 << 1)           // request reset  of the XMC  chip
#define ARINC429_SYSTEM_RESET_A429_DATA  (1 << 2)           // request reset  of the A429 data structure
#define ARINC429_SYSTEM_RESET_A429_CHIP  (1 << 3)           // request reset  of the A429 chip

// requests - channel level
#define ARINC429_UPDATE_SPEED_PARITY     (1 << 0)           // request update of speed and/or parity setting
#define ARINC429_UPDATE_FIFO_FILTER      (1 << 1)           // request update of the FIFO filter
#define ARINC429_UPDATE_OPERATING_MODE   (1 << 2)           // request update of operating mode
#define ARINC429_UPDATE_CALLBACK_MODE    (1 << 3)           // request update of callback  mode

// internal encodings
#define ARINC429_SET                     0                  // set   a filter in a filter map
#define ARINC429_CLEAR                   1                  // clear a filter in a filter map


/****************************************************************************/
/* DATA STRUCTURES (--> all data structures are word-aligned to 32 bit <--) */
/****************************************************************************/

// heartbeat
typedef struct
{
	uint32_t         last_time;                             //     4 time when last heartbeat was sent
	uint16_t         period;                                //     2 heartbeat period
	uint8_t          mode;                                  //     1 mode: off, on, on change only
	uint8_t          seq_number;                            //     1 next sequence number
}                                                           // =====
PACKED ARINC429Heartbeat;                                   //     8 byte


// callback queue
typedef struct
{
	uint8_t          message;                               //     1 message type and channel id
	uint8_t          buffer;                                //     1 pointer to frame buffer or task index
	uint16_t         timestamp;                             //     2 message creation time
}                                                           //  ====
PACKED ARINC429CBQueue;                                     //     4 byte


// callback
typedef struct
{
	uint16_t         head;                                  //     2 message queue head index
	uint16_t         tail;                                  //     2 message queue tail index
	uint16_t         spare;                                 //     2 unused / for alignment purpose
	uint8_t          seq_number_frame;                      //     1 sequence number of the frame     message callback
	uint8_t          seq_number_scheduler;                  //     1 sequence number of the scheduler message callback
	ARINC429CBQueue  queue[ARINC429_CB_QUEUE_SIZE];         // 1.200 message queue (ring buffer)
}                                                           // =====
PACKED ARINC429Callback;                                    // 1.208 byte


// common config and status data for all channel types
typedef struct
{
	// operating modes
	uint8_t          parity_speed;                          //     1 parity mode upper nibble) and wire speed (lower nibble)
	uint8_t          operating_mode;                        //     1 operating mode (passive, active, run)
	uint8_t          callback_mode;                         //     1 callback  mode
	uint8_t          change_request;                        //     1 pending configuration change

	// statistics
	uint16_t         frames_processed_curr;                 //     2 statistics counter - processed frames - current       value
	uint16_t         frames_processed_last;                 //     2 statistics counter - processed frames - last reported value
	uint16_t         frames_lost_curr;                      //     2 statistics counter - dropped   frames - current       value
	uint16_t         frames_lost_last;                      //     2 statistics counter - dropped   frames - last reported value
}                                                           // =====
PACKED ARINC429Common;                                      //    12 byte


// config and status of a TX channel
typedef struct
{
	// common part
	ARINC429Common   common;                                //     12 common config and status data for all channel types

	// immediate transmit
	uint32_t         queue[ARINC429_TX_QUEUE_SIZE];         //     64 frame queue
	uint8_t          head;                                  //      1 frame queue head index
	uint8_t          tail;                                  //      1 frame queue tail index
	uint16_t         spare;                                 //      2 unused / for alignment purpose

	// scheduled transmit
	uint16_t         scheduler_jobs_used;                   //      2 number of used job entries
	uint16_t         job_index;                             //      2 index of the current job
	uint16_t         job_index_jump;                        //      2 index of the last jump job
	uint8_t          spare1;                                //      1 unused / for alignment purpose
	uint8_t          dwell_time_jump;                       //      1 dwell time of the last jump command
	uint32_t         last_job_exec_time;                    //      4 execution time of the last job
	uint32_t         last_job_dwell_time;                   //      4 dwell     time of the last job
	uint16_t         job_frame[ARINC429_TX_JOBS_NUM];       //  2.000 bits 15-12: action (mute, single, cyclic), bits 11-0: index frame[] table
	uint8_t          dwell_time[ARINC429_TX_JOBS_NUM];      //  1.000 waiting time in ms before advancing to the next job
	uint32_t         frame_buffer[ARINC429_TX_BUFFER_NUM];  //  1.024 scheduled TX frames
	uint32_t         frame_buffer_map[8];                   //     32 single transmit status tracking
}                                                           //  =====
PACKED ARINC429TXChannel;                                   //  4.072 byte


// received frame buffer
typedef struct
{
	uint32_t         frame;                                 //     4 received frame
	uint16_t         frame_age;                             //     2 frame age [ms]
	uint16_t         last_rx_time;                          //     2 time when frame was received for the last time (lower 2 byte from the system clock)
}                                                           //  ====
PACKED ARINC429RXBuffer;                                    //     8 byte


// config and status of a RX channel
typedef struct
{
	// common part
	ARINC429Common   common;                                //     12 common config and status data for all channel types

	// timeout check
	uint16_t         timeout_period;                        //      2 timeout time [ms]

	// frame buffers
	uint16_t         frame_buffers_used;                    //      2 number of used frame buffers
	ARINC429RXBuffer frame_buffer[ARINC429_RX_BUFFER_NUM];  //  2.048 frames buffers

	// software frame filters
	uint32_t         frame_filter_map[32];                  //    128 frame filter assignment table
	uint8_t          frame_filter[ARINC429_RX_FILTERS_NUM]; //  1.024 index into frame buffer table

	// hardware frame filters
	uint8_t          hardware_filter[32];                   //     32 hardware filter assignment table
}                                                           //  =====
PACKED ARINC429RXChannel;                                   //  3.248 byte


// system settings
typedef struct
{
    uint8_t           operating_mode;                       //      1 A429 operations selector
    uint8_t           change_request;                       //      1 request  for system setting changes
    uint8_t           spare1;                               //      1 unused / for alignment purpose
    uint8_t           spare2;                               //      1 unused / for alignment purpose
}                                                           //  =====
PACKED ARINC429System;                                      //      4 byte


// final combined data structure
typedef struct
{
	// channels
	ARINC429TXChannel tx_channel[ARINC429_TX_CHANNELS_NUM]; //  4.072 TX channels
	ARINC429RXChannel rx_channel[ARINC429_RX_CHANNELS_NUM]; //  6.496 RX channels

	// callbacks
	ARINC429Heartbeat heartbeat;                            //      8 bricklet heartbeat
	ARINC429Callback  callback;                             //  1.208 callback queue

	// system - Attention: needs to be placed at the end
	//                     of the ARINC429 data structure!
	ARINC429System    system;                               //      4 system settings
}                                                           // ======
PACKED ARINC429;                                            // 11.788 byte (~11.5 kByte)


/****************************************************************************/
/* PROTOTYPES                                                               */
/****************************************************************************/

extern ARINC429 arinc429;

void arinc429_tick(void);
void arinc429_tick_task(void);

void update_tx_buffer_map(uint8_t channel_index, uint16_t buffer_index, uint8_t task);
bool  check_tx_buffer_map(uint8_t channel_index, uint16_t buffer_index);

#endif  // ARINC429_H

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
