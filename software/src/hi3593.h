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


/****************************************************************************/
/* DATA STRUCTURES                                                          */
/****************************************************************************/

typedef struct {

	// RX / TX LEDs
	LEDFlickerState led_flicker_state_rx;
	LEDFlickerState led_flicker_state_tx;

	// SPI bus with A429 chip
	SPIFifo spi_fifo;
}
HI3593;


/****************************************************************************/
/* PROTOTYPES                                                               */
/****************************************************************************/

extern HI3593                  hi3593;

extern const uint8_t           hi3593_input_pins[HI3593_INPUT_PINS_NUM];
extern XMC_GPIO_PORT_t  *const hi3593_input_ports[HI3593_INPUT_PINS_NUM];

void     hi3593_tick          (void);
void     hi3593_init_data     (void);
void     hi3593_init_chip     (void);
uint32_t hi3593_write_register(const uint8_t opcode, const uint8_t *data, const uint8_t length);
uint32_t hi3593_read_register (const uint8_t opcode,       uint8_t *data, const uint8_t length);


/****************************************************************************/
/* DEFINES                                                                  */
/****************************************************************************/

// A429 SPI opcodes

// hardware setup
#define HI3593_CMD_MASTER_RESET     0x04    // master reset
#define HI3593_CMD_WRITE_FLAG_IRQ   0x34    // discretes     setup
#define HI3593_CMD_WRITE_ACLK_DIV   0x38    // clock divider setup

// TX channel
#define HI3593_CMD_WRITE_TX1_CTRL   0x08    // general  control  - write
#define HI3593_CMD_READ_TX1_CTRL    0x84    // general  control  - read
#define HI3593_CMD_WRITE_TX1_FIFO   0x0C    // transmit buffer

// RX1 channel
#define HI3593_CMD_WRITE_RX1_CTRL   0x10    // general  control - write
#define HI3593_CMD_READ_RX1_CTRL    0x94    // general  control - read
#define HI3593_CMD_WRITE_RX1_PRIO   0x18    // priority control - write
#define HI3593_CMD_READ_RX1_PRIO    0x9C    // priority control - read
#define HI3593_CMD_WRITE_RX1_FILTER 0x14    // hardware filter  - write
#define HI3593_CMD_READ_RX1_FIFO    0xA0    // FIFO  buffer
#define HI3593_CMD_READ_RX1_PRIO1   0xA4    // prio1 buffer
#define HI3593_CMD_READ_RX1_PRIO2   0xA8    // prio2 buffer
#define HI3593_CMD_READ_RX1_PRIO3   0xAC    // prio3 buffer

// RX2 channel
#define HI3593_CMD_WRITE_RX2_CTRL   0x24    // general  control - write
#define HI3593_CMD_READ_RX2_CTRL    0xB4    // general  control - read
#define HI3593_CMD_WRITE_RX2_PRIO   0x2C    // priority control - write
#define HI3593_CMD_READ_RX2_PRIO    0xBC    // priority control - read
#define HI3593_CMD_WRITE_RX2_FILTER 0x28    // hardware filter  - write
#define HI3593_CMD_READ_RX2_FIFO    0xC0    // FIFO  buffer
#define HI3593_CMD_READ_RX2_PRIO1   0xC4    // prio1 buffer
#define HI3593_CMD_READ_RX2_PRIO2   0xC8    // prio2 buffer
#define HI3593_CMD_READ_RX2_PRIO3   0xCC    // prio3 buffer


#endif  // HI3593_H

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
