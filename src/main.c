/**
*   This file is part of os_labs
*   (https://github.com/rromanotero/os_labs).
*
*   Copyright (c) 2019 Rafael Roman Otero.
*
*   This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program.  If not, see <https://www.gnu.org/licenses/>.
*
**/

#include <stdint.h>
#include <stddef.h>
#include "system.h"
#include "hal/hal.h"
#include "alice.h"
#include "hal/shapes.h"
#include "drivers/uart/uart.h"
#include "kprintf/kprintf.h"


/*
*     For some reason, GCC's built in memcpy doesn't work on the Physical device
*     (even though it works on QEMU), so until I figure this out,
*     here is a custom copy of memcpy
*/
void *memcpy(void *dest, void *src, size_t n){
	uint8_t *my_dest = (uint8_t *)dest;
	uint8_t *my_src = (uint8_t *)src;

	while (n--) {
		*my_dest++ = *my_src++;
	}

	return dest;
}

void wait_for_ten_secs(void);

uint32_t zoom_title=1;
uint32_t zoom_text=1;
bool changed=true;

void main(){

    hal_io_serial_init();

    if( hal_video_init() == HAL_FAILED ){
        while(true){
            hal_io_serial_puts( SerialA, "Failed initializing Video. Cannot continue.\n\r" );
            wait_for_ten_secs();
        }
    }



	
	
	
	// GAME LOOP (~20 fps)
	int i = 2; // SHOULD START AT 0, BUT THAT WOULD BE OFFSCREEN
	while(true){
    	uint8_t c=0;
		uart0_nonblocking_getc(&c);
		if (c == 'a')
			ship.origin.x -= 10;
		if (c == 'd')
			ship.origin.x += 10;
		
		
		enemy.origin = enemyPath[i];
		if (i < 108)
			++i;
		if (i == 108)
			i = 0;
		
		drawShape(&ship);
		drawShape(&ship2);
		drawShape(&enemy);
		draw();
		hal_cpu_delay(50);
	}

}

void wait_for_ten_secs(void){
    for(uint32_t i=0; i<10; i++)
        hal_cpu_delay(1000);
}
