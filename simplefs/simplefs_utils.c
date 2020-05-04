#include "simplefs_utils.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//define parse_path error
#define EINPATH -150

int parse_path(char* path, char*** subpath)
{
    int dirCount = 0;
    //path is invalid if path is NULL
    //or it is not absolute
    if(path == 0 || strncmp(path, "/", 1))
    {
        return EINPATH;
    }
    //count approximate number of path's components
    for (int i = 0; path[i]; ++i)
    {
        dirCount += (path[i] == '/');
    }
    //allocate space for all names and root dir
    *subpath = (char** ) malloc((dirCount + 1) * sizeof(char*));

    int i = 0;
    char* tmp = strdup(path);
    char* token = strtok(tmp, "/");

    (*subpath)[i++] = strdup("/");
    while(token != NULL)
    {
        (*subpath)[i] = strdup(token);
        token = strtok(NULL, "/");
        ++i;
    }

    free(tmp);
    return i;
}

