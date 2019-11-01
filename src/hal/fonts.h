#ifndef FONTS_H
#define FONTS_H

typedef struct {
	float x;
	float y; 
} Point;

Point scalePoint(Point p, uint32_t scale);

typedef struct {
	Point start;
	Point end;
} Line;

typedef struct {
	Line lines[10];
	uint32_t numLines;
} FontCharacter;

uint32_t charSize;
FontCharacter characters[26];
void fonts_init(void);
FontCharacter* getCharacter(uint8_t c);



#endif
