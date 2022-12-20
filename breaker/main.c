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
#include "lib/call.h"
#include "lib/fs.h"
#include "lib/ps2keyb.h"
#include "lib/scancode.h"

#define PROGRAM_NAME    "BREAKER"
#include "lib/debug.h"

void api_handler(void *req);
api_space_t keyboard_api_space(void);
void start_conway(void);

api_space_t keyboard_api_space(void)
{
    api_listing_t *apis = api_get_syscall_listing();

    for(uint32_t i = 0x00; i < 0xFF; i++)
    {
        if(apis[i].filename[0] == '\0')
            continue;

        if(!strcmp_until(&apis[i].filename[0], "PS2KEYB.DRV", sizeof(apis[i].filename)))
            return apis[i].start_syscall_space; 
    }

    return 0;
}

void start_conway(void)
{
    api_space_t keyb_space = keyboard_api_space();

    assert(keyb_space);
    ps2keyb_api_req r = {
        .hdr.system_call = (syscall_t) (keyb_space | PS2KEYB_CALL_LAST_KEY)
    };
    
    screen_print("Will start conway.elf when you press [ENTER]:\n");

    uint16_t keycode = KEYCODE_UNUSED;
    while(keycode != KEYCODE_ENTER && keycode != KEYCODE_KEYP_ENTER)
    {
        PERFORM_SYSCALL(&r);
        keycode = (uint16_t) r.hdr.response;
    }
    
    if(program_start_new("CD0/CONWAY.ELF"))
        screen_print("Error executing conway");

    // should not get here
}

int main(uint32_t argc, char **argv)
{
    char s[128];
    
    // check if the syscalls exist
    assert(debug_nop());

    // screen_clear();
    screen_print("Breaker for ");
    
    char *ver = kernel_get_version_str();
    screen_print((const char *) ver);
    screen_print("\n");
    
    vfree(ver);

    screen_print("(c) 2021-2022 MIT licensed\n");
    screen_print("==============================\n\n");
    
    screen_print("This program will continue in 3 seconds...\n");
    
    kernel_sleep(3 * 1000);

    str_add_val(&s[0], "\nargc: %x, ", (uint32_t) argc);
    screen_print(&s[0]);

    str_add_val(&s[0], "argv[0]: %s, ", (uint32_t) argv[0]);
    screen_print(&s[0]);
    str_add_val(&s[0], "argv[1]: %s, ", (uint32_t) argv[1]);
    screen_print(&s[0]);
    str_add_val(&s[0], "argv[2]: %s\n\n", (uint32_t) argv[2]);
    screen_print(&s[0]);

    partition_info_t *p = disk_get_partition_info((char *) "HD0P0");
    str_add_val(&s[0], "starting sector: %i\n", p->starting_sector);
    screen_print(&s[0]);
    vfree(p);

    disk_info_t *d = disk_get_drive_list();
    str_add_val(&s[0], "disk sector size: %i\n\0", d[0].sector_size);
    screen_print(&s[0]);
    str_add_val(&s[0], "disk size (in bytes): %i\n\0", d[0].disk_size);
    screen_print(&s[0]);
    vfree(p);
    
    driver_add("CD0/SYS/PS2KEYB.DRV", 1);
    screen_print("\nStarted PS2KEYB.DRV (PS/2 keyboard driver)\n");

    api_space_t api = api_get_api_space((function_t) api_handler);
    syscall_hdr_t test = {.system_call = (api_space_t) (api + 3u)};

    screen_print("\n");
    PERFORM_SYSCALL(&test);

    screen_print("Out of syscall\n");

    // try to remove a reserved kernel api space
    api_free_api_space(0xc00); 
    assert(debug_nop()); // ensure that the request was ignored (i.e. the api space is still working)

    // oops, sorry, wrong one! :P
    api_free_api_space((api_space_t) (api + 3u));
    PERFORM_SYSCALL(&test);

    screen_print("Saving file...\n");
    char *test_file = (char *) "THIS IS A TEST!!!!!!!!!!!!\n haha!";
    fs_write_file((char *) "HD0P0/FILE.TXT", test_file, strlen(test_file) + 1, FAT_FILE_ATTRIB_FILE);
    
    screen_set_color((SCREEN_COLOR_BLACK << 4) | SCREEN_COLOR_GREEN);
    screen_print("OK.\n");
    screen_set_color(SCREEN_COLOR_DEFAULT);

    start_conway();

    return EXIT_CODE_GLOBAL_GENERAL_FAIL;
}

void api_handler(void *req)
{
    syscall_hdr_t *hdr = req;

    screen_print("API SPACE WORKS! :)\n");
    screen_print("SYSCALL: ");
    screen_print(hexstr(hdr->system_call, 0));
    screen_print("\n");
}
