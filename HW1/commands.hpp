#ifndef _COMMANDS_H
#define _COMMANDS_H
#include <unistd.h> 
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "jobs.hpp"
#include <fcntl.h>


#define MAX_LINE_SIZE 80
#define MAX_ARG 20
//typedef enum { FALSE , TRUE } bool;
bool compareByJobID(const Job &a, const Job &b);
int BgCmd(char* lineSize, Manager& manager);
int ExeCmd(Manager& manager, char* lineSize, char* cmdString);
void ExeExternal(char *args[MAX_ARG], char* cmd, char* cmdString , Manager& manager);
bool is_built_in(char* cmd);
#endif

