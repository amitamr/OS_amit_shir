#include "BankAccount.hpp"


Account::Account(int account, int new_password, int new_balance) : acc_num(account), password(new_password), balance(new_balance),
                                                                   rd_count (0), curr_writing(0), wr_wait(0), valid(1){
    pthread_mutex_init(&bank_acc_lock, NULL);
    pthread_cond_init(&acc_rd_cond, NULL);
    pthread_cond_init(&acc_wr_cond, NULL);
}

Account::~Account(){}

void BankAccount::acc_rd_start(){
        pthread_mutex_lock(&bank_acc_lock);
        while(curr_writing || wr_wait > 0){ //the logic rd_count != 0 is not included to prevent writer starvation- if there is a writer waiting then stop entering reader
            pthread_cond_wait(&acc_rd_cond, &bank_acc_lock);
        }
        rd_count++;
        pthread_cond_broadcast(&acc_rd_cond); //wake up the other reading threads
        pthread_mutex_unlock(&bank_acc_lock);
    }

    void BankAccount::acc_rd_end(){
        pthread_mutex_lock(&bank_acc_lock);
        rd_count--;
        if(rd_count == 0){
            pthread_cond_signal(&acc_wr_cond); //if we want several writers shpuld change to broadcast
        }
        pthread_mutex_unlock(&bank_acc_lock);
    }

    void BankAccount::acc_wr_start(){
        pthread_mutex_lock(&bank_acc_lock);
        wr_wait++;
        while(rd_count > 0 || curr_writing){
            pthread_cond_wait(&acc_wr_cond, &bank_acc_lock);

        }
        curr_writing = true;
        wr_wait--;
        pthread_mutex_unlock(&bank_acc_lock);
    }

    void BankAccount::acc_wr_end(){
        pthread_mutex_lock(&bank_acc_lock);
        curr_writing = false;
        pthread_cond_broadcast(&acc_rd_cond);
        pthread_cond_signal(&acc_wr_cond);
        pthread_mutex_unlock(&bank_acc_lock);
    }

