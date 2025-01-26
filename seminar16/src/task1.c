#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#define _POSIX_SOURCE

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <time.h>

#define VERIFY_CONTRACT(contract, message) \
if (!(contract)) { \
    printf(message); \
    exit(-1); \
}

#define VERIFY_CONTRACT_F(contract, message, ...) \
if (!(contract)) { \
    printf(message, ##__VA_ARGS__); \
    exit(-1); \
}


void  ls(char *pathname);
void  ls_file(char *name, char* dir);
void  print_access_flags(mode_t mode);


//
// Short doc: README.md
//

int main(int argc, char* argv[])
{
    // if no argument provided, ls working directory
    if (argc == 1)
        ls("./");
    else // ls provided file or directory
        for (int i = 1; i < argc; ++i)
            ls(argv[i]);
    return 0;
}

void ls(char *pathname)
{
    // read file attributes
    struct stat file_stat;
    VERIFY_CONTRACT_F(stat(pathname, &file_stat) >= 0, "Cannot access \"%s\": no such file or directory", pathname);

    // if not directory, ls single file
    if (!S_ISDIR(file_stat.st_mode))
    {
        ls_file(pathname, "./");
        return;
    }

    //
    // list the directory
    //
    printf("%s:\n", pathname); // print pathname as list header

    // open the directory
    DIR* dir;
    VERIFY_CONTRACT((dir = opendir(pathname)) != NULL, "Cannot open directory\n");

    // read each file from directory in loop
    struct dirent *entry;
    while ((entry = readdir(dir)))
    {
        ls_file(entry->d_name, pathname);
    }

    free(entry);
}

void ls_file(char *name, char* dir)
{
    char path[256]; // relative path to name from working directory (not dir)

    // path := dir + "/" + name
    int len = strlen(dir);
    strcpy(path, dir);

    if (path[len - 1] != '/')
        strcat(path, "/");
    strcat(path, name);

    // read file attributes
    struct stat file_stat;
    VERIFY_CONTRACT_F(stat(path, &file_stat) >= 0, "Cannot access \"%s\": no such file or directory\n", name);

    // print access flags
    print_access_flags(file_stat.st_mode);

    // print number of hard links
    printf("%2lu ", file_stat.st_nlink);

    // print group name
    struct group* grp;
    grp = getgrgid(file_stat.st_gid);
    printf("%10s ", grp->gr_name);

    // print username
    struct passwd* pw;
    pw = getpwuid(file_stat.st_uid);
    printf("%10s ", pw->pw_name);

    // print file size
    printf("%6lu ", file_stat.st_size);

    // print name
    printf("%s ", name);   

    printf("\n");
}

void print_access_flags(mode_t mode)
{   
    if      (S_ISREG(mode))  printf("-");
    else if (S_ISDIR(mode))  printf("d");
    else if (S_ISLNK(mode))  printf("l");
    else if (S_ISFIFO(mode)) printf("p");
    else if (S_ISBLK(mode))  printf("b");
    else if (S_ISCHR(mode))  printf("c");
    else { 
        printf("ERROR: unknown file type\n"); 
        exit(-1); 
    }

    char full[] = "rwxrwxrwx";
    for (unsigned mask = 0400, i = 0; mask != 0; mask >>= 1, i++)
        if (!(mode & mask))
            full[i] = '-';
    printf("%s ", full);
}
