#ifndef SHAPES_H
#define SHAPES_H

#define PIXEL_SIZE 4
#define MAX_BULLETS 3
#define MAX_ENEMIES 20

enum ObjType{Enemy, Ship, Bullet}; 
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
	uint8_t currentPath;
	uint8_t pathPos;
	Point start;
} Object;


typedef struct {
	Line lines[10];
	uint32_t numLines;
} FontCharacter;

uint32_t charSize;
FontCharacter characters[26];
void shapes_init(void);
FontCharacter* getCharacter(uint8_t c);



// Shape ship;
Object ship;
Object enemy;
Object enemyArr[MAX_ENEMIES];
uint8_t curEnemyArr[MAX_ENEMIES];
uint8_t curEnemy;
void addEnemy(uint8_t index);
void delEnemy(uint8_t index);
Point addPoint(Point p1, Point p2);
Point subtractPoint(Point p1, Point p2);

Shape objectShapes[3];
Point objectSize[3];

Point relativePath[5][128];
uint8_t relativePathSizes[5];
void (*pathUpdateFuncs[7])(Object*);
void (*pathCompleteFuncs[7])(Object*);

Object shipBullets[MAX_BULLETS];
Object enemyBullets[MAX_ENEMIES];

#endif
