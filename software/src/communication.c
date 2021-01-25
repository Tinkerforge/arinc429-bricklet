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


/****************************************************************************/
/* defines                                                                  */
/****************************************************************************/

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

BootloaderHandleMessageResponse handle_message(const void *message, void *response)
{
	switch(tfp_get_fid_from_message(message))
	{
		case FID_GET_CAPABILITIES                     : return get_capabilities                     (message, response);

		case FID_SET_HEARTBEAT_CALLBACK_CONFIGURATION : return set_heartbeat_callback_configuration (message          );
		case FID_GET_HEARTBEAT_CALLBACK_CONFIGURATION : return get_heartbeat_callback_configuration (message, response);

		case FID_SET_CHANNEL_CONFIGURATION            : return set_channel_configuration            (message          );
		case FID_GET_CHANNEL_CONFIGURATION            : return get_channel_configuration            (message, response);

		case FID_SET_CHANNEL_MODE                     : return set_channel_mode                     (message          );
		case FID_GET_CHANNEL_MODE                     : return get_channel_mode                     (message, response);

		case FID_CLEAR_ALL_RX_FILTERS                 : return clear_all_rx_filters                 (message          );
		case FID_CLEAR_RX_FILTER                      : return clear_rx_filter                      (message, response);
		case FID_SET_RX_STANDARD_FILTERS              : return set_rx_standard_filters              (message          );
		case FID_SET_RX_FILTER                        : return set_rx_filter                        (message, response);
		case FID_GET_RX_FILTER                        : return get_rx_filter                        (message, response);

		case FID_READ_FRAME                           : return read_frame                           (message, response);

		case FID_SET_RECEIVE_CALLBACK_CONFIGURATION   : return set_rx_callback_configuration        (message          );
		case FID_GET_RECEIVE_CALLBACK_CONFIGURATION   : return get_rx_callback_configuration        (message, response);

		case FID_WRITE_FRAME_DIRECT                   : return write_frame_direct                   (message          );
		case FID_WRITE_FRAME_SCHEDULED                : return write_frame_scheduled                (message          );

		case FID_CLEAR_SCHEDULE_ENTRIES               : return clear_schedule_entries               (message          );
		case FID_SET_SCHEDULE_ENTRY                   : return set_schedule_entry                   (message          );
		case FID_GET_SCHEDULE_ENTRY                   : return get_schedule_entry                   (message, response);

		case FID_RESTART                              : return restart                              (message          );

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


/* check the software filter map for a filter assignment */
bool check_sw_filter_map(uint8_t channel_index, uint16_t ext_label)
{
	// get the lower 5 bits from the extended label (SDI + label)
	uint16_t ext_label_lo = (ext_label & 0x001F) >> 0;

	// get the upper 5 bits from the extended label
	uint16_t ext_label_hi = (ext_label & 0x03E0) >> 5;

	// get the map word
	uint32_t map = arinc429.rx_channel[channel_index].frame_filter_map[ext_label_hi];

	// check if there is a filter assigned
	if(map & (1 << ext_label_lo)) return true;
	else                          return false;
}


/* update the software filter map */
void update_sw_filter_map(uint8_t channel_index, uint16_t ext_label, uint8_t task)
{
	// get the lower 5 bits from the extended label (SDI + label)
	uint16_t ext_label_lo = (ext_label & 0x001F) >> 0;

	// get the upper 5 bits from the extended label
	uint16_t ext_label_hi = (ext_label & 0x03E0) >> 5;

	// get a pointer to the respective byte in the map
	uint32_t *map = &(arinc429.rx_channel[channel_index].frame_filter_map[ext_label_hi]);

	// modify the filter map
	switch(task)
	{
		case ARINC429_SET   : *map |=  (1 << ext_label_lo); break; // set   filter -> set   respective bit
		case ARINC429_CLEAR : *map &= ~(1 << ext_label_lo); break; // clear filter -> clear respective bit
		default             :                               break; // unknown task, do nothing
	}

	// done
	return;
}


/* update the hardware filter map                        */
void update_hw_filter_map(uint8_t channel_index, uint8_t label, uint8_t task)
{
	// get the lower 3 bits from the label
	uint8_t label_lo = (label & 0x07) >> 0;

	// get the upper 5 bits from the label
	uint8_t label_hi = (label & 0xF8) >> 3;

	// get a pointer to the respective byte in the map (lowest index is highest byte)
	uint8_t *map = &(arinc429.rx_channel[channel_index].hardware_filter[31 - label_hi]);

	// modify the filter map
	switch(task)
	{
		case ARINC429_SET   : *map |=  (1 << label_lo); break; // set   filter -> set   respective bit
		case ARINC429_CLEAR : *map &= ~(1 << label_lo); break; // clear filter -> clear respective bit
		default             :                           break; // unknown task, do nothing
	}

	// done
	return;
}


/* clear a RX filter and free the frame buffer if applicable */
/* helper function to clear_rx_filter()                      */
bool clear_rx_filter_helper(uint8_t channel_index, uint8_t label, uint8_t sdi)
{
	uint16_t  ext_label;
	uint8_t   buffer_index;

	// get a pointer to the channel
	ARINC429RXChannel *channel = &(arinc429.rx_channel[channel_index]);

	// compute the filter index from the given SDI and label, thereby replace SDI_DATA by SDI 0
	ext_label = ((sdi & 0x03) << 8) | label;

	// abort if the SDI/label combination does not have a filter assigned
	if(!check_sw_filter_map(channel_index, ext_label)) return false;

	// get the assigned frame buffer index
	buffer_index = channel->frame_filter[ext_label];

	// shall remove a SDI_DATA filter?
	if(sdi == ARINC429_SDI_DATA)
	{
		// check if the filter is a SDI_DATA filter, i.e. are the filters
		// for all SDI values enabled and point to the same frame buffer?
		if(    (check_sw_filter_map(channel_index, (0 << 8) | label)   )
		    && (check_sw_filter_map(channel_index, (1 << 8) | label)   )
		    && (check_sw_filter_map(channel_index, (2 << 8) | label)   )
		    && (check_sw_filter_map(channel_index, (3 << 8) | label)   )
		    && (channel->frame_filter[(0 << 8) | label] == buffer_index)
		    && (channel->frame_filter[(1 << 8) | label] == buffer_index)
		    && (channel->frame_filter[(2 << 8) | label] == buffer_index)
		    && (channel->frame_filter[(3 << 8) | label] == buffer_index) )
		{
			// yes, disable the software filter for all SDI values
			update_sw_filter_map(channel_index, (0 << 8) | label, ARINC429_CLEAR);
			update_sw_filter_map(channel_index, (1 << 8) | label, ARINC429_CLEAR);
			update_sw_filter_map(channel_index, (2 << 8) | label, ARINC429_CLEAR);
			update_sw_filter_map(channel_index, (3 << 8) | label, ARINC429_CLEAR);

			// disable the hardware filter
			update_hw_filter_map(channel_index, label, ARINC429_CLEAR);

			// free the frame buffer
			channel->frame_buffer[buffer_index].frame_age = ARINC429_RX_BUFFER_UNUSED;

			// done, filter successfully removed
			return true;
		}
		else
		{
			// no, it is not a SDI_DATA filter, the filter was not removed
			return false;
		}
	}
	else
	{
		// check if the filter is a single SDI filter, i.e. is the frame
		// buffer not used by any enabled filter for other SDI values?
		if(    ((sdi == 0) || (!check_sw_filter_map(channel_index, (0 << 8) | label)) || (channel->frame_filter[(0 << 8) | label] != buffer_index))
		    && ((sdi == 1) || (!check_sw_filter_map(channel_index, (1 << 8) | label)) || (channel->frame_filter[(1 << 8) | label] != buffer_index))
		    && ((sdi == 2) || (!check_sw_filter_map(channel_index, (2 << 8) | label)) || (channel->frame_filter[(2 << 8) | label] != buffer_index))
		    && ((sdi == 3) || (!check_sw_filter_map(channel_index, (3 << 8) | label)) || (channel->frame_filter[(3 << 8) | label] != buffer_index)) )
		{
			//yes, remove the software filter for the given SDI
			update_sw_filter_map(channel_index, ext_label, ARINC429_CLEAR);

			// can the hardware filter be removed, i.e. is there no filter set for any SDI?
			if(    (check_sw_filter_map(channel_index, (0 << 8) | label))
				&& (check_sw_filter_map(channel_index, (1 << 8) | label))
				&& (check_sw_filter_map(channel_index, (2 << 8) | label))
				&& (check_sw_filter_map(channel_index, (3 << 8) | label)) )
			{
				// yes, remove the hardware filter
				update_hw_filter_map(channel_index, label, ARINC429_CLEAR);
			}

			// free the frame buffer
			channel->frame_buffer[buffer_index].frame_age = ARINC429_RX_BUFFER_UNUSED;

			// done, filter successfully removed
			return true;
		}
		else
		{
			// no, it is not a single SDI filter, the filter was not removed
			return false;
		}
	}

	// never get here
	return false;
}


/* find a free storage location in the RX frame buffer */
/* helper function for set_rx_filter_helper()          */
uint8_t find_free_rx_frame_buffer(uint8_t channel_index)
{

	ARINC429RXChannel *channel = &(arinc429.rx_channel[channel_index]);
	uint8_t            index   = 0;

	// scan the frame buffers for an unused buffer
	do
	{
		// is this buffer unused?
		if(channel->frame_buffer[index].frame_age == ARINC429_RX_BUFFER_UNUSED)
		{
			// yes, found a free buffer
			break;
		}
	}
	while(++index < (ARINC429_RX_BUFFER_NUM - 1));

	// return the index of the buffer found (or ARINC429_RX_BUFFER_NUM - 1)
	return index;
}


/* set up a RX frame filter            */
/* helper function for set_rx_filter() */
bool set_rx_filter_helper(uint8_t channel_index, uint8_t label, uint8_t sdi)
{
	uint8_t   buffer_index;

	// abort if all frame buffers are in use already
	if(arinc429.rx_channel[channel_index].frame_buffers_used == ARINC429_RX_BUFFER_NUM) return false;

	// get a pointer to the channel
	ARINC429RXChannel *channel = &(arinc429.rx_channel[channel_index]);

	// find a free frame buffer
	buffer_index = find_free_rx_frame_buffer(channel_index);

	// initialize the frame buffer
	channel->frame_buffer[buffer_index].frame        = 0;
	channel->frame_buffer[buffer_index].frame_age    = ARINC429_RX_BUFFER_EMPTY;
	channel->frame_buffer[buffer_index].last_rx_time = 0;

	// shall create a SDI_DATA filter?
	if(sdi == ARINC429_SDI_DATA)
	{
		// does the SDI_DATA filter not exist yet, i.e. are the
		// software filters for all SDI values disabled up to now?
		if(    (!check_sw_filter_map(channel_index, (0 << 8) | label))
		    && (!check_sw_filter_map(channel_index, (1 << 8) | label))
		    && (!check_sw_filter_map(channel_index, (2 << 8) | label))
		    && (!check_sw_filter_map(channel_index, (3 << 8) | label)) )
		{
			// yes, assign the filters for all SDI values the new frame buffer
			channel->frame_filter[(0 << 8) | label] = buffer_index;
			channel->frame_filter[(1 << 8) | label] = buffer_index;
			channel->frame_filter[(2 << 8) | label] = buffer_index;
			channel->frame_filter[(3 << 8) | label] = buffer_index;

			// activate the software filters
			update_sw_filter_map(channel_index, (0 << 8) | label, ARINC429_SET);
			update_sw_filter_map(channel_index, (1 << 8) | label, ARINC429_SET);
			update_sw_filter_map(channel_index, (2 << 8) | label, ARINC429_SET);
			update_sw_filter_map(channel_index, (3 << 8) | label, ARINC429_SET);

			// activate the hardware filter
			update_hw_filter_map(channel_index, label, ARINC429_SET);

			// done, filter successfully created
			return true;
		}
		else
		{
			// no, can not create the filter
			return false;
		}
	}
	else
	{
		// single SDI filter, compute the extended label from the given SDI and label
		uint16_t  ext_label = ((sdi & 0x03) << 8) | label;

		// does no filter for the given SDI and label exist yet?
		if(!check_sw_filter_map(channel_index, ext_label))
		{
			// yes, assign the filter the new frame buffer
			channel->frame_filter[ext_label] = buffer_index;

			// activate the software filter
			update_sw_filter_map(channel_index, ext_label, ARINC429_SET);

			// activate the hardware filter
			update_hw_filter_map(channel_index, label, ARINC429_SET);

			// done, filter successfully created
			return true;
		}
		else
		{
			// no, can not create the filter
			return false;
		}
	}

	// never get here
	return false;
}


/****************************************************************************/
/* message handlers                                                         */
/****************************************************************************/


/* get the capabilities of the bricklet */
BootloaderHandleMessageResponse get_capabilities(const GetCapabilities          *data,
                                                       GetCapabilities_Response *response)
{
	// prepare the response
	response->header.length      = sizeof(GetCapabilities_Response);

	// collect the response data
	response->tx_total_scheduler_jobs = ARINC429_TX_JOBS_NUM;                         // total number of TX  scheduler job entries
	response->tx_used_scheduler_jobs  = arinc429.tx_channel[0].scheduler_jobs_used;   // number of used  TX  scheduler job entries

	// the number of frame filters is limited by the number of frame buffers
	response->rx_total_frame_filters   = ARINC429_RX_BUFFER_NUM;                      // total number of RX  frame filters
	response->rx_used_frame_filters[0] = arinc429.rx_channel[0].frame_buffers_used;   // number of used  RX1 frame filters
	response->rx_used_frame_filters[1] = arinc429.rx_channel[1].frame_buffers_used;   // number of used  RX2 frame filters

	// done, send the response
	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}


/* set heartbeat configuration */
BootloaderHandleMessageResponse set_heartbeat_callback_configuration(const SetHeartbeatCallbackConfiguration *data)
{
	uint8_t  mode;

	// determine the mode - step 1: on_change or unconditional?
	mode = (data->value_has_to_change == true) ? ARINC429_CALLBACK_ON_CHANGE : ARINC429_CALLBACK_ON;

	// determine the mode - step 2: on(_change) or off?
	if(data->period == 0)  mode = ARINC429_CALLBACK_OFF;

	// store the new configuration
	arinc429.heartbeat.period = (uint16_t)(data->period * 1000);  // period is stored in ms
	arinc429.heartbeat.mode   = mode;

	// reset the sequence number if the mode is set to 'off'
	if(mode == ARINC429_CALLBACK_OFF)
	{
		// restart the sequence numbers from zero
		arinc429.heartbeat.seq_number = 0;
	}

	// set the reference time for the timing of the next heartbeat
	arinc429.heartbeat.last_time = system_timer_get_ms();

	// done, no response
	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}


/* get heartbeat configuration */
BootloaderHandleMessageResponse get_heartbeat_callback_configuration(const GetHeartbeatCallbackConfiguration          *data,
                                                                           GetHeartbeatCallbackConfiguration_Response *response)
{
	// prepare the response
	response->header.length = sizeof(GetHeartbeatCallbackConfiguration_Response);

	// collect the response data
	switch(arinc429.heartbeat.mode)
	{
		default                          : /* FALLTHROUGH */
		case ARINC429_CALLBACK_OFF       : response->value_has_to_change = false;  break;
		case ARINC429_CALLBACK_ON        : response->value_has_to_change = false;  break;
		case ARINC429_CALLBACK_ON_CHANGE : response->value_has_to_change = true;   break;
	}

	response->period = (uint8_t)(arinc429.heartbeat.period / 1000);  // period was stored in ms

	// done, send response
	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}


/* set the channel configuration */
BootloaderHandleMessageResponse set_channel_configuration(const SetChannelConfiguration *data)
{
	// check the parameters, abort if invalid
	if(!check_channel(data->channel, GROUP_ALL))  return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	if( data->parity  > ARINC429_PARITY_AUTO   )  return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	if( data->speed   > ARINC429_SPEED_LS      )  return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;

	// do all TX channels
	for(uint8_t i = 0; i < ARINC429_TX_CHANNELS_NUM; i++)
	{
		// channel selected?
		if((data->channel == ARINC429_CHANNEL_TX) || (data->channel == ARINC429_CHANNEL_TX1 + i))
		{
			// yes, update the channel configuration
			arinc429.tx_channel[i].common.parity_speed = (data->parity << 4) | (data->speed << 0);

			// request execution of the update
			arinc429.tx_channel[i].common.change_request |= ARINC429_UPDATE_SPEED_PARITY;
		}
	}

	// do all RX channels
	for(uint8_t i = 0; i < ARINC429_RX_CHANNELS_NUM; i++)
	{
		// channel selected?
		if((data->channel == ARINC429_CHANNEL_RX) || (data->channel == ARINC429_CHANNEL_RX1 + i))
		{
			// yes, update the channel configuration
			arinc429.rx_channel[i].common.parity_speed = (data->parity << 4) | (data->speed << 0);

			// request execution of the update
			arinc429.rx_channel[i].common.change_request |= ARINC429_UPDATE_SPEED_PARITY;
		}
	}

	// done, no response
	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}


/* get the channel configuration */
BootloaderHandleMessageResponse get_channel_configuration(const GetChannelConfiguration          *data,
                                                                GetChannelConfiguration_Response *response)
{
	ARINC429Common *config;

	// prepare the response
	response->header.length = sizeof(GetChannelConfiguration_Response);

	// pick the selected channel
	switch(data->channel)
	{
		default                   : return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;

		case ARINC429_CHANNEL_TX1 : config = &(arinc429.tx_channel[0].common);  break;
		case ARINC429_CHANNEL_RX1 : config = &(arinc429.rx_channel[0].common);  break;
		case ARINC429_CHANNEL_RX2 : config = &(arinc429.rx_channel[1].common);  break;
	}

	// collect response data
	response->parity  = (config->parity_speed & 0xF0) ? 1 : 0;
	response->speed   = (config->parity_speed & 0x0F) ? 1 : 0;

	// done, send response
	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}


/* set the channel operating mode (passive/active/run) */
BootloaderHandleMessageResponse set_channel_mode(const SetChannelMode *data)
{
	// check the parameters, abort if invalid
	if(!check_channel(data->channel, GROUP_ALL))  return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;

	// do all TX channels
	for(uint8_t i = 0; i < ARINC429_TX_CHANNELS_NUM; i++)
	{
		// channel selected?
		if((data->channel == ARINC429_CHANNEL_TX) || (data->channel == ARINC429_CHANNEL_TX1 + i))
		{
			// yes, check 'mode' parameter, abort if invalid
			if(data->mode > ARINC429_CHANNEL_MODE_RUN)  return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;

			// update the channel operating mode
			arinc429.tx_channel[i].common.operating_mode = data->mode;

			// request execution of the update
			arinc429.tx_channel[i].common.change_request |= ARINC429_UPDATE_OPERATING_MODE;
		}
	}

	// do all RX channels
	for(uint8_t i = 0; i < ARINC429_RX_CHANNELS_NUM; i++)
	{
		// channel selected?
		if((data->channel == ARINC429_CHANNEL_RX) || (data->channel == ARINC429_CHANNEL_RX1 + i))
		{
			// yes, check 'mode' parameter, abort if invalid
			if(data->mode > ARINC429_CHANNEL_MODE_ACTIVE)  return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;

			// update the channel operating mode
			arinc429.rx_channel[i].common.operating_mode = data->mode;

			// request execution of the update
			arinc429.rx_channel[i].common.change_request |= ARINC429_UPDATE_OPERATING_MODE;
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

	// prepare the response
	response->header.length = sizeof(GetChannelMode_Response);

	// pick the selected channel
	switch(data->channel)
	{
		default                   : return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;

		case ARINC429_CHANNEL_TX1 : config = &(arinc429.tx_channel[0].common); break;
		case ARINC429_CHANNEL_RX1 : config = &(arinc429.rx_channel[0].common); break;
		case ARINC429_CHANNEL_RX2 : config = &(arinc429.rx_channel[1].common); break;
	}

	// collect the response data
	response->mode = config->operating_mode;

	// done, send response
	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}


/* clear all RX filters */
BootloaderHandleMessageResponse clear_all_rx_filters(const ClearAllRXLabelFilters *data)
{
	// check the channel parameter, abort if invalid
	if(!check_channel(data->channel, GROUP_RX))  return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;

	// do all RX channels
	for(uint8_t i = 0; i < ARINC429_RX_CHANNELS_NUM; i++)
	{
		// channel selected?
		if((data->channel == ARINC429_CHANNEL_RX) || (data->channel == ARINC429_CHANNEL_RX1 + i))
		{
			// yes, get a pointer to the channel
			ARINC429RXChannel *channel = &(arinc429.rx_channel[i]);

			// disable all software filters
			for(uint8_t j = 0; j < 32; j++)
			{
				channel->frame_filter_map[j] = 0;
			}

			// disable all hardware filters
			for(uint8_t j = 0; j < 32; j++)
			{
				channel->hardware_filter[j] = 0;
			}

			// revert all frame buffers to unused state
			for(uint16_t  j = 0; j < ARINC429_RX_BUFFER_NUM; j++)
			{
				channel->frame_buffer[j].frame_age = ARINC429_RX_BUFFER_UNUSED;
			}

			// no frame buffer is used any more now
			channel->frame_buffers_used = 0;

			// request execution of the FIFO hardware filter update
			channel->common.change_request |= ARINC429_UPDATE_FIFO_FILTER;
		}
	}

	// done, no response
	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}


/* clear one RX filter */
BootloaderHandleMessageResponse clear_rx_filter(const ClearRXLabelFilter          *data,
                                                      ClearRXLabelFilter_Response *response)
{
	// prepare the response
	response->header.length = sizeof(ClearRXLabelFilter_Response);

	// check the parameters, abort if invalid
	if(!check_channel(data->channel, GROUP_RX))  return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	if( data->sdi     > ARINC429_SDI_DATA     )  return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;

	// default is successful filter removal
	response->success = true;

	// do all RX channels
	for(uint8_t i = 0; i < ARINC429_RX_CHANNELS_NUM; i++)
	{
		// channel selected?
		if((data->channel == ARINC429_CHANNEL_RX) || (data->channel == ARINC429_CHANNEL_RX1 + i))
		{
			// yes, try to clear the filter and free its frame buffer, success?
			if(clear_rx_filter_helper(i, data->label, data->sdi))
			{
				// yes, decrement the number of filters in use
				arinc429.rx_channel[i].frame_buffers_used--;

				// request an update of the FIFO hardware filter
				arinc429.rx_channel[i].common.change_request |= ARINC429_UPDATE_FIFO_FILTER;
			}
			else
			{
				// no, filter removal failed at least once
				response->success = false;
			}
		}
	}

	// done, send response
	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}


/* set RX standard filters */
BootloaderHandleMessageResponse set_rx_standard_filters(const SetRXStandardFilters *data)
{
	// check the channel parameter, abort if invalid
	if(!check_channel(data->channel, GROUP_RX))  return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;

	// do all RX channels
	for(uint8_t i = 0; i < ARINC429_RX_CHANNELS_NUM; i++)
	{
		// channel selected?
		if((data->channel == ARINC429_CHANNEL_RX) || (data->channel == ARINC429_CHANNEL_RX1 + i))
		{
			// yes, get a pointer to the channel
			ARINC429RXChannel *channel = &(arinc429.rx_channel[i]);

			// enable all software filters
			for(uint8_t j = 0; j < 32; j++)
			{
				channel->frame_filter_map[j] = ~0;
			}

			// enable all hardware filters
			for(uint8_t j = 0; j < 32; j++)
			{
				channel->hardware_filter[j] = ~0;
			}

			// link the frame buffers
			for(uint16_t j = 0; j < ARINC429_RX_FILTERS_NUM; j++)
			{
				channel->frame_filter[j] = (uint8_t)(j & 0x00FF);
			}

			// initialize all frame buffers
			for(uint16_t j = 0; j < ARINC429_RX_BUFFER_NUM; j++)
			{
				channel->frame_buffer[j].frame        = 0;
				channel->frame_buffer[j].frame_age    = ARINC429_RX_BUFFER_EMPTY;
				channel->frame_buffer[j].last_rx_time = 0;
			}

			// all frame buffers are in use now
			channel->frame_buffers_used = ARINC429_RX_BUFFER_NUM;

			// request execution of the FIFO hardware filter update
			channel->common.change_request |= ARINC429_UPDATE_FIFO_FILTER;
		}
	}

	// done, no response
	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}


/* set one RX filter */
BootloaderHandleMessageResponse set_rx_filter(const SetRXFilter          *data,
                                                    SetRXFilter_Response *response)
{
	// prepare the response
	response->header.length = sizeof(SetRXFilter_Response);

	// check parameters, abort if invalid
	if(!check_channel(data->channel, GROUP_RX))  return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	if( data->sdi     > ARINC429_SDI_DATA     )  return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;

	// default is successful filter creation
	response->success = true;

	// do all RX channels
	for(uint8_t i = 0; i < ARINC429_RX_CHANNELS_NUM; i++)
	{
		// channel selected?
		if((data->channel == ARINC429_CHANNEL_RX) || (data->channel == ARINC429_CHANNEL_RX1 + i))
		{
			// yes, try to set up the filter, success?
			if (set_rx_filter_helper(i, data->label, data->sdi))
			{
				// yes, increment the number of filters in use
				arinc429.rx_channel[i].frame_buffers_used++;

				// request an update of the FIFO hardware filter
				arinc429.rx_channel[i].common.change_request |= ARINC429_UPDATE_FIFO_FILTER;
			}
			else
			{
				// no, filter creation failed at least once
				response->success = false;
			}
		}
	}

	// done, send response
	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}


/* get a RX filter configuration */
BootloaderHandleMessageResponse get_rx_filter(const GetRXFilter          *data,
                                                    GetRXFilter_Response *response)
{
	ARINC429RXChannel *channel;
	uint8_t            channel_index;
	uint8_t            buffer_index[4] = {0,0,0,0};  // initialization for sole purpose of avoiding a compiler warning

	// prepare the response
	response->header.length = sizeof(GetRXFilter_Response);

	// default result is 'filter is configured'
	response->configured = true;

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;

	// pick the selected channel
	switch(data->channel)
	{
		default                   : return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;

		case ARINC429_CHANNEL_RX1 : channel = &(arinc429.rx_channel[0]);  channel_index = 0; break;
		case ARINC429_CHANNEL_RX2 : channel = &(arinc429.rx_channel[1]);  channel_index = 1; break;
	}

	// pick the selected SDI
	switch(data->sdi)
	{
		default                : return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;

		case ARINC429_SDI_DATA : /* FALLTHROUGH */

		case ARINC429_SDI0     : if(!check_sw_filter_map(channel_index,  (0 << 8) | data->label)) { response->configured = false;  break; }
		                         if(data->sdi != ARINC429_SDI_DATA                              ) {                                break; }

		                         buffer_index[0] = channel->frame_filter[(0 << 8) | data->label];

		                         /* FALLTHROUGH */

		case ARINC429_SDI1     : if(!check_sw_filter_map(channel_index,  (1 << 8) | data->label)) { response->configured = false;  break; }
		                         if(data->sdi != ARINC429_SDI_DATA                              ) {                                break; }

		                         buffer_index[1] = channel->frame_filter[(1 << 8) | data->label];

		                         /* FALLTHROUGH */

		case ARINC429_SDI2     : if(!check_sw_filter_map(channel_index,  (2 << 8) | data->label)) { response->configured = false;  break; }
		                         if(data->sdi != ARINC429_SDI_DATA                              ) {                                break; }

		                         buffer_index[2] = channel->frame_filter[(2 << 8) | data->label];

		                         /* FALLTHROUGH */

		case ARINC429_SDI3     : if(!check_sw_filter_map(channel_index,  (3 << 8) | data->label)) { response->configured = false;  break; }
		                         if(data->sdi != ARINC429_SDI_DATA                              ) {                                break; }

		                         buffer_index[3] = channel->frame_filter[(3 << 8) | data->label];

		                         /* FALLTHROUGH */

		                         // for SDI_DATA, all 4 buffer indices also need to point to the same frame buffer
		                         if(    (buffer_index[0] != buffer_index[1])
		                             || (buffer_index[1] != buffer_index[2])
		                             || (buffer_index[2] != buffer_index[3]) )                    { response->configured = false;  break; }
	}

	// done, send the response
	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}


/* do a direct read of an A429 frame by channel, label and SDI */
BootloaderHandleMessageResponse read_frame(const ReadFrame          *data,
                                                 ReadFrame_Response *response)
{
	ARINC429RXChannel *channel;
	uint8_t            channel_index;

	// prepare the response
	response->header.length = sizeof(ReadFrame_Response);

	// pick the selected channel
	switch(data->channel)
	{
		default                   : return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;

		case ARINC429_CHANNEL_RX1 : channel = &(arinc429.rx_channel[0]);  channel_index = 0; break;
		case ARINC429_CHANNEL_RX2 : channel = &(arinc429.rx_channel[1]);  channel_index = 1; break;
	}

	// compute the filter index from the SDI and label, thereby replacing SDI_DATA by SDI 0
	uint16_t ext_label = ((data->sdi & 0x03) << 8) | data->label;

	// does the SDI/label combination have a filter assigned?
	if(check_sw_filter_map(channel_index, ext_label))
	{
		// yes, retrieve the buffer index
		uint8_t buffer_index = channel->frame_filter[ext_label];

		// collect the response data
		switch(channel->frame_buffer[buffer_index].frame_age)
		{
			case ARINC429_RX_BUFFER_EMPTY   : response->status = false;
											  response->frame  = 0;
											  response->age    = 0;
											  break;

			case ARINC429_RX_BUFFER_TIMEOUT : response->status = true;
											  response->frame  = channel->frame_buffer[buffer_index].frame;
											  response->age    = channel->timeout_period;
											  break;

			default                         : response->status = true;
											  response->frame  = channel->frame_buffer[buffer_index].frame;
											  response->age    = channel->frame_buffer[buffer_index].frame_age;
											  break;
		}
	}
	else
	{
		// no, set a default response
		response->status = false;
		response->frame  = 0;
		response->age    = 0;
	}

	// done, send the response
	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}


/* set configuration of the RX frame callback */
BootloaderHandleMessageResponse set_rx_callback_configuration(const SetRXCallbackConfiguration *data)
{
	// check the parameters, abort if invalid
	if(!check_channel(data->channel, GROUP_RX))  return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	if( data->timeout < 10                    )  return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	if( data->timeout > 60000                 )  return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;

	// determine the mode - step 1: on_change or not
	uint8_t mode = (data->value_has_to_change == true) ? ARINC429_CALLBACK_ON_CHANGE : ARINC429_CALLBACK_ON;

	// determine the mode - step 2: on(_change) or off
	if(data->enabled == false)  mode = ARINC429_CALLBACK_OFF;

	// do all RX channels
	for(uint8_t i = 0; i < ARINC429_RX_CHANNELS_NUM; i++)
	{
		// channel selected?
		if((data->channel == ARINC429_CHANNEL_RX) || (data->channel == ARINC429_CHANNEL_RX1 + i))
		{
			// yes, update the channel data
			arinc429.rx_channel[i].common.callback_mode = mode;
			arinc429.rx_channel[i].timeout_period       = data->timeout;

			// request execution of the update
			arinc429.rx_channel[i].common.change_request |= ARINC429_UPDATE_CALLBACK_MODE;
		}
	}

	// done, no response
	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}


/* get configuration of the RX frame callback */
BootloaderHandleMessageResponse get_rx_callback_configuration(const GetRXCallbackConfiguration          *data,
                                                                    GetRXCallbackConfiguration_Response *response)
{
	ARINC429RXChannel *channel;

	// prepare the response
	response->header.length = sizeof(GetRXCallbackConfiguration_Response);

	// pick the selected channel
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

	// done, send response
	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}


/* send a frame immediately */
BootloaderHandleMessageResponse write_frame_direct(const WriteFrameDirect *data)
{
	uint8_t  next_head;  // head position in immediate transmit queue

	// check the channel parameter, abort if invalid
	if(!check_channel(data->channel, GROUP_TX))  return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;

	// do all TX channels
	for(uint8_t i = 0; i < ARINC429_TX_CHANNELS_NUM; i++)
	{
		// channel selected?
		if((data->channel == ARINC429_CHANNEL_TX) || (data->channel == ARINC429_CHANNEL_TX1 + i))
		{
			// get a pointer to the channel
			ARINC429TXChannel *channel = &(arinc429.tx_channel[i]);

			// compute the next head position
			if(++next_head >= ARINC429_TX_QUEUE_SIZE) next_head = 0;

			// is the immediate transmit queue able to accept a frame?
			if(next_head == channel->tail)
			{
				// no, increment the statistics counter on lost frames
				channel->common.frames_lost_curr++;
			}
			else
			{
				// yes, enqueue the frame
				channel->queue[next_head] = data->frame;

				// update the head position
				channel->head = next_head;
			}
		}
	}

	// done, no response
	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}


/* define or update a frame sent via the scheduler */
BootloaderHandleMessageResponse write_frame_scheduled(const WriteFrameScheduled *data)
{
	// check the parameters, abort if invalid
	if(!check_channel(data->channel, GROUP_TX)     )  return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	if( data->frame_index >= ARINC429_TX_BUFFER_NUM)  return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;

	// do all TX channels
	for(uint8_t i = 0; i < ARINC429_TX_CHANNELS_NUM; i++)
	{
		// channel selected?
		if((data->channel == ARINC429_CHANNEL_TX) || (data->channel == ARINC429_CHANNEL_TX1 + i))
		{
			// yes, store frame in the TX frame table
			arinc429.tx_channel[i].frame_buffer[data->frame_index] = data->frame;

			// eligible the frame for transmit
			update_tx_buffer_map(i, data->frame_index, ARINC429_SET);
		}
	}

	// done, no response
	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}


/* clear a range of scheduler entries */
BootloaderHandleMessageResponse clear_schedule_entries(const ClearScheduleEntries *data)
{
	// check the parameters, abort if invalid
	if(!check_channel(data->channel, GROUP_TX)       )  return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	if( data->job_index_first >= ARINC429_TX_JOBS_NUM)  return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	if( data->job_index_first >  data->job_index_last)  return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	if( data->job_index_last  >= ARINC429_TX_JOBS_NUM)  return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;

	// do all TX channels
	for(uint8_t i = 0; i < ARINC429_TX_CHANNELS_NUM; i++)
	{
		// channel selected?
		if((data->channel == ARINC429_CHANNEL_TX) || (data->channel == ARINC429_CHANNEL_TX1 + i))
		{
			// yes, get a pointer to the channel
			ARINC429TXChannel *channel = &(arinc429.tx_channel[i]);

			// clear the task entries (this can be done while the scheduler is running)
			for(uint16_t j = data->job_index_first; j <= data->job_index_last; j++)
			{
				// is the task already unused?
				if((channel->job_frame[j] & ARINC429_TX_JOB_JOBCODE_MASK) != (ARINC429_SCHEDULER_JOB_SKIP << ARINC429_TX_JOB_JOBCODE_POS))
				{
					// no, set the task to 'skip' aka unused
					channel->job_frame[j] = ARINC429_SCHEDULER_JOB_SKIP << ARINC429_TX_JOB_JOBCODE_POS;

					// decrement the number of used task entries
					channel->scheduler_jobs_used--;
				}
			}
		}
	}

	// done, no response
	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}


/* set a scheduler entry */
BootloaderHandleMessageResponse set_schedule_entry(const SetScheduleEntry *data)
{
	// check the parameters, abort if invalid
	if(!check_channel(data->channel, GROUP_TX)                  )  return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	if( data->job_index    >= ARINC429_TX_JOBS_NUM              )  return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	if( data->job          >  ARINC429_SCHEDULER_JOB_RETRANS_RX2)  return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	if( data->dwell_time   >  250                               )  return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;

	// check the frame index parameter
	switch(data->job)
	{
		// transmit from TX frame buffer - abort on invalid TX frame table index
		case ARINC429_SCHEDULER_JOB_SINGLE      : /* FALLTHROUGH */
		case ARINC429_SCHEDULER_JOB_CYCLIC      :

			if(data->frame_index < ARINC429_TX_BUFFER_NUM) break; else return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;


		// transmit from RX frame buffer - abort on invalid extended label (SDI + label)
		case ARINC429_SCHEDULER_JOB_RETRANS_RX1 : /* FALLTHROUGH */
		case ARINC429_SCHEDULER_JOB_RETRANS_RX2 :

			if(data->frame_index < 0x0400                ) break; else return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;


		// callback label - abort on invalid label number
		case ARINC429_SCHEDULER_JOB_CALLBACK    :

			if(data->frame_index < 0x0100                ) break; else return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;

		// jump command
		case ARINC429_SCHEDULER_JOB_JUMP        :

			if(data->frame_index < ARINC429_TX_JOBS_NUM  ) break; else return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;

		// any other job not using the frame index parameter
		default:                                           break;
	}

	// do all TX channels
	for(uint8_t i = 0; i < ARINC429_TX_CHANNELS_NUM; i++)
	{
		// channel selected?
		if((data->channel == ARINC429_CHANNEL_TX) || (data->channel == ARINC429_CHANNEL_TX1 + i))
		{
			// yes, get a pointer to the channel
			ARINC429TXChannel *channel = &(arinc429.tx_channel[i]);

			// is the task in use already?
			if((channel->job_frame[data->job_index] & ARINC429_TX_JOB_JOBCODE_MASK) == (ARINC429_SCHEDULER_JOB_SKIP << ARINC429_TX_JOB_JOBCODE_POS))
			{
				// no, increment the number of used task entries
				channel->scheduler_jobs_used++;
			}

			// update the task table
			channel->dwell_time[data->job_index] = data->dwell_time;
			channel->job_frame [data->job_index] = (data->job << ARINC429_TX_JOB_JOBCODE_POS) | (data->frame_index << ARINC429_TX_JOB_INDEX_POS);
		}
	}

	// done, no response
	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}


/* get a scheduler entry */
BootloaderHandleMessageResponse get_schedule_entry(const GetScheduleEntry          *data,
                                                         GetScheduleEntry_Response *response)
{
	ARINC429TXChannel *channel;

	// prepare the response
	response->header.length = sizeof(GetScheduleEntry_Response);

	// check the parameter, abort if invalid
	if(data->job_index >= ARINC429_TX_JOBS_NUM)  return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;

	// pick the selected channel
	switch(data->channel)
	{
		default                   : return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;

		case ARINC429_CHANNEL_TX1 : channel = &(arinc429.tx_channel[0]);  break;
	}

	// collect the response data
	response->job         = (channel->job_frame[data->job_index] & ARINC429_TX_JOB_JOBCODE_MASK) >> ARINC429_TX_JOB_JOBCODE_POS;
	response->frame_index = (channel->job_frame[data->job_index] & ARINC429_TX_JOB_INDEX_MASK  ) >> ARINC429_TX_JOB_INDEX_POS;

	response->dwell_time  = (response->job == ARINC429_SCHEDULER_JOB_SKIP) ? 0 : channel->dwell_time  [data->job_index     ];
	response->frame       = (response->job == ARINC429_SCHEDULER_JOB_SKIP) ? 0 : channel->frame_buffer[response->frame_index];

	// done, send the response
	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}


/* restart the bricklet */
BootloaderHandleMessageResponse restart(const Restart *data)
{
//	// request a reset of the A429 data structure and chip
//	arinc429.system.change_request |= (ARINC429_SYSTEM_RESET_A429_DATA | ARINC429_SYSTEM_RESET_A429_CHIP);

	// request a complete reset of everything
	arinc429.system.change_request = 0xFF;

	// done, no response
	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}


/****************************************************************************/
/* callbacks                                                                */
/****************************************************************************/

// enqueue a callback message
bool enqueue_message(uint8_t message, uint16_t timestamp, uint8_t buffer)
{
	// get the current head position in the callback queue
	uint16_t next_head = arinc429.callback.head;

	// compute the next head position
	if(++next_head >= ARINC429_CB_QUEUE_SIZE) next_head = 0;

	// abort if there is no free space in the message queue
	if(next_head == arinc429.callback.tail)  return false;

	// enqueue the message
	arinc429.callback.queue[next_head].message   = message;
	arinc429.callback.queue[next_head].timestamp = timestamp;
	arinc429.callback.queue[next_head].buffer    = buffer;

	// update the head position
	arinc429.callback.head = next_head;

	// done, message successfully enqueued
	return true;
}


/* generate callbacks */
bool handle_callbacks(void)
{
	static Heartbeat_Callback  cb_heartbeat;
	static Frame_Callback      cb_frame;
	static Scheduler_Callback  cb_scheduler;
	
	// done if there is no pending message request in the queue
	if(arinc429.callback.tail == arinc429.callback.head)           return false;

	// done if the last callback is still pending transmission
	if(!bootloader_spitfp_is_send_possible(&bootloader_status.st)) return false;

	// get the current tail position in the callback queue
	uint16_t next_tail = arinc429.callback.tail;

	// compute the next tail position
	if(++next_tail >= ARINC429_CB_QUEUE_SIZE) next_tail = 0;

	// get the message data
	uint8_t  message   = arinc429.callback.queue[next_tail].message;
	uint8_t  buffer    = arinc429.callback.queue[next_tail].buffer;
	uint16_t timestamp = arinc429.callback.queue[next_tail].timestamp;

	// update the tail position
	arinc429.callback.tail = next_tail;

	// switch on the message type to send
	switch(message)
	{
		case ARINC429_CALLBACK_JOB_HEARTBEAT :

			//create the callback message
			tfp_make_default_header(&cb_heartbeat.header, bootloader_get_uid(), sizeof(Heartbeat_Callback), FID_CALLBACK_HEARTBEAT);

			// collect the callback message data
			cb_heartbeat.seq_number          = arinc429.heartbeat.seq_number;
			cb_heartbeat.timestamp           = timestamp;
			cb_heartbeat.frames_processed[0] = arinc429.tx_channel[0].common.frames_processed_curr;
			cb_heartbeat.frames_processed[1] = arinc429.rx_channel[0].common.frames_processed_curr;
			cb_heartbeat.frames_processed[2] = arinc429.rx_channel[1].common.frames_processed_curr;
			cb_heartbeat.frames_lost[0]      = arinc429.tx_channel[0].common.frames_lost_curr;
			cb_heartbeat.frames_lost[1]      = arinc429.rx_channel[0].common.frames_lost_curr;
			cb_heartbeat.frames_lost[2]      = arinc429.rx_channel[1].common.frames_lost_curr;

			// increment the sequence number, thereby skipping the value 0
			if(++arinc429.heartbeat.seq_number == 0)  arinc429.heartbeat.seq_number = 1;

			// send the callback message
			bootloader_spitfp_send_ack_and_message(&bootloader_status, (uint8_t*)&cb_heartbeat, sizeof(Heartbeat_Callback));

			// ARINC429_CALLBACK_JOB_HEARTBEAT done
			break;


		case ARINC429_CALLBACK_JOB_FRAME_RX1 :  /* FALLTHROUGH */
		case ARINC429_CALLBACK_JOB_FRAME_RX2 :

			// create the callback message
			tfp_make_default_header(&cb_frame.header, bootloader_get_uid(), sizeof(Frame_Callback), FID_CALLBACK_FRAME_MESSAGE);

			// collect the callback message data
			cb_frame.channel    = ARINC429_CHANNEL_RX1 + (message & 1);
			cb_frame.seq_number = arinc429.callback.seq_number_frame;
			cb_frame.timestamp  = timestamp;
			cb_frame.frame      = arinc429.rx_channel[message & 1].frame_buffer[buffer].frame;

			// new or update?
			if(arinc429.rx_channel[message & 1].frame_buffer[buffer].frame_age > 60000)
			{
				// new
				cb_frame.frame_status = ARINC429_FRAME_STATUS_NEW;
				cb_frame.age          = 0;
			}
			else
			{
				// update
				cb_frame.frame_status = ARINC429_FRAME_STATUS_UPDATE;
				cb_frame.age          = arinc429.rx_channel[message & 1].frame_buffer[buffer].frame_age;
			}

			// increment the sequence number, thereby skipping the value 0
			if(++arinc429.callback.seq_number_frame == 0)  arinc429.callback.seq_number_frame = 1;

			// send the callback message
			bootloader_spitfp_send_ack_and_message(&bootloader_status, (uint8_t*)&cb_frame, sizeof(Frame_Callback));

			// ARINC429_CALLBACK_JOB_FRAME_RX* done
			break;


		case ARINC429_CALLBACK_JOB_TIMEOUT_RX1 :  /* FALLTHROUGH */
		case ARINC429_CALLBACK_JOB_TIMEOUT_RX2 :

			// create the callback message
			tfp_make_default_header(&cb_frame.header, bootloader_get_uid(), sizeof(Frame_Callback), FID_CALLBACK_FRAME_MESSAGE);

			// collect the callback message data
			cb_frame.channel      = ARINC429_CHANNEL_RX1 + (message & 1);
			cb_frame.seq_number   = arinc429.callback.seq_number_frame;
			cb_frame.timestamp    = timestamp;
			cb_frame.frame_status = ARINC429_FRAME_STATUS_TIMEOUT;
			cb_frame.frame        = arinc429.rx_channel[message & 1].frame_buffer[buffer].frame;
			cb_frame.age          = arinc429.rx_channel[message & 1].timeout_period;

			// increment the sequence number, thereby skipping the value 0
			if(++arinc429.callback.seq_number_frame == 0)  arinc429.callback.seq_number_frame = 1;

			// send the callback message
			bootloader_spitfp_send_ack_and_message(&bootloader_status, (uint8_t*)&cb_frame, sizeof(Frame_Callback));

			// ARINC429_CALLBACK_JOB_TIMEOUT_RX* done
			break;


		case ARINC429_CALLBACK_JOB_SCHEDULER_CB :

			// create the callback message
			tfp_make_default_header(&cb_scheduler.header, bootloader_get_uid(), sizeof(Scheduler_Callback), FID_CALLBACK_SCHEDULER_MESSAGE);

			// collect the callback message data
			cb_scheduler.channel      = ARINC429_CHANNEL_TX1 + (message & 1);
			cb_scheduler.seq_number   = arinc429.callback.seq_number_scheduler;
			cb_scheduler.timestamp    = timestamp;
			cb_scheduler.token        = buffer;

			// increment the sequence number, thereby skipping the value 0
			if(++arinc429.callback.seq_number_scheduler == 0)  arinc429.callback.seq_number_scheduler = 1;

			// send the callback message
			bootloader_spitfp_send_ack_and_message(&bootloader_status, (uint8_t*)&cb_scheduler, sizeof(Scheduler_Callback));

			// ARINC429_CALLBACK_JOB_SCHEDULER_CB done
			break;


		default :

			// erroneous message type - do nothing
			break;
	}

	return true;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
