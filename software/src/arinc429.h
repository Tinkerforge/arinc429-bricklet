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
// #define PACKED

#define ARINC429_CHANNEL_RX_NUM       2
#define ARINC429_CHANNEL_TX_NUM       1
#define ARINC429_CHANNEL_TOTAL_NUM    (ARINC429_CHANNEL_RX_NUM + ARINC429_CHANNEL_TX_NUM)

#define ARINC429_RX_LABEL_NUM         256
#define ARINC429_TX_LABEL_NUM         256
#define ARINC429_RX_PRIO_LABEL_NUM    3

#define ARINC429_RX_FRAME_NUM         256
#define ARINC429_TX_FRAME_NUM         256

#define ARINC429_TX_SLOTS_NUM         512

typedef struct {
	uint16_t frames_processed;
	uint16_t frames_lost;
	uint8_t mode;
	uint8_t heartbeat_period;
	uint8_t heartbeat_timer;

	bool    config_new;
	uint8_t config_parity;
	uint8_t config_speed;
} PACKED ARINC429Common;

#if 1
typedef struct {
	uint32_t data_age;
	uint8_t timeout;
} PACKED ARINC429RXFrame;
#else
typedef struct {
	uint8_t sdi;
	uint32_t data_age[ARINC429_RX_PRIO_LABEL_NUM];
	uint8_t timeout[ARINC429_RX_PRIO_LABEL_NUM];
} PACKED ARINC429RXFrame;
#endif

typedef struct {
	uint8_t prio_enabled;
	uint8_t label[ARINC429_RX_LABEL_NUM];
	uint8_t prio_label[ARINC429_RX_PRIO_LABEL_NUM];
	ARINC429RXFrame frame[ARINC429_RX_FRAME_NUM];
	ARINC429Common common;
} PACKED ARINC429RXChannel;

typedef struct {
	uint16_t job_index;
	uint8_t dweel;
} PACKED ARINC429TXSlot;

typedef struct {
	uint32_t frame;
} PACKED ARINC429TXFrame;

typedef struct {
	uint8_t schedule_slot;
	ARINC429TXSlot tx_slot[ARINC429_TX_SLOTS_NUM];
	ARINC429TXFrame frame[ARINC429_TX_FRAME_NUM];
	ARINC429Common common;
} PACKED ARINC429TXChannel;


typedef struct {
	ARINC429TXChannel tx_channel[ARINC429_CHANNEL_TX_NUM];
	ARINC429RXChannel rx_channel[ARINC429_CHANNEL_RX_NUM];
} PACKED ARINC429;

extern ARINC429 arinc429;

void arinc429_init(void);
void arinc429_tick(void);

#endif