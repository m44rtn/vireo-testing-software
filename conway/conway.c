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

#include "conway.h"

// syslib
#include "lib/screen.h"
#include "lib/kernel.h"
#include "lib/exit_code.h"
#include "lib/util.h"

#define PROGRAM_NAME "CONWAY"
#include "lib/debug.h"

screen_info_t *screen_info = NULL;

static void conway_fillScreen(void);
static uint8_t conway_check_neighbours(char *buffer, uint32_t x, uint32_t y);

int main(void)
{
    conways_game_of_life();

    return EXIT_CODE_GLOBAL_SUCCESS;
}

void conways_game_of_life(void)
{
    uint32_t x, y;
    uint8_t neighbours;

    err_t err; // ignored
    screen_info = screen_get_info(&err);

    conway_fillScreen();
    
    while(1)
    {
        kernel_sleep(64); // ~ 16 fps
        void *b_screen = screen_get_buffer(&err);

        assert(!err);
        
        for(y = 0; y < screen_info->height; ++y)
        {
            for(x = 0; x < screen_info->width; ++x)
            {
                neighbours = conway_check_neighbours(b_screen, x, y);
                char c = conway_get_screen_byte(b_screen, x, y);

                if(neighbours == 3)
                    screen_print_at("X", x, y);
                else if(c == 'X' && neighbours == 2)
                    screen_print_at("X", x, y);
                else
                    screen_print_at(" ", x, y);
          }
        }

        vfree(b_screen);
    }
}

static void conway_fillScreen(void)
{
    screen_clear();

    // XX
    // XX
    screen_print_at("X", 1, 5);
    screen_print_at("X", 2, 5);
    screen_print_at("X", 1, 6);
    screen_print_at("X", 2, 6);

    //       XX
    //      X  
    //     X    
    //     X   
    //     X   
    //      X
    //       XX
    screen_print_at("X", 14, 2);
    screen_print_at("X", 13, 2);
    screen_print_at("X", 12, 3);
    screen_print_at("X", 11, 4);
    screen_print_at("X", 11, 5);
    screen_print_at("X", 11, 6);
    screen_print_at("X", 12, 7);
    screen_print_at("X", 13, 8);
    screen_print_at("X", 14, 8);

    // X
    screen_print_at("X", 15, 5);

    // X
    //  X
    //  XX
    //  X
    // X
    screen_print_at("X", 16, 3);
    screen_print_at("X", 17, 4);
    screen_print_at("X", 17, 5);
    screen_print_at("X", 18, 5);
    screen_print_at("X", 17, 6);
    screen_print_at("X", 16, 7);

    //   X
    // XX
    // XX
    // XX
    //   X
    screen_print_at("X", 23, 1);

    screen_print_at("X", 21, 2);
    screen_print_at("X", 21, 3);
    screen_print_at("X", 21, 4);
    screen_print_at("X", 22, 2);
    screen_print_at("X", 22, 3);
    screen_print_at("X", 22, 4);

    screen_print_at("X", 23, 5);

    // X
    // X
    // 
    // 
    // 
    // X
    // X
    screen_print_at("X", 25, 0);
    screen_print_at("X", 25, 1);
    screen_print_at("X", 25, 5);
    screen_print_at("X", 25, 6);


    // XX
    // XX
    screen_print_at("X", 35, 2);
    screen_print_at("X", 36, 2);
    screen_print_at("X", 35, 3);
    screen_print_at("X", 36, 3);
}

static uint8_t conway_check_neighbours(char *buffer, uint32_t x, uint32_t y)
{
    uint32_t xmax = x + 2;
    uint8_t cells = 0;

    // top & bottom
    for(uint32_t tx = (x == 0) ? x : x - 1; tx < xmax; ++tx)
    {
        if(y > 0 && conway_get_screen_byte(buffer, tx, y - 1) == 'X')
            cells++;
        
        if(conway_get_screen_byte(buffer, tx, y + 1) == 'X')
            cells++;
    }
    
    // left & right
    if(x > 0 && conway_get_screen_byte(buffer, x - 1, y) == 'X')
        cells++;
    if(conway_get_screen_byte(buffer, x + 1, y) == 'X')
        cells++;

    return cells;

}

char conway_get_screen_byte(char *buffer, uint32_t x, uint32_t y)
{
    if(x > screen_info->width || y > screen_info->height)
        return ' ';

    return buffer[(y * screen_info->width + x) * screen_info->depth];
}
