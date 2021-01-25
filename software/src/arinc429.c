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

#include "arinc429.h"
#include "hi3593.h"
#include "communication.h"

#include "bricklib2/os/coop_task.h"
#include "bricklib2/logging/logging.h"

#include <string.h>

extern const int8_t opcode_length[256];


/****************************************************************************/
/* data structures                                                          */
/****************************************************************************/

ARINC429 arinc429;
CoopTask arinc429_task;


/****************************************************************************/
/* prototypes                                                               */
/****************************************************************************/

void arinc429_init_data(void);
void arinc429_init_chip(void);



/****************************************************************************/
/* local helper functions                                                   */
/****************************************************************************/

/* check the TX frame buffer map for a transmit allowance */
bool check_tx_buffer_map(uint8_t channel_index, uint16_t buffer_index)
{
	// get the lower 5 bits from the buffer index
	uint16_t buffer_index_lo = (buffer_index & 0x001F) >> 0;

	// get the upper 3 bits from the buffer index
	uint16_t buffer_index_hi = (buffer_index & 0x00E0) >> 5;

	// get the map word
	uint32_t map = arinc429.tx_channel[channel_index].frame_buffer_map[buffer_index_hi];

	// check if there is a filter assigned
	if(map & (1 << buffer_index_lo)) return true;
	else                             return false;
}


/* update the TX frame buffer map */
void update_tx_buffer_map(uint8_t channel_index, uint16_t buffer_index, uint8_t task)
{
	// get the lower 5 bits from the buffer index
	uint16_t buffer_index_lo = (buffer_index & 0x1F) >> 0;

	// get the upper 3 bits from the buffer index
	uint16_t buffer_index_hi = (buffer_index & 0xE0) >> 5;

	// get a pointer to the map word
	uint32_t *map = &(arinc429.tx_channel[channel_index].frame_buffer_map[buffer_index_hi]);

	// modify the map
	switch(task)
	{
		case ARINC429_SET   : *map |=  (1 << buffer_index_lo); break; // set   bit
		case ARINC429_CLEAR : *map &= ~(1 << buffer_index_lo); break; // clear bit
		default             :                                  break; // unknown task, do nothing
	}

	// done
	return;
}


/****************************************************************************/
/* local functions                                                          */
/****************************************************************************/

// update system configuration
void arinc429_task_update_system_config(void)
{
	// XMC data structure
	if(arinc429.system.change_request & ARINC429_SYSTEM_RESET_XMC_DATA)
	{
		// (re-)initialize the XMC data structure
		hi3593_init_data();
	}

	// XMC chip
	if(arinc429.system.change_request & ARINC429_SYSTEM_RESET_XMC_CHIP)
	{
		// (re-)initialize the XMC chip
		hi3593_init_chip();
	}

	// A429 data structure
	if(arinc429.system.change_request & ARINC429_SYSTEM_RESET_A429_DATA)
	{
		// (re-)initialize the A429 data structure
		arinc429_init_data();
	}

	// A429 chip
	if(arinc429.system.change_request & ARINC429_SYSTEM_RESET_A429_CHIP)
	{
		// (re-)initialize the A429 chip
		arinc429_init_chip();
	}

	// clear all request flags
	arinc429.system.change_request = 0;

	// done
	return;
}


// update channel configuration
void arinc429_task_update_channel_config(void)
{
	// TX channel opcodes
	const uint8_t reg_tx_ctrl[1] = {HI3593_CMD_WRITE_TX1_CTRL};

	// do TX channel(s)
	for(uint8_t i = 0; i < ARINC429_TX_CHANNELS_NUM; i++)
	{
		// get pointer to channel
		ARINC429TXChannel *channel = &(arinc429.tx_channel[i]);

		// skip the channel if there is no pending change request
		if(!channel->common.change_request)  continue;

		// reset statistics counters if operating mode is changed or repeated set to 'passive'
		if(    (channel->common.change_request  & ARINC429_UPDATE_OPERATING_MODE)
		    && (channel->common.operating_mode == ARINC429_CHANNEL_MODE_PASSIVE ) )
		{
			// reset statistics counters
			channel->common.frames_processed_curr = channel->common.frames_processed_last = 0;
			channel->common.frames_lost_curr      = channel->common.frames_lost_last      = 0;
		}

		// update scheduler
		if(channel->common.change_request & ARINC429_UPDATE_OPERATING_MODE)
		{
			// scheduler started?
			if(channel->common.operating_mode == ARINC429_CHANNEL_MODE_RUN)
			{
				// yes, reset scheduler
				channel->last_job_exec_time  = system_timer_get_ms();
				channel->last_job_dwell_time = 0;
				channel->job_index           = ARINC429_TX_JOBS_NUM - 1;   // the job execution starts with incrementing the index

				// restart sequence number from 0 (the counter is common to all TX channels)
				arinc429.callback.seq_number_scheduler = 0;
			}
		}

		// update the transmit control register
		if(    (channel->common.change_request & ARINC429_UPDATE_OPERATING_MODE)
		    || (channel->common.change_request & ARINC429_UPDATE_SPEED_PARITY  ) )
		{
			// determine HI-Z mode
			uint8_t hi_z   = (channel->common.operating_mode == ARINC429_CHANNEL_MODE_PASSIVE) ? 1 : 0;

			// isolate parity and speed modes
			uint8_t parity = (channel->common.parity_speed & 0xF0) ? 1 : 0;
			uint8_t speed  = (channel->common.parity_speed & 0x0F) ? 1 : 0;

			// set up new control register value
			uint8_t ctrl =   (hi_z          << 7)    // HI-Z mode
			               | (ARINC429_FLIP << 6)    // flip label bits
			               | (1             << 5)    // no dedicated transmit_enable needed
			               | (0             << 4)    // no self-test mode
			               | (0             << 3)    // select odd parity when in auto-parity mode
			               | (parity        << 2)    // parity mode
			               | (0             << 1)    // unused
			               | (speed         << 0);   // line speed

			// write control register value to A429 chip and check for success
			hi3593_write_register(reg_tx_ctrl[i], &ctrl, opcode_length[reg_tx_ctrl[i]]); // TODO handle SPI write failure
		}

		// clear all request flags
		channel->common.change_request = 0;
	}


	// RX channel opcodes
	const uint8_t reg_rx_read[2] = {HI3593_CMD_READ_RX1_FIFO,    HI3593_CMD_READ_RX2_FIFO   };
	const uint8_t reg_hfilter[2] = {HI3593_CMD_WRITE_RX1_FILTER, HI3593_CMD_WRITE_RX2_FILTER};
	const uint8_t reg_rx_ctrl[2] = {HI3593_CMD_WRITE_RX1_CTRL,   HI3593_CMD_WRITE_RX2_CTRL  };
	const uint8_t disc_qempty[2] = {HI3593_R1FLAG_INDEX,         HI3593_R2FLAG_INDEX        };


	// do RX channels
	for(uint8_t i = 0; i < ARINC429_RX_CHANNELS_NUM; i++)
	{
		// get pointer to channel
		ARINC429RXChannel *channel = &(arinc429.rx_channel[i]);

		// skip the channel if there is no pending change request
		if(!channel->common.change_request)  continue;

		// reset the statistics counters if operating mode is changed or repeated set to 'passive'
		if(    (channel->common.change_request  & ARINC429_UPDATE_OPERATING_MODE)
		    && (channel->common.operating_mode == ARINC429_CHANNEL_MODE_PASSIVE ) )
		{
			// yes, reset statistics counters
			channel->common.frames_processed_curr = channel->common.frames_processed_last = 0;
			channel->common.frames_lost_curr      = channel->common.frames_lost_last      = 0;
		}

		// reset the frame callback sequence number if the callback is set to 'off'
		if(    (channel->common.change_request  & ARINC429_UPDATE_CALLBACK_MODE)
		    && (channel->common.callback_mode  == ARINC429_CALLBACK_OFF        ) )
		{
			// yes, restart sequence number from 0 (the counter is common to all RX channels)
			arinc429.callback.seq_number_frame = 0;
		}

		// reset the frame buffers if operating mode is changed (or repeated set) to 'active'
		if(    (channel->common.change_request  & ARINC429_UPDATE_OPERATING_MODE)
			&& (channel->common.operating_mode == ARINC429_CHANNEL_MODE_ACTIVE  ) )
		{
			// yes, reset the frame buffers
			for(uint16_t j = 0; j < ARINC429_RX_BUFFER_NUM; j++)
			{
				// skip unused buffers
				if(channel->frame_buffer[j].frame_age == ARINC429_RX_BUFFER_UNUSED) continue;

				channel->frame_buffer[j].frame        = 0;
				channel->frame_buffer[j].last_rx_time = 0;
				channel->frame_buffer[j].frame_age    = ARINC429_RX_BUFFER_EMPTY;
			}
		}

		// update the FIFO hardware filter
		if(channel->common.change_request & ARINC429_UPDATE_FIFO_FILTER)
		{
			// load the hardware filter bitmap into the A429 chip
			hi3593_write_register(reg_hfilter[i], channel->hardware_filter, opcode_length[reg_hfilter[i]]);
		}

		// update the receive control register
		if(channel->common.change_request & ARINC429_UPDATE_SPEED_PARITY)
		{
			// isolate parity and speed
			uint8_t parity = (channel->common.parity_speed & 0xF0) ? 1 : 0;
			uint8_t speed  = (channel->common.parity_speed & 0x0F) ? 1 : 0;

			// yes, set up new control register value
			uint8_t ctrl =   (ARINC429_FLIP << 7)    // flip label bits
			               | (0             << 6)    // SD9 bit filter value
			               | (0             << 5)    // SD8 bit filter value
			               | (0             << 4)    // SD  bit filter disabled
			               | (parity        << 3)    // parity mode
			               | (1             << 2)    // enable hardware label filtering
			               | (0             << 1)    // priority buffers not used
			               | (speed         << 0);   // line speed

			// write control register value to A429 chip
			hi3593_write_register(reg_rx_ctrl[i], &ctrl, opcode_length[reg_rx_ctrl[i]]); // TODO handle SPI write failure
		}

		// drain FIFO buffer on parity/speed change or mode change
		if(    (channel->common.change_request & ARINC429_UPDATE_SPEED_PARITY  )
		    || (channel->common.change_request & ARINC429_UPDATE_OPERATING_MODE) )
		{
			uint8_t tmp[4];   // target for dummy reads

			// clear potential remains in RX FIFO when changing config
			for(uint8_t j = 0; j < ARINC429_RX_FIFO_BUFFER_NUM; j++)
			{
				// done if queue is empty
				if(XMC_GPIO_GetInput(hi3593_input_ports[disc_qempty[i]], hi3593_input_pins[disc_qempty[i]]) == 0) break;

				// read from RX FIFO
				hi3593_read_register(reg_rx_read[i], tmp, opcode_length[reg_rx_read[i]]);
			}
		}

		// clear all request flags
		channel->common.change_request = 0;
	}

	// done
	return;
}


// send frames via the immediate TX queue
void arinc429_task_tx_immediate(void)
{
	// TX channel opcodes and discretes
	const uint8_t   reg_tx_queue[1] = {HI3593_CMD_WRITE_TX1_FIFO};
	const uint8_t   disc_tfull[1]   = {HI3593_TFULL_INDEX};

	// do TX channel(s)
	for(uint8_t i = 0; i < ARINC429_TX_CHANNELS_NUM; i++)
	{
		// get a pointer to the channel
		ARINC429TXChannel *channel = &(arinc429.tx_channel[i]);

		// skip the channel if it has a pending configuration change
		if(channel->common.change_request)  continue;

		// is there a frame to be sent?
		if(channel->tail != channel->head)
		{
			// yes, is the TX queue of the A429 chip able to accept a new frame?
			if(XMC_GPIO_GetInput(hi3593_input_ports[disc_tfull[i]], hi3593_input_pins[disc_tfull[i]]) == 0)
			{
				uint8_t frame[4];   // frame broken down into individual bytes
				uint8_t data[4];    // transfer buffer for hi3593_write_register()
				uint8_t next_tail;  // next tail position in TX queue

				// compute the next tail position
				if(++next_tail >= ARINC429_TX_QUEUE_SIZE) next_tail = 0;

				// get the frame and convert it from uint32_t to an array of uint8_t
				memcpy(frame, &channel->queue[next_tail], 4);

				// reverse the byte sequence (the A429 chip wants the highest byte first)
				data[0] = frame[3];
				data[1] = frame[2];
				data[2] = frame[1];
				data[3] = frame[0];

				// enqueue the frame
				hi3593_write_register(reg_tx_queue[i], data, opcode_length[reg_tx_queue[i]]); // TODO handle SPI write failure

				// pulse the TX LED
				hi3593.led_flicker_state_tx.counter += LED_PULSE_TIME;

				// update the tail
				channel->tail = next_tail;

				// increment the statistics counter on processed frames
				channel->common.frames_processed_curr++;
			}
		}
	}

	// done
	return;
}


// send frames via the scheduler
void arinc429_task_tx_scheduled(void)
{
	// TX channel opcodes and discretes
	const uint8_t reg_tx_queue[1] = {HI3593_CMD_WRITE_TX1_FIFO};
	const uint8_t disc_tfull[1]   = {HI3593_TFULL_INDEX};

	// do TX channel(s)
	for(uint8_t i = 0; i < ARINC429_TX_CHANNELS_NUM; i++)
	{
		uint16_t job_frame;
		uint32_t dwell_time;
		uint16_t jobcode;
		uint16_t index;

		uint16_t no_tx_tasks_budget = ARINC429_TX_JOBS_NUM;          // number of none transmitting          jobs that are executed per tick
		uint8_t  zero_dwell_budget  = ARINC429_TX_ZERO_DWELL_BUDGET; // number of successive zero dwell time jobs that are executed per tick

		// get a pointer to the channel
		ARINC429TXChannel *channel = &(arinc429.tx_channel[i]);

		// skip the channel if the scheduler is not activated
		if(channel->common.operating_mode != ARINC429_CHANNEL_MODE_RUN)  continue;

		// skip the channel if it has a pending configuration change
		if(channel->common.change_request)  continue;

		// done for now if the dwell time of the last job has not yet elapsed
		if(!system_timer_is_time_elapsed_ms(channel->last_job_exec_time, channel->last_job_dwell_time))  continue;

		/*** execute the next job ***/

next_task:

		// advance to the next job
		if(++channel->job_index >= ARINC429_TX_JOBS_NUM) channel->job_index = 0;

		// get the job data
		job_frame  =           channel->job_frame [channel->job_index];
		dwell_time = (uint32_t)channel->dwell_time[channel->job_index];  // casted from unit8_t to uint32_t

		// extract the job code and the frame index (or special meaning) to the frame table
		jobcode = (job_frame & ARINC429_TX_JOB_JOBCODE_MASK) >> ARINC429_TX_JOB_JOBCODE_POS;
		index   = (job_frame & ARINC429_TX_JOB_INDEX_MASK  ) >> ARINC429_TX_JOB_INDEX_POS;

		// is it a stop command?
		if(jobcode == ARINC429_SCHEDULER_JOB_STOP)
		{
			// yes, update the channel operating mode
			channel->common.operating_mode = ARINC429_CHANNEL_MODE_ACTIVE;

			// request execution of the update
			channel->common.change_request |= ARINC429_UPDATE_OPERATING_MODE;

			// done with this channel
			continue;
		}

		// is it a jump command?
		if(jobcode == ARINC429_SCHEDULER_JOB_JUMP)
		{
			// yes, store the current job index
			channel->job_index_jump = channel->job_index;

			// store the assigned dwell time, it will be executed with the next RETURN job
			channel->dwell_time_jump = channel->dwell_time[channel->job_index];

			// relocate the job index (the job execution starts with incrementing the index)
			channel->job_index = (index > 0) ? index - 1 : ARINC429_TX_JOBS_NUM - 1;
		}

		// is it a return command?
		if(jobcode == ARINC429_SCHEDULER_JOB_RETURN)
		{
			// yes, recall the execution position from where the last JUMP was executed
			channel->job_index = channel->job_index_jump;

			// recall the dwell time that was given with the JUMP command
			dwell_time = (uint32_t)channel->dwell_time_jump;
		}

		// is it a callback command?
		if(jobcode == ARINC429_SCHEDULER_JOB_CALLBACK)
		{
			// yes, prepare scheduler message
			uint16_t curr_time = (uint16_t)(system_timer_get_ms() & 0x0000FFFF);
			uint8_t  token     = (uint8_t )(index                 &     0x00FF);

			// enqueue scheduler message, success?
			if(!enqueue_message(ARINC429_CALLBACK_JOB_SCHEDULER_CB, curr_time, token))
			{
				// no, increment counter on lost frames
				channel->common.frames_lost_curr++;
			}
		}

		// is it job that does not use the dwell time?
		if(jobcode < ARINC429_SCHEDULER_JOB_RETURN)
		{
			// yes, reduce the budget for non-transmitting tasks
			no_tx_tasks_budget--;

			// is there budget left over for executing another job right now?
			if(no_tx_tasks_budget > 0)
			{
				// yes, execute the next job right now
				goto next_task;
			}
			else
			{
				// no, force the dwell time to be zero
				dwell_time = 0;
			}
		}

		// does the job include a transmit activity?
		if(jobcode >= ARINC429_SCHEDULER_JOB_SINGLE)
		{
			// yes, check if the TX hardware queue is able to take a new frame
			if(XMC_GPIO_GetInput(hi3593_input_ports[disc_tfull[i]], hi3593_input_pins[disc_tfull[i]]) == 0)
			{
				uint8_t frame[4];  // frame broken down into individual bytes
				uint8_t data[4];   // transfer buffer for hi3593_write_register()

				// select frame source
				if(jobcode < ARINC429_SCHEDULER_JOB_RETRANS_RX1)
				{
					// transmit from TX frame buffer - check buffer map if frame is eligible for transmit
					if(!check_tx_buffer_map(i, index))
					{
						// no, skip transmission and proceed to dwelling
						goto next_dwell;
					}

					// transmit from TX frame buffer, convert the frame from uint32_t to an array of uint8_t
					memcpy(frame, &(channel->frame_buffer[index]), 4);
				}
				else
				{
					// retransmit from RX1 or RX2, get the channel index
					uint8_t channel_index = jobcode - ARINC429_SCHEDULER_JOB_RETRANS_RX1;

					// does the SDI/label combination have a filter assigned?
					if(!check_sw_filter_map(channel_index, index))
					{
						// no, skip transmission and proceed to dwelling
						goto next_dwell;
					}

					// retrieve the buffer index
					uint8_t buffer_index = arinc429.rx_channel[channel_index].frame_filter[index];

					// frame not received yet or in timeout?
					if(arinc429.rx_channel[channel_index].frame_buffer[buffer_index].frame_age > 60000)
					{
						// yes, skip transmission and proceed to dwelling
						goto next_dwell;
					}

					// get the frame and convert it from uint32_t to an array of uint8_t
					memcpy(frame, &(arinc429.rx_channel[channel_index].frame_buffer[buffer_index].frame), 4);
				}

				// reverse the byte sequence (the A429 chip wants the highest byte first)
				data[0] = frame[3];
				data[1] = frame[2];
				data[2] = frame[1];
				data[3] = frame[0];

				// enqueue the frame
				hi3593_write_register(reg_tx_queue[i], data, opcode_length[reg_tx_queue[i]]); // TODO handle SPI write failure

				// pulse the TX LED
				hi3593.led_flicker_state_tx.counter += LED_PULSE_TIME;

				// increment the statistics counter on processed frames
				(channel->common.frames_processed_curr)++;

				// shall do a single transmit only?
				if(jobcode == ARINC429_SCHEDULER_JOB_SINGLE)
				{
					// yes, update the frame buffer map - ineligible the frame
					update_tx_buffer_map(i, index, ARINC429_CLEAR);
				}
							}
			else
			{
				// no, the frame is not transmitted on this round - increment statistics counter on lost frames 
				(channel->common.frames_lost_curr)++;
			}
		}

next_dwell:

		// does the current task have a zero dwell time?
		if(dwell_time == 0)
		{
			// yes, reduce budget for successive zero dwell time tasks
			zero_dwell_budget--;

			// is there budget left over for executing another task?
			if(zero_dwell_budget > 0)
			{
				// yes, execute the next task right away
				goto next_task;
			}
		}

		// update the last job execution time (modulo 2^32 ms = ~ 50 days)
		channel->last_job_exec_time += channel->last_job_dwell_time;

		// memorize the dwell time of this job
		channel->last_job_dwell_time = dwell_time;
	}

	// done
	return;
}


/* scan receive buffers for new frames */
void arinc429_task_receive_frames(void)
{
	// RX channel opcodes and discretes
	const uint8_t spi_buffer_read[ARINC429_RX_CHANNELS_NUM] = {HI3593_CMD_READ_RX1_FIFO, HI3593_CMD_READ_RX2_FIFO};
	const uint8_t disc_new_frame [ARINC429_RX_CHANNELS_NUM] = {HI3593_R1FLAG_INDEX,      HI3593_R2FLAG_INDEX     };

	uint32_t  new_frame;     // buffer for received frame
	uint16_t  curr_time;     // cache  for current time
	uint8_t   data[4];       // transfer buffer for hi3593_read_register()
	uint8_t   frame[4];      // frame broken down into individual bytes
	uint8_t   frame_budget;  // max number of frames read per channel within one invocation
	uint16_t  ext_label;     // extended label code (label + SDI)
	uint8_t   buffer_index;  // index of the frame buffer
	uint16_t  age;           // computed age of the received frame


	// get the current time, chopped to 16 bit
	curr_time = (uint16_t)(system_timer_get_ms() & 0x0000FFFF);

	// scan all RX channels
	for(uint8_t i = 0; i < ARINC429_RX_CHANNELS_NUM; i++)
	{
		// get a pointer to the channel
		ARINC429RXChannel *channel = &(arinc429.rx_channel[i]);

		// skip the channel if it is in passive mode
		if(channel->common.operating_mode == ARINC429_CHANNEL_MODE_PASSIVE)  continue;

		// skip the channel if it has a pending configuration change
		if(channel->common.change_request)  continue;

		// set the budget for the maximum number of frames to be read
		frame_budget = ARINC429_RX_FRAME_BUDGET;

		while(frame_budget--)
		{
			// done with this channel if it has no pending new frame (any more)
			if(XMC_GPIO_GetInput(hi3593_input_ports[disc_new_frame[i]], hi3593_input_pins[disc_new_frame[i]]) == 0)  break;

			// get the frame
			hi3593_read_register(spi_buffer_read[i], data, opcode_length[spi_buffer_read[i]]);

			// pulse the RX LED
			hi3593.led_flicker_state_rx.counter += LED_PULSE_TIME;

			// is the parity set to auto, i.e. shall the parity be checked?
			if((channel->common.parity_speed & 0xF0) == (ARINC429_PARITY_AUTO << 4))
			{
				// yes, parity error? (the hardware parity checking sets bit 32 on parity error)
				if(data[0] & 0x80)
				{
					// yes, increment the counter on lost frames
					channel->common.frames_lost_curr++;

					// skip further frame processing
					continue;
				}
			}

			// reverse the byte sequence (the A429 chip delivers the highest byte first)
			frame[3] = data[0];
			frame[2] = data[1];
			frame[1] = data[2];
			frame[0] = data[3];

			// convert the frame from an array of uint8_t to an uint32_t
			memcpy(&new_frame, frame, 4);

			// extract the extended label code (label + SDI), aka index for the frame filter table
			ext_label = (uint16_t)(new_frame & ARINC429_RX_FRAME_EXT_LABEL_MASK);

			// does the SDI/label combination have a filter assigned?
			if(check_sw_filter_map(i, ext_label))
			{
				// yes, look-up the assigned frame buffer
				buffer_index = channel->frame_filter[ext_label];

				// get a pointer to the frame buffer
				ARINC429RXBuffer *buffer = &(channel->frame_buffer[buffer_index]);

				// 1st frame ever or after a timeout?
				if(buffer->frame_age <= ARINC429_RX_BUFFER_NEW)
				{
					// no, compute the age of the frame
					age = curr_time - buffer->last_rx_time;

					// limit the age to 60 sec = 60000 ms
					if(age > 60000)  age = 60000;
				}
				else
				{
					// yes, memorize its the 1st frame ever or after timeout
					age = ARINC429_RX_BUFFER_NEW;
				}

				// shall send a callback?
				if(    ((channel->common.callback_mode == ARINC429_CALLBACK_ON       )                                                                 )
				    || ((channel->common.callback_mode == ARINC429_CALLBACK_ON_CHANGE) && ((buffer->frame != new_frame) || (buffer->frame_age > ARINC429_RX_BUFFER_NEW))) )
				{
					// yes, enqueue a new frame message, success?
					if(!enqueue_message(ARINC429_CALLBACK_JOB_FRAME_RX1 + i, curr_time, buffer_index))
					{
						// no, increment counter on lost frames
						channel->common.frames_lost_curr++;
					}
				}

				// store the frame, its age and its receive time
				buffer->frame        = new_frame;
				buffer->frame_age    = age;
				buffer->last_rx_time = curr_time;

				// increment the statistics counter
				channel->common.frames_processed_curr++;
			}
		} // while(frame_budget)
	} // for(channel)

	// done
	return;
}


/* scan frame buffers for timeouts */
void arinc429_task_check_timeout(void)
{
	static uint8_t            buffer_index   = ARINC429_RX_BUFFER_NUM   - 1;  // will trigger a channel change            on the 1st run
	static uint8_t            channel_index  = ARINC429_RX_CHANNELS_NUM - 1;  // will trigger a change to the 1st channel on the 1st run
	static ARINC429RXChannel *channel;                                        // pointer to the current channel
	static uint16_t           timeout_period;                                 // timeout period of the current channel

	       uint16_t  curr_time;                                               // cache  for current time
	       uint8_t   check_budget = ARINC429_TIMEOUT_CHECK_BUDGET;            // number of buffers checked per invocation


	// get the current time, chopped to 16 bit
	curr_time = (uint16_t)(system_timer_get_ms() & 0x0000FFFF);

	while(check_budget--)
	{
		// advance to the next buffer, all buffers of the current channel done?
		if(++buffer_index == 0)
		{
			// yes, advance to the next channel, wrap-around after last channel
			if(++channel_index == ARINC429_RX_CHANNELS_NUM)  channel_index = 0;

			// cache a pointer to the channel
			channel = &(arinc429.rx_channel[channel_index]);

			// is the channel in passive mode?
			if(channel->common.operating_mode == ARINC429_CHANNEL_MODE_PASSIVE)
			{
				// yes, set the buffer index to the last buffer to trigger a channel change on the next invocation
				buffer_index = ARINC429_RX_BUFFER_NUM  - 1;

				// done for this time
				return;
			}
			else
			{
				// no, cache the channel's timeout period
				timeout_period = channel->timeout_period;
			}
		}

		// get a pointer to the current buffer
		ARINC429RXBuffer *buffer = &(channel->frame_buffer[buffer_index]);

		// does the buffer need to be checked, i.e. is it not unused, not empty, nor already found to be in timeout?
		if(buffer->frame_age <= 60000)
		{
			// yes, is the buffer in timeout now? (the subtraction is modulo 2^16)
			if((curr_time - buffer->last_rx_time) > timeout_period)
			{
				// yes, tag buffer as being in timeout
				buffer->frame_age = ARINC429_RX_BUFFER_TIMEOUT;

				// callbacks enabled?
				if(channel->common.callback_mode != ARINC429_CALLBACK_OFF)
				{
					// yes, enqueue a timeout message, success?
					if(!enqueue_message(ARINC429_CALLBACK_JOB_TIMEOUT_RX1 + channel_index, curr_time, buffer_index))
					{
						// no, increment counter on lost frames
						channel->common.frames_lost_curr++;
					}
				}
			}
		}
	}

	// done
	return;
}


// generate bricklet heartbeat
void generate_heartbeat_callback(void)
{
	// is the heartbeat enabled?
	if(arinc429.heartbeat.mode != ARINC429_CALLBACK_OFF)
	{
		// yes, is it time for the next heartbeat?
		if(system_timer_is_time_elapsed_ms(arinc429.heartbeat.last_time, (uint32_t)arinc429.heartbeat.period))
		{
			// yes, update heartbeat time
			arinc429.heartbeat.last_time += arinc429.heartbeat.period;

			// shall heartbeats only be sent on changed values (i.e. 'value_has_to_change' set to 'true')?
			if(arinc429.heartbeat.mode == ARINC429_CALLBACK_ON_CHANGE)
			{
				// yes - check if the values have actually changed
				if(    (arinc429.tx_channel[0].common.frames_processed_curr == arinc429.tx_channel[0].common.frames_processed_last)
					&& (arinc429.tx_channel[0].common.frames_lost_curr      == arinc429.tx_channel[0].common.frames_lost_last     )
					&& (arinc429.rx_channel[0].common.frames_processed_curr == arinc429.rx_channel[0].common.frames_processed_last)
					&& (arinc429.rx_channel[0].common.frames_lost_curr      == arinc429.rx_channel[0].common.frames_lost_last     )
					&& (arinc429.rx_channel[1].common.frames_processed_curr == arinc429.rx_channel[1].common.frames_processed_last)
					&& (arinc429.rx_channel[1].common.frames_lost_curr      == arinc429.rx_channel[1].common.frames_lost_last     ) )
				{
					// no change, done
					return;
				}
				else
				{
					// yes, update last statistics counters with the current values
					arinc429.tx_channel[0].common.frames_processed_last = arinc429.tx_channel[0].common.frames_processed_curr;
					arinc429.tx_channel[0].common.frames_lost_last      = arinc429.tx_channel[0].common.frames_lost_curr;
					arinc429.rx_channel[0].common.frames_processed_last = arinc429.rx_channel[0].common.frames_processed_curr;
					arinc429.rx_channel[0].common.frames_lost_last      = arinc429.rx_channel[0].common.frames_lost_curr;
					arinc429.rx_channel[1].common.frames_processed_last = arinc429.rx_channel[1].common.frames_processed_curr;
					arinc429.rx_channel[1].common.frames_lost_last      = arinc429.rx_channel[1].common.frames_lost_curr;
				}
			}

			// get the current time, chopped to 16 bit
			uint16_t  curr_time = (uint16_t)(system_timer_get_ms() & 0x0000FFFF);

			// enqueue a heartbeat callback message (the buffer argument is not used with heartbeats)
			enqueue_message(ARINC429_CALLBACK_JOB_HEARTBEAT, curr_time, 0);
		}
	}

	// done
	return;
}


/* initialize A429 chip */
void arinc429_init_chip(void)
{
	uint8_t  data;

	// give the chip time to awake in case we just had power-on
	coop_task_sleep_ms(100);

	// do a master reset
	hi3593_write_register(HI3593_CMD_MASTER_RESET,   NULL,  0);     // TODO evaluate return code

	// give the chip some time to restart
	coop_task_sleep_ms(100);

	// configure the clock divider for an applied clock signal of 1 MHz
	data =  0x00 << 1;
	hi3593_write_register(HI3593_CMD_WRITE_ACLK_DIV, &data, 1);     // TODO evaluate return code

	// configure the discretes
	data =   0x0 << 6   // R2INT  pulses high on reception of a frame on channel RX2
	       | 0x3 << 4   // R2FLAG goes   high when the RX2 FIFO contains >= 1 frame
	       | 0x0 << 2   // R1INT  pulses high on reception of a frame on channel RX1
	       | 0x3 << 0;  // R1FLAG goes   high when the RX1 FIFO contains >= 1 frame

	hi3593_write_register(HI3593_CMD_WRITE_FLAG_IRQ, &data, 1);     // TODO evaluate return code
}


/* initialize A429 data structure */
void arinc429_init_data(void)
{
	// clear the complete data structure, but not the system part at the end of it
	memset(&arinc429, 0, sizeof(ARINC429) - sizeof(ARINC429System));

	// initialize all TX channels
	for(uint8_t i = 0; i < ARINC429_TX_CHANNELS_NUM; i++)
	{
		// get a pointer to the channel
		ARINC429TXChannel *channel = &(arinc429.tx_channel[i]);

		// operating modes
		channel->common.parity_speed          = (ARINC429_PARITY_AUTO << 4) | (ARINC429_SPEED_LS << 0);
		channel->common.operating_mode        = ARINC429_CHANNEL_MODE_PASSIVE;
		channel->common.callback_mode         = ARINC429_CALLBACK_OFF;

		// requests
		channel->common.change_request        = 0xFF;  // request update of everything

		// statistics
		channel->common.frames_processed_curr = 0;
		channel->common.frames_processed_last = 0;
		channel->common.frames_lost_curr      = 0;
		channel->common.frames_lost_last      = 0;

		// immediate transmit queue
		channel->head                         = 0;
		channel->tail                         = 0;

		// immediate transmit frame buffers
		for(uint8_t j = 0; j < ARINC429_TX_QUEUE_SIZE; j++)
		{
			channel->queue[j]                 = 0;
		}

		// scheduled transmit
		channel->last_job_exec_time           = 0;
		channel->last_job_dwell_time          = 0;
		channel->scheduler_jobs_used          = 0;
		channel->job_index                    = ARINC429_TX_JOBS_NUM - 1;   // the job execution starts with incrementing the index
		channel->job_index_jump               = 0;

		// scheduler task table
		for(uint16_t j = 0; j < ARINC429_TX_JOBS_NUM; j++)
		{
			channel->job_frame[j]             = ARINC429_SCHEDULER_JOB_SKIP << ARINC429_TX_JOB_JOBCODE_POS;
			channel->dwell_time[j]            = 0;
		}

		// scheduler frame table
		for(uint16_t j = 0; j < ARINC429_TX_BUFFER_NUM; j++)
		{
			channel->frame_buffer[j]          = 0;
		}
		
		// scheduler frame transmit eligible map
		for(uint16_t j = 0; j < 8; j++)
		{
			channel->frame_buffer_map[j]      = 0;
		}
	}

	// initialize all RX channels
	for(uint8_t i = 0; i < ARINC429_RX_CHANNELS_NUM; i++)
	{
		// get a pointer to the channel
		ARINC429RXChannel *channel = &(arinc429.rx_channel[i]);

		// operating modes
		channel->common.parity_speed          = (ARINC429_PARITY_AUTO << 4) | (ARINC429_SPEED_LS << 0);
		channel->common.operating_mode        = ARINC429_CHANNEL_MODE_PASSIVE;
		channel->common.callback_mode         = ARINC429_CALLBACK_OFF;

		// requests
		channel->common.change_request        = 0xFF;  // request update of everything

		// statistics
		channel->common.frames_processed_curr = 0;
		channel->common.frames_processed_last = 0;
		channel->common.frames_lost_curr      = 0;
		channel->common.frames_lost_last      = 0;

		// timeout check
		channel->timeout_period               = 1000;

		// hardware frame filters
		for(uint8_t j = 0; j < 32; j++)
		{
			channel->hardware_filter[j]       = 0;
		}

		// software frame filters
		for(uint8_t  j = 0; j < 32; j++)
		{
			channel->frame_filter_map[j]      = 0;
		}

		for(uint16_t j = 0; j < ARINC429_RX_FILTERS_NUM; j++)
		{
			channel->frame_filter[j]          = 0;
		}

		// frame buffers
		channel->frame_buffers_used           = 0;

		for(uint16_t j = 0; j < ARINC429_RX_BUFFER_NUM; j++)
		{
			channel->frame_buffer[j].frame         = 0;
			channel->frame_buffer[j].frame_age     = ARINC429_RX_BUFFER_UNUSED;
			channel->frame_buffer[j].last_rx_time  = 0;
		}
	}

	// heartbeat callback
	arinc429.heartbeat.last_time              = 0;
	arinc429.heartbeat.period                 = 0;
	arinc429.heartbeat.mode                   = ARINC429_CALLBACK_OFF;
	arinc429.heartbeat.seq_number             = 0;

	// callback queue
	arinc429.callback.head                    = 0;
	arinc429.callback.tail                    = 0;
	arinc429.callback.seq_number_frame        = 0;
	arinc429.callback.seq_number_scheduler    = 0;

	// callback queue buffers
	for(uint16_t i = 0; i < ARINC429_CB_QUEUE_SIZE; i++)
	{
		arinc429.callback.queue[i].message    = ARINC429_CALLBACK_JOB_NONE;
		arinc429.callback.queue[i].buffer     = 0;
	}

	// done
	return;
}


/****************************************************************************/
/* task & tick functions                                                    */
/****************************************************************************/

void arinc429_tick(void)
{
	// restart arinc429_tick_task()
	coop_task_tick(&arinc429_task);
}

void arinc429_tick_task(void)
{
	while(true)
	{
		// conditionally do the system jobs
		if(arinc429.system.change_request)
		{
			// update the system configuration
			arinc429_task_update_system_config();
		}

		// conditionally do the normal operation mode jobs
		if(arinc429.system.operating_mode == ARINC429_A429_MODE_NORMAL)
		{
			// update the channel configuration
			arinc429_task_update_channel_config();

			// do the TX operations
			arinc429_task_tx_immediate();    // send frames via the immediate TX queue
			arinc429_task_tx_scheduled();    // send frames via the scheduler

			// do the RX operations
			arinc429_task_receive_frames();  // scan receive buffers for new frames
			arinc429_task_check_timeout();   // scan frame   buffers for timeouts

			// operate the RX/TX LEDs
			hi3593_tick();
		}

		// generate the heartbeat
		generate_heartbeat_callback();

		// done for now
		coop_task_yield();
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
