#ifndef _SIGS_H
#define _SIGS_H
#include "jobs.hpp"
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

void Ctrl_C_handler(Manager& manager);
void Ctrl_Z_handler(Manager& manager)

#endif

