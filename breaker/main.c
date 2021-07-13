/*
MIT license
Copyright (c) 2021 Maarten Vermeulen

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

#include "lib/memory.h"
#include "lib/kernel.h"
#include "lib/screen.h"


void main(void)
{
    screen_clear();
    screen_print("Breaker for ");
    char *ver = kernel_get_version_str();
    screen_print((const char *) ver);
    screen_print("\n");

    vfree(ver);

    screen_print("(c) 2021 MIT licensed\n");
    screen_print("==============================\n\n");

    screen_print("This program will continue in 10 seconds...\n");
    
    kernel_sleep(10 * 1000);

    screen_set_color((SCREEN_COLOR_BLACK << 4) | SCREEN_COLOR_GREEN);
    screen_print("OK.\n");
    screen_set_color(SCREEN_COLOR_DEFAULT);

    while(1);
}