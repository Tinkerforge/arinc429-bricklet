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


#include "arinc429.h"
#include "hi3593.h"
#include "communication.h"

#include "bricklib2/utility/communication_callback.h"
#include "bricklib2/protocols/tfp/tfp.h"
#include "bricklib2/logging/logging.h"

#include "xmc_gpio.h"


extern const int8_t opcode_length[256];


// labels used for checking channel parameter
#define GROUP_TX   (1 << 0)               // check for TX, TX1
#define GROUP_RX   (1 << 1)               // check for RX, RX1, RX2
#define GROUP_ALL  (GROUP_TX | GROUP_RX)  // check for all of above



/****************************************************************************/
/* init and tick functions                                                  */
/****************************************************************************/

/* communication initialization, called from main() */
void communication_init(void)
{
	communication_callback_init();
}


/* communication tick, called from main task */
void communication_tick(void)
{
	communication_callback_tick();
}


/****************************************************************************/
/* message dispatcher                                                       */
/****************************************************************************/

BootloaderHandleMessageResponse handle_message(const void *message, void *response)      // %%%  thread C
{
	switch(tfp_get_fid_from_message(message))
	{
		case FID_DEBUG_GET_DISCRETES                  : return debug_get_discretes                  (message, response);
		case FID_DEBUG_READ_REGISTER_LOW_LEVEL        : return debug_read_register_low_level        (message, response);
		case FID_DEBUG_WRITE_REGISTER_LOW_LEVEL       : return debug_write_register_low_level       (message, response);

		case FID_GET_CAPABILITIES                     : return get_capabilities                     (message, response);

		case FID_SET_HEARTBEAT_CALLBACK_CONFIGURATION : return set_heartbeat_callback_configuration (message          );
		case FID_GET_HEARTBEAT_CALLBACK_CONFIGURATION : return get_heartbeat_callback_configuration (message, response);

		case FID_SET_CHANNEL_CONFIGURATION            : return set_channel_configuration            (message          );
		case FID_GET_CHANNEL_CONFIGURATION            : return get_channel_configuration            (message, response);

		case FID_SET_CHANNEL_MODE                     : return set_channel_mode                     (message          );
		case FID_GET_CHANNEL_MODE                     : return get_channel_mode                     (message, response);

		case FID_CLEAR_PRIO_LABELS                    : return clear_prio_labels                    (message          );
		case FID_SET_PRIO_LABELS                      : return set_prio_labels                      (message          );
		case FID_GET_PRIO_LABELS                      : return get_prio_labels                      (message, response);

		case FID_CLEAR_ALL_RX_LABEL_FILTERS           : return clear_all_rx_filters                 (message          );
		case FID_CLEAR_RX_LABEL_FILTER                : return clear_rx_filter                      (message          );
		case FID_SET_RX_LABEL_FILTER                  : return set_rx_filter                        (message          );
		case FID_GET_RX_LABEL_FILTER                  : return get_rx_filter                        (message, response);

		case FID_READ_NEXT_FRAME                      : return read_frame                           (message, response);

		case FID_SET_RECEIVE_CALLBACK_CONFIGURATION   : return set_rx_callback_configuration        (message          );
		case FID_GET_RECEIVE_CALLBACK_CONFIGURATION   : return get_rx_callback_configuration        (message, response);

		case FID_WRITE_FRAME_DIRECT                   : return write_frame_direct                   (message          );
		case FID_WRITE_FRAME_SCHEDULED                : return write_frame_scheduled                (message          );

		case FID_CLEAR_SCHEDULE_ENTRIES               : return clear_schedule_entries               (message          );
		case FID_SET_SCHEDULE_ENTRY                   : return set_schedule_entry                   (message          );
		case FID_GET_SCHEDULE_ENTRY                   : return get_schedule_entry                   (message, response);

		default                                       : return HANDLE_MESSAGE_RESPONSE_NOT_SUPPORTED;
	}
}


/****************************************************************************/
/* local helper functions                                                   */
/****************************************************************************/

/* check channel parameter */
static bool check_channel(const uint8_t channel, const uint8_t group)
{
	// check TX group, allowed are TX and TX1
	if( (group & GROUP_TX) && (    (channel == ARINC429_CHANNEL_TX )
	                            || (channel == ARINC429_CHANNEL_TX1) ))
	{
		// allowed
		return true;
	}

	// check RX group, allowed are RX, RX1 and RX2
	if( (group & GROUP_RX) && (    (channel == ARINC429_CHANNEL_RX )
	                            || (channel == ARINC429_CHANNEL_RX1)
	                            || (channel == ARINC429_CHANNEL_RX2) ))
	{
		// allowed
		return true;
	}

	// not allowed
	return false;
}


/* clear a RX filter and free the frame buffer if applicable */
/* helper function to clear_rx_filter()                      */
void clear_rx_filter_helper(uint8_t channel_index, uint8_t label, uint8_t sdi)
{
	ARINC429RXChannel *channel;
	uint16_t           index;
	uint8_t            buffer;


	// get pointer to channel
	channel = &(arinc429.rx_channel[channel_index]);

	// compute filter index from SDI and label, replacing SDI_DATA by SDI 0
	index = ((sdi & 0x03) << 8) | label;

	// retrieve buffer index
	buffer = channel->frame_filter[index];

	// disable the filter
	channel->frame_filter[index] = ARINC429_RX_FILTER_UNUSED;

	// SDI set to data?
	if(sdi == ARINC429_SDI_DATA)
	{
		// yes, disable the filters for the remaining 3 SDI values
		channel->frame_filter[index + (1 << 8)] = ARINC429_RX_FILTER_UNUSED;
		channel->frame_filter[index + (2 << 8)] = ARINC429_RX_FILTER_UNUSED;
		channel->frame_filter[index + (3 << 8)] = ARINC429_RX_FILTER_UNUSED;

		// free the frame buffer
		channel->frame_buffer[buffer].frame_age = ARINC429_RX_BUFFER_UNUSED;
	}
	else
	{
		// no, is the buffer used by any of the other SDI values?
		// (the own filter is already set to ARINC429_RX_FILTER_UNUSED)
		if(    (channel->frame_filter[label + (0 << 8)] != buffer)
		    && (channel->frame_filter[label + (1 << 8)] != buffer)
		    && (channel->frame_filter[label + (2 << 8)] != buffer)
		    && (channel->frame_filter[label + (3 << 8)] != buffer) )
		{
			// no, can free the frame buffer
			channel->frame_buffer[buffer].frame_age = ARINC429_RX_BUFFER_UNUSED;
		}
	}

	// done
	return;
}


/* find a free storage location in the RX frame buffer */
/* helper function for set_rx_filter_helper()          */
bool find_free_rx_frame_buffer(uint8_t channel_index, uint8_t *buffer)
{
	uint8_t  buffer_index = 0;

	// scan frame buffers for an unused buffer
	do
	{
		// buffer at index unused?
		if(arinc429.rx_channel[channel_index].frame_buffer[buffer_index].frame_age == ARINC429_RX_BUFFER_UNUSED)
		{
			// yes, export buffer index and signal success
			*buffer = buffer_index;
			return true;
		}
	}
	while(++buffer_index < (ARINC429_RX_BUFFER_NUM - 1));

	// signal no free buffer found
	return false;
}


/* set up a RX frame filter            */
/* helper function for set_rx_filter() */
bool set_rx_filter_helper(uint8_t channel_index, uint8_t label, uint8_t sdi)
{
	ARINC429RXChannel *channel;
	uint8_t           *filter;    // pointer to filter
	uint16_t           index;     // index to frame_filter[]
	uint8_t            buffer;    // index to frame_buffer[]

	// compute filter index from SDI and label, replacing SDI_DATA by SDI 0
	index = ((sdi & 0x03) << 8) | label;

	// get pointers to channel and filter
	channel = &(arinc429.rx_channel[channel_index]);
	filter  = &(channel->frame_filter[index]);

	// retrieve currently assigned buffer
	buffer = *filter;

	// does the filter already have a frame buffer assigned?
	if(buffer == ARINC429_RX_FILTER_UNUSED)
	{
		// no, find a free frame buffer, abort if none found
		if(find_free_rx_frame_buffer(channel_index, &buffer) == false) return false;
	}
	else
	{
		// yes, is the filter set up for an individual SDI?
		if(sdi != ARINC429_SDI_DATA)
		{
			// yes, check if the buffer is also used by another filter
			for(uint16_t i = 0; i < ARINC429_RX_FILTERS_NUM; i++)
			{
				// skip own filter
				if(i == index) continue;

				// own buffer also used by this foreign filter?
				if(channel->frame_filter[i] == buffer)
				{
					// yes, find a new frame buffer, abort if none found
					if(find_free_rx_frame_buffer(channel_index, &buffer) == false) return false;

					// new buffer found, done with check
					break;
				}
			}
		}
	}

	// disable the filter while updating
	*filter = ARINC429_RX_FILTER_UNUSED;

	// configure the frame buffer
	channel->frame_buffer[buffer].frame        = 0;
	channel->frame_buffer[buffer].frame_age    = ARINC429_RX_BUFFER_UNUSED;
	channel->frame_buffer[buffer].last_rx_time = 0;

	// update done, activate the filter
	*filter = buffer;

	// SDI used for data?
	if(sdi == ARINC429_SDI_DATA)
	{
		// yes, activate filter for the remaining 3 SDI values,
		// with all filters pointing to the same frame buffer
		channel->frame_filter[index + (1 << 8)] = buffer;
		channel->frame_filter[index + (2 << 8)] = buffer;
		channel->frame_filter[index + (3 << 8)] = buffer;
	}

	// done, filter successfully set up
	return true;
}


/****************************************************************************/
/* message handlers                                                         */
/****************************************************************************/

/*** debug functions ****/

/* get status of the discretes */
BootloaderHandleMessageResponse debug_get_discretes(const DebugGetDiscretes *data, DebugGetDiscretes_Response *response)
{
	// prepare response
	response->header.length = sizeof(DebugGetDiscretes_Response);

	// assemble response data
	response->rx_discretes  = (XMC_GPIO_GetInput(hi3593_input_ports[HI3593_R1INT_INDEX],  hi3593_input_pins[HI3593_R1INT_INDEX] ) << 0) |
	                          (XMC_GPIO_GetInput(hi3593_input_ports[HI3593_R1FLAG_INDEX], hi3593_input_pins[HI3593_R1FLAG_INDEX]) << 1) |
	                          (XMC_GPIO_GetInput(hi3593_input_ports[HI3593_MB13_INDEX],   hi3593_input_pins[HI3593_MB13_INDEX]  ) << 2) |
	                          (XMC_GPIO_GetInput(hi3593_input_ports[HI3593_MB12_INDEX],   hi3593_input_pins[HI3593_MB12_INDEX]  ) << 3) |
	                          (XMC_GPIO_GetInput(hi3593_input_ports[HI3593_MB11_INDEX],   hi3593_input_pins[HI3593_MB11_INDEX]  ) << 4) |
	                          (XMC_GPIO_GetInput(hi3593_input_ports[HI3593_R2INT_INDEX],  hi3593_input_pins[HI3593_R2INT_INDEX] ) << 5) |
	                          (XMC_GPIO_GetInput(hi3593_input_ports[HI3593_R2FLAG_INDEX], hi3593_input_pins[HI3593_R2FLAG_INDEX]) << 6) |
	                          (XMC_GPIO_GetInput(hi3593_input_ports[HI3593_MB23_INDEX],   hi3593_input_pins[HI3593_MB23_INDEX]  ) << 7) |
	                          (XMC_GPIO_GetInput(hi3593_input_ports[HI3593_MB22_INDEX],   hi3593_input_pins[HI3593_MB22_INDEX]  ) << 8) |
	                          (XMC_GPIO_GetInput(hi3593_input_ports[HI3593_MB21_INDEX],   hi3593_input_pins[HI3593_MB21_INDEX]  ) << 9);

	response->tx_discretes  = (XMC_GPIO_GetInput(hi3593_input_ports[HI3593_TEMPTY_INDEX], hi3593_input_pins[HI3593_TEMPTY_INDEX]) << 0) |
	                          (XMC_GPIO_GetInput(hi3593_input_ports[HI3593_TFULL_INDEX],  hi3593_input_pins[HI3593_TFULL_INDEX] ) << 1);

	// done
	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}


/* execute a SPI read access to the A429 chip */
BootloaderHandleMessageResponse debug_read_register_low_level(const DebugReadRegisterLowLevel          *data,
                                                                    DebugReadRegisterLowLevel_Response *response)
{
	// prepare response
	response->header.length = sizeof(DebugReadRegisterLowLevel_Response);

	// check opcode
	if(opcode_length[data->op_code] < 0)
	{
		// invalid opcode
		response->rw_error     = ARINC429_RW_ERROR_INVALID_OP_CODE;
		response->value_length = 0;
	}
	else if((data->op_code & (1 << 7)) == 0)
	{
		// opcode does not belong to a read function
		response->rw_error     = ARINC429_RW_ERROR_NO_READ;
		response->value_length = 0;
	}
	else
	{
		// execute read access
		uint32_t ret = hi3593_task_read_register(data->op_code, response->value_data, opcode_length[data->op_code]);

		// check for success
		if(ret == 0)
		{
			// read access succeeded
			response->rw_error     = ARINC429_RW_ERROR_OK;
			response->value_length = opcode_length[data->op_code];
		}
		else
		{
			// read access failed
			response->rw_error     = ARINC429_RW_ERROR_SPI;
			response->value_length = 0;
		}
	}

	// done
	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}


/* execute a SPI write access to the A429 chip */
BootloaderHandleMessageResponse debug_write_register_low_level(const DebugWriteRegisterLowLevel          *data,
                                                                     DebugWriteRegisterLowLevel_Response *response)
{
	// check value_legth parameter, abort if invalid
	if(data->value_length > 32)  return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;

	// prepare response
	response->header.length = sizeof(DebugWriteRegisterLowLevel_Response);

	// check opcode
	if(opcode_length[data->op_code] < 0)
	{
		// invalid opcode
		response->rw_error = ARINC429_RW_ERROR_INVALID_OP_CODE;
	}
	else if(data->op_code & (1 << 7))
	{
		// opcode does not belong to a write function
		response->rw_error = ARINC429_RW_ERROR_NO_WRITE;
	}
	else if(opcode_length[data->op_code] != data->value_length)
	{
		// number of data bytes provided does not match with opcode
		response->rw_error = ARINC429_RW_ERROR_INVALID_LENGTH;
	}
	else
	{
		// execute write access
		uint32_t ret = hi3593_task_write_register(data->op_code, data->value_data, opcode_length[data->op_code]);

		// check for success
		if(ret == 0)
		{
			// write access succeeded
			response->rw_error = ARINC429_RW_ERROR_OK;
		}
		else
		{
			// write access failed
			response->rw_error = ARINC429_RW_ERROR_SPI;
		}
	}

	// done
	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}


/*** user-level functions ****/


/* get the capabilities of the bricklet */
BootloaderHandleMessageResponse get_capabilities(const GetCapabilities          *data,
                                                       GetCapabilities_Response *response)
{
	// prepare response
	response->header.length      = sizeof(GetCapabilities_Response);

	// collect response data
	response->rx_channels         = ARINC429_RX_CHANNELS_NUM;    // number of available RX channels
	response->rx_frame_filters    = ARINC429_RX_BUFFER_NUM - 1;  // number of available RX frame filters (one buffer is reserved for internal purpose)
	response->tx_channels         = ARINC429_TX_CHANNELS_NUM;    // number of available TX channels
	response->tx_schedule_entries = ARINC429_TX_TASKS_NUM;       // number of available TX schedule job   entries
	response->tx_schedule_frames  = ARINC429_TX_BUFFER_NUM;      // number of available TX schedule frame entries

	// done
	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}


/* set heartbeat configuration */
BootloaderHandleMessageResponse set_heartbeat_callback_configuration(const SetHeartbeatCallbackConfiguration *data)
{
	uint8_t  mode;

	// determine mode - step 1: on_change or not
	mode = (data->value_has_to_change == true) ? ARINC429_CALLBACK_ON_CHANGE : ARINC429_CALLBACK_ON;

	// determine mode - step 2: on(_change) or off
	if(data->period == 0)  mode = ARINC429_CALLBACK_OFF;

	// store new configuration
	arinc429.heartbeat.period = (uint16_t)(data->period * 1000);  // period is stored in ms
	arinc429.heartbeat.mode   = mode;

	// restart the sequence numbers
	arinc429.heartbeat.seq_number = 0;

	// reset statistics counters - TX channel(s)
	for(uint8_t i = 0; i < ARINC429_TX_CHANNELS_NUM; i++)
	{
		arinc429.tx_channel[i].common.frames_processed_curr = arinc429.tx_channel[i].common.frames_processed_last = 0;
		arinc429.tx_channel[i].common.frames_lost_curr      = arinc429.tx_channel[i].common.frames_lost_last      = 0;
	}

	// reset statistics counters - RX channels
	for(uint8_t i = 0; i < ARINC429_RX_CHANNELS_NUM; i++)
	{
		arinc429.rx_channel[i].common.frames_processed_curr = arinc429.rx_channel[i].common.frames_processed_last = 0;
		arinc429.rx_channel[i].common.frames_lost_curr      = arinc429.rx_channel[i].common.frames_lost_last      = 0;
	}

	// set time reference
	arinc429.heartbeat.last_time = system_timer_get_ms();

	// done
	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}


/* get heartbeat configuration */
BootloaderHandleMessageResponse get_heartbeat_callback_configuration(const GetHeartbeatCallbackConfiguration          *data,
                                                                           GetHeartbeatCallbackConfiguration_Response *response)
{
	// prepare response
	response->header.length = sizeof(GetHeartbeatCallbackConfiguration_Response);

	// collect response
	switch(arinc429.heartbeat.mode)
	{
		default                          : /* FALLTHROUGH */
		case ARINC429_CALLBACK_OFF       : response->value_has_to_change = false;  break;
		case ARINC429_CALLBACK_ON        : response->value_has_to_change = false;  break;
		case ARINC429_CALLBACK_ON_CHANGE : response->value_has_to_change = true;   break;
	}

	response->period = (uint8_t)(arinc429.heartbeat.period / 1000);  // period was stored in ms

	// done
	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}


/* set the channel configuration */
BootloaderHandleMessageResponse set_channel_configuration(const SetChannelConfiguration *data)
{
	// check parameters, abort if invalid
	if(!check_channel(data->channel, GROUP_ALL))  return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	if( data->parity  > ARINC429_PARITY_AUTO   )  return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	if( data->speed   > ARINC429_SPEED_LS      )  return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;

	// do TX channel(s)
	for(uint8_t i = 0; i < ARINC429_TX_CHANNELS_NUM; i++)
	{
		// channel selected?
		if((data->channel == ARINC429_CHANNEL_TX) || (data->channel == ARINC429_CHANNEL_TX1 + i))
		{
			// yes, indicate a pending configuration change
			arinc429.tx_channel[i].common.pending_change = 1;

			// update channel configuration
			arinc429.tx_channel[i].common.parity_speed = ARINC429_CHANGE_REQUEST | (data->parity << 4) | (data->speed << 0);
		}
	}

	// do RX channels
	for(uint8_t i = 0; i < ARINC429_RX_CHANNELS_NUM; i++)
	{
		// channel selected?
		if((data->channel == ARINC429_CHANNEL_RX) || (data->channel == ARINC429_CHANNEL_RX1 + i))
		{
			// yes, indicate a pending configuration change
			arinc429.rx_channel[i].common.pending_change = 1;

			// update channel configuration
			arinc429.rx_channel[i].common.parity_speed = ARINC429_CHANGE_REQUEST | (data->parity << 4) | (data->speed << 0);
		}
	}

	// done
	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}


/* get the channel configuration */
BootloaderHandleMessageResponse get_channel_configuration(const GetChannelConfiguration          *data,
                                                                GetChannelConfiguration_Response *response)
{
	ARINC429Common *config;

	// prepare response
	response->header.length = sizeof(GetChannelConfiguration_Response);

	switch(data->channel)
	{
		default                   : return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;

		case ARINC429_CHANNEL_TX1 : config = &(arinc429.tx_channel[0].common);  break;
		case ARINC429_CHANNEL_RX1 : config = &(arinc429.rx_channel[0].common);  break;
		case ARINC429_CHANNEL_RX2 : config = &(arinc429.rx_channel[1].common);  break;
	}

	// collect response
	response->parity  = (config->parity_speed & 0x70) ? 1 : 0;
	response->speed   = (config->parity_speed & 0x07) ? 1 : 0;

	// done
	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}


/* set the channel operating mode (passive/active/run) */
BootloaderHandleMessageResponse set_channel_mode(const SetChannelMode *data)
{
	// check parameters, abort if invalid
	if(!check_channel(data->channel, GROUP_ALL))  return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;

	// do TX channel(s)
	for(uint8_t i = 0; i < ARINC429_TX_CHANNELS_NUM; i++)
	{
		// channel selected?
		if((data->channel == ARINC429_CHANNEL_TX) || (data->channel == ARINC429_CHANNEL_TX1 + i))
		{
			// yes, check mode parameter, abort if invalid
			if(data->mode > ARINC429_CHANNEL_MODE_RUN)  return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;

			// indicate a pending configuration change
			arinc429.tx_channel[i].common.pending_change = 1;

			// update channel configuration
			arinc429.tx_channel[i].common.operating_mode = ARINC429_CHANGE_REQUEST | data->mode;
		}
	}

	// do RX channels
	for(uint8_t i = 0; i < ARINC429_RX_CHANNELS_NUM; i++)
	{
		// channel selected?
		if((data->channel == ARINC429_CHANNEL_RX) || (data->channel == ARINC429_CHANNEL_RX1 + i))
		{
			// yes, check mode parameter, abort if invalid
			if(data->mode > ARINC429_CHANNEL_MODE_ACTIVE)  return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;

			// indicate a pending configuration change
			arinc429.rx_channel[i].common.pending_change = 1;

			// update channel configuration
			arinc429.rx_channel[i].common.operating_mode = ARINC429_CHANGE_REQUEST | data->mode;
		}
	}

	// done
	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}


/* get the channel operating mode */
BootloaderHandleMessageResponse get_channel_mode(const GetChannelMode          *data,
                                                       GetChannelMode_Response *response)
{
	ARINC429Common *config;

	// prepare response
	response->header.length = sizeof(GetChannelMode_Response);

	// switch on selected channel
	switch(data->channel)
	{
		default                   : return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;

		case ARINC429_CHANNEL_TX1 : config = &(arinc429.tx_channel[0].common); break;
		case ARINC429_CHANNEL_RX1 : config = &(arinc429.rx_channel[0].common); break;
		case ARINC429_CHANNEL_RX2 : config = &(arinc429.rx_channel[1].common); break;
	}

	// collect response
	response->mode = config->operating_mode & 0x07;

	// done
	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}


/* disable the priority filters */
BootloaderHandleMessageResponse clear_prio_labels(const ClearPrioLabels *data)
{
	// check channel parameter, abort if invalid
	if(!check_channel(data->channel, GROUP_RX))  return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;

	// do RX channels
	for(uint8_t i = 0; i < ARINC429_RX_CHANNELS_NUM; i++)
	{
		// channel selected?
		if((data->channel == ARINC429_CHANNEL_RX) || (data->channel == ARINC429_CHANNEL_RX1 + i))
		{
			// yes, indicate a pending configuration change
			arinc429.rx_channel[i].common.pending_change = 1;

			// clear priority labels
			memset(arinc429.rx_channel[i].prio_label, 0, 3);

			// disable priority filters
			arinc429.rx_channel[i].prio_mode = ARINC429_CHANGE_REQUEST | ARINC429_PRIORITY_DISABLED;
		}
	}

	// done
	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}


/* set the priority filter configuration */
BootloaderHandleMessageResponse set_prio_labels(const SetPrioLabels *data)
{
	// check channel parameter, abort if invalid
	if(!check_channel(data->channel, GROUP_RX))  return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;

	// do RX channels
	for(uint8_t i = 0; i < ARINC429_RX_CHANNELS_NUM; i++)
	{
		// channel selected?
		if((data->channel == ARINC429_CHANNEL_RX) || (data->channel == ARINC429_CHANNEL_RX1 + i))
		{
			// yes, indicate a pending configuration change
			arinc429.rx_channel[i].common.pending_change = 1;

			// copy priority labels
			memcpy(arinc429.rx_channel[i].prio_label, data->label, 3);

			// enable priority filters
			arinc429.rx_channel[i].prio_mode = ARINC429_CHANGE_REQUEST | ARINC429_PRIORITY_ENABLED;
		}
	}

	// done
	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}


/* get the priority filter configuration */
BootloaderHandleMessageResponse get_prio_labels(const GetPrioLabels          *data,
                                                      GetPrioLabels_Response *response)
{
	ARINC429RXChannel *channel;

	// prepare response
	response->header.length = sizeof(GetPrioLabels_Response);

	// switch on selected channel
	switch(data->channel)
	{
		default                   : return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;

		case ARINC429_CHANNEL_RX1 : channel = &(arinc429.rx_channel[0]);  break;
		case ARINC429_CHANNEL_RX2 : channel = &(arinc429.rx_channel[1]);  break;
	}

	// collect priority labels
	memcpy(response->label,  channel->prio_label, 3);

	// extract mode
	response->prio_enabled = ((channel->prio_mode & 0x07) == ARINC429_PRIORITY_ENABLED) ? true : false;

	// done
	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}


/* clear all RX filters */
BootloaderHandleMessageResponse clear_all_rx_filters(const ClearAllRXLabelFilters *data)
{
	// check channel parameter, abort if invalid
	if(!check_channel(data->channel, GROUP_RX))  return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;

	// do RX channels
	for(uint8_t i = 0; i < ARINC429_RX_CHANNELS_NUM; i++)
	{
		// channel selected?
		if((data->channel == ARINC429_CHANNEL_RX) || (data->channel == ARINC429_CHANNEL_RX1 + i))
		{
			// yes, disable all frame filters
			for(uint16_t j = 0; j < ARINC429_RX_FILTERS_NUM; j++)
			{
				arinc429.rx_channel[i].frame_filter[j] = ARINC429_RX_FILTER_UNUSED;
			}

			// revert all frame buffers to unused state
			for(uint8_t  k = 0; k < (ARINC429_RX_BUFFER_NUM - 1); k++)
			{
				arinc429.rx_channel[i].frame_buffer[k].frame_age = ARINC429_RX_BUFFER_UNUSED;
			}
		}
	}

	// done
	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}


/* clear one RX filter */
BootloaderHandleMessageResponse clear_rx_filter(const ClearRXLabelFilter *data)
{
	// check parameters, abort if invalid
	if(!check_channel(data->channel, GROUP_RX))  return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	if( data->sdi     > ARINC429_SDI_DATA     )  return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;

	// do RX channels
	for(uint8_t i = 0; i < ARINC429_RX_CHANNELS_NUM; i++)
	{
		// channel selected?
		if((data->channel == ARINC429_CHANNEL_RX) || (data->channel == ARINC429_CHANNEL_RX1 + i))
		{
			// yes, clear the filter and free its frame buffer if applicable
			clear_rx_filter_helper(i, data->label, data->sdi);
		}
	}

	// done
	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}


/* set a RX filter configuration */
BootloaderHandleMessageResponse set_rx_filter(const SetRXLabelFilter *data)
{
	bool  success = true;

	// check parameters, abort if invalid
	if(!check_channel(data->channel, GROUP_RX))  return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	if( data->sdi     > ARINC429_SDI_DATA     )  return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;

	// do RX channels
	for(uint8_t i = 0; i < ARINC429_RX_CHANNELS_NUM; i++)
	{
		// channel selected?
		if((data->channel == ARINC429_CHANNEL_RX) || (data->channel == ARINC429_CHANNEL_RX1 + i))
		{
			// yes, try to set up the filter, memorize if failed
			if (set_rx_filter_helper(i, data->label, data->sdi) == false)  success = false;
		}
	}

	// done, report success
	if(success) return HANDLE_MESSAGE_RESPONSE_EMPTY;
	else        return HANDLE_MESSAGE_RESPONSE_NOT_SUPPORTED;
}


/* get a RX filter configuration */
BootloaderHandleMessageResponse get_rx_filter(const GetRXLabelFilter          *data,
                                                    GetRXLabelFilter_Response *response)
{
	ARINC429RXChannel *channel;
	uint16_t           index;
	uint8_t            buffer;

	// prepare response
	response->header.length = sizeof(GetRXLabelFilter_Response);

	// switch on selected channel
	switch(data->channel)
	{
		default                   : return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;

		case ARINC429_CHANNEL_RX1 : channel = &(arinc429.rx_channel[0]);  break;
		case ARINC429_CHANNEL_RX2 : channel = &(arinc429.rx_channel[1]);  break;
	}

	// compute filter index from SDI and label, replacing SDI_DATA by SDI 0
	index = ((data->sdi & 0x03) << 8) | data->label;

	// retrieve buffer index
	buffer = channel->frame_filter[index];

	// filter in use?
	response->configured = (buffer != ARINC429_RX_FILTER_UNUSED) ?  true : false;

	// done
	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}


/* do a direct read of an A429 frame by channel, label and SDI */
BootloaderHandleMessageResponse read_frame(const ReadFrame          *data,
                                                 ReadFrame_Response *response)
{
	ARINC429RXChannel *channel;
	uint16_t           index;
	uint8_t            buffer;

	// prepare response
	response->header.length = sizeof(ReadFrame_Response);

	// switch on selected channel
	switch(data->channel)
	{
		default                   : return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;

		case ARINC429_CHANNEL_RX1 : channel = &(arinc429.rx_channel[0]);  break;
		case ARINC429_CHANNEL_RX2 : channel = &(arinc429.rx_channel[1]);  break;
	}

	// compute filter index from SDI and label, replacing SDI_DATA by SDI 0
	index = ((data->sdi & 0x03) << 8) | data->label;

	// retrieve buffer index
	buffer = channel->frame_filter[index];

	// set default response
	response->status = false;
	response->frame  = 0;
	response->age    = 0;

	// filter activated?
	if((buffer == ARINC429_RX_FILTER_UNUSED))
	{
		// yes, any frame received yet?
		if(channel->frame_buffer[buffer].frame_age != ARINC429_RX_BUFFER_UNUSED)
		{
			// yes, collect response data
			response->status = true;
			response->frame  = channel->frame_buffer[buffer].frame;
			response->age    = channel->frame_buffer[buffer].frame_age;
		}
	}

	// done
	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}


/* set configuration of the RX frame callback */
BootloaderHandleMessageResponse set_rx_callback_configuration(const SetRXCallbackConfiguration *data)
{
	// check parameters, abort if invalid
	if(!check_channel(data->channel, GROUP_RX))  return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	if( data->timeout < 10                    )  return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	if( data->timeout > 60000                 )  return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;

	// determine mode - step 1: on_change or not
	uint8_t mode = (data->value_has_to_change == true) ? ARINC429_CALLBACK_ON_CHANGE : ARINC429_CALLBACK_ON;

	// determine mode - step 2: on(_change) or off
	if(data->enabled == false)  mode = ARINC429_CALLBACK_OFF;

	// do RX channels
	for(uint8_t i = 0; i < ARINC429_RX_CHANNELS_NUM; i++)
	{
		// channel selected?
		if((data->channel == ARINC429_CHANNEL_RX) || (data->channel == ARINC429_CHANNEL_RX1 + i))
		{
			// yes, indicate a pending configuration change
			arinc429.rx_channel[i].common.pending_change = 1;

			// update channel
			arinc429.rx_channel[i].common.callback_mode = ARINC429_CHANGE_REQUEST | mode;
			arinc429.rx_channel[i].timeout_period       = data->timeout;
		}
	}

	// done
	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}


/* get configuration of the RX frame callback */
BootloaderHandleMessageResponse get_rx_callback_configuration(const GetRXCallbackConfiguration          *data,
                                                                    GetRXCallbackConfiguration_Response *response)
{
	ARINC429RXChannel *channel;

	// prepare response
	response->header.length = sizeof(GetRXCallbackConfiguration_Response);

	// switch on selected channel
	switch(data->channel)
	{
		default                   : return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;

		case ARINC429_CHANNEL_RX1 : channel = &(arinc429.rx_channel[0]);  break;
		case ARINC429_CHANNEL_RX2 : channel = &(arinc429.rx_channel[1]);  break;
	}

	// determine the response for 'enabled' and 'value_has_to_change'
	switch(channel->common.callback_mode & 0x07)
	{
		default                          : /* FALLTHROUGH */
		case ARINC429_CALLBACK_OFF       : response->enabled = false; response->value_has_to_change = false;  break;
		case ARINC429_CALLBACK_ON        : response->enabled = true;  response->value_has_to_change = false;  break;
		case ARINC429_CALLBACK_ON_CHANGE : response->enabled = true;  response->value_has_to_change = true;   break;
	}

	// get the response for 'timeout'
	response->timeout = channel->timeout_period;

	// done
	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}


/* send a frame immediately */
BootloaderHandleMessageResponse write_frame_direct(const WriteFrameDirect *data)
{
	// check channel parameter, abort if invalid
	if(!check_channel(data->channel, GROUP_TX))  return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;

	// do TX channel(s)
	for(uint8_t i = 0; i < ARINC429_TX_CHANNELS_NUM; i++)
	{
		// channel selected?
		if((data->channel == ARINC429_CHANNEL_TX) || (data->channel == ARINC429_CHANNEL_TX1 + i))
		{
			// get pointer to channel
			ARINC429TXChannel *channel = &(arinc429.tx_channel[i]);

			// compute next head (modulo ARINC429_TX_QUEUE_SIZE)
			uint8_t next_head = channel->head + 1;
			next_head &= ARINC429_TX_QUEUE_SIZE - 1;

			// TX queue able to accept a frame?
			if(next_head == channel->tail)
			{
				// no, increment statistics counter on lost frames
				channel->common.frames_lost_curr++;
			}
			else
			{
				// yes, enqueue frame
				channel->queue[next_head] = data->frame;

				// update head
				channel->head = next_head;
			}
		}
	}

	// done
	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}


/* define or update a frame sent via the scheduler */
BootloaderHandleMessageResponse write_frame_scheduled(const WriteFrameScheduled *data)
{
	// check parameters, abort if invalid
	if(!check_channel(data->channel, GROUP_TX)     )  return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	if( data->frame_index >= ARINC429_TX_BUFFER_NUM)  return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;

	// do TX channel(s)
	for(uint8_t i = 0; i < ARINC429_TX_CHANNELS_NUM; i++)
	{
		// channel selected?
		if((data->channel == ARINC429_CHANNEL_TX) || (data->channel == ARINC429_CHANNEL_TX1 + i))
		{
			// write frame to TX buffer
			arinc429.tx_channel[i].frame_buffer[data->frame_index] = data->frame;
		}
	}

	// done
	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}


/* clear a range of scheduler entries */
BootloaderHandleMessageResponse clear_schedule_entries(const ClearScheduleEntries *data)
{
	ARINC429TXChannel *channel;

	// check parameters, abort if invalid
	if(!check_channel(data->channel, GROUP_TX)         )  return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	if( data->task_index_first >= ARINC429_TX_TASKS_NUM)  return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	if( data->task_index_first >  data->task_index_last)  return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	if( data->task_index_last  >= ARINC429_TX_TASKS_NUM)  return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;

	// do TX channel(s)
	for(uint8_t i = 0; i < ARINC429_TX_CHANNELS_NUM; i++)
	{
		// channel selected?
		if((data->channel == ARINC429_CHANNEL_TX) || (data->channel == ARINC429_CHANNEL_TX1 + i))
		{
			// yes, get pointer to channel
			channel = &(arinc429.tx_channel[i]);

			// clear job entries (this can be done while the scheduler is running)
			for(uint16_t j = data->task_index_first; j <= data->task_index_last; j++)
			{
				channel->job_frame[j] = ARINC429_SCHEDULER_JOB_EMPTY << ARINC429_TX_JOB_JOBCODE_POS;
			}
		}
	}

	// done
	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}


/* set a scheduler entry */
BootloaderHandleMessageResponse set_schedule_entry(const SetScheduleEntry *data)
{
	ARINC429TXChannel *channel;

	// check parameters, abort if invalid
	if(!check_channel(data->channel, GROUP_TX)             )  return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	if( data->task_index   >= ARINC429_TX_TASKS_NUM        )  return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	if( data->job          >  ARINC429_SCHEDULER_JOB_CYCLIC)  return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	if( data->frame_index  >= ARINC429_TX_BUFFER_NUM       )  return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	if( data->dwell_time   >  250                          )  return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;

	// do TX channel(s)
	for(uint8_t i = 0; i < ARINC429_TX_CHANNELS_NUM; i++)
	{
		// channel selected?
		if((data->channel == ARINC429_CHANNEL_TX) || (data->channel == ARINC429_CHANNEL_TX1 + i))
		{
			// yes, get pointer to channel
			channel = &(arinc429.tx_channel[i]);

			// update task tables
			channel->dwell_time[data->task_index] = data->dwell_time;
			channel->job_frame[data->task_index]  = (data->job << ARINC429_TX_JOB_JOBCODE_POS) | (data->frame_index << ARINC429_TX_JOB_INDEX_POS);
		}
	}

	// done
	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}


/* get a scheduler entry */
BootloaderHandleMessageResponse get_schedule_entry(const GetScheduleEntry          *data,
                                                         GetScheduleEntry_Response *response)
{
	ARINC429TXChannel *channel;

	// prepare response
	response->header.length = sizeof(GetScheduleEntry_Response);

	// check parameter, abort if invalid
	if(data->task_index >= ARINC429_TX_TASKS_NUM)  return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;

	// switch on selected channel
	switch(data->channel)
	{
		default                   : return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;

		case ARINC429_CHANNEL_TX1 : channel = &(arinc429.tx_channel[0]);  break;
	}

	// collect response data
	response->job         = (channel->job_frame[data->task_index] & ARINC429_TX_JOB_JOBCODE_MASK) >> ARINC429_TX_JOB_JOBCODE_POS;
	response->frame_index = (channel->job_frame[data->task_index] & ARINC429_TX_JOB_INDEX_MASK  ) >> ARINC429_TX_JOB_INDEX_POS;

	response->dwell_time  = (response->job == ARINC429_SCHEDULER_JOB_EMPTY) ? 0 : channel->dwell_time[data->task_index];
	response->frame       = (response->job == ARINC429_SCHEDULER_JOB_EMPTY) ? 0 : channel->frame_buffer[response->frame_index];

	// done
	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}



/****************************************************************************/
/* callbacks                                                                */
/****************************************************************************/

// enqueue a callback message request
bool enqueue_message(uint8_t message, uint8_t buffer)
{
	// compute next head position (modulo ARINC429_CB_QUEUE_SIZE)
	uint8_t next_head = arinc429.callback.head + 1;
	next_head &= ARINC429_CB_QUEUE_SIZE - 1;

	// abort if there is no free space in the message queue
	if(next_head == arinc429.callback.tail)  return false;

	// enqueue message request
	arinc429.callback.queue[next_head].message = message;
	arinc429.callback.queue[next_head].buffer  = buffer;

	// update head
	arinc429.callback.head = next_head;

	// done, message successfully enqueued
	return true;
}


/* generate callbacks */
bool handle_callbacks(void)              // %%% thread D
{
	static Heartbeat_Callback  cb_heartbeat;
	static Frame_Callback      cb_frame;

	       uint8_t             message;
	       uint8_t             buffer;

	// done if there is no pending message request in the queue
	if(arinc429.callback.tail == arinc429.callback.head)           return false;

	// done if the last callback is still pending transmission
	if(!bootloader_spitfp_is_send_possible(&bootloader_status.st)) return false;

	// compute next tail position (modulo ARINC429_CB_QUEUE_SIZE)
	uint8_t next_tail = arinc429.callback.tail + 1;
	next_tail &= ARINC429_CB_QUEUE_SIZE - 1;

	// get message and buffer
	message = arinc429.callback.queue[next_tail].message;
	buffer  = arinc429.callback.queue[next_tail].buffer;

	// update tail
	arinc429.callback.tail = next_tail;

	// switch on message type
	switch(message)
	{
		case ARINC429_CALLBACK_JOB_HEARTBEAT :

			//create callback message
			tfp_make_default_header(&cb_heartbeat.header, bootloader_get_uid(), sizeof(Heartbeat_Callback), FID_CALLBACK_HEARTBEAT);

			// collect callback message data
			cb_heartbeat.frames_processed[0] = arinc429.tx_channel[0].common.frames_processed_curr;
			cb_heartbeat.frames_processed[1] = arinc429.rx_channel[0].common.frames_processed_curr;
			cb_heartbeat.frames_processed[2] = arinc429.rx_channel[1].common.frames_processed_curr;
			cb_heartbeat.frames_lost[0]      = arinc429.tx_channel[0].common.frames_lost_curr;
			cb_heartbeat.frames_lost[1]      = arinc429.rx_channel[0].common.frames_lost_curr;
			cb_heartbeat.frames_lost[2]      = arinc429.rx_channel[1].common.frames_lost_curr;
			cb_heartbeat.seq_number          = arinc429.heartbeat.seq_number;

			// increment sequence number, skipping value 0
			if(++arinc429.heartbeat.seq_number == 0)  arinc429.heartbeat.seq_number = 1;

			// start transmission of callback message
			bootloader_spitfp_send_ack_and_message(&bootloader_status, (uint8_t*)&cb_heartbeat, sizeof(Heartbeat_Callback));

			// ARINC429_CALLBACK_JOB_HEARTBEAT done
			break;


		case ARINC429_CALLBACK_JOB_FRAME_RX1 :  /* FALLTHROUGH */
		case ARINC429_CALLBACK_JOB_FRAME_RX2 :

			// create callback message
			tfp_make_default_header(&cb_frame.header, bootloader_get_uid(), sizeof(Frame_Callback), FID_CALLBACK_FRAME_MESSAGE);

			// collect callback message data
			cb_frame.channel      = message & 1;
			cb_frame.frame_status = ARINC429_FRAME_STATUS_UPDATE;
			cb_frame.frame        = arinc429.rx_channel[message & 1].frame_buffer[buffer].frame;
			cb_frame.age          = arinc429.rx_channel[message & 1].frame_buffer[buffer].frame_age;
			cb_frame.seq_number   = arinc429.callback.seq_number;

			// increment sequence number, skipping value 0
			if(++arinc429.callback.seq_number == 0)  arinc429.callback.seq_number = 1;

			// start transmission of callback message
			bootloader_spitfp_send_ack_and_message(&bootloader_status, (uint8_t*)&cb_frame, sizeof(Frame_Callback));

			// ARINC429_CALLBACK_JOB_FRAME_RX* done
			break;


		case ARINC429_CALLBACK_JOB_TIMEOUT_RX1 :  /* FALLTHROUGH */
		case ARINC429_CALLBACK_JOB_TIMEOUT_RX2 :

			// create callback message
			tfp_make_default_header(&cb_frame.header, bootloader_get_uid(), sizeof(Frame_Callback), FID_CALLBACK_FRAME_MESSAGE);

			// collect callback message data
			cb_frame.channel      = message & 1;
			cb_frame.frame_status = ARINC429_FRAME_STATUS_TIMEOUT;
			cb_frame.frame        = arinc429.rx_channel[message & 1].frame_buffer[buffer].frame;
			cb_frame.age          = arinc429.rx_channel[message & 1].timeout_period;
			cb_frame.seq_number   = arinc429.callback.seq_number;

			// increment sequence number, skipping value 0
			if(++arinc429.callback.seq_number == 0)  arinc429.callback.seq_number = 1;

			// start transmission of callback message
			bootloader_spitfp_send_ack_and_message(&bootloader_status, (uint8_t*)&cb_frame, sizeof(Frame_Callback));

			// ARINC429_CALLBACK_JOB_TIMEOUT_RX* done
			break;


		default :

			// erroneous message type - do nothing
			break;
	}

	return true;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
