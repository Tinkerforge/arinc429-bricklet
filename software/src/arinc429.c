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

// Update parity and speed if necessary
void arinc429_task_update_channel_config(void) {
	if(arinc429.tx_channel[0].common.config_new) {
		uint8_t ctrl = (arinc429.tx_channel[0].common.config_parity << 2) | (arinc429.tx_channel[0].common.config_speed << 0);
		if(hi3593_task_write_register(HI3593_CMD_WRITE_TX1_CTRL, &ctrl, opcode_length[HI3593_CMD_WRITE_TX1_CTRL]) == 0) {
			arinc429.tx_channel[0].common.config_new = false;
			arinc429.tx_channel[0].common.mode = ARINC429_CHANNEL_MODE_ACTIVE;
		}
	}

	const uint8_t reg_ctrl[2] = {HI3593_CMD_WRITE_RX1_CTRL, HI3593_CMD_WRITE_RX2_CTRL};
	const uint8_t reg_read[2] = {HI3593_CMD_READ_RX1_FIFO, HI3593_CMD_READ_RX2_FIFO};
	for(uint8_t i = 0; i < ARINC429_CHANNEL_RX_NUM; i++) {
		if(arinc429.rx_channel[i].common.config_new) {
			// Clear potential remains in RX FIFO
			for(uint8_t j = 0; j < 32; j++) {
				uint8_t tmp[4];
				hi3593_task_read_register(reg_read[i], tmp, opcode_length[reg_read[i]]);
			}

			uint8_t ctrl = (arinc429.rx_channel[i].common.config_parity << 3) | (arinc429.rx_channel[i].common.config_speed << 0);
			if(hi3593_task_write_register(reg_ctrl[i], &ctrl, opcode_length[reg_ctrl[i]]) == 0) {
				arinc429.rx_channel[i].common.config_new = false;
				arinc429.rx_channel[0].common.mode = ARINC429_CHANNEL_MODE_ACTIVE;
			}
		}
	}
}
		
void arinc429_task_update_channel_mode(void) {
	if(arinc429.tx_channel[0].common.mode_new) {
		uint8_t ctrl = 0;
		hi3593_task_read_register(HI3593_CMD_READ_TX1_CTRL, &ctrl, opcode_length[HI3593_CMD_READ_TX1_CTRL]);

		if(arinc429.tx_channel[0].common.mode == ARINC429_CHANNEL_MODE_PASSIVE) {
			ctrl |= (1 << 7);
		} else {
			ctrl &= ~(1 << 7);
		}

		if(hi3593_task_write_register(HI3593_CMD_WRITE_TX1_CTRL, &ctrl, opcode_length[HI3593_CMD_WRITE_TX1_CTRL]) == 0) {
			if(arinc429.tx_channel[0].common.mode != ARINC429_CHANNEL_MODE_RUNNING) {
				arinc429.tx_channel[0].schedule_slot = 0;
			}

			arinc429.tx_channel[0].common.mode_new = false;
		}
	}
}

void arinc429_task_update_prio_labels(void) {
	const uint8_t reg_write_prio[2] = {HI3593_CMD_READ_RX1_PRIO1, HI3593_CMD_READ_RX2_PRIO1};
	const uint8_t reg_write_ctrl[2] = {HI3593_CMD_WRITE_RX1_CTRL, HI3593_CMD_WRITE_RX2_CTRL};
	const uint8_t reg_read_ctrl[2]  = {HI3593_CMD_READ_RX1_CTRL,  HI3593_CMD_READ_RX2_CTRL};

	for(uint8_t i = 0; i < ARINC429_CHANNEL_RX_NUM; i++) {
		if(arinc429.rx_channel[i].prio_label_new) {
			if(arinc429.rx_channel[i].prio_enabled) {
				uint8_t prio_label[3] = {
					arinc429.rx_channel[i].prio_label[2],
					arinc429.rx_channel[i].prio_label[1],
					arinc429.rx_channel[i].prio_label[0],
				};
				hi3593_task_write_register(reg_write_prio[i], prio_label, opcode_length[reg_write_prio[i]]);

				uint8_t ctrl = 0;
				hi3593_task_read_register(reg_read_ctrl[i], &ctrl, opcode_length[reg_read_ctrl[i]]);

				ctrl |= (1 << 1);
				if(hi3593_task_write_register(reg_write_ctrl[i], &ctrl, opcode_length[reg_write_ctrl[i]]) == 0) {
					arinc429.rx_channel[i].prio_label_new = false;
				}
			} else { // Clear
				uint8_t ctrl = 0;
				hi3593_task_read_register(reg_read_ctrl[i], &ctrl, opcode_length[reg_read_ctrl[i]]);

				ctrl &= ~(1 << 1);
				if(hi3593_task_write_register(reg_write_ctrl[i], &ctrl, opcode_length[reg_write_ctrl[i]]) == 0) {
					arinc429.rx_channel[i].prio_label_new = false;
				}
			}
		}
	}
}

void arinc429_task_buffer_check(void) {
	// TODO
}

void arinc429_task_timeout_check(void) {
	// TODO
}

void arinc429_task_tx_scheduler(void) {
	// TODO
}

void arinc429_tick_task(void) {
	// Reset
	coop_task_sleep_ms(100);
	hi3593_task_write_register(HI3593_CMD_MASTER_RESET, NULL, 0);
	coop_task_sleep_ms(100);

	// Configure clock
	uint8_t aclk = 0x00; // 1 MHz
	hi3593_task_write_register(HI3593_CMD_WRITE_ACLK_DIV, &aclk, 1);

	while(true) {
		arinc429_task_update_channel_config();
		arinc429_task_update_channel_mode();
		arinc429_task_update_prio_labels();

		arinc429_task_buffer_check();
		arinc429_task_timeout_check();
		arinc429_task_tx_scheduler();

		coop_task_yield();
	}
}

void arinc429_init(void) {
	memset(&arinc429, 0, sizeof(ARINC429));

	for(uint8_t i = 0; i < ARINC429_CHANNEL_RX_NUM; i++) {
		arinc429.rx_channel[i].common.mode = ARINC429_CHANNEL_MODE_UNINIT;
		arinc429.rx_channel[i].common.frames_processed = 0;
		arinc429.rx_channel[i].common.frames_lost = 0;
		arinc429.rx_channel[i].common.heartbeat_period = 0;
		arinc429.rx_channel[i].common.heartbeat_timer = 0;

		arinc429.rx_channel[i].common.config_parity = ARINC429_PARITY_TRANSPARENT;
		arinc429.rx_channel[i].common.config_speed = ARINC429_SPEED_HS;
		arinc429.rx_channel[i].common.config_new = true;

		arinc429.rx_channel[i].prio_enabled  = false;
		for(uint8_t j = 0; j < ARINC429_RX_PRIO_LABEL_NUM; j++) {
			arinc429.rx_channel[i].prio_label[j] = 0;
		}

		for(uint16_t j = 0; j < ARINC429_RX_LABEL_NUM; j++) {
			arinc429.rx_channel[i].label[j] = 0;
		}
	}

	for(uint8_t i = 0; i < ARINC429_CHANNEL_TX_NUM; i++) {
		arinc429.tx_channel[i].common.config_parity = ARINC429_PARITY_TRANSPARENT;
		arinc429.tx_channel[i].common.config_speed = ARINC429_SPEED_HS;
		arinc429.tx_channel[i].common.config_new = true;

		arinc429.tx_channel[i].schedule_slot = 0;

		for(uint16_t j = 0; j < ARINC429_TX_SLOTS_NUM; j++) {
			arinc429.tx_channel[i].tx_slot[j].job_index = ARINC429_SCHEDULER_JOB_MUTE;
			arinc429.tx_channel[i].tx_slot[j].dweel = 0;
		}

		for(uint16_t j = 0; j < ARINC429_TX_FRAME_NUM; j++) {
			arinc429.tx_channel[i].frame[j].frame = 0;
		}
	}

	coop_task_init(&arinc429_task, arinc429_tick_task);
}

void arinc429_tick(void) {
	coop_task_tick(&arinc429_task);
}