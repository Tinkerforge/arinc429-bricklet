/* arinc429-bricklet
 * Copyright (C) 2020 Olaf Lüke <olaf@tinkerforge.com>
 *
 * main.c: Initialization for ARINC429 Bricklet
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

#include <stdio.h>
#include <stdbool.h>

#include "configs/config.h"

#include "bricklib2/bootloader/bootloader.h"
#include "bricklib2/hal/system_timer/system_timer.h"
#include "bricklib2/logging/logging.h"
#include "bricklib2/os/coop_task.h"
#include "communication.h"
#include "hi3593.h"

// We run communication and bootloader tick in task,
// so we can yield from within a communication getter/setter.
CoopTask main_task;
void main_tick_task(void) {
	while(true) {
		bootloader_tick();
		communication_tick();
		coop_task_yield();
	}
}

int main(void) {
	logging_init();
	logd("Start ARINC429 Bricklet\n\r");

	communication_init();
	coop_task_init(&main_task, main_tick_task);
	hi3593_init();

	while(true) {
		coop_task_tick(&main_task);
		hi3593_tick();
	}
}