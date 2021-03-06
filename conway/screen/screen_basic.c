/*
MIT license
Copyright (c) 2019-2021 Maarten Vermeulen

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */

#include "screen_basic.h"

#include "../io/io.h"
#include "../include/types.h"
#include "../include/exit_code.h"

#define SCREEN_BASIC_DEFAULT_COLOR  0x07

#define SCREEN_BASIC_FIRST_SCNLINE	0
#define SCREEN_BASIC_LAST_SCNLINE	15

typedef struct SCREENDATA
{
	unsigned short cursorY;
	unsigned short cursorX;
    uint32_t SCREEN_FLAGS;

	unsigned char chScreenColor;
} SCREENDATA;

static SCREENDATA SCRscreenData;
uint8_t hexdigits = 0;

/* Static function defines */
static void screen_basic_char_put_on_screen(char c);
static void screen_basic_move_cursor_internal(void);
static void screen_basic_scroll(unsigned char line);
static void screen_basic_linecheck(void);
static void screen_basic_clear_line(unsigned char from, unsigned char to);

/* TODO & FIXME: 
	- comment some of this stuff 
	- defines for outb command
*/

/*
 * 'Public' part
 */

unsigned char screen_basic_init(void)
{
 	uint8_t check;

 	SCRscreenData.SCREEN_FLAGS = 0;

    SCRscreenData.chScreenColor = SCREEN_BASIC_DEFAULT_COLOR;

    SCRscreenData.cursorY = 0;
	SCRscreenData.cursorX = 0;

 	return EXIT_CODE_GLOBAL_SUCCESS;
}

void print(const char* str){
	
	unsigned int i = 0;

	while(str[i] != '\0')
	{ 
		screen_basic_char_put_on_screen(str[i]);	
		i++;
	}

}

void screen_basic_enable_cursor(unsigned char cursor_start, unsigned char cursor_end)
{
	
	outb(0x3D4, 0x0A);
	outb(0x3D5, (uint8_t) ((inb(0x3D5U) & 0xC0U) | (cursor_start & 0x0FU)));

	outb(0x3D4, 0x0B);
	outb(0x3D5, (uint8_t) ((inb(0x3D5U) & 0xE0U) | (cursor_end & 0x0FU)));


}

void screen_basic_disable_cursor(void)
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, 0x20);
}

uint8_t screen_basic_move_cursor(unsigned short x, unsigned short y)
{
	uint16_t verify_pos;
	uint16_t position = (uint16_t) (y * SCREEN_BASIC_WIDTH + x);

	/* just to be sure? */
	if(x >= SCREEN_BASIC_WIDTH) x = SCREEN_BASIC_WIDTH - 1;
	if(y >= SCREEN_BASIC_HEIGHT) y = SCREEN_BASIC_HEIGHT - 1;

	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t) ((position >> 8) & 0xFF));

	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t) (position & 0xFF));
	
	verify_pos = screen_basic_get_cursor_position();

	if(position != verify_pos) return SCREEN_BASIC_EXIT_CODE_CURSOR_MOVE_FAIL;
	
	return EXIT_CODE_GLOBAL_SUCCESS;
}

void screen_basic_clear_screen(void){
	/* uint16_t *vidmem = (uint16_t *) 0xb8000;*/
	screen_basic_clear_line(0, SCREEN_BASIC_HEIGHT);
	SCRscreenData.cursorX = 0;
	SCRscreenData.cursorY = 0;
	screen_basic_move_cursor(SCRscreenData.cursorX, SCRscreenData.cursorY);
}


/* TODO: remove screen_basic_getchar() sometime in the future */
char screen_basic_getchar(unsigned int x, unsigned int y)
{
	unsigned char* vidmem = (unsigned char*) 0xb8000;

	if(x > SCREEN_BASIC_WIDTH || y > SCREEN_BASIC_HEIGHT)
		return NULL;

	return (char) (vidmem[((y * SCREEN_BASIC_WIDTH + x) * SCREEN_BASIC_DEPTH)]);
}

void screen_basic_putchar(unsigned int x, unsigned int y, char c)
{
	unsigned char* vidmem = (unsigned char*) 0xb8000;

	if(x > SCREEN_BASIC_WIDTH || y > SCREEN_BASIC_HEIGHT)
		return;

	vidmem[((y * SCREEN_BASIC_WIDTH + x) * SCREEN_BASIC_DEPTH)] = (unsigned char) c;
	vidmem[((y * SCREEN_BASIC_WIDTH + x)* SCREEN_BASIC_DEPTH + 1)] = SCRscreenData.chScreenColor;
}

/*
 * 'Private' part
 */

static void screen_basic_char_put_on_screen(char c){
	unsigned char* vidmem = (unsigned char*) 0xb8000;

	switch(c){
			case ('\b'):
				SCRscreenData.cursorX--;
				vidmem[(SCRscreenData.cursorY * SCREEN_BASIC_WIDTH + SCRscreenData.cursorX)*SCREEN_BASIC_DEPTH] = 0;  
			
			break;
			
			case ('\n'):
			SCRscreenData.cursorY++;
			SCRscreenData.cursorX = 0;
			break;
			
			case ('\t'):
				SCRscreenData.cursorX = (unsigned short) (SCRscreenData.cursorX + 4);
			break;

			default:
			 vidmem[((SCRscreenData.cursorY * SCREEN_BASIC_WIDTH + SCRscreenData.cursorX) * SCREEN_BASIC_DEPTH)] = (unsigned char) c;
		     vidmem[((SCRscreenData.cursorY * SCREEN_BASIC_WIDTH + SCRscreenData.cursorX)* SCREEN_BASIC_DEPTH + 1)] = SCRscreenData.chScreenColor;
		     SCRscreenData.cursorX++;
			 break;
	}

	screen_basic_move_cursor_internal();
	screen_basic_linecheck();	
}

static void screen_basic_move_cursor_internal(void)
{
	uint16_t position = (uint16_t) (SCRscreenData.cursorY * SCREEN_BASIC_WIDTH + SCRscreenData.cursorX);

	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t) ((position >> 8) & 0xFF));

	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t) (position & 0xFF));
}

static void screen_basic_linecheck(void)
{
	if(SCRscreenData.cursorY >= SCREEN_BASIC_HEIGHT - 1){
		screen_basic_scroll(1);
	}
}

static void screen_basic_scroll(unsigned char line)
{
	
	char* vidmemloc = (char*) 0xb8000;
	const unsigned short EndOfScreen = SCREEN_BASIC_WIDTH * (SCREEN_BASIC_HEIGHT - 1) * SCREEN_BASIC_DEPTH;
	unsigned short i;
	
	screen_basic_clear_line(0, (unsigned char) (line - 1));

	for(i = 0; i < EndOfScreen; i++){
		/* a signed integer will be fine for here */
		vidmemloc[i] = vidmemloc[(int) (i + SCREEN_BASIC_WIDTH * SCREEN_BASIC_DEPTH * line)];
	}
	screen_basic_clear_line( (unsigned char) (SCREEN_BASIC_HEIGHT -  1 - line), (SCREEN_BASIC_HEIGHT - 1));
	
	if((SCRscreenData.cursorY - line) < 0){
		SCRscreenData.cursorY = 0;
		SCRscreenData.cursorX = 0;
	}
	else{
		SCRscreenData.cursorY = (unsigned short) (SCRscreenData.cursorY - line);
	}
	screen_basic_move_cursor(SCRscreenData.cursorX, SCRscreenData.cursorY);
}

static void screen_basic_clear_line(unsigned char from, unsigned char to)
{
	
	unsigned short i = (unsigned short) (SCREEN_BASIC_WIDTH * from * SCREEN_BASIC_DEPTH);
	char* vidmem = (char*) 0xb8000;
	
	for (; i < (SCREEN_BASIC_WIDTH*to*SCREEN_BASIC_DEPTH); i++){
		vidmem[(i / 2) * 2 + 1] = (char) SCRscreenData.chScreenColor;
		vidmem[(i / 2) * 2] = 0;
		/*vidmem[i] = 0;*/
	}
}

unsigned short screen_basic_get_cursor_position(void)
{
	uint16_t position = 0;

	outb(0x3D4, 0x0F);
	position |= (uint16_t) inb(0x3D5);

	outb(0x3D4, 0x0E);
	position |= (uint16_t) (inb(0x3D5) <<  8);

	return position;
}