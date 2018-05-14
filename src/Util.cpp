#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include "RoboVision/Util.h"

using namespace std;

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

// Utility class for utility functions.
Util::Util() {}

int Util::isFile( char* name) {
    // fprintf(stdout, "isFile:: checking path: %s . . .\n", name);
    struct stat path_stat;
    stat(name, &path_stat);
    
    if (S_ISDIR(path_stat.st_mode)) {
        // folder:
        return 0;
    } else if (S_ISREG(path_stat.st_mode)) {
        // regular file:
        return 1;
    }
    return -1;
}

std::vector<std::string> Util::getFileNames(char* dirPath) {
    std::vector<std::string> fileNames;
    DIR *dir = opendir(dirPath);
    struct dirent *ent;
    int index = strlen(dirPath);
    // add file separator if it wasn't passed.
    if (dirPath[index-1] != '/') {
        strcat(dirPath, "/");
    }
    while ((ent = readdir (dir)) != NULL) {
        char* fileName = ent->d_name;
        // +1 because of '\0' at the end
        char* fileStr = (char*) malloc(strlen(dirPath) + 1);
        strcpy(fileStr, dirPath);
        strcat(fileStr, fileName);
        if ((Util::isFile(fileStr) == 1) && (fileName[0] != '.')) {
            fileNames.push_back(fileStr);
        } else {
            //DEBUGGING
            if (fileName[0] != '.') {
               printf("Util::getFileNames: %s is not a regular file, skipping\n", fileStr); 
            }
        }
    }
    closedir(dir);
    return fileNames;
}

void Util::debugPrint(const char* methodName, const char* message) {
    printf(ANSI_COLOR_CYAN"DEBUG::%s -- %s\n" ANSI_COLOR_RESET, methodName, message);
}

void Util::errorPrint(const char* methodName, const char* message) {
    printf(ANSI_COLOR_RED "ERROR::%s -- %s\n" ANSI_COLOR_RESET, methodName, message);
}
