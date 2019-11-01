
#include <stdint.h>
#include <stdbool.h>
#include "hal.h"
#include "fonts.h"
#include "../drivers/fb/fb.h"

#define NULL 0


static uint32_t* fb; //Framebuffer pointer

uint32_t cursorPos = SYSTEM_SCREEN_WIDTH * 5 + 5;
uint32_t lineHeight = 0;

/*
*  HAL Video Init
*/
uint32_t hal_video_init( void ){
    if( (fb=fb_init()) == NULL ){       //Init framebuffer
        return HAL_FAILED;
    }
    fonts_init();

    return HAL_SUCCESS;

}

uint32_t x_y_to_raw(Point p){
	return (int)p.y * SYSTEM_SCREEN_WIDTH + (int)p.x;
}

int32_t abs(float v){
		if(v<0) return v*(-1);
		else return v;
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
	put_pixel_raw(x_y_to_raw(start) + cursorPos, color);
	while(start.x != end.x || start.y != end.y){
		start.x += xChange;
		start.y += yChange;
		put_pixel_raw(x_y_to_raw(start) + cursorPos, color);
	}
}


/*
*  HAL Video Puts
*
*/
void hal_video_puts( uint8_t* string, uint32_t size, VideoColor color ){
	while(*string)
		hal_video_putc(*string++, size, color);


}

/*
*  HAL Video Putc
*
*/
void hal_video_putc( uint8_t c, uint32_t size, VideoColor color  ){
	if (c >= 65 && c <= 90)
		c += 32;
	hal_io_serial_putc( SerialA, c );
	
	if(size * charSize + 5 > lineHeight)
		lineHeight = size * charSize + 5;
	
	switch(c){
	case 32:
		if (cursorPos % SYSTEM_SCREEN_WIDTH + charSize * size > SYSTEM_SCREEN_WIDTH){
			cursorPos += -(cursorPos % SYSTEM_SCREEN_WIDTH) + 5 + SYSTEM_SCREEN_WIDTH * lineHeight;
			lineHeight = 0;
		}
		else
			cursorPos += (charSize + (float)charSize / 8) * size;
		break;
	case 58:
		break;
	case 10:
		cursorPos += -(cursorPos % SYSTEM_SCREEN_WIDTH) + 5 + SYSTEM_SCREEN_WIDTH * lineHeight;
		lineHeight = 0;
		break;
	case 13:
		lineHeight = 0;
		break;
		
	default:
		if(c < 97 || c > 122)
			return;
		if (cursorPos % SYSTEM_SCREEN_WIDTH + charSize * size > SYSTEM_SCREEN_WIDTH){
			cursorPos += -(cursorPos % SYSTEM_SCREEN_WIDTH) + 5 + SYSTEM_SCREEN_WIDTH * lineHeight;
			lineHeight = 0;
		}
		for(int i = 0; i < characters[c - 97].numLines; ++i)
			drawLine(scalePoint(characters[c - 97].lines[i].start, size), scalePoint(characters[c - 97].lines[i].end, size), color);
		cursorPos += (charSize + (float)charSize / 8) * size;
	}
}


/*
*  HAL Video Putc XY
*
*/
void hal_video_putc_x_y( uint32_t x, uint32_t y, uint8_t c, uint32_t size, VideoColor color  ){
    //Solution goes here
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

//Solution goes here


///////////////////////////////
/////// DRIVER INTERFACE
//////////////////////////////
void put_pixel_raw( uint32_t raw_pos, uint32_t color ){
    *(fb + raw_pos) = color; //<<<-- Note that addition is pointer arithmetic!
}
