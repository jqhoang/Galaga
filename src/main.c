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
#include <stdlib.h> 
#include "system.h"
#include "hal/hal.h"
#include "drivers/delays/delays.h"
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
void bulletCheck(uint8_t index);
bool collisionCheck(Object obj1, Object obj2);

uint32_t zoom_title=1;
uint32_t zoom_text=1;
const uint8_t maxBullets = 10;
bool changed=true;
unsigned long prevbullet = 0;

void main(){

	kprintf("\r\nenemyposy:%d",enemy.origin.y);

    hal_io_serial_init();
	shapes_init();

    if( hal_video_init() == HAL_FAILED ){
        while(true){
            hal_io_serial_puts( SerialA, "Failed initializing Video. Cannot continue.\n\r" );
            wait_for_ten_secs();
        }
    }



	for (int i = 50; i < SYSTEM_SCREEN_WIDTH - 50; ++i)
		put_pixel_raw(i + SYSTEM_SCREEN_WIDTH * 50, VIDEO_COLOR_WHITE);
	for (int i = 50; i < SYSTEM_SCREEN_WIDTH - 50; ++i)
		put_pixel_raw(i + SYSTEM_SCREEN_WIDTH * (SYSTEM_SCREEN_LENGTH - 50), VIDEO_COLOR_WHITE);
	for (int i = 50; i < SYSTEM_SCREEN_LENGTH - 50; ++i)
		put_pixel_raw(50 + SYSTEM_SCREEN_WIDTH * i, VIDEO_COLOR_WHITE);
	for (int i = 50; i < SYSTEM_SCREEN_LENGTH - 50; ++i)
		put_pixel_raw(SYSTEM_SCREEN_WIDTH - 50 + SYSTEM_SCREEN_WIDTH * i, VIDEO_COLOR_WHITE);
	
	
	// GAME LOOP (~20 fps)
	int i0 = 0;
	int i1 = 0;
	int frameCount = 0;
	while(true){
    	uint8_t c=0;
		uart0_nonblocking_getc(&c);
		if (c == 'a')
			ship.origin.x -= 10;
		if (c == 'd')
			ship.origin.x += 10;
		if (c=='p') {
			curEnemy = 0;
			for(uint8_t i = 0; i < MAX_ENEMIES; i++) {
				if(enemyArr[i].origin.y != -10) {
					addEnemy(i);
				}
			}
		}
		if (c == 'k'){
			// kprintf("\r\norigX:%d",abs2(-160));
			if(get_system_timer() - prevbullet >= 150000) {
				for(uint8_t i = 0; i < MAX_BULLETS;i++) {
					if(shipBullets[i].origin.y <=0) {
						prevbullet = get_system_timer();
						shipBullets[i] = (Object){{ship.origin.x, ship.origin.y},Bullet};
						break;
					}
				}
			}
		}
		
		// if(enemyArr)
		//when enemy at 0 dies, we change the array so that the next enemy becomes 0
		//tis makes it look like the enemy that is moving did not get killed.
		for (uint8_t enemy = 0; enemy < curEnemy; ++enemy) {
			(*pathCompleteFuncs[enemyArr[curEnemyArr[enemy]].currentPath])(&enemyArr[curEnemyArr[enemy]]);
			(*pathUpdateFuncs[enemyArr[curEnemyArr[enemy]].currentPath])(&enemyArr[curEnemyArr[enemy]]);
		}


		drawShape(&ship);
		// curEnemy = 1;
		for(uint8_t i = 0; i <curEnemy;i++) {
			drawShape(&enemyArr[curEnemyArr[i]]);
		}

		for(uint8_t i = 0; i<MAX_BULLETS;i++) {
			if(shipBullets[i].origin.y > 0) {
				shipBullets[i].origin.y -=35;
				drawShape(&shipBullets[i]);
				for(uint8_t t = 0; t <curEnemy;t++) {
					if(collisionCheck(shipBullets[i],enemyArr[curEnemyArr[t]])){
						shipBullets[i].origin.y = 0;
						kprintf("\n\rkilled");
						delEnemy(t);
						break;
					}
				}
			}
		}
		for(uint8_t i = 0; i<MAX_ENEMIES;i++) {
			if(enemyBullets[i].origin.y > 0) {
				enemyBullets[i].origin.y =35;
				drawShape(&enemyBullets[i]);
				if(collisionCheck(enemyBullets[i], ship)){
					enemyBullets[i].origin.y = 0;
					kprintf("\n\rLost a life");
					break;
				}
			}
		}
		draw();
		hal_cpu_delay(50);
		frameCount++;
	}

}


//semi hardcoded for bullets and enemys should change this after, by adding types
bool collisionCheck(Object obj1, Object obj2) {

	if((abs2(obj1.origin.x-obj2.origin.x) < (objectSize[obj1.type].x + objectSize[obj2.type].x))
	&& (abs2(obj1.origin.y-obj2.origin.y) < (objectSize[obj1.type].y + objectSize[obj2.type].y)))
		return true;
	return false;
}

void wait_for_ten_secs(void){
    for(uint32_t i=0; i<10; i++)
        hal_cpu_delay(1000);
}
