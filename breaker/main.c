/*
MIT license
Copyright (c) 2021-2022 Maarten Vermeulen

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
#include "lib/disk.h"
#include "lib/program.h"
#include "lib/util.h"
#include "lib/driver.h"
#include "lib/api.h"
#include "lib/fs.h"

#define PROGRAM_NAME    "BREAKER"
#include "lib/debug.h"

void api_handler(void *req);

int main(void)
{
    // check if the syscalls exist
    assert(debug_nop());

    screen_clear();
    screen_print("Breaker for ");
    
    char *ver = kernel_get_version_str();
    screen_print((const char *) ver);
    screen_print("\n");
    
    vfree(ver);

    screen_print("(c) 2021-2022 MIT licensed\n");
    screen_print("==============================\n\n");

    screen_print("This program will continue in 3 seconds...\n");
    
    kernel_sleep(3 * 1000);

    partition_info_t *p = disk_get_partition_info((char *) "HD0P0");
    char *s = valloc(128);
    str_add_val(&s[0], "starting sector: %i\n", p->starting_sector);
    screen_print(&s[0]);
    vfree(p);

    disk_info_t *d = disk_get_drive_list();
    str_add_val(&s[0], "disk sector size: %i\n\0", d[0].sector_size);
    screen_print(&s[0]);
    str_add_val(&s[0], "disk size (in bytes): %i\n\0", d[0].disk_size);
    screen_print(&s[0]);
    vfree(p);
    
    screen_print("Will start register, execute and de-register driver 'drv' in 3 seconds:\n");
    kernel_sleep(3000);
    uint32_t *params = valloc(5 * sizeof(uint32_t));
    memset(s, 128, 0);
    str_add_val(&s[0], "&params: %x \n\0", (uint32_t) &params[0]);
    screen_print(&s[0]);
    driver_add("CD0/TEST/DRV.DRV", 1);

    api_space_t api = api_get_api_space((function_t) api_handler);
    syscall_hdr_t test = {.system_call = (api_space_t) (api + 3u)};

    screen_print("\n");
    PERFORM_SYSCALL(&test);

    // try to remove a reserved kernel api space
    api_free_api_space(0xc00); 
    assert(debug_nop());

    // oops, sorry, wrong one! :P
    api_free_api_space((api_space_t) (api + 3u));
    PERFORM_SYSCALL(&test);

    screen_print("Saving file...\n");
    char *test_file = (char *) "THIS IS A TEST!!!!!!!!!!!!\n haha!";
    fs_write_file((char *) "HD0P0/FILE.TXT", test_file, strlen(test_file) + 1, FAT_FILE_ATTRIB_FILE);
    
    screen_set_color((SCREEN_COLOR_BLACK << 4) | SCREEN_COLOR_GREEN);
    screen_print("OK.\n");
    screen_set_color(SCREEN_COLOR_DEFAULT);
    
    screen_print("Will start conway.elf in 3 seconds:\n");
    
    kernel_sleep(3 * 1000);
    
    err_t e = program_start_new("CD0/TEST/CONWAY.ELF\0", (function_t) (main));

    // will not get here
    str_add_val(&s[0], "error: %x\n\0", e);
    screen_print(&s[0]);

    return 0;
}

void api_handler(void *req)
{
    syscall_hdr_t *hdr = req;

    screen_print("API SPACE WORKS! :)\n");
    screen_print("SYSCALL: ");
    screen_print(intstr(hdr->system_call));
    screen_print("\n");
}
