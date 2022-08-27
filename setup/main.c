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

#include "lib/screen.h"
#include "lib/memory.h"
#include "lib/kernel.h"
#include "lib/program.h"
#include "lib/driver.h"
#include "lib/disk.h"
#include "lib/util.h"

#include "include/msg.h"
#include "include/fman.h"
#include "include/config.h"

int main(void)
{
    msg_greeting();
    
    screen_print("Will continue in 20 seconds\n");
    kernel_sleep(20 * 1000);

    char bootdisk[DISK_ID_MAX_SIZE];
    char *disk = disk_get_bootdisk();

    memcpy(&bootdisk[0], disk, strlen(disk) + 1);
    vfree(disk);
    
    screen_print("Reading INSTALL.LST...\n");
    size_t s;
    char *config = fman_get_config_file(bootdisk, &s);

    if(!config)
    {
        msg_error_config();
        return EXIT_CODE_GLOBAL_GENERAL_FAIL;
    }

    screen_print("Copying files...\n");
    config_exec(config, bootdisk);

    screen_print("Finished copying files!\n");

    vfree(config);
    
    return EXIT_CODE_GLOBAL_SUCCESS;
}