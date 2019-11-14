#ifndef SHAPES_H
#define SHAPES_H

#define PIXEL_SIZE 4
#define MAX_BULLETS 10
#define MAX_ENEMIES 10

enum ObjType{Enemy, Ship, Bullet}; 

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

Shape objectShapes[3];
Object bulletArr[MAX_BULLETS];

#endif
