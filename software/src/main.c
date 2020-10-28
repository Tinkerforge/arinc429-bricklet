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

#include "arinc429.h"
#include "hi3593.h"
#include "communication.h"

extern CoopTask arinc429_task;


/****************************************************************************/
/* data structures                                                          */
/****************************************************************************/

CoopTask main_task;


/****************************************************************************/
/* task & tick functions                                                    */
/****************************************************************************/

void main_tick(void)
{
	// restart main_tick_task()
	coop_task_tick(&main_task);
}

void main_tick_task(void)
{
	while(true)
	{
		// housekeeping
		bootloader_tick();

		// communication
		communication_tick();

		// done for now
		coop_task_yield();
	}
}


/****************************************************************************/
/* MAIN()                                                                   */
/****************************************************************************/

int main(void)
{
//	// start logging service
//	logging_init();
//	logd("Start ARINC429 Bricklet\n\r");

	// initialize communication
	communication_init();

	// initialize tasks
	coop_task_init(&main_task,     main_tick_task    );
	coop_task_init(&arinc429_task, arinc429_tick_task);

	// set initial operating mode
	arinc429.system.operating_mode = ARINC429_A429_MODE_NORMAL;

	// set initial system requests
	arinc429.system.change_request = 0xFF;  // do all

	// main-loop
	while(true)
	{
		// do housekeeping & communication
		main_tick();

		// do A429 specific operations
		arinc429_tick();
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

