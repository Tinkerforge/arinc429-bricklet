/* arinc429-bricklet
 * Copyright (C) 2020 Olaf Lüke <olaf@tinkerforge.com>
 *
 * hi3593.c: Driver for HI-3593 ARINC429 IC
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

#include "hi3593.h"

#include "bricklib2/hal/ccu4_pwm/ccu4_pwm.h"
#include "bricklib2/utility/util_definitions.h"
#include "bricklib2/os/coop_task.h"
#include "bricklib2/logging/logging.h"

#include "opcode_length.inc"


/****************************************************************************/
/* data structures                                                          */
/****************************************************************************/

// data structure used for all driver functions
HI3593  hi3593;

// pin and port numbers to interface with the discrete signals of the A429 chip
const uint8_t hi3593_input_pins[HI3593_INPUT_PINS_NUM] =
{
	HI3593_MB11_PIN,
	HI3593_MB12_PIN,
	HI3593_MB13_PIN,
	HI3593_MB21_PIN,
	HI3593_MB22_PIN,
	HI3593_MB23_PIN,
	HI3593_R2INT_PIN,
	HI3593_R2FLAG_PIN,
	HI3593_R1INT_PIN,
	HI3593_R1FLAG_PIN,
	HI3593_TEMPTY_PIN,
	HI3593_TFULL_PIN
};

XMC_GPIO_PORT_t *const hi3593_input_ports[HI3593_INPUT_PINS_NUM] =
{
	HI3593_MB11_PORT,
	HI3593_MB12_PORT,
	HI3593_MB13_PORT,
	HI3593_MB21_PORT,
	HI3593_MB22_PORT,
	HI3593_MB23_PORT,
	HI3593_R2INT_PORT,
	HI3593_R2FLAG_PORT,
	HI3593_R1INT_PORT,
	HI3593_R1FLAG_PORT,
	HI3593_TEMPTY_PORT,
	HI3593_TFULL_PORT
};


/****************************************************************************/
/* local functions                                                          */
/****************************************************************************/

/* SPI bus initialization */
static void hi3593_init_spi(void)
{
	hi3593.spi_fifo.channel             = HI3593_USIC_SPI;
	hi3593.spi_fifo.baudrate            = HI3593_SPI_BAUDRATE;

	hi3593.spi_fifo.rx_fifo_size        = HI3593_RX_FIFO_SIZE;
	hi3593.spi_fifo.rx_fifo_pointer     = HI3593_RX_FIFO_POINTER;
	hi3593.spi_fifo.tx_fifo_size        = HI3593_TX_FIFO_SIZE;
	hi3593.spi_fifo.tx_fifo_pointer     = HI3593_TX_FIFO_POINTER;

	hi3593.spi_fifo.slave               = HI3593_SLAVE;
	hi3593.spi_fifo.clock_output        = HI3593_CLOCK_OUTPUT;
	hi3593.spi_fifo.clock_passive_level = HI3593_CLOCK_PASSIVE_LEVEL;

	hi3593.spi_fifo.sclk_pin            = HI3593_SCLK_PIN;
	hi3593.spi_fifo.sclk_port           = HI3593_SCLK_PORT;
	hi3593.spi_fifo.sclk_pin_mode       = HI3593_SCLK_PIN_MODE;

	hi3593.spi_fifo.select_pin          = HI3593_SELECT_PIN;
	hi3593.spi_fifo.select_port         = HI3593_SELECT_PORT;
	hi3593.spi_fifo.select_pin_mode     = HI3593_SELECT_PIN_MODE;

	hi3593.spi_fifo.mosi_pin            = HI3593_MOSI_PIN;
	hi3593.spi_fifo.mosi_port           = HI3593_MOSI_PORT;
	hi3593.spi_fifo.mosi_pin_mode       = HI3593_MOSI_PIN_MODE;

	hi3593.spi_fifo.miso_pin            = HI3593_MISO_PIN;
	hi3593.spi_fifo.miso_port           = HI3593_MISO_PORT;
	hi3593.spi_fifo.miso_input          = HI3593_MISO_INPUT;
	hi3593.spi_fifo.miso_source         = HI3593_MISO_SOURCE;

	spi_fifo_init(&hi3593.spi_fifo);

	return;
}


/****************************************************************************/
/* global functions                                                         */
/****************************************************************************/

/* initialize driver data structure */
void hi3593_init_data(void)
{
	// clear data structure
	memset(&hi3593, 0, sizeof(HI3593));

	// done
	return;
}


/* initialize XMC (host) chip */
void hi3593_init_chip(void)
{
	// initialize SPI interface
	hi3593_init_spi();

	// prepare configuration data for input pins
	const XMC_GPIO_CONFIG_t pin_config_input =
	{
		.mode             = XMC_GPIO_MODE_INPUT_TRISTATE,
		.input_hysteresis = XMC_GPIO_INPUT_HYSTERESIS_STANDARD
	};

	// configure input pins
	for(uint8_t i = 0; i < HI3593_INPUT_PINS_NUM; i++)
	{
		XMC_GPIO_Init(hi3593_input_ports[i], hi3593_input_pins[i], &pin_config_input);
	}

	// prepare configuration data for output pins
	const XMC_GPIO_CONFIG_t pin_config_output =
	{
		.mode         = XMC_GPIO_MODE_OUTPUT_PUSH_PULL,
		.output_level = XMC_GPIO_OUTPUT_LEVEL_LOW,
	};

	// configure output pins
	XMC_GPIO_Init(HI3593_RESET,  &pin_config_output);
	XMC_GPIO_Init(HI3593_RX_LED, &pin_config_output);
	XMC_GPIO_Init(HI3593_TX_LED, &pin_config_output);

	// create 1 MHz clock on HI3593 CLOCK pin
	ccu4_pwm_init(HI3593_CLOCK, HI3593_CLOCK_SLICE_NUMBER, HI3593_CLOCK_PERIOD-1);
	ccu4_pwm_set_duty_cycle(HI3593_CLOCK_SLICE_NUMBER, HI3593_CLOCK_PERIOD/2);

	// configure RX/TX LEDs
	hi3593.led_flicker_state_rx.config = LED_FLICKER_CONFIG_STATUS;
	hi3593.led_flicker_state_tx.config = LED_FLICKER_CONFIG_STATUS;

	// done
	return;
}


/* SPI write access to A429 chip */
uint32_t hi3593_write_register(const uint8_t opcode, const uint8_t *data, const uint8_t length)
{
	// create buffer and load opcode
	uint8_t opcode_and_data[33] = {opcode};		// 1 byte opcode + 32 byte data

	// copy data from input to buffer
	memcpy(opcode_and_data+1, data, length);

	// execute SPI transfer
	const bool ret = spi_fifo_coop_transceive(&hi3593.spi_fifo, length+1, opcode_and_data, opcode_and_data);

	// done
	return ret ? 0 : 1;
}


/* SPI read access to A429 chip */
uint32_t hi3593_read_register(const uint8_t opcode, uint8_t *data, const uint8_t length)
{
	// create buffer and load opcode
	uint8_t opcode_and_data[33] = {(1 << 7) | opcode};		// 1 byte opcode + 32 byte data

	// execute SPI transfer
	const bool ret = spi_fifo_coop_transceive(&hi3593.spi_fifo, length+1, opcode_and_data, opcode_and_data);

	// copy data from buffer to output
	memcpy(data, opcode_and_data+1, length);

	// done
	return ret ? 0 : 1;
}


/****************************************************************************/
/* task & tick functions                                                    */
/****************************************************************************/

void hi3593_tick(void)
{
	// operate the RX/TX LEDs
	led_flicker_tick(&hi3593.led_flicker_state_rx, system_timer_get_ms(), HI3593_RX_LED);
	led_flicker_tick(&hi3593.led_flicker_state_tx, system_timer_get_ms(), HI3593_TX_LED);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
