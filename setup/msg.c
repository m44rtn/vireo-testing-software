/*
MIT license
Copyright (c) 2022 Maarten Vermeulen

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

#include "include/msg.h"

#include "lib/screen.h"

void msg_greeting(void)
{
    screen_print("\n=============================\n"
                   "Setup program for Vireo-II"
                "\n=============================\n");

    // Black on RED
    screen_set_color(SCREEN_COLOR_RED << 4);
    screen_print("!! WARNING !! Do NOT use this program on real hardware. It will\n" 
                "overwrite data without user confirmation!\n\n");
    screen_set_color(SCREEN_COLOR_DEFAULT);   
}

void msg_error_config(void)
{
    screen_set_color(SCREEN_COLOR_RED);
    screen_print("Error reading INSTALL.LST\n");
    screen_set_color(SCREEN_COLOR_DEFAULT);
}

void msg_failed_to_write(char *path)
{
    screen_print("Failed to write: ");
    screen_print(path);
    screen_print("\n");
}
