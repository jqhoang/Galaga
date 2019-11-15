#ifndef SHAPES_H
#define SHAPES_H

#define PIXEL_SIZE 4

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
void fonts_init(void);
FontCharacter* getCharacter(uint8_t c);

// Shape ship;
Object ship;
Object ship2;
Object enemy;

Shape objectShapes[3];

Point enemyPath[120];

#endif
