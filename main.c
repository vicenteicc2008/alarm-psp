#include <pspkernel.h>
#include <pspdebug.h>
#include <pspdisplay.h>
#include <pspgu.h>
#include <time.h>
#include <stdio.h>

#include "callback.h"
#include "glib2d.h"

#define VERS 0 //Talk about this
#define REVS 13

PSP_MODULE_INFO("Hello World", PSP_MODULE_USER, VERS, REVS);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);
PSP_HEAP_THRESHOLD_SIZE_KB(0);

#define printf pspDebugScreenPrintf

bool ClockRunnning = true;

// Concatenate integers, join them
unsigned int concatenate_int(unsigned int num1, unsigned int num2) {
    unsigned int pow = 10;
    while(num2 >= pow)
        pow *= 10;
	
	if (num2 > 9) return num1 * pow + num2;        
	else return num1 * pow * 10 + num2;
}

g2dTexture* zero; g2dTexture* one; g2dTexture* two;
g2dTexture* three; g2dTexture* four; g2dTexture* five;
g2dTexture* six; g2dTexture* seven; g2dTexture* eight;
g2dTexture* nine; g2dTexture* colon;

g2dTexture* first; g2dTexture* second;
g2dTexture* third; g2dTexture* fourth;

// Load all images located at "MainAssets/Images/Numbers"
void LoadImages() {
	zero = g2dTexLoad("assets/0.png", G2D_SWIZZLE);
	one = g2dTexLoad("assets/1.png", G2D_SWIZZLE);
	two = g2dTexLoad("assets/2.png", G2D_SWIZZLE);
	three = g2dTexLoad("assets/3.png", G2D_SWIZZLE);
	four = g2dTexLoad("assets/4.png", G2D_SWIZZLE);
	five = g2dTexLoad("assets/5.png", G2D_SWIZZLE);
	six = g2dTexLoad("assets/6.png", G2D_SWIZZLE);
	seven = g2dTexLoad("assets/7.png", G2D_SWIZZLE);
	eight = g2dTexLoad("assets/8.png", G2D_SWIZZLE);
	nine = g2dTexLoad("assets/9.png", G2D_SWIZZLE);
	colon = g2dTexLoad("assets/2point.png", G2D_SWIZZLE);
}

// Return image from number, can't use arrays for some reason
g2dTexture* getTexFromNum(int num) {
	switch(num) {
		case 0: return zero;
		case 1: return one;
		case 2: return two; 
		case 3: return three;
		case 4: return four; 
		case 5: return five;
		case 6: return six; 
		case 7: return seven;
		case 8: return eight; 
		case 9: return nine;
		default: return zero;
	}
}

int value_first; int value_second;
int value_third; int value_fourth;

// Transform time numbers to images
void TransformTimeToClock(struct tm* timeinfo)
{
	int hour = timeinfo->tm_hour;
	int min = timeinfo->tm_min;

	int hour_together;
	int value = 0;
	int i = 0;
	
	hour_together = concatenate_int(hour, min);
	value = hour_together;

	if (value > 0) { // If not 00:00
		while(value > 0) //do till num greater than 0
		{

			switch(i) {
				case 0: if (min != 0) value_fourth = value % 10; else value_fourth = 0; fourth = getTexFromNum(value_fourth);
				case 1: if (min > 9) value_third = value % 10; else value_third = 0; third = getTexFromNum(value_third);

				case 2: if (hour != 0) value_second = value % 10; else value_second = 0; second = getTexFromNum(value_second);
				case 3: if (hour > 9) value_first = value % 10; else value_first = 0; first = getTexFromNum(value_first);
			}
				
		
			value /= 10;
			i++;
		}
	} else {
		value_fourth = value_third = value_second = value_first = 0;
		fourth = third = second = first = getTexFromNum(0);
	}

}

int main(void) {
    g2dColor BackgroundGameColor = GU_ARGB(255, 0, 0, 0);

	int x_tiles = 60;
    int y_tiles = G2D_SCR_H/2;
	int i = 0;

	// Size of numbers
	float size_numb_x = 80.0f;
	float size_numb_y = 160.0f;

	SceInt64 cur_micros = 0;
	bool showing_colon = true;

	time_t rawtime;
    struct tm * timeinfo;

	setupExitCallback();
	LoadImages();

	// While Program Running
    while(ClockRunnning) {
		// Count time
		time(&rawtime);
    	timeinfo = localtime(&rawtime);
		TransformTimeToClock(timeinfo);

		g2dClear(BackgroundGameColor); // Background


		for (i=0; i < 4; i++) {
			switch(i) {
				// Set some coords for each tile, if tile is "1", change its size to look better
				case 0: g2dBeginRects(first); g2dSetCoordXY(x_tiles, y_tiles); if (value_first == 1) size_numb_x = 7.0f; break;
				case 1: g2dBeginRects(second); g2dSetCoordXY(x_tiles+105, y_tiles); if (value_second == 1) size_numb_x = 7.0f; break;
				case 2: g2dBeginRects(third); g2dSetCoordXY(x_tiles+255, y_tiles); if (value_third == 1) size_numb_x = 7.0f; break;
				case 3: g2dBeginRects(fourth); g2dSetCoordXY(x_tiles+360, y_tiles); if (value_fourth == 1) size_numb_x = 7.0f; break;
			}
			g2dSetCoordMode(G2D_CENTER);
    		g2dSetScaleWH(size_numb_x, size_numb_y);
			// Reset size value to default
			size_numb_x = 80.0f;
    		g2dAdd();
			g2dEnd();
		}

		// Every 1 second, the middle colon changes it state
		if (sceKernelGetSystemTimeWide() > (cur_micros + 1 * 1000000)) {
			cur_micros = sceKernelGetSystemTimeWide();
			if (showing_colon) showing_colon = false;
			else showing_colon = true;
		}

		// Draw colon to screen
		if (showing_colon) {
			g2dBeginRects(colon);
			g2dSetCoordMode(G2D_CENTER);
			g2dSetScaleWH(12.0f, 150.0f);
			g2dSetCoordXY(x_tiles+180, y_tiles);
			g2dAdd();
			g2dEnd();
		}

    	g2dFlip(G2D_VSYNC);  
    }

	// Quit Game
	ClockRunnning = false;
	sceKernelExitGame();
    return 0;
}