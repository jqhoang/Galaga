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



	
	
	
	// GAME LOOP (~20 fps)
	int i = 2; // SHOULD START AT 0, BUT THAT WOULD BE OFFSCREEN
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
			if(get_system_timer() - prevbullet >= 200000) {
				for(uint8_t i = 0; i < MAX_BULLETS;i++) {
					if(bulletArr[i].origin.y <=0) {
						prevbullet = get_system_timer();
						bulletArr[i] = (Object){{ship.origin.x, ship.origin.y},Bullet};
						break;
					}
				}
			}
		}
		
		
		enemyArr[curEnemyArr[0]].origin = enemyPath[i];
		if (i < 108)
			++i;
		if (i == 108)
			i = 0;

		//clearDrawScreen();
		// drawShape(&enemy);
		drawShape(&ship);
		// curEnemy = 1;
		// drawShape(&enemy);
		for(uint8_t i = 0; i <curEnemy;i++) {
			drawShape(&enemyArr[curEnemyArr[i]]);
		}
		// for(uint8_t i = 0; i < MAX_ENEMIES; i++) {
		// 	if(enemyArr[i].origin.y != -10) {
		// 		drawShape(&enemyArr[i]);
				
		// 	}
		// }

		for(uint8_t i = 0; i<MAX_BULLETS;i++) {
			if(bulletArr[i].origin.y >=10) {
				drawShape(&bulletArr[i]);
				bulletCheck(i);
			}
		}
		draw();
		hal_cpu_delay(50);
	}

}
//
void bulletCheck(uint8_t index) {
	// // bool killed = false;
	// for(uint8_t t = 0; t < MAX_ENEMIES; t++) {
	// 	if(enemyArr[t].origin.y != -10) {
	// 		if((bulletArr[i].origin.y <= enemyArr[t].origin.y+20 && bulletArr[i].origin.y >= enemyArr[t].origin.y-20)
	// 		&& (bulletArr[i].origin.x <= enemyArr[t].origin.x+20 && bulletArr[i].origin.x >= enemyArr[t].origin.x-20)){
	// 			bulletArr[i].origin.y = 0;
	// 			kprintf("\n\rkilled");
	// 			// killed=true;
	// 			return;
	// 		}
	// 	}
	// }
	// // if(!killed) {
	// 	kprintf("\r\nbulletposy:%d",bulletArr[i].origin.y);
	// 	bulletArr[i].origin.y -=10;
	// // }
	// for(uint8_t t = 0; t <curEnemy;t++) {
	// 	if((bulletArr[index].origin.y <= enemyArr[curEnemyArr[t]].origin.y+objectSize[Enemy].x && bulletArr[index].origin.y >= enemyArr[curEnemyArr[t]].origin.y-objectSize[Enemy].x)
	// 	&& (bulletArr[index].origin.x <= enemyArr[curEnemyArr[t]].origin.x+objectSize[Enemy].y && bulletArr[index].origin.x >= enemyArr[curEnemyArr[t]].origin.x-objectSize[Enemy].y)){
	// 		bulletArr[index].origin.y = 0;
	// 		kprintf("\n\rkilled");
	// 		delEnemy(t);
	// 		return;
	// 	}
	// }
	for(uint8_t t = 0; t <curEnemy;t++) {
		if(collisionCheck(bulletArr[index],enemyArr[curEnemyArr[t]])){
			// kprintf("\r\nbulletposy:%d",bulletArr[index].origin.y);
			bulletArr[index].origin.y = 0;
			kprintf("\n\rkilled");
			
			delEnemy(t);
			return;
		}
	}
	// kprintf("\r\nbulletposy:%d",bulletArr[index].origin.y);
	bulletArr[index].origin.y -=10;
}
//semi hardcoded for bullets and enemys should change this after, by adding types
bool collisionCheck(Object obj1, Object obj2) {
	// if((p1X <= p2X+objectSize[Enemy].x && p1X >= p2X-objectSize[Enemy].x)
	// && (p1Y <= p2Y+objectSize[Enemy].y && p1Y >= p2Y-objectSize[Enemy].y))
	// kprintf("\r\norigX:%d",abs2(obj1.origin.x));
	if((abs2(obj1.origin.x-obj2.origin.x) < (objectSize[obj1.type].x + objectSize[obj2.type].x))
	&& (abs2(obj1.origin.y-obj2.origin.y) < (objectSize[obj1.type].y + objectSize[obj2.type].y)))
		return true;
	return false;
}

void wait_for_ten_secs(void){
    for(uint32_t i=0; i<10; i++)
        hal_cpu_delay(1000);
}
