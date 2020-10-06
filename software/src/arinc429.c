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
ARINC429 arinc429;
CoopTask arinc429_task;


/****************************************************************************/
/* local functions                                                          */
/****************************************************************************/


// update channel configuration
void arinc429_task_update_channel_config(void)   // %%% thread A-1
{
	// TX channel opcodes
	const uint8_t reg_tx_ctrl[1] = {HI3593_CMD_WRITE_TX1_CTRL};

	// do TX channel(s)
	for(uint8_t i = 0; i < ARINC429_TX_CHANNELS_NUM; i++)
	{
		// get pointer to channel
		ARINC429TXChannel *channel = &(arinc429.tx_channel[i]);

		// skip if no pending change
		if(!channel->common.pending_change)  continue;

		// update scheduler
		if(channel->common.operating_mode & ARINC429_CHANGE_REQUEST)
		{
			// scheduler started?
			if((channel->common.operating_mode & 0x07) == ARINC429_CHANNEL_MODE_RUN)
			{
				// yes, reset scheduler
				channel->last_job_exec_time  = system_timer_get_ms();
				channel->last_job_dwell_time = 0;
				channel->next_job_index      = 0;
			}
		}

		// update A429 chip setup
		if(    (channel->common.operating_mode & ARINC429_CHANGE_REQUEST)
		    || (channel->common.parity_speed   & ARINC429_CHANGE_REQUEST) )
		{

			// determine HI-Z mode
			uint8_t hi_z   = ((channel->common.operating_mode & 0x07) == ARINC429_CHANNEL_MODE_PASSIVE) ? 1 : 0;

			// isolate parity and speed modes
			uint8_t parity = (channel->common.parity_speed & 0x70) ? 1 : 0;
			uint8_t speed  = (channel->common.parity_speed & 0x07) ? 1 : 0;

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
			hi3593_task_write_register(reg_tx_ctrl[i], &ctrl, opcode_length[reg_tx_ctrl[i]]); // TODO handle SPI write failure
		}

		// clear request flags
		channel->common.parity_speed   &= ~ARINC429_CHANGE_REQUEST;
		channel->common.operating_mode &= ~ARINC429_CHANGE_REQUEST;

		// no pending changes any more
		channel->common.pending_change = 0;
	}

	// RX channel opcodes
	const uint8_t reg_rx_read[2] = {HI3593_CMD_READ_RX1_FIFO,  HI3593_CMD_READ_RX2_FIFO };
	const uint8_t reg_p1_read[2] = {HI3593_CMD_READ_RX1_PRIO1, HI3593_CMD_READ_RX2_PRIO1};
	const uint8_t reg_p2_read[2] = {HI3593_CMD_READ_RX1_PRIO2, HI3593_CMD_READ_RX2_PRIO2};
	const uint8_t reg_p3_read[2] = {HI3593_CMD_READ_RX1_PRIO3, HI3593_CMD_READ_RX2_PRIO3};
	const uint8_t reg_rx_prio[2] = {HI3593_CMD_WRITE_RX1_PRIO, HI3593_CMD_WRITE_RX2_PRIO};
	const uint8_t reg_rx_ctrl[2] = {HI3593_CMD_WRITE_RX1_CTRL, HI3593_CMD_WRITE_RX2_CTRL};
	const uint8_t disc_qempty[2] = {HI3593_R1FLAG_INDEX,       HI3593_R2FLAG_INDEX      };

	// do RX channels
	for(uint8_t i = 0; i < ARINC429_RX_CHANNELS_NUM; i++)
	{
		// get pointer to channel
		ARINC429RXChannel *channel = &(arinc429.rx_channel[i]);

		// skip if no pending change
		if(!channel->common.pending_change)  continue;

		// update frame callback
		if(channel->common.callback_mode & ARINC429_CHANGE_REQUEST)
		{
			// reset callback message sequence counter (counter is common to all RX channels)
			arinc429.callback.seq_number = 0;
		}

		// update timeout scan
		if(channel->common.operating_mode & ARINC429_CHANGE_REQUEST)
		{
			// operating mode changed to active?
			if((channel->common.operating_mode & 0x07) == ARINC429_CHANNEL_MODE_ACTIVE)
			{
				// yes, initialize scan for timeouts in frame buffers (run scans dispersed in time)
				channel->last_timeout_scan = system_timer_get_ms() + i * (ARINC429_RX_TIMEOUT_SCAN_PERIOD / ARINC429_RX_CHANNELS_NUM);

				// reset the frame buffers
				for(uint16_t j = 0; j < ARINC429_RX_BUFFER_NUM; j++)
				{
					channel->frame_buffer[j].frame        = 0;
					channel->frame_buffer[j].last_rx_time = 0;
					channel->frame_buffer[j].frame_age    = ARINC429_RX_BUFFER_UNUSED;
				}
			}
		}

		// update A429 chip setup
		if(    (channel->common.parity_speed & ARINC429_CHANGE_REQUEST)
		    || (channel->prio_mode           & ARINC429_CHANGE_REQUEST) )
		{
			// isolate parity, speed and priority modes
			uint8_t parity = (channel->common.parity_speed & 0x70) ? 1 : 0;
			uint8_t speed  = (channel->common.parity_speed & 0x07) ? 1 : 0;
			uint8_t prio   = (channel->prio_mode           & 0x07) ? 1 : 0;

			// yes, set up new control register value
			uint8_t ctrl =   (ARINC429_FLIP << 7)    // flip label bits
			               | (0             << 6)    // SD9 bit filter value
			               | (0             << 5)    // SD8 bit filter value
			               | (0             << 4)    // SD  bit filter disabled
			               | (parity        << 3)    // parity mode
			               | (0             << 2)    // disable hardware label filtering
			               | (prio          << 1)    // priority buffers mode
			               | (speed         << 0);   // line speed

			// write control register value to A429 chip and check for success
			hi3593_task_write_register(reg_rx_ctrl[i], &ctrl, opcode_length[reg_rx_ctrl[i]]); // TODO handle SPI write failure
		}

		// update priority filters
		if(channel->prio_mode & ARINC429_CHANGE_REQUEST)
		{
			// priority filters enabled?
			if(channel->prio_mode & ARINC429_PRIORITY_ENABLED)
			{
				uint8_t tmp[4];   // target for dummy reads

				// yes, prepare new labels for loading
				uint8_t prio_labels[3] = {channel->prio_label[2], channel->prio_label[1], channel->prio_label[0]};

				// load new labels into A429 chip
				hi3593_task_write_register(reg_rx_prio[i], prio_labels, opcode_length[reg_rx_prio[i]]);

				// drain priority buffers
				hi3593_task_read_register(reg_p1_read[i], tmp, opcode_length[reg_p1_read[i]]);
				hi3593_task_read_register(reg_p2_read[i], tmp, opcode_length[reg_p2_read[i]]);
				hi3593_task_read_register(reg_p3_read[i], tmp, opcode_length[reg_p3_read[i]]);
			}
		}

		// drain FIFO on parity or speed change
		if(channel->common.parity_speed & ARINC429_CHANGE_REQUEST)
		{
			uint8_t tmp[4];   // target for dummy reads

			// clear potential remains in RX FIFO when changing config (ignoring potential new frame arriving meanwhile)
			for(uint8_t j = 0; j < ARINC429_RX_FIFO_BUFFER_NUM; j++)
			{
				// done if queue is empty
				if(XMC_GPIO_GetInput(hi3593_input_ports[disc_qempty[i]], hi3593_input_pins[disc_qempty[i]]) == 0) break;

				// read from RX FIFO
				hi3593_task_read_register(reg_rx_read[i], tmp, opcode_length[reg_rx_read[i]]);
			}
		}

		// clear request flags
		channel->common.parity_speed   &= ~ARINC429_CHANGE_REQUEST;
		channel->common.operating_mode &= ~ARINC429_CHANGE_REQUEST;
		channel->common.callback_mode  &= ~ARINC429_CHANGE_REQUEST;
		channel->prio_mode             &= ~ARINC429_CHANGE_REQUEST;

		// no pending changes any more
		channel->common.pending_change = 0;
	}

	// done
	return;
}


// send frames via the immediate TX queue
void arinc429_task_tx_immediate(void)   // %%% thread A-2
{
	// TX channel opcodes and discretes
	const uint8_t   reg_tx_queue[1] = {HI3593_CMD_WRITE_TX1_FIFO};
	const uint8_t   disc_tfull[1]   = {HI3593_TFULL_INDEX};

	// do TX channel(s)
	for(uint8_t i = 0; i < ARINC429_TX_CHANNELS_NUM; i++)
	{
		// get pointer to channel
		ARINC429TXChannel *channel = &(arinc429.tx_channel[i]);

		// skip channel if it has a pending configuration change
		if(channel->common.pending_change)  continue;

		// is there a frame to be sent?
		if(channel->tail != channel->head)
		{
			// yes, is the TX queue of the A429 chip able to accept a frame?
			if(XMC_GPIO_GetInput(hi3593_input_ports[disc_tfull[i]], hi3593_input_pins[disc_tfull[i]]) == 0)
			{
				uint8_t data[4];  // transfer buffer for hi3593_task_write_register()

				// compute next tail position (modulo ARINC429_TX_QUEUE_SIZE)
				uint8_t next_tail = channel->tail + 1;
				next_tail &= ARINC429_TX_QUEUE_SIZE - 1;

				// get frame and convert it from uint32_t to an array of uint8_t
				memcpy(data, &channel->queue[next_tail], 4);

				// enqueue frame
				hi3593_task_write_register(reg_tx_queue[i], data, opcode_length[reg_tx_queue[i]]); // TODO handle SPI write failure

				// update tail
				channel->tail = next_tail;

				// increment statistics counter on processed frames
				channel->common.frames_processed_curr++;
			}
		}
	}

	// done
	return;
}


// send frames via the scheduler
void arinc429_task_tx_scheduled(void)   // %%% thread A-3
{
	// TX channel opcodes and discretes
	const uint8_t reg_tx_queue[1] = {HI3593_CMD_WRITE_TX1_FIFO};
	const uint8_t disc_tfull[1]   = {HI3593_TFULL_INDEX};

	// do TX channel(s)
	for(uint8_t i = 0; i < ARINC429_TX_CHANNELS_NUM; i++)
	{
		// get pointer to channel
		ARINC429TXChannel *channel = &(arinc429.tx_channel[i]);

		// skip channel if it has a pending configuration change
		if(channel->common.pending_change)  continue;

		// done with channel if the scheduler is not activated
		if(channel->common.operating_mode != ARINC429_CHANNEL_MODE_RUN)  continue;

		// done for now if the dwell time of the last job has not yet elapsed
		if(!system_timer_is_time_elapsed_ms(channel->last_job_exec_time, channel->last_job_dwell_time))  continue;

		/*** it is time to execute the next job ***/

		// update last job execution time
		channel->last_job_exec_time += channel->last_job_dwell_time;

		// get the next job
		uint16_t job_frame  =           channel->job_frame[channel->next_job_index];
		uint32_t dwell_time = (uint32_t)channel->dwell_time[channel->next_job_index];

		// extract the job code & index to the frame table
		uint16_t jobcode = (job_frame & ARINC429_TX_JOB_JOBCODE_MASK) >> ARINC429_TX_JOB_JOBCODE_POS;
		uint16_t index   = (job_frame & ARINC429_TX_JOB_INDEX_MASK  ) >> ARINC429_TX_JOB_INDEX_POS;

		// does the job include a transmit activity?
		if(jobcode >= ARINC429_SCHEDULER_JOB_SINGLE)
		{
			// yes, check if the TX hardware queue is able to take a frame
			if(XMC_GPIO_GetInput(hi3593_input_ports[disc_tfull[i]], hi3593_input_pins[disc_tfull[i]]) == 0)
			{
			    uint8_t data[4]; // transfer buffer for hi3593_task_write_register()
			
			    // yes, get frame to transmit and convert it from uint32_t to an array of uint8_t
			    memcpy(data, &(channel->frame_buffer[index]), 4);
			
				// enqueue the frame
				hi3593_task_write_register(reg_tx_queue[i], data, opcode_length[reg_tx_queue[i]]); // TODO handle SPI write failure

				// increment statistics counter on processed frames
				(channel->common.frames_processed_curr)++;

				// shall do a single transmit only?
				if(jobcode == ARINC429_SCHEDULER_JOB_SINGLE)
				{
					// yes, change the job to 'mute'
					jobcode   = ARINC429_SCHEDULER_JOB_MUTE;
					job_frame = (jobcode << ARINC429_TX_JOB_JOBCODE_POS) | (index << ARINC429_TX_JOB_INDEX_POS);
					channel->job_frame[channel->next_job_index] = job_frame;
				}
			}
			else
			{
				// no, increment statistics counter on lost frames
				(channel->common.frames_lost_curr)++;
			}
		}

		// memorize the dwell time of this job
		channel->last_job_dwell_time = (jobcode == ARINC429_SCHEDULER_JOB_EMPTY) ? 0 : dwell_time;

		// advance to next job
		if(++channel->next_job_index == ARINC429_TX_TASKS_NUM) channel->next_job_index = 0;
	}

	// done
	return;
}


/* scan receive buffers for new frames */
void arinc429_task_receive_frames(void)   // %%% thread A-4
{
	// RX channel opcodes and discretes
	const uint8_t spi_buffer_read[4 * ARINC429_RX_CHANNELS_NUM] = {HI3593_CMD_READ_RX1_FIFO,
	                                                               HI3593_CMD_READ_RX1_PRIO1,
	                                                               HI3593_CMD_READ_RX1_PRIO2,
	                                                               HI3593_CMD_READ_RX1_PRIO3,
	                                                               HI3593_CMD_READ_RX2_FIFO,
	                                                               HI3593_CMD_READ_RX2_PRIO1,
	                                                               HI3593_CMD_READ_RX2_PRIO2,
	                                                               HI3593_CMD_READ_RX2_PRIO3
	                                                              };

	const uint8_t disc_new_frame[4 * ARINC429_RX_CHANNELS_NUM]  = {HI3593_R1FLAG_INDEX,
	                                                               HI3593_MB11_INDEX,
	                                                               HI3593_MB12_INDEX,
	                                                               HI3593_MB13_INDEX,
	                                                               HI3593_R2FLAG_INDEX,
	                                                               HI3593_MB21_INDEX,
	                                                               HI3593_MB22_INDEX,
	                                                               HI3593_MB23_INDEX
	                                                              };

	uint32_t new_frame;  // buffer for received frame
	uint16_t curr_time;  // cache  for current time
    uint8_t  data[4];    // transfer buffer for hi3593_task_read_register()


	// get the current time, chopped to 16 bit
	curr_time = (uint16_t)(system_timer_get_ms() & 0x0000FFFF);

	// scan all RX channels
	for(uint8_t i = 0; i < ARINC429_RX_CHANNELS_NUM; i++)
	{
		// get pointer to channel
		ARINC429RXChannel *channel = &(arinc429.rx_channel[i]);

		// skip channel if it has a pending configuration change
		if(channel->common.pending_change)  continue;

		// scan all mail boxes (0= FIFO, 1= PRIO1, 2= PRIO2, 3= PRIO3)
		for(uint8_t mailbox = 0; mailbox < 4; mailbox++)
		{
			// done with this buffer if it has no pending new frame
			if(XMC_GPIO_GetInput(hi3593_input_ports[disc_new_frame[4*i + mailbox]], hi3593_input_pins[disc_new_frame[4*i + mailbox]]) == 0)  continue;

			// get the frame (also clears the discrete)
			hi3593_task_read_register(spi_buffer_read[4*i + mailbox], data, opcode_length[spi_buffer_read[4*i + mailbox]]);

            // store the frame, converting it from an array of uint8_t to an uint32_t
            memcpy(&new_frame, data, 4);

			// done if channel is in passive mode (trash frame)
			if(channel->common.operating_mode == ARINC429_CHANNEL_MODE_PASSIVE)  continue;

			// frame read from FIFO?
			if(mailbox == 0)
			{
				// yes, priority filters enabled?
				if(channel->prio_mode == ARINC429_PRIORITY_ENABLED)
				{
					// yes, extract label code
					uint8_t label = (uint8_t)(new_frame & ARINC429_RX_FRAME_LABEL_MASK);

					// trash frame if its label is configured in one of the priority filters
					if(label == channel->prio_label[0])  continue;
					if(label == channel->prio_label[1])  continue;
					if(label == channel->prio_label[2])  continue;
				}
			}

			// extract the extended label code (label + SDI), aka index for the frame filter table
			uint16_t filter_index = (uint16_t)(new_frame & ARINC429_RX_FRAME_EXT_LABEL_MASK);

			// look-up the assigned frame buffer
			uint8_t buffer_index = channel->frame_filter[filter_index];

			// trash the frame if it has no frame buffer assigned
			if(buffer_index == ARINC429_RX_FILTER_UNUSED)  continue;

			/*** we have a new frame to be stored and reported ***/

			// get pointer to buffer
			ARINC429RXBuffer *buffer = &(channel->frame_buffer[buffer_index]);

			// compute the age of the frame
			uint16_t age = curr_time - buffer->last_rx_time;

			// limit the age to 60 sec = 60000 ms
			if(age > 60000)  age = 60000;

			// also set the age to 60 sec if the buffer was not in use or had a timeout before
			if(buffer->frame_age > 60000)  age = 60000;

			// store the frame age and the current time
			buffer->frame_age    = age;
			buffer->last_rx_time = curr_time;

			// callback enabled?
			if(channel->common.callback_mode != ARINC429_CALLBACK_OFF)
			{
				// yes, send anyway (i.e. not in 'on_change_only' mode) or did the frame change? 
				if(    (channel->common.callback_mode != ARINC429_CALLBACK_ON_CHANGE)
					|| (buffer->frame                 != new_frame                  ) )
				{
					// yes, enqueue a new frame message, success?
					if(!enqueue_message(ARINC429_CALLBACK_JOB_FRAME_RX1 + i, buffer_index))
					{
						// no, increment counter on lost frames
						channel->common.frames_lost_curr++;
					}
				}
			}

			// store the new frame
			buffer->frame = new_frame;

			// increment statistics counter
			channel->common.frames_processed_curr++;

		} // for(mailbox)
	} // for(channel)

	// done
	return;
}


/* scan frame buffers for timeouts */
void arinc429_task_check_timeout(void)   // %%% thread B      // TODO rewrite such that the scan is done in smaller junks per invocation and then call it more often
{
	uint16_t  curr_time = (uint16_t)(system_timer_get_ms() & 0x0000FFFF);

	// do all RX channels
	for(uint8_t i = 0; i < ARINC429_RX_CHANNELS_NUM; i++)
	{
		// get pointer to channel
		ARINC429RXChannel *channel = &(arinc429.rx_channel[i]);

		// skip channel if it has a pending configuration change
		if(channel->common.pending_change)  continue;

		uint16_t timeout_period = channel->timeout_period;

		// skip channel if it is in passive mode
		if(channel->common.operating_mode == ARINC429_CHANNEL_MODE_PASSIVE)  continue;

		// skip channel if it is not yet time for the next scan
		if(!system_timer_is_time_elapsed_ms(channel->last_timeout_scan, ARINC429_RX_TIMEOUT_SCAN_PERIOD))  continue;

		// update last timeout scan time
		channel->last_timeout_scan += ARINC429_RX_TIMEOUT_SCAN_PERIOD;

		// scan all operational frame buffers
		for(uint8_t  buffer_index = 0; buffer_index < (ARINC429_RX_BUFFER_NUM - 1); buffer_index++)
		{
			// get pointer to buffer
			ARINC429RXBuffer *buffer = &(channel->frame_buffer[buffer_index]);

			// skip buffer if it is not in use or already found to be in timeout
			if(buffer->frame_age > 60000) continue;

			// buffer in timeout? (the subtraction is modulo 2^16)
			if((curr_time - buffer->last_rx_time) > timeout_period)
			{
				// yes, tag buffer as being in timeout
				buffer->frame_age = ARINC429_RX_BUFFER_TIMEOUT;

				// callbacks enabled?
				if(channel->common.callback_mode != ARINC429_CALLBACK_OFF)
				{
					// yes, enqueue a timeout message, success?
					if(!enqueue_message(ARINC429_CALLBACK_JOB_TIMEOUT_RX1 + i, buffer_index))
					{
						// no, increment counter for lost frames
						channel->common.frames_lost_curr++;
					}
				}
			}
		}  // for (buffer_index)
	}  // for (channel)

	// done
	return;
}


// generate bricklet heartbeat
void generate_heartbeat_callback(void)   // %%% thread E
{
	// heartbeat enabled?
	if(arinc429.heartbeat.mode != ARINC429_CALLBACK_OFF)
	{
		// time for the next heartbeat?
		if(system_timer_is_time_elapsed_ms(arinc429.heartbeat.last_time, (uint32_t)arinc429.heartbeat.period))
		{
			// yes, update last heartbeat time
			arinc429.heartbeat.last_time += arinc429.heartbeat.period;

			// 'value_has_to_change' enabled?
			if(arinc429.heartbeat.mode == ARINC429_CALLBACK_ON_CHANGE)
			{
				// yes - check if values have actually changed
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
					// yes, update last statistics counters with current values
					arinc429.tx_channel[0].common.frames_processed_last = arinc429.tx_channel[0].common.frames_processed_curr;
					arinc429.tx_channel[0].common.frames_lost_last      = arinc429.tx_channel[0].common.frames_lost_curr;
					arinc429.rx_channel[0].common.frames_processed_last = arinc429.rx_channel[0].common.frames_processed_curr;
					arinc429.rx_channel[0].common.frames_lost_last      = arinc429.rx_channel[0].common.frames_lost_curr;
					arinc429.rx_channel[1].common.frames_processed_last = arinc429.rx_channel[1].common.frames_processed_curr;
					arinc429.rx_channel[1].common.frames_lost_last      = arinc429.rx_channel[1].common.frames_lost_curr;
				}
			}

			// enqueue a heartbeat callback message
			enqueue_message(ARINC429_CALLBACK_JOB_HEARTBEAT, 0);
		}
	}

	// done
	return;
}


/****************************************************************************/
/* task & tick functions                                                    */
/****************************************************************************/

/* A429 tick             */
/* called from main-loop */
void arinc429_tick(void)
{
	// restart arinc429_tick_task()
	coop_task_tick(&arinc429_task);
}

void arinc429_tick_task(void)
{
	while(true)
	{
		// update channel configuration
		arinc429_task_update_channel_config();  // update channel configuration

		// TX operations
		arinc429_task_tx_immediate();           // send frames via the immediate TX queue
		arinc429_task_tx_scheduled();           // send frames via the scheduler

		// RX operations
		arinc429_task_receive_frames();         // scan receive buffers for new frames
		arinc429_task_check_timeout();          // scan frame buffers for timeouts

		// heartbeat generation
		generate_heartbeat_callback();          // generate bricklet heartbeat

		coop_task_yield();
	}
}


/****************************************************************************/
/* global functions                                                         */
/****************************************************************************/

/* initialize data structures, called from main() */
void arinc429_init(void)
{
	// clear complete data structure
	memset(&arinc429, 0, sizeof(ARINC429));

	// initialize all TX channels
	for(uint8_t i = 0; i < ARINC429_TX_CHANNELS_NUM; i++)
	{
		// get pointer to channel
		ARINC429TXChannel *channel = &(arinc429.tx_channel[i]);

		// operating mode
		channel->common.parity_speed          = (ARINC429_PARITY_AUTO << 4) | (ARINC429_SPEED_LS             << 0);
		channel->common.operating_mode        =                               (ARINC429_CHANNEL_MODE_PASSIVE << 0);
		channel->common.callback_mode         =                               (ARINC429_CALLBACK_OFF         << 0);

		// statistics
		channel->common.frames_processed_curr = 0;
		channel->common.frames_processed_last = 0;
		channel->common.frames_lost_curr      = 0;
		channel->common.frames_lost_last      = 0;

		// immediate transmit
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
		channel->next_job_index               = 0;

		// scheduler entries
		for(uint16_t j = 0; j < ARINC429_TX_TASKS_NUM; j++)
		{
			channel->job_frame[j]             = ARINC429_SCHEDULER_JOB_EMPTY << ARINC429_TX_JOB_JOBCODE_POS;
			channel->dwell_time[j]            = 0;
		}

		// scheduler frame buffers
		for(uint16_t j = 0; j < ARINC429_TX_BUFFER_NUM; j++)
		{
			channel->frame_buffer[j]          = 0;
		}
	}

	// initialize all RX channels
	for(uint8_t i = 0; i < ARINC429_RX_CHANNELS_NUM; i++)
	{
		// get pointer to channel
		ARINC429RXChannel *channel = &(arinc429.rx_channel[i]);

		// operating mode
		channel->common.parity_speed          = (ARINC429_PARITY_AUTO << 4) | (ARINC429_SPEED_LS             << 0);
		channel->common.operating_mode        =                               (ARINC429_CHANNEL_MODE_PASSIVE << 0);
		channel->common.callback_mode         =                               (ARINC429_CALLBACK_OFF         << 0);

		// statistics
		channel->common.frames_processed_curr = 0;
		channel->common.frames_processed_last = 0;
		channel->common.frames_lost_curr      = 0;
		channel->common.frames_lost_last      = 0;

		// timeout check
		channel->last_timeout_scan            = 0;
		channel->timeout_period               = 1000;

		// priority filters
		channel->prio_mode                    = ARINC429_PRIORITY_DISABLED;

		for(uint8_t j = 0; j < 3; j++)
		{
			channel->prio_label[j]            = 0;
		}

		// frame filters
		for(uint16_t j = 0; j < ARINC429_RX_FILTERS_NUM; j++)
		{
			channel->frame_filter[j]          = ARINC429_RX_FILTER_UNUSED;
		}

		// frame buffers
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
	arinc429.callback.seq_number              = 0;

	for(uint16_t i = 0; i < ARINC429_CB_QUEUE_SIZE; i++)
	{
		arinc429.callback.queue[i].message    = ARINC429_CALLBACK_JOB_NONE;
		arinc429.callback.queue[i].buffer     = 0;
	}


	// done
	return;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

