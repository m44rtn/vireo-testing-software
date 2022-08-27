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

screen_info_t *screen_info = NULL;

static void conway_fillScreen(void);
static uint8_t conway_check_neighbours(uint32_t x, uint32_t y);
static void conway_shouldIBeAliveOrNotAndMakeItSo(uint32_t x, uint32_t y);

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

    screen_print("hello from conway.elf!\n");


    conway_fillScreen();
    
    while(1)
    {
        kernel_sleep(16); // ~ 60 fps

        for(y = 0; y < screen_info->height; ++y)
        {
            for(x = 0; x < screen_info->width; ++x)
            {
                if(screen_get_byte_at(x, y, &err) != 'X')
                {
                    conway_shouldIBeAliveOrNotAndMakeItSo(x, y);
                    continue;
                }

                neighbours = conway_check_neighbours(x, y);

                if(neighbours < 2 || neighbours > 3) 
                    screen_print_at(" ", x, y);
          }
        }
        
    }
}

static void conway_fillScreen(void)
{
    uint32_t x, y;

    for(y = 0; y < 25; ++y)
            for(x = 0; x < 80; ++x)
                if((!y || x % 2 || y % 3) || (x % 5 && y % 7)) screen_print_at("X", x, y);
}

static uint8_t conway_check_neighbours(uint32_t x, uint32_t y)
{
    uint32_t xa;
    uint8_t live_cells = 0;
    err_t err; // ignored

    uint8_t isxScreenWidth =  !((x+1) == screen_info->width);

    /* check the three places above us (if there's a row above us) */
    if(y > 0)
        for(xa = (x <= 0) ? 0 : x-1; xa <= (x + isxScreenWidth); ++xa)
            if(screen_get_byte_at(xa, y-1, &err) == 'X') ++live_cells;
    
    /* check our row */
    if(x > 0)
        if(screen_get_byte_at(x-1, y, &err) == 'X') ++live_cells;
    
    if(x+1 <= screen_info->width)
        if(screen_get_byte_at(x+1, y, &err) == 'X') ++live_cells;
    
    
    /* check row below us if there is one */
    if(y < screen_info->height)
        for(xa = (x <= 0) ? 0 : x-1; xa <= (x + isxScreenWidth); ++xa)
            if(screen_get_byte_at(xa, y+1, &err) == 'X') ++live_cells;

    return live_cells;

}

static void conway_shouldIBeAliveOrNotAndMakeItSo(uint32_t x, uint32_t y)
{
    if(conway_check_neighbours(x, y) == 3)
        screen_print_at("X", x, y);
}
