// #define _POSIX_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

int symlink(const char *target, const char *linkpath);


#define VERIFY_CONTRACT(contract, message) \
if (!(contract)) { \
    printf(message); \
    exit(-1); \
}

void next_name(char *pathname, int *size);
void update_path(char* pathname, char* filename);

static const char linkdir[] = "links/";

int main(void)
{
    int fd;
    int     total = 0;
    char    pathname[1000]  = "links/";
    char    prev_file[1000] = "";
    char    cur_file[1000]  = "a";
    int     size = strlen(cur_file);

    // Create directory for links
    VERIFY_CONTRACT((mkdir(pathname, O_CREAT | 0777) >= 0), "Cannot create directory\n");

    // Create first file
    update_path(pathname, cur_file);

    VERIFY_CONTRACT(
            (fd = open(pathname, O_CREAT | O_RDWR, 0777)) >= 0, 
            "Can\'t open file\n");

    VERIFY_CONTRACT(close(fd) >= 0, "Cannot close file\n");


    // create symlinks recursive
    while (1)
    {
        // update file names
        strcpy(prev_file, cur_file);
        next_name(cur_file, &size);
        update_path(pathname, cur_file);

        if (total >= 10000)
            break;

        // create symlink
        VERIFY_CONTRACT(symlink(prev_file, pathname) >= 0, "Cannot create symlink\n");

        // try to open symlink
        if ((fd = open(pathname, 0)) != -1)
            total++;
        else
            break;

        // close file
        VERIFY_CONTRACT(close(fd) >= 0, "Cannot close file\n");

    }

    printf("%d\n", total);
    return 0;
}

void update_path(char* pathname, char* filename)
{
    strcpy(pathname, linkdir);
    strcat(pathname, filename);
}

void next_name(char *pathname, int *size)
{
    int pos = *size - 1;

    while ((pos >= 0) && (pathname[pos] == 'z'))
        pos--;

    memset(pathname + pos + 1, 'a', *size - pos - 1);

    if (pos == -1)
    {
        strcat(pathname, "a");
        *size += 1;
        return;
    }

    pathname[pos] += 1;
}