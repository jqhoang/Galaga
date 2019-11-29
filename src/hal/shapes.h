#ifndef SHAPES_H
#define SHAPES_H

#define PIXEL_SIZE 4
#define MAX_BULLETS 3
#define MAX_ENEMIES 20
#define NUMBER_LEVELS 1
#define FRAMES_FOR_ENTRY_FINISH 12
#define IDLE_SHIFT 3
// row 1 is 100 
// row 2 is 150 
// row 3 is 200

enum ObjType{Enemy, Ship, Bullet}; 
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
bool spawnEnemies(uint8_t frames,Spawn spawn);
void startLevel(uint8_t level);
FontCharacter* getCharacter(uint8_t c);

//Level levels[NUMBER_LEVELS];

// Shape ship;
Object ship;
EnemyObj enemyArr[MAX_ENEMIES];
uint8_t curEnemyArr[MAX_ENEMIES];
uint8_t curEnemy;
void addEnemy(uint8_t index);
void delEnemy(uint8_t index);
Point addPoint(Point p1, Point p2);
Point subtractPoint(Point p1, Point p2);
Level levels[NUMBER_LEVELS];
Shape objectShapes[3];
Point objectSize[3];

Point relativePath[5][128];
uint8_t relativePathSizes[5];
void (*pathUpdateFuncs[8])(EnemyObj*, Point);

Object shipBullets[MAX_BULLETS];
Object enemyBullets[MAX_ENEMIES];

#endif
