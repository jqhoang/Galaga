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








uint8_t lives = 3;
uint8_t scoreIncrement = 10;
uint16_t score = 0;
uint16_t scoreLimit = 65530; //65536 = max size of uint16_t, 2^16
uint8_t scoreLimitDigits = 5;
uint8_t digitOffset = -25;

void main(){

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
	int frameCount = 0;
	int currentLevel = 0;
	bool tempEnemySpawn = false;
	uint8_t spawn=0;
	int8_t idleShift = 0;
	int8_t idleDirec = 1;
	unsigned long prevbullet = 0;
	while(true){
    	uint8_t c=0;
		uart0_nonblocking_getc(&c);
		if (c == 'a')
			ship.origin.x -= 10;
		if (c == 'd')
			ship.origin.x += 10;
		if (c=='p') {
			// curEnemy = 0;
			// for(uint8_t i = 0; i < MAX_ENEMIES; i++) {
			// 	if(enemyArr[i].o.origin.y != -10) {
			// 		addEnemy(i);
			// 	}
			// }
			tempEnemySpawn = true;
			frameCount = 0;

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


		//this will trigger is users click p
		if(tempEnemySpawn && spawn < levels[currentLevel].numEnemies) {
			// kprintf("\n\r%d",spawn);
			while(spawnEnemies(frameCount,levels[currentLevel].enemies[spawn]) && spawn < levels[currentLevel].numEnemies) {
				// kprintf("\n\r%d change",spawn);
				spawn++;
			}
		}
		
		// if(enemyArr)
		//when enemy at 0 dies, we change the array so that the next enemy becomes 0
		//tis makes it look like the enemy that is moving did not get killed.
		
		
		
		for (uint8_t enemy = 0; enemy < curEnemy; ++enemy) {
			if (enemyArr[curEnemyArr[enemy]].currentPath == Idle && frameCount % 40 == 0 && frameCount != 0)
			{
				//TODO only select 2 idle guys
				uint8_t rand = get_system_timer() % 3;
				enemyArr[curEnemyArr[enemy]].currentPath = rand + 2;
				enemyArr[curEnemyArr[enemy]].start = enemyArr[curEnemyArr[enemy]].o.origin;
			}

			Point p = {0, 0};
			switch(enemyArr[curEnemyArr[enemy]].currentPath){
				case Attack1:
				case Attack2:
				case Attack3:
					p = ship.origin;
					break;
				
				case Entry1:
				case Entry2:
				case Finish:
					p.y = idleDirec;
				case Idle:
					p.x = idleShift;


					break;
			}
			(*pathUpdateFuncs[enemyArr[curEnemyArr[enemy]].currentPath])(&enemyArr[curEnemyArr[enemy]], p);

			//Enemy ship collision check
			if (collisionCheck(enemyArr[curEnemyArr[enemy]].o, ship)) {
				lives--;
			}
		}

		drawShape(&ship);
		// curEnemy = 1;
		// kprintf("\n\r%d",curEnemy);
		for(uint8_t i = 0; i <curEnemy;i++) {
			drawShape(&(enemyArr[curEnemyArr[i]].o));
		}

		for(uint8_t i = 0; i<MAX_BULLETS;i++) {
			if(shipBullets[i].origin.y > 0) {
				shipBullets[i].origin.y -= 35;
				drawShape(&shipBullets[i]);
				for(uint8_t t = 0; t <curEnemy;t++) {
					if(collisionCheck(shipBullets[i],enemyArr[curEnemyArr[t]].o)){
						shipBullets[i].origin.y = 0;
						kprintf("\n\rkilled");
						score += scoreIncrement;
						//drawStatic()
						delEnemy(t);
						break;
					}
				}
			}
		}
		for(uint8_t i = 0; i < MAX_ENEMIES; i++) {
			if(enemyBullets[i].origin.y < 868) {
				enemyBullets[i].origin.x += enemyBullets[i].speed.x;
				enemyBullets[i].origin.y += enemyBullets[i].speed.y;
				drawShape(&enemyBullets[i]);
				if(collisionCheck(enemyBullets[i], ship)){
					enemyBullets[i].origin.y = 868;
					lives--;
					break;
				}
			}
		}
		draw();
		frameCount++;
		//Previous check for lives/score

		//TODO move outside of while and call drawStatic()
		/*
		
		if (lives == 3) {
			drawShape(&shipLife1);
			drawShape(&shipLife2);
		}
		else if (lives == 2) {
			drawShape(&shipLife1);
		}
		//need to call draw static a million times, so 5 max digits
		uint16_t scoreCopy = score;
		for(uint8_t digit = 0; digit < scoreLimitDigits; ++digit) {
			uint8_t rightMostNumber = scoreCopy % 10;
			//300, 720 needs to change to actual position
			Object numberToDraw = (Object) { {digit * digitOffset + 300,720}, ObjType[7 + rightMostNumber]}
			//drawStatic(&numberToDraw)
			scoreCopy /= 10;
		}
		drawShape(&letterS);
		drawShape(&letterC);
		drawShape(&letterO);
		drawShape(&letterR);
		drawShape(&letterE);
		drawShape(&number0);
		drawShape(&number1);
		drawShape(&number2);
		drawShape(&number3);
		drawShape(&number4);
		drawShape(&number5);
		drawShape(&number6);
		drawShape(&number7);
		drawShape(&number8);
		drawShape(&number9);
		//
		*/
		idleShift += idleDirec * IDLE_SHIFT;
		if (idleShift == 60 || idleShift == -60)
			idleDirec *= -1;
		hal_cpu_delay(50);
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