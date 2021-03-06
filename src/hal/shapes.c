
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "shapes.h"
#include "../system.h"
#include "../drivers/delays/delays.h"
#include "../kprintf/kprintf.h"
#include "hal.h"

void relativePathUpdate(EnemyObj* obj){
	if (obj->pathDirection == Right)
		obj->o.origin = addPoint(obj->start, subtractPoint(relativePath[obj->currentPath][obj->pathPos], relativePath[obj->currentPath][0]));
	else{
		Point p = subtractPoint(relativePath[obj->currentPath][obj->pathPos], relativePath[obj->currentPath][0]);
		obj->o.origin = (Point){obj->start.x - p.x, obj->start.y + p.y};
	}
	++obj->pathPos;
}

void pathUpdate(EnemyObj* obj){
	obj->o.origin = relativePath[obj->currentPath][obj->pathPos];
	++obj->pathPos;
}

void pathRepeat(EnemyObj* obj){
	if (obj->pathPos == relativePathSizes[obj->currentPath]
		|| obj->o.origin.y > SYSTEM_SCREEN_LENGTH - 25) {
		obj->pathPos = 0;
		obj->o.origin = relativePath[obj->currentPath][0];
	}
}

void attackShoot(EnemyObj* obj, Point p){
	for (uint8_t i = 0; i < MAX_ENEMIES; i++) 
		if (enemyBullets[i].origin.y >= 868) {
			float slope = absf((obj->o.origin.y - p.y) / (float)(obj->o.origin.x - p.x));
			float division = 35 / (slope + 1);
			enemyBullets[i] = (Object){{obj->o.origin.x, obj->o.origin.y}, Bullet, {sign(p.x - obj->o.origin.x) * division, 35 - division}};
			return;
		}
}

void entry1Update(EnemyObj* obj, Point p){
	if (obj->pathDirection == Right)
		obj->o.origin = relativePath[obj->currentPath][obj->pathPos];
	else
		obj->o.origin = (Point){SYSTEM_SCREEN_WIDTH - relativePath[obj->currentPath][obj->pathPos].x, relativePath[obj->currentPath][obj->pathPos].y};
	++obj->pathPos;
	if (obj->pathPos == relativePathSizes[obj->currentPath]
		|| obj->o.origin.y > SYSTEM_SCREEN_LENGTH - 25) {
		obj->currentPath = Finish;
		obj->o.speed.x = 0 + (obj->o.origin.x - (p.y * (60 - absf(60 - (p.y * (p.x + (p.y * (FRAMES_FOR_ENTRY_FINISH * IDLE_SHIFT))))))+ obj->gridPos.x * 55 + 156)) / (float)FRAMES_FOR_ENTRY_FINISH;
		obj->o.speed.y = (obj->o.origin.y - (150 + obj->gridPos.y * 50)) / (float)FRAMES_FOR_ENTRY_FINISH;

	}
}
void entry2Update(EnemyObj* obj, Point p){
	if (obj->pathDirection == Left)
		obj->o.origin = relativePath[obj->currentPath][obj->pathPos];
	else
		obj->o.origin = (Point){SYSTEM_SCREEN_WIDTH - relativePath[obj->currentPath][obj->pathPos].x, relativePath[obj->currentPath][obj->pathPos].y};
	++obj->pathPos;
	if (obj->pathPos == relativePathSizes[obj->currentPath]
		|| obj->o.origin.y > SYSTEM_SCREEN_LENGTH - 25) {
		obj->currentPath = Finish;
		obj->o.speed.x = 0 + (obj->o.origin.x - (p.y*(60 - absf(60 - (p.y*(p.x+(p.y*(FRAMES_FOR_ENTRY_FINISH * IDLE_SHIFT))))))+ obj->gridPos.x * 55 + 156)) / (float)FRAMES_FOR_ENTRY_FINISH;
		obj->o.speed.y = (obj->o.origin.y - (150 + obj->gridPos.y * 50)) / (float)FRAMES_FOR_ENTRY_FINISH;

	}
}
void attack1Update(EnemyObj* obj, Point p){
	relativePathUpdate(obj);
	if (obj->pathPos == 15) 
		attackShoot(obj, p);
	if (obj->pathPos == relativePathSizes[obj->currentPath]
		|| obj->o.origin.y > SYSTEM_SCREEN_LENGTH - 25) {
		obj->currentPath = ReEntry;
		obj->pathPos = 0;
		obj->o.origin.y = 25;
		obj->o.origin.x= 25;
	

	}
}
void attack2Update(EnemyObj* obj, Point p){
	relativePathUpdate(obj);
	if (obj->pathPos == 15)
		attackShoot(obj, p);
	if (obj->pathPos == relativePathSizes[obj->currentPath]
		|| obj->o.origin.y > SYSTEM_SCREEN_LENGTH - 25) {
		obj->currentPath = ReEntry;
		obj->pathPos = 0;
		obj->o.origin.y = 25;
		obj->o.origin.x= 25;
	}
}
void attack3Update(EnemyObj* obj, Point p){
	relativePathUpdate(obj);
	if (obj->pathPos == 10)
		attackShoot(obj, p);
	if (obj->pathPos == relativePathSizes[obj->currentPath]
		|| obj->o.origin.y > SYSTEM_SCREEN_LENGTH - 25) {
		obj->currentPath = ReEntry;
		obj->pathPos = 0;
		obj->o.origin.y = 25;
		obj->o.origin.x= 25;
	}
}
void idleUpdate(EnemyObj* obj, Point p){
	obj->o.origin = (Point){p.x + obj->gridPos.x * 55 + 156, 150 + obj->gridPos.y * 50};
}

void reEntryUpdate(EnemyObj* obj, Point p){
	// int framesforrentry = ((150 + obj->gridPos.y*50) - 50)/25;
	//p.x is current idle shift in pixels
	//framesforrentry is how many frames to get from y:50 to the y:gridpos
	//idle shift is number of pixels per frame
	//
	//x:pos in 10 frames p.x+(p.y*(framesforrentry * IDLE_SHIFT));
	if(obj->o.origin.y == 25) {
		int framesforrentry = ((150 + obj->gridPos.y*50) - 25)/25;
		obj->o.origin.x = (p.y*(60 - abs2(60 - p.y*(p.x+(p.y*(framesforrentry * IDLE_SHIFT))))))+ obj->gridPos.x * 55 + 156;
		// kprintf("\n\r%d",obj->o.origin.x);
		obj->o.origin.y +=25;
	} else {
		obj->o.origin.y +=25;
		if(obj->o.origin.y >= 150 + obj->gridPos.y * 50) {
			obj->currentPath = Idle;
		}
	}


	// obj->o.speed.x = 0 + (obj->o.origin.x - (p.y*(60 - abs2(60 - (p.y*(p.x+(p.y*(FRAMES_FOR_ENTRY_FINISH * IDLE_SHIFT))))))+ obj->gridPos.x * 55 + 156))/FRAMES_FOR_ENTRY_FINISH;
	// obj->o.speed.y = (obj->o.origin.y - (150 + obj->gridPos.y * 50))/FRAMES_FOR_ENTRY_FINISH;

}

//path 7 starting from 0
void entryFinish(EnemyObj* obj, Point p) {
	// obj->o.origin = (Point){p.x + obj->gridPos.x * 55 + 156, 150 + obj->gridPos.y * 50};
	obj->o.origin.x -= obj->o.speed.x;
	obj->o.origin.y -= obj->o.speed.y;
	if (obj->o.origin.y < 190 + obj->gridPos.y * 50) {
		obj->currentPath = Idle;
		obj->pathPos = 0;
	}
}


void (*pathUpdateFuncs[8])(EnemyObj*, Point) = {&entry1Update, &entry2Update, &attack1Update, &attack2Update, &attack3Update, &idleUpdate, &reEntryUpdate, &entryFinish};

Point addPoint(Point p1, Point p2) {
	return (Point) { p1.x + p2.x, p1.y + p2.y };
}

Point subtractPoint(Point p1, Point p2) {
	return (Point) { p1.x - p2.x, p1.y - p2.y };
}

void shapes_init(void){


	for(uint8_t i = 0; i < MAX_BULLETS; i++) {
		shipBullets[i] = (Object){{0, -10}, Bullet, {0, -35}};
	}

	for(uint8_t i = 0; i < MAX_ENEMIES; i++) {
		enemyBullets[i] = (Object){{0, 868}, Bullet};
	}
	
	/*uint8_t row = 0;
	uint8_t col = 0;
	for (uint8_t i = 0; i < 2; i++) {
		enemyArr[i] = (EnemyObj) { { {156 + (col * 55), 150 +(row * 50)}, Enemy}, 2	, 0, { 156+(col * 55), 150 + (row * 50) }, { col, row } };
		col++;
		if ((i+1) % (MAX_ENEMIES/3) == 0 && i != 0) {
			row++;
			col = 0;
		}
		curEnemyArr[i] = i;
		curEnemy++;
	}*/
	
	
	
	//curEnemyArr[0] = MAX_ENEMIES-1;

	// uint8_t rand = get_system_timer() % 2;
	// enemyArr[MAX_ENEMIES - 1] = (EnemyObj) { {{300, 300}, Enemy}, 3, 0, { 300, 300 }, {0, 0} };
	// uint8_t rand2 = get_system_timer() % 2;
	// enemyArr[MAX_ENEMIES - 2] = (EnemyObj) { {{300, 100}, Enemy}, 1, 0, { 300, 100 }, {0, 0} };
	// curEnemyArr[0] = MAX_ENEMIES-1;
	// for(uint8_t i = 0; i < MAX_ENEMIES; i++) {
	// 	if(enemyArr[i].o.origin.y != -10) {
	// 		addEnemy(i);
	// 	}
	// }


	

}


void startLevel(uint8_t level){
	for(uint8_t i = 0; i < MAX_ENEMIES; i++) {
		curEnemyArr[i] = i;
	}
	for(uint8_t i = 0; i < levels[level].numEnemies; i++) {
		enemyArr[i] = levels[level].enemies[i].enemy;
	}
}

bool spawnEnemies(uint8_t frames, Spawn spawn, uint8_t i) {
	if(frames == spawn.frame) {
		curEnemyArr[curEnemy] = i;
		curEnemy +=1;
		return true;
	}
	return false;


}

void delEnemy(uint8_t index) {
	curEnemy -= 1;
	if(index!=curEnemy) {
		curEnemyArr[index] = curEnemyArr[curEnemy];
	}

}

Object ship = (Object) {
	{349, 740},
		Ship
};

Object shipLife1 = (Object) { {70, 25}, Ship };
Object shipLife2 = (Object) { {120, 25}, Ship };
Object letterS = (Object) { {175, 25}, S };
Object letterC = (Object) { {200, 25}, C };
Object letterO = (Object) { {225, 25}, O };
Object letterR = (Object) { {250, 25}, R };
Object letterE = (Object) { {275, 25}, E };
Object n01 = (Object) { {310, 25}, n0 };
Object n02 = (Object) { {335, 25}, n0 };
Object n03 = (Object) { {360, 25}, n0 };
Object n04 = (Object) { {385, 25}, n0 };
Object n05 = (Object) { {410, 25}, n0 };

void drawInitialStatics() {
	staticDraw(&shipLife1);
	staticDraw(&shipLife2);
	staticDraw(&letterS);
	staticDraw(&letterC);
	staticDraw(&letterO);
	staticDraw(&letterR);
	staticDraw(&letterE);
	staticDraw(&n01);
	staticDraw(&n02);
	staticDraw(&n03);
	staticDraw(&n04);
	staticDraw(&n05);
}



Level levels[NUMBER_LEVELS] = {

	(Level){
		{
			{ { {{300, 150}, Enemy}, 0, 0, Right, { 0, 0 }, {0,0} }, 0 }
			,
			{ { {{250, 100}, Enemy}, 1, 0, Left, { 250, 100 }, {6,2} }, 0 }
			,
			{ { {{300, 150}, Enemy}, 0, 0, Left, { 0, 0 }, {7,0} }, 4 }
			,
			{ { {{250, 100}, Enemy}, 1, 0, Right, { 250, 100 }, {1,2} }, 5 }
			,
			{ { {{300, 150}, Enemy}, 0, 0, Right, { 0, 0 }, {2,1} }, 8 }
			,
			{ { {{250, 100}, Enemy}, 1, 0, Left, { 250, 100 }, {5,2} }, 10 }
			,
			{ { {{300, 150}, Enemy}, 0, 0, Left, { 0, 0 }, {5,1} }, 12 }
			,
			{ { {{250, 100}, Enemy}, 1, 0, Right, { 250, 100 }, {2,2} }, 15 }
			,
			{ { {{250, 100}, Enemy}, 1, 0, Left, { 250, 100 }, {1,1} }, 20 }
			,
			{ { {{250, 100}, Enemy}, 1, 0, Right, { 250, 100 }, {6,1} }, 25 }
			,
			{ { {{250, 100}, Enemy}, 1, 0, Left, { 250, 100 }, {3,1} }, 30 }
			,
			{ { {{250, 100}, Enemy}, 1, 0, Right, { 250, 100 }, {4,1} }, 35 }
			,
			{ { {{300, 150}, Enemy}, 0, 0, Right, { 0, 0 }, {3,2} }, 60 }
			,
			{ { {{300, 150}, Enemy}, 0, 0, Right, { 0, 0 }, {4,2} }, 65 }
		},
		14
		
	},

	(Level){
		{
			{ { {{300, 150}, Enemy}, 1, 0, Right, { 0, 0 }, {0,0} }, 0 }
			,
			{ { {{250, 100}, Enemy}, 1, 0, Left, { 250, 100 }, {7,0} }, 0 }
			,
			{ { {{300, 150}, Enemy}, 1, 0, Right, { 0, 0 }, {0,2} }, 4 }
			,
			{ { {{250, 100}, Enemy}, 1, 0, Left, { 250, 100 }, {7,2} }, 4 }
			,
			{ { {{300, 150}, Enemy}, 1, 0, Right, { 0, 0 }, {2,1} }, 8 }
			,
			{ { {{250, 100}, Enemy}, 1, 0, Left, { 250, 100 }, {5,1} }, 8 }
			,
			{ { {{300, 150}, Enemy}, 0, 0, Right, { 0, 0 }, {1,0} }, 62 }
			,
			{ { {{250, 100}, Enemy}, 0, 0, Left, { 250, 100 }, {6,0} }, 62 }
			,
			{ { {{300, 150}, Enemy}, 0, 0, Right, { 0, 0 }, {2,0} }, 66 }
			,
			{ { {{250, 100}, Enemy}, 0, 0, Left, { 250, 100 }, {5,0} }, 66 }
			,
			{ { {{300, 150}, Enemy}, 0, 0, Right, { 0, 0 }, {3,0} }, 70 }
			,
			{ { {{250, 100}, Enemy}, 0, 0, Left, { 250, 100 }, {4,0} }, 70 }
			,
			{ { {{300, 150}, Enemy}, 0, 0, Right, { 0, 0 }, {1,1} }, 74 }
			,
			{ { {{250, 100}, Enemy}, 0, 0, Left, { 250, 100 }, {6,1} }, 74 }	
			,
			{ { {{300, 150}, Enemy}, 0, 0, Right, { 0, 0 }, {1,2} }, 120 }
			,
			{ { {{250, 100}, Enemy}, 0, 0, Left, { 250, 100 }, {6,2} }, 120 }
			,
			{ { {{300, 150}, Enemy}, 1, 0, Right, { 0, 0 }, {2,2} }, 121 }
			,
			{ { {{250, 100}, Enemy}, 1, 0, Left, { 250, 100 }, {5,2} }, 121 }
			,
			{ { {{300, 150}, Enemy}, 0, 0, Right, { 0, 0 }, {3,2} }, 130 }
			,
			{ { {{250, 100}, Enemy}, 0, 0, Left, { 250, 100 }, {4,2} }, 130 }
			,
			{ { {{300, 150}, Enemy}, 1, 0, Right, { 0, 0 }, {3,1} }, 131 }
			,
			{ { {{250, 100}, Enemy}, 1, 0, Left, { 250, 100 }, {4,1} }, 131 }		
		},
		22
		
	}
};

Shape objectShapes[18] = { 
	(Shape){{
	{{-4, 4}, 0x000000FF},{{4, 4}, 0x000000FF},
	{{-4, 3}, 0x000000FF},{{4, 3}, 0x000000FF}, {{-1, 3}, 0x00FF0000},{{1, 3}, 0x00FF0000},
	{{-4, 2}, 0x000000FF},{{4, 2}, 0x000000FF}, {{-1, 2}, 0x000000FF},{{1, 2}, 0x000000FF},{{-2, 2}, 0xFFFFFFFF},{{0, 2}, 0xFFFFFFFF},{{2, 2}, 0xFFFFFFFF},
	{{-4, 1}, 0x000000FF},{{4, 1}, 0x000000FF}, {{-1, 1}, 0xFFFFFFFF},{{1, 1}, 0xFFFFFFFF},{{-2, 1}, 0xFFFFFFFF},{{0, 1}, 0xFFFFFFFF},{{2, 1}, 0xFFFFFFFF},
	{{-4, 0}, 0x000000FF},{{4, 0}, 0x000000FF}, {{-1, 0}, 0xFFFFFFFF},{{1, 0}, 0xFFFFFFFF},{{-2, 0}, 0x000000FF},{{0, 0}, 0xFFFFFFFF},{{2, 0}, 0x000000FF},{{-3, 0}, 0x000000FF},{{3, 0}, 0x000000FF},
	{{-1, -1}, 0x00FF0000},{{1, -1}, 0x00FF0000},{{-2, -1}, 0x000000FF},{{0, -1}, 0x00FF0000},{{2, -1}, 0x000000FF},{{-3, -1}, 0x000000FF},{{3, -1}, 0x000000FF},
	{{-4, -2}, 0x000000FF},{{4, -2}, 0x000000FF}, {{-1, -2}, 0xFFFFFFFF},{{1, -2}, 0xFFFFFFFF},{{-2, -2}, 0x000000FF},{{0, -2}, 0xFFFFFFFF},{{2, -2}, 0x000000FF},{{-3, -2}, 0x000000FF},{{3, -2}, 0x000000FF},
	{{-4, -3}, 0x000000FF},{{4, -3}, 0x000000FF}, {{-1, -3}, 0x00FF0000},{{1, -3}, 0x00FF0000},{{0, -3}, 0x00FF0000}, {{-3, -3}, 0x000000FF}, {{3, -3}, 0x000000FF},
	{{-4,-4}, 0x000000FF},{{4,-4}, 0x000000FF}, {{0,-4}, 0x00FF0000},{{-3, -4}, 0x000000FF},{{3, -4}, 0x000000FF},
	}, 57},
	(Shape){{
																					{{0, -4}, 0xFBB74F},
																{{-1, -3}, 0xFBB74F},{{0, -3}, 0x2BFD41},{{1, -3}, 0xFBB74F},
																{{-1, -2}, 0xFBB74F},{{0, -2}, 0x2BFD41},{{1, -2}, 0xFBB74F},
											{{-2, -1}, 0xFBB74F},{{-1, -1}, 0x2BFD41},{{0, -1}, 0x2BFD41},{{1, -1}, 0x2BFD41},{{2, -1}, 0xFBB74F},
							{{-3, 0}, 0xFBB74F},{{-2, 0}, 0xFBB74F},{{-1, 0}, 0xFBB74F},{{0, 0}, 0x2BFD41},{{1, 0}, 0xFBB74F},{{2, 0}, 0xFBB74F},{{3, 0}, 0xFBB74F},
		{{-4, 1}, 0xFBB74F},{{-3, 1}, 0xFBB74F},{{-2, 1}, 0x00A9FF},{{-1, 1}, 0xB7B7BB},{{0, 1}, 0xB7B7BB},{{1, 1}, 0xB7B7BB},{{2, 1}, 0x00A9FF},{{3, 1}, 0xFBB74F},{{4, 1}, 0xFBB74F},
		{{-4, 2}, 0xFBB74F},{{-3, 2}, 0x00A9FF},{{-2, 2}, 0x2BEEFD},{{-1, 2}, 0xB7B7BB},{{0, 2}, 0xB7B7BB},{{1, 2}, 0xB7B7BB},{{2, 2}, 0x2BEEFD},{{3, 2}, 0x00A9FF},{{4, 2}, 0xFBB74F},
		{{-4, 3}, 0xFBB74F},{{-3, 3}, 0x2BEEFD},										{{0, 3}, 0xB7B7BB},										 {{3, 3}, 0x2BEEFD},{{4, 3}, 0xFBB74F},
		{{-4, 4}, 0xFBB74F},{{-3, 4}, 0x2BEEFD},					{{-1, 4}, 0xB7B7BB},{{0, 4}, 0xB7B7BB},{{1, 4}, 0xB7B7BB},					 {{3, 4}, 0x2BEEFD},{{4, 4}, 0xFBB74F},
		}, 49	
	},
	(Shape){{
		{{0, -2}, 0x2B2BFD},
		{{0, -1}, 0xE813DC},
		{{0, 0}, 0xE813DC},
		{{0, 1}, 0x2B2BFD},
		{{0, 2}, 0x0DE3FF},
		}, 5
	},
	(Shape) {{ // Letter S
		{{-2, -3}, 0x0000FF},{{-2, -2}, 0x0000FF},{{-2, -1}, 0x0000FF},{{-2, 0}, 0x0000FF},{{-2,2}, 0x0000FF},{{-2,3}, 0x0000FF},
		{{-1,-3}, 0x0000FF},{{0,-3}, 0x0000FF},{{1,-3}, 0x0000FF},{{2,-3}, 0x0000FF},
		{{-1,0}, 0x0000FF},{{0,0}, 0x0000FF},{{1,0}, 0x0000FF},{{2,0}, 0x0000FF},
		{{2, -3}, 0x0000FF},{{2, -2}, 0x0000FF},{{2, 1}, 0x0000FF},{{2, 0}, 0x0000FF},{{2,2}, 0x0000FF},{{2,3}, 0x0000FF},
		{{-1,3}, 0x0000FF},{{0,3}, 0x0000FF},{{1,-3}, 0x0000FF},{{1, 3}, 0x0000FF}
		}, 24
	},
	(Shape) {{//Letter C
		{{-2, -3}, 0x0000FF},{{-2, -2}, 0x0000FF},{{-2, -1}, 0x0000FF},{{-2, 0}, 0x0000FF},{{-2, 1}, 0x0000FF},{{-2,2}, 0x0000FF},{{-2,3}, 0x0000FF},
		{{-1, -3}, 0x0000FF},{{0, -3}, 0x0000FF},{{1, -3}, 0x0000FF},{{2, -3}, 0x0000FF},
		{{-1, 3}, 0x0000FF},{{0, 3}, 0x0000FF},{{1, 3}, 0x0000FF},{{2, 3}, 0x0000FF},
		{{2, -2}, 0x0000FF},{{2, 2}, 0x0000FF}
		}, 17
	},
	(Shape) {{//Letter O
		{ {-2, -3}, 0x0000FF},{ {-2, -2}, 0x0000FF },{ {-2, -1}, 0x0000FF },{ {-2, 0}, 0x0000FF },{ {-2, 1}, 0x0000FF },{ {-2,2}, 0x0000FF },{ {-2,3}, 0x0000FF },
		{ {2, -3}, 0x0000FF },{ {2, -2}, 0x0000FF },{ {2, -1}, 0x0000FF },{ {2, 0}, 0x0000FF },{ {2, 1}, 0x0000FF },{ {2,2}, 0x0000FF },{ {2,3}, 0x0000FF },
		{ {-1, -3}, 0x0000FF },{ {0, -3}, 0x0000FF },{ {1, -3}, 0x0000FF },{ {-1, 3}, 0x0000FF },{ {0, 3}, 0x0000FF },{ {1, 3}, 0x0000FF }
		}, 20
	},
	(Shape) {{//Letter R
		{ {-2, -3}, 0x0000FF},{ {-2, -2}, 0x0000FF },{ {-2, -1}, 0x0000FF },{ {-2, 0}, 0x0000FF },{ {-2, 1}, 0x0000FF },{ {-2,2}, 0x0000FF },{ {-2,3}, 0x0000FF },
		{ {-1, -3}, 0x0000FF },{ {0, -3}, 0x0000FF },{ {1, -3}, 0x0000FF },
		{ {2, -1}, 0x0000FF },{ {2, -2}, 0x0000FF },
		{ {-1, 0}, 0x0000FF },{ {0, 0}, 0x0000FF },{ {1, 0}, 0x0000FF },
		{ {0, 1}, 0x0000FF },{ {1, 2}, 0x0000FF },{ {2, 3}, 0x0000FF }
		}, 18
	},
	(Shape) {{//Letter E
		{ {-2, -3}, 0x0000FF},{ {-2, -2}, 0x0000FF },{ {-2, -1}, 0x0000FF },{ {-2, 0}, 0x0000FF },{ {-2, 1}, 0x0000FF },{ {-2,2}, 0x0000FF },{ {-2,3}, 0x0000FF },
		{ {-1, -3}, 0x0000FF },{ {0, -3}, 0x0000FF },{ {1, -3}, 0x0000FF },{ {2, -3}, 0x0000FF },
		{ {-1, 0}, 0x0000FF },{ {0, 0}, 0x0000FF },{ {1, 0}, 0x0000FF },{ {2, 0}, 0x0000FF },
		{ {-1, 3}, 0x0000FF },{ {0, 3}, 0x0000FF },{ {1, 3}, 0x0000FF },{ {2, 3}, 0x0000FF }
		}, 19
	},
	(Shape) {{//Number 0
		{ {-2, -3}, 0xFFFFFF},{ {-2, -2}, 0xFFFFFF },{ {-2, -1}, 0xFFFFFF },{ {-2, 0}, 0xFFFFFF },{ {-2, 1}, 0xFFFFFF },{ {-2,2}, 0xFFFFFF },{ {-2,3}, 0xFFFFFF },
		{ {2, -3}, 0xFFFFFF },{ {2, -2}, 0xFFFFFF },{ {2, -1}, 0xFFFFFF },{ {2, 0}, 0xFFFFFF },{ {2, 1}, 0xFFFFFF },{ {2,2}, 0xFFFFFF },{ {2,3}, 0xFFFFFF },
		{ {-1, -3}, 0xFFFFFF },{ {0, -3}, 0xFFFFFF },{ {1, -3}, 0xFFFFFF },
		{ {-1, 3}, 0xFFFFFF },{ {0, 3}, 0xFFFFFF },{ {1, 3}, 0xFFFFFF }
		}, 20
	},
	(Shape) {{//Number 1
		{ {0, -3}, 0xFFFFFF},{ {0, -2}, 0xFFFFFF },{ {0, -1}, 0xFFFFFF },{ {0, 0}, 0xFFFFFF },{ {0, 1}, 0xFFFFFF },{ {0,2}, 0xFFFFFF },{ {0,3}, 0xFFFFFF },
		}, 7
	},
	(Shape) {{//Number 2
		{ {-2, -1}, 0xFFFFFF},{ {-1, -3}, 0xFFFFFF },{ {0, -3}, 0xFFFFFF },{ {1, -3}, 0xFFFFFF },{ {2,-1}, 0xFFFFFF },
		{ {1, 0}, 0xFFFFFF },{ {0, 1}, 0xFFFFFF },{ {-1, 2}, 0xFFFFFF },{ {-2, 3}, 0xFFFFFF },{ {-1, 3}, 0xFFFFFF },{ {0, 3}, 0xFFFFFF },{ {1, 3}, 0xFFFFFF },{ {2, 3}, 0xFFFFFF },{ {2, -2}, 0xFFFFFF },{ {-2, -2}, 0xFFFFFF },
		}, 15
	},
	(Shape) {{//Number 3
		{ {2, -3}, 0xFFFFFF},{ {2, -2}, 0xFFFFFF },{ {2, -1}, 0xFFFFFF },{ {2, 0}, 0xFFFFFF },{ {2, 1}, 0xFFFFFF },{ {2,2}, 0xFFFFFF },{ {2,3}, 0xFFFFFF },
		{ {-1, -3}, 0xFFFFFF },{ {0, -3}, 0xFFFFFF },{ {1, -3}, 0xFFFFFF },{ {-2, -3}, 0xFFFFFF },
		{ {-1, 0}, 0xFFFFFF },{ {0, 0}, 0xFFFFFF },{ {1, 0}, 0xFFFFFF },{ {-2, 0}, 0xFFFFFF },
		{ {-1, 3}, 0xFFFFFF },{ {0, 3}, 0xFFFFFF },{ {1, 3}, 0xFFFFFF },{ {-2, 3}, 0xFFFFFF }
		}, 19
	},
	(Shape) {{//Number 4
		{ {2, -3}, 0xFFFFFF },{ {2, -2}, 0xFFFFFF },{ {2, -1}, 0xFFFFFF },{ {2, 0}, 0xFFFFFF },{ {2, 1}, 0xFFFFFF },{ {2,2}, 0xFFFFFF },{ {2,3}, 0xFFFFFF },
		{ {-1, 0}, 0xFFFFFF },{ {0, 0}, 0xFFFFFF },{ {1, 0}, 0xFFFFFF },
		{ {-2, -3}, 0xFFFFFF },{ {-2, -2}, 0xFFFFFF },{ {-2, -1}, 0xFFFFFF },{ {-2, 0}, 0xFFFFFF }
		}, 14
	},	
	(Shape) {{//Number 5
		{ {2, -3}, 0xFFFFFF},{ {-2, -2}, 0xFFFFFF },{ {-2, -1}, 0xFFFFFF },{ {2, 0}, 0xFFFFFF },{ {2, 1}, 0xFFFFFF },{ {2,2}, 0xFFFFFF },{ {2,3}, 0xFFFFFF },
		{ {-1, -3}, 0xFFFFFF },{ {0, -3}, 0xFFFFFF },{ {1, -3}, 0xFFFFFF },{ {-2, -3}, 0xFFFFFF },
		{ {-1, 0}, 0xFFFFFF },{ {0, 0}, 0xFFFFFF },{ {1, 0}, 0xFFFFFF },{ {-2, 0}, 0xFFFFFF },
		{ {-1, 3}, 0xFFFFFF },{ {0, 3}, 0xFFFFFF },{ {1, 3}, 0xFFFFFF },{ {-2, 3}, 0xFFFFFF }
		}, 19
	},
	(Shape) {{//Number 6
		{ {2, -3}, 0xFFFFFF},{ {-2, -2}, 0xFFFFFF },{ {-2, -1}, 0xFFFFFF },{ {2, 0}, 0xFFFFFF },{ {2, 1}, 0xFFFFFF },{ {2,2}, 0xFFFFFF },{ {2,3}, 0xFFFFFF },
		{ {-1, -3}, 0xFFFFFF },{ {0, -3}, 0xFFFFFF },{ {1, -3}, 0xFFFFFF },{ {-2, -3}, 0xFFFFFF },
		{ {-1, 0}, 0xFFFFFF },{ {0, 0}, 0xFFFFFF },{ {1, 0}, 0xFFFFFF },{ {-2, 0}, 0xFFFFFF },
		{ {-1, 3}, 0xFFFFFF },{ {0, 3}, 0xFFFFFF },{ {1, 3}, 0xFFFFFF },{ {-2, 3}, 0xFFFFFF },
		{ {-2, 1}, 0xFFFFFF },{ {-2, 2}, 0xFFFFFF }
		}, 21
	},
	(Shape) {{//Number 7
		{ {-2, -3}, 0xFFFFFF},{ { -1, -3}, 0xFFFFFF},{ {0, -3}, 0xFFFFFF },{ {1, -3}, 0xFFFFFF },{ { -1, -3}, 0xFFFFFF },{ {2, -3}, 0xFFFFFF },
		{ {2, -2}, 0xFFFFFF },{ {2, -1}, 0xFFFFFF },{ {2, 0}, 0xFFFFFF },{ {2, 1}, 0xFFFFFF },{ {2,2}, 0xFFFFFF },{ {2,3}, 0xFFFFFF }
		}, 12
	},
	(Shape) {{//Number 8
		{ {2, -3}, 0xFFFFFF},{ {-2, -2}, 0xFFFFFF },{ {-2, -1}, 0xFFFFFF },{ {2, 0}, 0xFFFFFF },{ {2, 1}, 0xFFFFFF },{ {2,2}, 0xFFFFFF },{ {2,3}, 0xFFFFFF },
		{ {-1, -3}, 0xFFFFFF },{ {0, -3}, 0xFFFFFF },{ {1, -3}, 0xFFFFFF },{ {-2, -3}, 0xFFFFFF },
		{ {-1, 0}, 0xFFFFFF },{ {0, 0}, 0xFFFFFF },{ {1, 0}, 0xFFFFFF },{ {-2, 0}, 0xFFFFFF },
		{ {-1, 3}, 0xFFFFFF },{ {0, 3}, 0xFFFFFF },{ {1, 3}, 0xFFFFFF },{ {-2, 3}, 0xFFFFFF },
		{ {-2, 1}, 0xFFFFFF },{ {-2, 2}, 0xFFFFFF },
		{ {2, -2}, 0xFFFFFF },{ {2, -1}, 0xFFFFFF }
		}, 23
	},
	(Shape) {{//Number 9
		{ {2, -3}, 0xFFFFFF},{ {-2, -2}, 0xFFFFFF },{ {-2, -1}, 0xFFFFFF },{ {2, 0}, 0xFFFFFF },{ {2, 1}, 0xFFFFFF },{ {2,2}, 0xFFFFFF },{ {2,3}, 0xFFFFFF },
		{ {-1, -3}, 0xFFFFFF },{ {0, -3}, 0xFFFFFF },{ {1, -3}, 0xFFFFFF },{ {-2, -3}, 0xFFFFFF },
		{ {-1, 0}, 0xFFFFFF },{ {0, 0}, 0xFFFFFF },{ {1, 0}, 0xFFFFFF },{ {-2, 0}, 0xFFFFFF },
		{ {2, -2}, 0xFFFFFF },{ {2, -1}, 0xFFFFFF }
		}, 17
	}
};

//size of the object, width and height 
Point objectSize[3] = { 
	(Point){(9*PIXEL_SIZE)/2,(9*PIXEL_SIZE)/2},
	(Point){(1*PIXEL_SIZE)/2,(5*PIXEL_SIZE)/2},
	(Point){(9*PIXEL_SIZE)/2,(9*PIXEL_SIZE)/2}
};

uint8_t relativePathSizes[5] = { 54, 49, 71, 120, 73 };

Point relativePath[5][128] = {{{666.49, 753.818}, {651.689, 751.39}, {637.015, 748.287}, {622.499, 744.513}, {608.171, 740.078}, {594.062, 734.991}, {580.2, 729.262}, {566.616, 722.904}, {553.338, 715.929}, {540.393, 708.352}, 
	{527.81, 700.191}, {515.614, 691.46}, {503.831, 682.18}, {492.486, 672.369}, {481.603, 662.048}, {471.205, 651.239}, {461.313, 639.964}, {451.949, 628.248}, {443.131, 616.115}, {434.88, 603.59}, 
	{427.211, 590.7}, {420.142, 577.472}, {413.687, 563.933}, {407.859, 550.113}, {402.671, 536.04}, 
	{395.385, 512.117}, {393.079, 497.325}, {393.971, 482.38}, {398.021, 467.967}, {405.043, 454.744}, {414.716, 443.318}, {426.598, 434.209}, {440.145, 427.836}, {454.738, 424.489}, {469.708, 424.322}, 
	{484.372, 427.342}, {498.057, 433.411}, {510.14, 442.252}, {520.066, 453.46}, {527.381, 466.522}, {531.752, 480.841}, {532.978, 495.762}, {531.003, 510.603}, {525.917, 524.684}, {517.953, 537.361}, 
	{507.475, 548.055}, {494.963, 556.276}, {480.989, 561.649}, {466.192, 563.927}, {451.249, 563.007}, {436.844, 558.93}, {423.635, 551.882}, {412.226, 542.187}, {403.141, 530.288}, {396.793, 516.729} 
	},
	
	{{390, 50}, {390, 65}, {390, 80}, {390, 95}, {390, 110}, {390, 125}, {390, 140}, 
	{390, 150}, {389.064, 164.961}, {386.269, 179.688}, {381.661, 193.953}, {375.31, 207.531}, {367.316, 220.212}, 
	{349, 239.85}, {337.571, 249.565}, {326.142, 259.279}, {314.713, 268.994}, {303.284, 278.709}, {291.855, 288.424}, {280.425, 298.138}, {268.996, 307.853}, {257.567, 317.568}, {246.138, 327.283}, 
	{234.709, 336.997}, {223.28, 346.712}, {211.851, 356.427}, {200.422, 366.141}, {188.993, 375.856}, {177.564, 385.571}, {166.135, 395.286}, 
	{155.44, 404.492}, {145.49, 415.687}, {137.802, 428.541}, {132.644, 442.603}, {130.198, 457.38}, {130.549, 472.354}, {133.685, 487}, {139.496, 500.805}, {147.779, 513.284}, {158.243, 524.001}, 
	{170.521, 532.58}, {184.182, 538.72}, {198.749, 542.205}, {213.711, 542.914}, {228.542, 540.822}, {242.723, 536.001}, {255.757, 528.622}, {267.187, 518.943}, {276.613, 507.302}, {283.704, 494.109}},
	// 72
	{{230, 250}, {228.135, 235.156}, {222.655, 221.234}, {213.901, 209.102}, {202.418, 199.512}, {188.919, 193.061}, {174.244, 190.15}, {159.305, 190.961}, {145.031, 195.442}, {132.31, 203.316}, 
	{121.931, 214.092}, {114.542, 227.1}, {110.6, 241.533}, {110.352, 256.492}, {113.813, 271.047}, {120.766, 284.294}, {130.781, 295.408}, {143.235, 303.699}, 
	{164.552, 310.638}, {179.104, 314.276}, {193.656, 317.914}, {208.209, 321.552}, {222.761, 325.19}, {237.313, 328.828}, {251.865, 332.466}, {266.417, 336.104}, {280.969, 339.742}, 
	{295.521, 343.38}, {310.074, 347.018}, {324.626, 350.656}, {339.178, 354.294}, {353.73, 357.932}, {368.282, 361.571}, {382.834, 365.209}, {397.386, 368.847}, {411.938, 372.485}, 
	{427.88, 378.138}, {441.476, 384.454}, {454.255, 392.294}, {466.046, 401.552}, {476.693, 412.106}, {486.054, 423.816}, {494.004, 436.526}, {500.439, 450.066}, {505.272, 464.258}, {508.438, 478.911}, 
	{510, 499}, {510, 514}, {510, 529}, {510, 544}, {510, 559}, {510, 574}, {510, 589}, {510, 604}, {510, 619}, {510, 634}, 
	{510, 649}, {510, 664}, {510, 679}, {510, 694}, {510, 709}, {510, 724}, {510, 739}, {510, 754}, {510, 769}, {510, 784}, {510, 799}, {510, 814}, {510, 829}, {510, 844}, {510, 859}, {510, 874}},
	// 121
	{{230, 250}, {228.135, 235.156}, {222.655, 221.234}, {213.901, 209.102}, {202.418, 199.512}, {188.919, 193.061}, {174.244, 190.15}, {159.305, 190.961}, {145.031, 195.442}, {132.31, 203.316}, 
	{121.931, 214.092}, {114.542, 227.1}, {110.6, 241.533}, {110.352, 256.492}, {113.813, 271.047}, {120.766, 284.294}, {130.781, 295.408}, {143.235, 303.699}, 
	{164.552, 310.638}, {179.104, 314.276}, {193.656, 317.914}, {208.209, 321.552}, {222.761, 325.19}, {237.313, 328.828}, {251.865, 332.466}, {266.417, 336.104}, {280.969, 339.742}, 
	{295.521, 343.38}, {310.074, 347.018}, {324.626, 350.656}, {339.178, 354.294}, {353.73, 357.932}, {368.282, 361.571}, {382.834, 365.209}, {397.386, 368.847}, {411.938, 372.485}, 
	{427.88, 378.138}, {441.476, 384.454}, {454.255, 392.294}, {466.046, 401.552}, {476.693, 412.106}, {486.054, 423.816}, {494.004, 436.526}, {500.439, 450.066}, {505.272, 464.258}, {508.438, 478.911}, 
	{510, 499}, {510, 514}, {510, 529}, {510, 544}, {510, 559}, {510, 574}, {510, 589}, {510, 604}, {510, 619}, {510, 634}, 
	{510, 649}, 
	{510, 670}, {509.064, 684.961}, {506.269, 699.688}, {501.661, 713.953}, {495.31, 727.531}, {487.316, 740.212}, {477.803, 751.797}, {466.92, 762.105}, {454.836, 770.977}, {441.741, 778.272}, 
	{427.839, 783.878}, {413.346, 787.707}, {398.488, 789.699}, {383.499, 789.824}, {368.61, 788.078}, {354.056, 784.49}, {340.062, 779.116}, {326.848, 772.038}, {314.619, 763.369}, {303.567, 753.242}, 
	{293.863, 741.817}, {285.659, 729.27}, {279.084, 715.799}, {274.239, 701.614}, {271.201, 686.934}, {270.017, 671.991}, {270.704, 657.017}, {273.254, 642.245}, {277.625, 627.906}, {283.75, 614.224}, 
	{291.533, 601.413}, {300.852, 589.671}, {311.563, 579.184}, {323.497, 570.113}, {336.469, 562.601}, {350.277, 556.765}, {364.704, 552.696}, {379.527, 550.458}, {394.512, 550.085}, {409.427, 551.583}, 
	{424.039, 554.929}, {438.12, 560.071}, {451.45, 566.928}, {463.821, 575.393}, {475.04, 585.335}, {484.932, 596.598}, {493.343, 609.006}, {500.141, 622.367}, {505.22, 636.47}, {508.502, 651.097}, 
	{510, 679}, {510, 694}, {510, 709}, {510, 724}, {510, 739}, {510, 754}, {510, 769}, {510, 784}, {510, 799}, {510, 814}, {510, 829}, {510, 844}, {510, 859}, {510, 874}},
	// 73
	{{200, 150}, {200, 165}, {200, 180}, {200, 195}, {200, 210}, {200, 225}, {200, 245}, 
	{201.865, 264.844}, {207.345, 278.766}, {216.099, 290.898}, 
	{227, 300}, {240.106, 307.296}, {253.213, 314.591}, {266.319, 321.887}, {279.425, 329.182}, {292.531, 336.478}, {305.638, 343.774}, {318.744, 351.069}, {331.85, 358.365}, {344.956, 365.66}, 
	{358.063, 372.956}, {371.169, 380.252}, {384.275, 387.547}, {397.381, 394.843}, {410.488, 402.138},
	{426.302, 412.072}, {438.827, 420.256}, {448.937, 431.284}, {456.004, 444.47}, {459.59, 458.995}, {459.47, 473.956}, {455.653, 488.422}, {448.376, 501.493}, 
	{430, 518}, {416.9, 525.306}, {403.799, 532.612}, {390.699, 539.918}, {377.598, 547.224}, {364.498, 554.53}, {351.397, 561.836}, {338.297, 569.142}, {325.196, 576.448}, {312.096, 583.754}, 
	{298.995, 591.06}, {285.895, 598.366}, {272.794, 605.672}, {259.694, 612.978}, {246.594, 620.284}, {233.493, 627.59}, {220.393, 634.896}, {207.292, 642.202}, {194.192, 649.508}, {181.091, 656.814}, 
	{170, 663.038}, {158.077, 672.076}, {148.761, 683.782}, {142.63, 697.43}, {140.067, 712.169}, {141.23, 727.085}, {146.047, 741.249}, {154.218, 753.782}, 
	{170, 767}, {183.019, 774.451}, {196.037, 781.902}, {209.056, 789.353}, {222.074, 796.804}, {235.093, 804.255}, {248.112, 811.705}, {261.13, 819.156}, {274.149, 826.607}, {287.167, 834.058}, 
	{300.186, 841.509}, {313.205, 848.96}}};
