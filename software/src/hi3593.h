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

#endif