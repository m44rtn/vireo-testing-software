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

#include "include/fman.h"
#include "include/config.h"
#include "include/usr.h"
#include "include/msg.h"

#define PROGRAM_NAME    "SETUP"
#include "lib/debug.h"
#include "lib/memory.h"
#include "lib/disk.h"
#include "lib/util.h"

#define MAX_PATH_LEN                512 // bytes
#define INSTALL_LST_PATH            "%s/INSTALL.LST"

#define FAT32_CODE_SECTION_START    0x5a
#define SYSLINUX_BPB_HDR_LEN        0x0B // bytes

#define PATH_DELIM                  "/"

char *fman_get_config_file(char *bootdisk_name, size_t *o_size)
{
    char *path = valloc(DISK_ID_MAX_SIZE + sizeof(INSTALL_LST_PATH));

    str_add_val(path, INSTALL_LST_PATH, (unsigned int) bootdisk_name);

    err_t err;
    file_t *f = fs_read_file(path, o_size, &err);

    vfree(path);
    return (err) ? NULL : (char *) f;
}

void fman_write_to_install_disk(char *path, file_t *file, size_t fsize)
{
    screen_print(path);
    screen_print("\n");
    if(path[0] == CONFIG_SECTOR_NUMBER)
    {
        fman_write_absolute(path, usr_get_selected_install_disk(), file, fsize);
        return;
    }

    err_t err = fs_write_file(path, file, fsize, FAT_FILE_ATTRIB_FILE);

    if(!err) 
        return;    
    
    if(err != EXIT_CODE_FS_FILE_NOT_FOUND)
    {
        msg_failed_to_write(path);
        return;
    }

    // if we get here then the directory was not found
    fman_new_dir(path);
    fs_write_file(path, file, fsize, FAT_FILE_ATTRIB_FILE);
}

static uint32_t fman_convert_to_int(char *str)
{
    // something that has behavior like this would actually be
    // nice to have in syslib util

    uint32_t res = 0;
    size_t len = strlen(str);

    for(uint32_t i = 0; i < len; ++i)
    {
        uint32_t val = strdigit_to_int(str[i]);

        for(uint32_t j = 0; j < i; ++j)
            val = val * 10;
        
        res = res + val;
    }

    return res;
}

static err_t fman_prepare_bootloader(char *disk, file_t *file)
{
    uint8_t *f = file;

    // read current FAT32 BPB
    uint8_t *sctr = disk_absolute_read(disk, 0, 1);
    
    if(!sctr)
        return EXIT_CODE_GLOBAL_GENERAL_FAIL;

    // copy the BPB information to the bootloader file    
    memcpy(&f[SYSLINUX_BPB_HDR_LEN], &sctr[SYSLINUX_BPB_HDR_LEN], (FAT32_CODE_SECTION_START - SYSLINUX_BPB_HDR_LEN));

    vfree(sctr);
    return EXIT_CODE_GLOBAL_SUCCESS;
}

void fman_write_absolute(char *sn, char *disk, file_t *file, size_t size)
{    
    uint32_t sect_num = fman_convert_to_int(&sn[1]);

    err_t err = EXIT_CODE_GLOBAL_SUCCESS;
    
    if(sect_num == 0)
       err = fman_prepare_bootloader(disk, file);
    
    assert(!err);

    err = disk_absolute_write(disk, sect_num, file, size);
}

void fman_new_dir(char *path)
{
    fs_mkdir(path);
}
