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
#include "configs/config_hi3593.h"

#include "bricklib2/hal/ccu4_pwm/ccu4_pwm.h"
#include "bricklib2/utility/util_definitions.h"
#include "bricklib2/os/coop_task.h"
#include "bricklib2/logging/logging.h"

CoopTask hi3593_task;
HI3593 hi3593;

uint8_t hi3593_input_pins[HI3593_INPUT_PINS_NUM] = {
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

XMC_GPIO_PORT_t *hi3593_input_ports[HI3593_INPUT_PINS_NUM] = {
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

void hi3593_write_register(const uint8_t opcode, const uint8_t *data, const uint8_t length) {
	uint8_t opcode_and_data[257] = {opcode};
	memcpy(opcode_and_data+1, data, length);
	bool ret = spi_fifo_coop_transceive(&hi3593.spi_fifo, length+1, opcode_and_data, opcode_and_data);
	// TODO: Check ret, handle error
}

void hi3593_read_register(const uint8_t opcode, uint8_t *data, const uint8_t length) {
	uint8_t opcode_and_data[257] = {(1 << 7) | opcode};
	bool ret = spi_fifo_coop_transceive(&hi3593.spi_fifo, length+1, opcode_and_data, opcode_and_data);
	// TODO: Check ret, handle error
	memcpy(data, opcode_and_data+1, length);
}

void hi3593_task_tick(void) {
	uint8_t test_write = 0b10101010;
	hi3593_write_register(0x10, &test_write, 1);

	uint8_t test_read = 0;
	while(true) {
		hi3593_read_register(0x94, &test_read, 1);
		logd("0x94 -> %d\n\r", test_read);
		hi3593_read_register(0x80, &test_read, 1);
		logd("0x80 -> %d\n\r", test_read);

		coop_task_sleep_ms(500);
		coop_task_yield();
	}
}

static void hi3593_init_spi(void) {
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
}

void hi3593_init(void) {
	memset(&hi3593, 0, sizeof(HI3593));

	hi3593_init_spi();

	const XMC_GPIO_CONFIG_t pin_config_input = {
		.mode             = XMC_GPIO_MODE_INPUT_TRISTATE,
		.input_hysteresis = XMC_GPIO_INPUT_HYSTERESIS_STANDARD
	};

	for(uint8_t i = 0; i < HI3593_INPUT_PINS_NUM; i++) {
		XMC_GPIO_Init(hi3593_input_ports[i], hi3593_input_pins[i], &pin_config_input);
	}

	const XMC_GPIO_CONFIG_t pin_config_output = {
		.mode         = XMC_GPIO_MODE_OUTPUT_PUSH_PULL,
		.output_level = XMC_GPIO_OUTPUT_LEVEL_LOW,
	};
	XMC_GPIO_Init(HI3593_RESET,  &pin_config_output);
	XMC_GPIO_Init(HI3593_RX_LED, &pin_config_output);
	XMC_GPIO_Init(HI3593_TX_LED, &pin_config_output);

	// Create 1MHz Clock on HI3593 CLOCK Pin
	ccu4_pwm_init(HI3593_CLOCK, HI3593_CLOCK_SLICE_NUMBER, HI3593_CLOCK_PERIOD-1);
	ccu4_pwm_set_duty_cycle(HI3593_CLOCK_SLICE_NUMBER, HI3593_CLOCK_PERIOD/2);

	coop_task_init(&hi3593_task, hi3593_task_tick);
}

void hi3593_tick(void) {
	coop_task_tick(&hi3593_task);
}