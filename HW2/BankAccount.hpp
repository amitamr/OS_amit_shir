#ifndef _BANKACCOUNT_H
#define _BANKACCOUNT_H
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
    bool curr_writing;
    int wr_wait;
    bool valid;
    pthread_mutex_t bank_acc_lock;
    pthread_cond_t acc_rd_cond;
    pthread_cond_t acc_wr_cond;


    Account(int account, int new_password, int new_balance); //Constructor

    ~Account(); //Distructor
    void acc_rd_start();
    void acc_rd_end();
    void acc_wr_start();
    void acc_wr_end();

};

#endif