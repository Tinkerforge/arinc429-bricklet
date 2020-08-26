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

#include <string.h>

ARINC429 arinc429;

void arinc429_init(void) {
	memset(&arinc429, 0, sizeof(ARINC429));

	for(uint8_t i = 0; i < ARINC429_CHANNEL_TOTAL_NUM; i++) {
		arinc429.channel[i].mode = ARINC429_CHANNEL_MODE_UNINIT;
		arinc429.channel[i].frames_processed = 0;
		arinc429.channel[i].frames_lost = 0;
		arinc429.channel[i].heartbeat_period = 0;
		arinc429.channel[i].heartbeat_timer = 0;
	}

	for(uint8_t i = 0; i < ARINC429_CHANNEL_RX_NUM; i++) {
		arinc429.rx_channel[i].callback_mode = ARINC429_CALLBACK_MODE_OFF;
		arinc429.rx_channel[i].prio_enabled  = false;
		for(uint8_t j = 0; j < ARINC429_RX_PRIO_LABEL_NUM; j++) {
			arinc429.rx_channel[i].prio_label[j] = 0;
		}

		for(uint16_t j = 0; j < ARINC429_RX_LABEL_NUM; j++) {
			arinc429.rx_channel[i].label[j] = 0;
			arinc429.rx_channel[i].frame[j].sdi = 0;
			for(uint8_t k = 0; k < ARINC429_RX_PRIO_LABEL_NUM; k++) {
				arinc429.rx_channel[i].frame[j].data_age[k] = 0;
				arinc429.rx_channel[i].frame[j].timeout[k] = 0;
			}
		}
	}

	for(uint8_t i = 0; i < ARINC429_CHANNEL_TX_NUM; i++) {
		arinc429.tx_channel[i].schedule_slot = 0;
		for(uint8_t j = 0; j < ARINC429_TX_SLOTS_NUM; j++) {
			arinc429.tx_channel[i].tx_slot[j].job_index = ARINC429_SCHEDULE_SLOT_ACTION_MUTE;
			arinc429.tx_channel[i].tx_slot[j].dweel = 0;
		}

		for(uint8_t j = 0; j < ARINC429_TX_LABEL_NUM; j++) {
			arinc429.tx_channel[i].frame[j].frame = 0;
		}
	}
}

void arinc429_tick(void) {

}