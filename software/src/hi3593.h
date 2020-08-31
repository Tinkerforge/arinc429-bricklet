/* arinc429-bricklet
 * Copyright (C) 2020 Olaf Lüke <olaf@tinkerforge.com>
 *
 * hi3593.h: Driver for HI-3593 ARINC429 IC
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

#ifndef HI3593_H
#define HI3593_H

#include "configs/config_hi3593.h"
#include "bricklib2/hal/spi_fifo/spi_fifo.h"
#include "bricklib2/utility/led_flicker.h"

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    LEDFlickerState led_flicker_state_rx;
    LEDFlickerState led_flicker_state_tx;

    SPIFifo spi_fifo;
} HI3593;

extern HI3593 hi3593;
extern const uint8_t hi3593_input_pins[HI3593_INPUT_PINS_NUM];
extern XMC_GPIO_PORT_t *const hi3593_input_ports[HI3593_INPUT_PINS_NUM];

void hi3593_init(void);
void hi3593_tick(void);
uint32_t hi3593_task_write_register(const uint8_t opcode, const uint8_t *data, const uint8_t length);
uint32_t hi3593_task_read_register(const uint8_t opcode, uint8_t *data, const uint8_t length);


#define HI3593_INDEX_MASK          0x0FFF
#define HI3593_JOB_MASK            0xF000
#define HI3593_JOB_MUTE            0x0000
#define HI3593_JOB_SINGLE          0x1000
#define HI3593_JOB_CYCLIC          0x2000

#define HI3593_RX_LABEL_CONFIGURED 0x8000
#define HI3593_RX_LABEL_SDI_ADDR   0x4000
#define HI3593_RX_LABEL_INDEX_MASK 0x0FFF

#define HI3593_RX_FRAME_AGE_MASK   0x000000FF

#define HI3593_FRAME_LABEL_MASK    0x000000FF
#define HI3593_FRAME_DATA_MASK     0xFFFFFF00


#define HI3593_CMD_MASTER_RESET    0x04

#define HI3593_CMD_WRITE_FLAG_IRQ  0x34
#define HI3593_CMD_WRITE_ACLK_DIV  0x38

#define HI3593_CMD_WRITE_TX1_CTRL  0x08
#define HI3593_CMD_READ_TX1_CTRL   0x84
#define HI3593_CMD_WRITE_TX1_FIFO  0x0C

#define HI3593_CMD_WRITE_RX1_CTRL  0x10
#define HI3593_CMD_READ_RX1_CTRL   0x94
#define HI3593_CMD_WRITE_RX1_PRIO  0x18
#define HI3593_CMD_READ_RX1_PRIO   0x9C
#define HI3593_CMD_READ_RX1_FIFO   0xA0
#define HI3593_CMD_READ_RX1_PRIO1  0xA4
#define HI3593_CMD_READ_RX1_PRIO2  0xA8
#define HI3593_CMD_READ_RX1_PRIO3  0xAC

#define HI3593_CMD_WRITE_RX2_CTRL  0x24
#define HI3593_CMD_READ_RX2_CTRL   0xB4
#define HI3593_CMD_WRITE_RX2_PRIO  0x2C
#define HI3593_CMD_READ_RX2_PRIO   0xBC
#define HI3593_CMD_READ_RX2_FIFO   0xC0
#define HI3593_CMD_READ_RX2_PRIO1  0xC4
#define HI3593_CMD_READ_RX2_PRIO2  0xC8
#define HI3593_CMD_READ_RX2_PRIO3  0xCC

#endif