#ifndef SHAPES_H
#define SHAPES_H

#define PIXEL_SIZE 4
#define MAX_BULLETS 3
#define MAX_ENEMIES 24
#define NUMBER_LEVELS 2
#define FRAMES_FOR_ENTRY_FINISH 14
#define IDLE_SHIFT 3
// row 1 is 100 
// row 2 is 150 
// row 3 is 200

//enum ObjType{Enemy, Ship, Bullet, S, C, O, R, E, n0, n1, n2, n3, n4, n5, n6,n7,n8,n9}; 
typedef enum ObjType { Enemy, Ship, Bullet, S, C, O, R, E, n0, n1, n2, n3, n4, n5, n6, n7, n8, n9 } ObjType;
enum Paths{Entry1, Entry2, Attack1, Attack2, Attack3, Idle, ReEntry, Finish}; 
enum Direction{Left, Right};

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
	int8_t pathDirection;
	Point start;
	Point gridPos;
} EnemyObj;

typedef struct {
	Line lines[10];
	uint8_t numLines;
} FontCharacter;


typedef struct {
	 EnemyObj enemy;
	 uint8_t frame;
	 
} Spawn;

typedef struct {
	Spawn enemies[MAX_ENEMIES];
	uint8_t numEnemies;
} Level;


uint32_t charSize;
FontCharacter characters[26];
void shapes_init(void);
bool spawnEnemies(uint8_t frames, Spawn spawn, uint8_t i);
void startLevel(uint8_t level);
FontCharacter* getCharacter(uint8_t c);

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
Level levels[NUMBER_LEVELS];
Point objectSize[3];

Point relativePath[5][128];
uint8_t relativePathSizes[5];
void (*pathUpdateFuncs[8])(EnemyObj*, Point);

Object shipBullets[MAX_BULLETS];
Object enemyBullets[MAX_ENEMIES];

#endif
