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

#include "include/config.h"
#include "include/usr.h"
#include "include/fman.h"

#define PROGRAM_NAME    "SETUP"
#include "lib/debug.h"
#include "lib/fs.h"
#include "lib/disk.h"
#include "lib/memory.h"
#include "lib/util.h"

#define LINE_LEN            512

#define LINE_DELIM          "\n"
#define ACTION_DELIM        " " // delimiter between [copy from] and [write to]

#define LANG_COMMENT        '#'
#define LANG_SECTOR_NUMBER  CONFIG_SECTOR_NUMBER

#define PATH_FROM_SELECTED  '/'

void config_exec(char *config, char *bootdisk)
{
    char *line = valloc(LINE_LEN);
    uint32_t pindex = 0;

    while(str_get_part(line, config, LINE_DELIM, &pindex))
    {
        if(line[0] == LANG_COMMENT)
            continue;
        else if(line[0] < PATH_FROM_SELECTED || line[0] > 'z')
            continue;

        config_exec_line(line, bootdisk);
    }

    vfree(line);
}

// !! WARNING !! Destroys original string!
void config_add_disk_to_path(char *path, char *disk)
{
    // !! WARNING !! Destroys original string!

    // something that has behavior like this would actually be
    // nice to have in util
    
    size_t dlen = strlen(disk);
    size_t plen = strlen(path);

    for(uint32_t i = plen; i > 0; --i)
        path[i + dlen] = path[i];
    
    memcpy(path, disk, dlen);
    path[dlen] = '/';
}

void config_exec_line(char *line, char *bootdisk)
{
    char *copy_from = valloc(LINE_LEN),
         *copy_to = (char *) ((copy_from) + (LINE_LEN / 2));
    uint32_t pindex = 0;

    assert(copy_from);

    str_get_part(copy_from, line, ACTION_DELIM, &pindex);
    str_get_part(copy_to, line, ACTION_DELIM, &pindex);

    if(copy_from[0] == PATH_FROM_SELECTED)
        config_add_disk_to_path(copy_from, bootdisk);
    
    if(copy_to[0] == PATH_FROM_SELECTED)
        config_add_disk_to_path(copy_to, usr_get_selected_install_disk());
    
    size_t fsize;
    err_t err;

    file_t *file = fs_read_file(copy_from, &fsize, &err);
   
    if(file && !err)
        fman_write_to_install_disk(copy_to, file, fsize);
    
    vfree(copy_from); // also frees copy_to (same alloc space)
    vfree(file);
}
