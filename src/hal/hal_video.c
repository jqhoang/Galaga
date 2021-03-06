
#include <stdint.h>
#include <stdbool.h>
#include "hal.h"
#include "shapes.h"
#include "../drivers/fb/fb.h"
#include "../kprintf/kprintf.h"


static uint32_t* fb; //Framebuffer pointer

uint32_t cursorPos = SYSTEM_SCREEN_WIDTH * 5 + 5;
uint32_t lineHeight = 0;
Object* objectsToDraw[100];
uint32_t objectsToDrawCount = 0;
Object lastFrameObjetsToDraw[100];
uint32_t lastFrameObjectsToDrawCount = 0;

/*
*  HAL Video Init
*/
uint32_t hal_video_init( void ){
    if( (fb=fb_init()) == NULL ){       //Init framebuffer
        return HAL_FAILED;
    }
	//fb=(void*)((unsigned long)1007681536); // THIS IS THE NUMBER THAT THE FRAME BUFFER IS SET TO, CAN WE CHANGE THIS TO ANY NUMBER WE WANT TO INCORPORATE FRONT AND BACK FRAME BUFFERS, 3c100000f
	//kprintf( "\n\rMain Frame Buffer: %0xf", fb );
    return HAL_SUCCESS;
}

uint32_t point_to_raw(Point p){
	return (int)p.y * SYSTEM_SCREEN_WIDTH + (int)p.x;
}

uint32_t x_y_to_raw(int16_t x, int16_t y){
	return (int)y * SYSTEM_SCREEN_WIDTH + (int)x;
}

int32_t abs(float v){
		// kprintf( "\n\rPrint V: %d", v );
		if(v<0) return v*(-1);
		else return v;
}

float absf(float v){
		// kprintf( "\n\rPrint V: %d", v );
		if(v<0) return v*(-1);
		else return v;
}

int16_t abs2(int16_t v){
		// kprintf( "\n\rPrint V2: %d", v );
		if(v<0) return v*(-1);
		else return v;
}

int32_t sign(int32_t x){
	return (x < 0) ? -1 : 1;
}	

/*
*	Drawing screen functions
*/

void drawShape(Object* s){
	objectsToDraw[objectsToDrawCount] = s;
	++objectsToDrawCount;
}	

void draw(){
	for(uint32_t s = 0; s < lastFrameObjectsToDrawCount; ++s)
		for(uint8_t i = 0; i < objectShapes[lastFrameObjetsToDraw[s].type].pixelNum; ++i)
			for (uint8_t x = 0; x < PIXEL_SIZE; ++x)
				for (uint8_t y = 0; y < PIXEL_SIZE; ++y)
					if (abs2(objectShapes[lastFrameObjetsToDraw[s].type].pixels[i].p.x * PIXEL_SIZE + x + lastFrameObjetsToDraw[s].origin.x - SYSTEM_SCREEN_WIDTH / 2) < SYSTEM_SCREEN_WIDTH / 2 - 50
						&& abs2(objectShapes[lastFrameObjetsToDraw[s].type].pixels[i].p.y * PIXEL_SIZE + y + lastFrameObjetsToDraw[s].origin.y - SYSTEM_SCREEN_LENGTH / 2) < SYSTEM_SCREEN_LENGTH / 2 - 50)
						put_pixel_raw(
							x_y_to_raw(objectShapes[lastFrameObjetsToDraw[s].type].pixels[i].p.x * PIXEL_SIZE + x, objectShapes[lastFrameObjetsToDraw[s].type].pixels[i].p.y * PIXEL_SIZE + y) + point_to_raw(lastFrameObjetsToDraw[s].origin), 
								VIDEO_COLOR_BLACK);
	for(uint32_t s = 0; s < objectsToDrawCount; ++s){
		lastFrameObjetsToDraw[s] = (Object){objectsToDraw[s]->origin, objectsToDraw[s]->type};
		for(uint8_t i = 0; i < objectShapes[objectsToDraw[s]->type].pixelNum; ++i)
			for (uint8_t x = 0; x < PIXEL_SIZE; ++x)
				for (uint8_t y = 0; y < PIXEL_SIZE; ++y)
					if (abs2(objectShapes[objectsToDraw[s]->type].pixels[i].p.x * PIXEL_SIZE + x + objectsToDraw[s]->origin.x - SYSTEM_SCREEN_WIDTH / 2) < SYSTEM_SCREEN_WIDTH / 2 - 50
						&& abs2(objectShapes[objectsToDraw[s]->type].pixels[i].p.y * PIXEL_SIZE + y + objectsToDraw[s]->origin.y - SYSTEM_SCREEN_LENGTH / 2) < SYSTEM_SCREEN_LENGTH / 2 - 50)
						put_pixel_raw(
							x_y_to_raw(objectShapes[objectsToDraw[s]->type].pixels[i].p.x * PIXEL_SIZE + x, objectShapes[objectsToDraw[s]->type].pixels[i].p.y * PIXEL_SIZE + y) + point_to_raw(objectsToDraw[s]->origin), 
								objectShapes[objectsToDraw[s]->type].pixels[i].colour);
	}
	lastFrameObjectsToDrawCount = objectsToDrawCount;
	objectsToDrawCount = 0;
}

void staticDraw(Object* s){
	for (int8_t i = -2; i <= 2; ++i) 
		for (int8_t j = -3; j <= 3; ++j) 
			for (uint8_t x = 0; x < PIXEL_SIZE; ++x) 
				for (uint8_t y = 0; y < PIXEL_SIZE; ++y) 
					put_pixel_raw(
						x_y_to_raw(i * PIXEL_SIZE + x, j * PIXEL_SIZE + y) + point_to_raw(s->origin),
						VIDEO_COLOR_BLACK);
	for (uint8_t i = 0; i < objectShapes[s->type].pixelNum; ++i) 
		for (uint8_t x = 0; x < PIXEL_SIZE; ++x) 
			for (uint8_t y = 0; y < PIXEL_SIZE; ++y) 
				put_pixel_raw(
					x_y_to_raw(objectShapes[s->type].pixels[i].p.x * PIXEL_SIZE + x, objectShapes[s->type].pixels[i].p.y * PIXEL_SIZE + y) + point_to_raw(s->origin),
					objectShapes[s->type].pixels[i].colour);
}

void staticDeleteShip(Object* s) {
	for (int8_t i = -4; i <= 4; ++i)
		for (int8_t j = -4; j <= 4; ++j)
			for (uint8_t x = 0; x < PIXEL_SIZE; ++x)
				for (uint8_t y = 0; y < PIXEL_SIZE; ++y)
					put_pixel_raw(
						x_y_to_raw(i * PIXEL_SIZE + x, j * PIXEL_SIZE + y) + point_to_raw(s->origin),
						VIDEO_COLOR_BLACK);
}

void drawLine(Point start, Point end, VideoColor color){
	float xChange = end.x - start.x;
	float yChange = end.y - start.y;
	if (xChange == 0 || yChange == 0){
		if (xChange == 0)
			yChange /= abs(yChange);
		else
			xChange /= abs(xChange);
	}
	else if (abs(xChange) > abs(yChange)){
		yChange /= abs(xChange);
		xChange /= abs(xChange);
	}else{
		xChange /= abs(yChange);
		yChange /= abs(yChange);
	}
	put_pixel_raw(point_to_raw(start) + cursorPos, color);
	while(start.x != end.x || start.y != end.y){
		start.x += xChange;
		start.y += yChange;
		put_pixel_raw(point_to_raw(start) + cursorPos, color);
	}
}

/*
*  HAL Video Clear
*/
void hal_video_clear( void ){
	for(uint32_t i=0; i<SYSTEM_SCREEN_WIDTH; i++)
		for(uint32_t j=0; j<SYSTEM_SCREEN_LENGTH; j++)
			put_pixel_raw( j * SYSTEM_SCREEN_WIDTH + i, VIDEO_COLOR_BLACK );
	cursorPos = SYSTEM_SCREEN_WIDTH * 5 + 5;
}


///////////////////////////////
/////// DRIVER INTERFACE
//////////////////////////////
void put_pixel_raw( uint32_t raw_pos, uint32_t color ){
    *(fb + raw_pos) = color; //<<<-- Note that addition is pointer arithmetic!
}
