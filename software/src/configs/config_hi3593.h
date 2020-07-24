/* arinc429-bricklet
 * Copyright (C) 2020 Olaf Lüke <olaf@tinkerforge.com>
 *
 * config_hi3593.h: Configuration for HI-3593 ARINC429 IC
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

#ifndef CONFIG_HI3593_H
#define CONFIG_HI3593_H

#include "xmc_gpio.h"
#include "xmc_spi.h"

#define HI3593_SPI_BAUDRATE           1000000
#define HI3593_USIC_CHANNEL           USIC1_CH1
#define HI3593_USIC_SPI               XMC_SPI1_CH1

#define HI3593_RX_FIFO_SIZE           XMC_USIC_CH_FIFO_SIZE_32WORDS
#define HI3593_RX_FIFO_POINTER        0
#define HI3593_TX_FIFO_SIZE           XMC_USIC_CH_FIFO_SIZE_32WORDS
#define HI3593_TX_FIFO_POINTER        32

#define HI3593_CLOCK_PASSIVE_LEVEL    XMC_SPI_CH_BRG_SHIFT_CLOCK_PASSIVE_LEVEL_0_DELAY_ENABLED
#define HI3593_CLOCK_OUTPUT           XMC_SPI_CH_BRG_SHIFT_CLOCK_OUTPUT_SCLK
#define HI3593_SLAVE                  XMC_SPI_CH_SLAVE_SELECT_0

#define HI3593_SCLK_PORT              XMC_GPIO_PORT0
#define HI3593_SCLK_PIN               3
#define HI3593_SCLK_PIN_MODE          (XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT8 | P0_3_AF_U1C1_SCLKOUT)

#define HI3593_SELECT_PORT            XMC_GPIO_PORT0
#define HI3593_SELECT_PIN             4
#define HI3593_SELECT_PIN_MODE        (XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT8 | P0_4_AF_U1C1_SELO0)

#define HI3593_MOSI_PORT              XMC_GPIO_PORT0
#define HI3593_MOSI_PIN               0
#define HI3593_MOSI_PIN_MODE          (XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT9 | P0_0_AF_U1C1_DOUT0)

#define HI3593_MISO_PORT              XMC_GPIO_PORT0
#define HI3593_MISO_PIN               1
#define HI3593_MISO_INPUT             XMC_USIC_CH_INPUT_DX0
#define HI3593_MISO_SOURCE            0b001 // DX0B


#define HI3593_TX_LED                 P0_6
#define HI3593_TX_LED_PORT            XMC_GPIO_PORT0
#define HI3593_TX_LED_PIN             6
#define HI3593_RX_LED                 P0_8
#define HI3593_RX_LED_PORT            XMC_GPIO_PORT0
#define HI3593_RX_LED_PIN             8

#define HI3593_CLOCK                  P0_5
#define HI3593_CLOCK_SLICE_NUMBER     0
#define HI3593_CLOCK_PERIOD           96 // 96MHz / 96 = 1MHz
#define HI3593_CLOCK_PORT             XMC_GPIO_PORT0
#define HI3593_CLOCK_PIN              5
#define HI3593_RESET                  P0_7
#define HI3593_RESET_PORT             XMC_GPIO_PORT0
#define HI3593_RESET_PIN              7

#define HI3593_INPUT_PINS_NUM         12

#define HI3593_MB11_PORT              XMC_GPIO_PORT2
#define HI3593_MB11_PIN               0
#define HI3593_MB11_INDEX             0
#define HI3593_MB12_PORT              XMC_GPIO_PORT2
#define HI3593_MB12_PIN               1
#define HI3593_MB12_INDEX             1
#define HI3593_MB13_PORT              XMC_GPIO_PORT2
#define HI3593_MB13_PIN               2
#define HI3593_MB13_INDEX             2
#define HI3593_MB21_PORT              XMC_GPIO_PORT2
#define HI3593_MB21_PIN               3
#define HI3593_MB21_INDEX             3
#define HI3593_MB22_PORT              XMC_GPIO_PORT2
#define HI3593_MB22_PIN               4
#define HI3593_MB22_INDEX             4
#define HI3593_MB23_PORT              XMC_GPIO_PORT2
#define HI3593_MB23_PIN               5
#define HI3593_MB23_INDEX             5

#define HI3593_R2INT_PORT             XMC_GPIO_PORT2
#define HI3593_R2INT_PIN              6
#define HI3593_R2INT_INDEX            6
#define HI3593_R2FLAG_PORT            XMC_GPIO_PORT2
#define HI3593_R2FLAG_PIN             7
#define HI3593_R2FLAG_INDEX           7
#define HI3593_R1INT_PORT             XMC_GPIO_PORT2
#define HI3593_R1INT_PIN              8
#define HI3593_R1INT_INDEX            8
#define HI3593_R1FLAG_PORT            XMC_GPIO_PORT2
#define HI3593_R1FLAG_PIN             9
#define HI3593_R1FLAG_INDEX           9

#define HI3593_TEMPTY_PORT            XMC_GPIO_PORT2
#define HI3593_TEMPTY_PIN             10
#define HI3593_TEMPTY_INDEX           10
#define HI3593_TFULL_PORT             XMC_GPIO_PORT2
#define HI3593_TFULL_PIN              11
#define HI3593_TFULL_INDEX            11

#endif