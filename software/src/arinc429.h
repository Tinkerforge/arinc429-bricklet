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
#define ARINC429_TX_CHANNELS_NUM         1                  // number of TX channels                                     ** given by hardware **
#define ARINC429_RX_CHANNELS_NUM         2                  // number of RX channels                                     ** given by hardware **
#define ARINC429_RX_FIFO_BUFFER_NUM     32                  // number of buffers in receive FIFO                         ** given by hardware **
#define ARINC429_CHANNEL_TOTAL_NUM      (ARINC429_TX_CHANNELS_NUM + ARINC429_RX_CHANNELS_NUM)

// A429 chip setup
#define ARINC429_FLIP                    1                  // reverse the bit order of the first 8 bits of each frame   ** given by application design  **

// RX filter
#define ARINC429_RX_FILTERS_NUM          1024               // number of extended labels (label + SDI)                   ** given by application design  **
#define ARINC429_RX_FILTER_UNUSED        255                // value in frame_filter[] to mark filter as unused          ** given by application design  **
#define ARINC429_RX_BUFFER_NUM           256                // number of frame buffers                                   ## customizable, max 256        ##
#define ARINC429_RX_BUFFER_TIMEOUT       0xFFFE             // value in frame_buffer[].frame_age for buffers in timeout  ** given by application design  **
#define ARINC429_RX_BUFFER_UNUSED        0xFFFF             // value in frame_buffer[].frame_age for buffers unused      ** given by application design  **
#define ARINC429_RX_FRAME_LABEL_MASK     0x000000FF         // mask for frame label                                      ** given by A429 standard       **
#define ARINC429_RX_FRAME_EXT_LABEL_MASK 0x000003FF         // mask for frame label including SDI ("extended label")     ** given by A429 standard       **
#define ARINC429_RX_TIMEOUT_SCAN_PERIOD  100                // period of RX buffer scans for timeouts [ms]               ## customizable, min 2 and even ##

// TX scheduler
#define ARINC429_TX_TASKS_NUM            512                // number of TX tasks                                        ## customizable, multiple of 4  ##
#define ARINC429_TX_BUFFER_NUM           256                // number of TX frame buffers                                ## customizable, max 4096       ##
#define ARINC429_TX_JOB_JOBCODE_MASK     0xF000             // mask for job   code                                       ** given by application design  **
#define ARINC429_TX_JOB_INDEX_MASK       0x0FFF             // mask for frame index                                      ** given by application design  **
#define ARINC429_TX_JOB_JOBCODE_POS      12                 // LSB position of job   code                                ** given by application design  **
#define ARINC429_TX_JOB_INDEX_POS        0                  // LSB position of frame index                               ** given by application design  **

// callback queue
#define ARINC429_CB_QUEUE_SIZE           256                // number of entries in the callback queue                   ## customizable, = 2^x, max 256 ##

// immediate transmit queue
#define ARINC429_TX_QUEUE_SIZE           16                 // number of entries in the immediate transmit queue         ## customizable, = 2^x, max 256 ##

// flags
#define ARINC429_CHANGE_REQUEST          (1 << 7)           // change requests are stored in bit 7 of the mode variables ** given by application design  **


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
	uint8_t          buffer;                                //     1 pointer to frame buffer
}                                                           //  ====
PACKED ARINC429CBQueue;                                     //     2 byte


// callback
typedef struct
{
	uint8_t          head;                                  //     1 message queue head index
	uint8_t          tail;                                  //     1 message queue tail index
	uint8_t          seq_number;                            //     1 sequence number of the frame message callback
	uint8_t          spare;                                 //     1 unused / for alignment purpose
	ARINC429CBQueue  queue[ARINC429_CB_QUEUE_SIZE];         //   512 message queue (ring buffer)
}                                                           // =====
PACKED ARINC429Callback;                                    //   516 byte


// common config and status data for all channel types
typedef struct
{
	// operating modes
	uint8_t          parity_speed;                          //     1 parity mode upper nibble) and wire speed (lower nibble)
	uint8_t          operating_mode;                        //     1 operating mode (passive, active, run)
	uint8_t          callback_mode;                         //     1 callback  mode
	uint8_t          pending_change;                        //     1 pending configuration change

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
	ARINC429Common   common;                                //    12 common config and status data for all channel types

	// immediate transmit
	uint32_t         queue[ARINC429_TX_QUEUE_SIZE];         //    64 frame queue
	uint8_t          head;                                  //     1 frame queue head index
	uint8_t          tail;                                  //     1 frame queue tail index

	// scheduled transmit
	uint16_t         next_job_index;                        //     2 index of next job
	uint32_t         last_job_exec_time;                    //     4 execution time of last job
	uint32_t         last_job_dwell_time;                   //     4 dwell     time of last job
	uint16_t         job_frame[ARINC429_TX_TASKS_NUM];      // 1.024 bits 15-12: action (mute, single, cyclic), bits 11-0: index frame[] table
	uint8_t          dwell_time[ARINC429_TX_TASKS_NUM];     //   512 waiting time in ms before advancing to the next job
	uint32_t         frame_buffer[ARINC429_TX_BUFFER_NUM];  // 1.024 scheduled frames table
}                                                           // =====
PACKED ARINC429TXChannel;                                   // 2.636 byte


// received frame buffer
typedef struct
{
	uint32_t         frame;                                 //    4 received frame
	uint16_t         frame_age;                             //    2 frame age [ms]
	uint16_t         last_rx_time;                          //    2 time when frame was received for the last time (lower 2 byte from the system clock)
}                                                           // ====
PACKED ARINC429RXBuffer;                                    //    8 byte


// config and status of a RX channel
typedef struct
{
	// common part
	ARINC429Common   common;                                //    12 common config and status data for all channel types

	// timed tasks
	uint32_t         last_timeout_scan;                     //     4 time of last timeout scan
	uint16_t         timeout_period;                        //     2 timeout time [ms]
	uint16_t         spare;                                 //     2 unused / for alignment purpose

	// priority buffers
	uint8_t          prio_mode;                             //     1 priority buffers enabled / disabled
	uint8_t          prio_label[3];                         //     3 priority labels

	// frame filters
	uint8_t          frame_filter[ARINC429_RX_FILTERS_NUM]; // 1.024 pointer to frame buffer

	// frame buffers
	ARINC429RXBuffer frame_buffer[ARINC429_RX_BUFFER_NUM];  // 2.048 received frames buffer
}                                                           // =====
PACKED ARINC429RXChannel;                                   // 3.608 byte


// final combined data structure
typedef struct
{
	// channels
	ARINC429TXChannel tx_channel[ARINC429_TX_CHANNELS_NUM]; // 2.636 TX channels
	ARINC429RXChannel rx_channel[ARINC429_RX_CHANNELS_NUM]; // 7.216 RX channels

	// callbacks
	ARINC429Heartbeat heartbeat;                            //     8 bricklet heartbeat
	ARINC429Callback  callback;                             //   516 callback queue
}                                                           // =====
PACKED ARINC429;                                            //10.376 byte (~ 10 kByte)


/****************************************************************************/
/* PROTOTYPES                                                               */
/****************************************************************************/

extern ARINC429 arinc429;

void arinc429_init(void);
void arinc429_tick(void);
void arinc429_tick_task(void);


#endif  // #ifndef ARINC429_H

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
