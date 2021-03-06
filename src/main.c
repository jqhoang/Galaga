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
void drawNumbers();
void drawInitialStatics();
void deleteShipLives();

uint8_t lives = 3;
uint8_t scoreIncrement = 10;
uint16_t score = 0;
uint16_t scoreLimit = 65530; //65536 = max size of uint16_t, 2^16
uint8_t digitOffset = 25;
unsigned long prevLife = 0;
uint32_t invulnerabilityTime = 800000;
bool roundEnd = false;
bool gameEnd = false;

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
	uint8_t spawn = 0;
	int8_t idleShift = 0;
	int8_t idleDirec = 1;
	unsigned long prevbullet = 0;
	unsigned long roundEndTime = 0;
	unsigned long gameStart = 0;

	
	drawInitialStatics();

	while(true){
    	uint8_t c=0;
		uart0_nonblocking_getc(&c);
		if (c == 'a')
		{
			if (ship.origin.x - 25 >= 60)
			{
				ship.origin.x -= 20;
			}
		}
		if (c == 'd')
		{
			if (ship.origin.x + 25 <= SYSTEM_SCREEN_WIDTH - 55)
			{
				ship.origin.x += 20;
			}
		}
		if (c=='p') {
			if(tempEnemySpawn == false) {
				tempEnemySpawn = true;
				frameCount = 0;
				startLevel(currentLevel);
				kprintf("\n\rLevel: %d",currentLevel);
				gameStart = get_system_timer();
			}
		}
		if (c == 'k'){
			// kprintf("\r\norigX:%d",abs2(-160));
			if(get_system_timer() - prevbullet >= 500000) {
				for(uint8_t i = 0; i < MAX_BULLETS;i++) {
					if(shipBullets[i].origin.y <=0) {
						prevbullet = get_system_timer();
						shipBullets[i] = (Object){{ship.origin.x, ship.origin.y},Bullet};
						break;
					}
				}
			}
		}

		if(roundEnd && get_system_timer() - roundEndTime >= 5000000 && currentLevel < NUMBER_LEVELS && !gameEnd) {
			roundEnd = false;
			spawn = 0;
			currentLevel += 1;
			frameCount = 0;
			startLevel(currentLevel);

		}
		if(gameEnd && roundEnd) {
			kprintf("\n\rGame Finished, You Win");
			kprintf("\n\rYour Time is %d", get_system_timer() - gameStart);
			roundEnd =false;
		}


		//this will trigger is users click p
		if(tempEnemySpawn && spawn < levels[currentLevel].numEnemies) {
			while(spawnEnemies(frameCount, levels[currentLevel].enemies[spawn], spawn) && spawn < levels[currentLevel].numEnemies) {
				
				//kprintf("\n\rSpawning %d", spawn);
				spawn++;
			}
		}
		
		uint8_t idlingEnemies[MAX_ENEMIES];
		uint8_t idlingEnemiesCount = 0;
		uint8_t ship1Attack = -1;
		uint8_t ship2Attack = -1;
		if (frameCount % 60 == 0 
			&& frameCount > levels[currentLevel].enemies[levels[currentLevel].numEnemies - 1].frame + relativePathSizes[levels[currentLevel].enemies[levels[currentLevel].numEnemies - 1].enemy.currentPath] + FRAMES_FOR_ENTRY_FINISH - 40){
			for (uint8_t enemy = 0; enemy < curEnemy; ++enemy) 
				if (enemyArr[curEnemyArr[enemy]].currentPath == Idle) {
					idlingEnemies[idlingEnemiesCount] = curEnemyArr[enemy];
					idlingEnemiesCount++;
				}
			if (idlingEnemiesCount > 0) {
				ship1Attack = idlingEnemies[get_system_timer() % idlingEnemiesCount];
				idlingEnemies[ship1Attack] = idlingEnemies[idlingEnemiesCount];
				idlingEnemiesCount--;
			}
			if (idlingEnemiesCount > 0) 
				ship2Attack = idlingEnemies[get_system_timer() % idlingEnemiesCount];
		}
		
		
		for (uint8_t enemy = 0; enemy < curEnemy; ++enemy) {
			if (curEnemyArr[enemy] == ship1Attack || curEnemyArr[enemy] == ship2Attack)
			{
				uint8_t rand = get_system_timer() % 3;
				enemyArr[curEnemyArr[enemy]].currentPath = rand + 2;
				enemyArr[curEnemyArr[enemy]].start = enemyArr[curEnemyArr[enemy]].o.origin;
				if (enemyArr[curEnemyArr[enemy]].o.origin.x > SYSTEM_SCREEN_WIDTH / 2)
					enemyArr[curEnemyArr[enemy]].pathDirection = Left;
				else
					enemyArr[curEnemyArr[enemy]].pathDirection = Right;

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
				case ReEntry:
					p.y = idleDirec;
				case Idle:
					p.x = idleShift;
					break;
			}
			(*pathUpdateFuncs[enemyArr[curEnemyArr[enemy]].currentPath])(&enemyArr[curEnemyArr[enemy]], p);

			//Enemy ship collision check
			if (collisionCheck(enemyArr[curEnemyArr[enemy]].o, ship)) {
				if (get_system_timer() - prevLife >= invulnerabilityTime) {
					//kprintf("\n\rdied");
					lives--;
					deleteShipLives();
					prevLife = get_system_timer();
				}	
			}
		}

		drawShape(&ship);
		
		for(uint8_t i = 0; i <curEnemy;i++) {
			drawShape(&(enemyArr[curEnemyArr[i]].o));
		}

		for(uint8_t i = 0; i<MAX_BULLETS;i++) {
			if(shipBullets[i].origin.y > 0) {
				drawShape(&shipBullets[i]);
				for(uint8_t t = 0; t <curEnemy;t++) {
					if(collisionCheck(shipBullets[i],enemyArr[curEnemyArr[t]].o)){
						shipBullets[i].origin.y = 0;
						// kprintf("\n\rkilled");
						if (score <= scoreLimit) {
							score += scoreIncrement;
							drawNumbers();
						}
						delEnemy(t);
						//kprintf("\n\rLcurEnemy: %d",curEnemy);
						//kprintf("\n\rSpawn %d",spawn);
						if(spawn == levels[currentLevel].numEnemies && curEnemy == 0) {
							roundEnd = true;
						 	roundEndTime = get_system_timer();
							kprintf("\n\rLevel: %d",currentLevel+1);
							if(currentLevel + 1 == NUMBER_LEVELS) {
								gameEnd = true;
							}
						}

						break;
					}
				}
				shipBullets[i].origin.y -= 35;
			}
		}
		for(uint8_t i = 0; i < MAX_ENEMIES; i++) {
			if(enemyBullets[i].origin.y < 868) {
				drawShape(&enemyBullets[i]);
				if(collisionCheck(enemyBullets[i], ship)){
					enemyBullets[i].origin.y = 868;
					if (get_system_timer() - prevLife >= invulnerabilityTime) {
						//kprintf("\n\rdied");
						lives--;
						deleteShipLives();
						prevLife = get_system_timer();
					}
					break;
				}
				enemyBullets[i].origin.x += enemyBullets[i].speed.x;
				enemyBullets[i].origin.y += enemyBullets[i].speed.y;
			}
		}
		draw();
		frameCount++;
		
		if (lives == 0) {
			kprintf("\n\rGAME OVER!");
			break;
		}
		
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

void drawNumbers() {
	uint16_t scoreCopy = score;
	uint8_t digit = 0;
	while(scoreCopy) {
		uint8_t rightMostNumber = scoreCopy % 10;
		Object numberToDraw = (Object) { {-(digit * digitOffset) + 410, 25}, (ObjType)(8 + rightMostNumber), { 0,0 } };
		staticDraw(&numberToDraw);
		scoreCopy /= 10;
		digit++;
	}
}

void deleteShipLives() {
	if (lives == 2) 
		staticDeleteShip(&shipLife1);
	else if (lives == 1)
		staticDeleteShip(&shipLife2);
	
}