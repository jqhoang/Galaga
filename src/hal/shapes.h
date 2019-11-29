#ifndef SHAPES_H
#define SHAPES_H

#define PIXEL_SIZE 4
#define MAX_BULLETS 3
#define MAX_ENEMIES 24
#define NUMBER_LEVELS 1


enum ObjType{Enemy, Ship, Bullet, S, C, O, R, E, n0, n1, n2, n3, n4, n5, n6,n7,n8,n9}; 
enum Paths{Entry1, Entry2, Attack1, Attack2, Attack3, Idle, ReEntry}; 

typedef struct {
	int16_t x;
	int16_t y; 
} Point;

typedef struct {
	Point p;
	uint32_t colour;
} Pixel;

typedef struct {
	Point start;
	Point end;
} Line;

typedef struct {
	Pixel pixels[81];
	uint8_t pixelNum;
} Shape;

typedef struct {
	Point origin;//center of object
	enum ObjType type;
	Point speed;
} Object;

typedef struct {
	Object o;
	uint8_t currentPath;
	uint8_t pathPos;
	Point start;
	Point gridPos;
} EnemyObj;

typedef struct {
	Line lines[10];
	uint8_t numLines;
} FontCharacter;


typedef struct {
	 EnemyObj enemies[MAX_ENEMIES];
	 int numEnemies[MAX_ENEMIES];
} EnemiesStartDelay;

// typedef struct {
// 	int enemies[MAX_ENEMIES];
// 	// int numPath;
// } Level;


uint32_t charSize;
FontCharacter characters[26];
void shapes_init(void);
FontCharacter* getCharacter(uint8_t c);

//Level levels[NUMBER_LEVELS];

// Shape ship;
Object ship;
Object shipLife1;
Object shipLife2;
Object letterS;
Object letterC;
Object letterO;
Object letterR;
Object letterE;
Object number0;
Object number1;
Object number2;
Object number3;
Object number4;
Object number5;
Object number6;
Object number7;
Object number8;
Object number9;
EnemyObj enemyArr[MAX_ENEMIES];
uint8_t curEnemyArr[MAX_ENEMIES];
uint8_t curEnemy;
void addEnemy(uint8_t index);
void delEnemy(uint8_t index);
Point addPoint(Point p1, Point p2);
Point subtractPoint(Point p1, Point p2);

Shape objectShapes[18];
Point objectSize[3];

Point relativePath[5][128];
uint8_t relativePathSizes[5];
void (*pathUpdateFuncs[7])(EnemyObj*, Point);

Object shipBullets[MAX_BULLETS];
Object enemyBullets[MAX_ENEMIES];

#endif
