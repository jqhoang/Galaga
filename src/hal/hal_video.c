
#include <stdint.h>
#include <stdbool.h>
#include "hal.h"
#include "shapes.h"
#include "../drivers/fb/fb.h"

#define NULL 0


static uint32_t* fb; //Framebuffer pointer

uint32_t cursorPos = SYSTEM_SCREEN_WIDTH * 5 + 5;
uint32_t lineHeight = 0;
uint32_t drawingScreen[SYSTEM_SCREEN_WIDTH * SYSTEM_SCREEN_LENGTH];

/*
*  HAL Video Init
*/
uint32_t hal_video_init( void ){
    if( (fb=fb_init()) == NULL ){       //Init framebuffer
        return HAL_FAILED;
    }
	for(uint32_t i=0; i<SYSTEM_SCREEN_WIDTH; i++)
		for(uint32_t j=0; j<SYSTEM_SCREEN_LENGTH; j++)
			drawingScreen[j * SYSTEM_SCREEN_WIDTH + i] = VIDEO_COLOR_BLACK;
    return HAL_SUCCESS;

}

uint32_t point_to_raw(Point p){
	return (int)p.y * SYSTEM_SCREEN_WIDTH + (int)p.x;
}

uint32_t x_y_to_raw(int16_t x, int16_t y){
	return (int)y * SYSTEM_SCREEN_WIDTH + (int)x;
}

int32_t abs(float v){
		if(v<0) return v*(-1);
		else return v;
}

int32_t sign(int32_t x){
	return (x < 0) ? -1 : 1;
}	

/*
*	Drawing screen functions
*/

void drawShape(Shape* s){
	for(uint8_t i = 0; i < s->numPixels; ++i)
		for (uint8_t x = 0; x < PIXEL_SIZE; ++x)
			for (uint8_t y = 0; y < PIXEL_SIZE; ++y)
				drawingScreen[x_y_to_raw(s->pixels[i].p.x * PIXEL_SIZE + x, s->pixels[i].p.y * PIXEL_SIZE + y) + point_to_raw(s->origin)] = s->pixels[i].colour;
}	

void clearDrawScreen(){
	for(uint32_t i=0; i<SYSTEM_SCREEN_WIDTH; i++)
		for(uint32_t j=0; j<SYSTEM_SCREEN_LENGTH; j++)
			drawingScreen[j * SYSTEM_SCREEN_WIDTH + i] = VIDEO_COLOR_BLACK;
}

void draw(){
	for(uint32_t i=450; i<SYSTEM_SCREEN_WIDTH; i++)
		for(uint32_t j=550; j<SYSTEM_SCREEN_LENGTH; j++) {
			*(fb + j * SYSTEM_SCREEN_WIDTH + i) = drawingScreen[j * SYSTEM_SCREEN_WIDTH + i];
			drawingScreen[j * SYSTEM_SCREEN_WIDTH + i] = VIDEO_COLOR_BLACK;
		}
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
