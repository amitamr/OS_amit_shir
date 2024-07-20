#ifndef _ACCOUNT_H
#define _ACCOUNT_H
#include <iostream>
#include <vector>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string> 
#include <cstring>

class Account{
    public:
    int acc_num;
    int password;
    int balance;
    int rd_count;
    pthread_mutex_t bank_acc_lock;
    bool valid;


    Account(int account, int new_password, int new_balance); //Constructor

    ~Account(); //Distructor

}

