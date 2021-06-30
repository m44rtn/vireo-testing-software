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

#include "util.h"

#include "../include/types.h"
#include "../io/io.h"

#define p_ctrl_port 0x3F6

void ticks(void)
{
	for(uint32_t i = 100000/1.5f; i > 0; --i)
	{
		// four of these apparently waste ~400ns meaning:
		// 100000 / 1.5 = 66666 2/3
		// 66666 2/3 * 400 = 26666666 2/3 ns
		// (26666666 2/3) / 1000 / 1000 = ~27 milliseconds
		inb(p_ctrl_port);
		inb(p_ctrl_port);
		inb(p_ctrl_port);
		inb(p_ctrl_port);
	}
}
