#ifndef SHAPES_H
#define SHAPES_H

#define PIXEL_SIZE 4

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
	Pixel pixels[160];
	uint8_t numPixels;
	Point origin;
} Shape;

typedef struct {
	Line lines[10];
	uint32_t numLines;
} FontCharacter;

uint32_t charSize;
FontCharacter characters[26];
void fonts_init(void);
FontCharacter* getCharacter(uint8_t c);

Shape ship;

#endif
