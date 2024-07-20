#ifndef _ATM_H
#define _ATM_H
#include <iostream>
#include <vector>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <ctime>
#include <unistd.h>
#include <cstdlib>
#include <fstream>
#include "BankAccount.hpp"
#include "bank.hpp"

#define ACT_TIME 1
#define WAKEUP_ATM_TIME 0.1


void* thread_function(void* filename_arg);
//void openaccount(char* args){};
#endif