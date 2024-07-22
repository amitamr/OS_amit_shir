#ifndef _BANK_H
#define _BANK_H
#include <iostream>
#include <vector>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <ctime> 
#include <random>
#include <fstream>
#include "BankAccount.hpp"
#include "atm.hpp"


#define SLEEP_ACT_TIME 1
#define WAKEUP_ATM_TIME 0.1


struct atm_data{
    int thread_id;
    char* filename;
};

class Bank{
public:
    std::vector<Account> accounts;
    //Account Bank_Meta; //bank's secured account
    int bank_balance;
    int accounts_cnt;
    int rd_count;
    int wr_wait;
    bool curr_writing;

    pthread_mutex_t bank_lock;
    //pthread_mutex_t rd_lock;
   // pthread_mutex_t bank_lock;

   pthread_cond_t rd_cond;
   pthread_cond_t wr_cond;
   


    Bank(); // Constructor
    ~Bank(); //Distructor

    void addaccount(int account_num, int password, int initial_amount);
    void removeaccount(int account_num);
    int findAccount(int account);
    void bank_rd_start(int thread_id);
    void bank_rd_end(int thread_id);
    void bank_wr_start(int thread_id);
    void bank_wr_end(int thread_id);

};

void* commission(void* args); //charges random commission from all accounts every 3 secs
void* printAccounts(void* args); //prints all accounts to the screen every half sec

#endif