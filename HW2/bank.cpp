#include "bank.hpp"

Bank::Bank(): bank_balance(0), accounts_cnt(0), rd_count(0), wr_wait(0), curr_writing(false){
    std::vector<Account> accounts;
    pthread_mutex_init(&bank_lock, NULL);
    pthread_cond_init(&rd_cond, NULL);
    pthread_cond_init(&wr_cond, NULL);
}

Bank::~Bank(){};

int Bank::findAccount(int account){

    for(int i = 0; i < accounts_cnt; i ++){
        if (accounts[i].acc_num == account){
            return i;
        }
    }
    return -1;
}

void Bank::addaccount(int account_num, int password, int initial_amount){
    accounts.push_back(Account(account_num, password, initial_amount));
    accounts_cnt++;
}
void Bank::removeaccount(int acc_index){
    account.erase(accounts.begin() + acc_index);
    accounts_cnt--;
}

    void Bank::bank_rd_start(){
        pthread_mutex_lock(&bank_lock);
        while(curr_writing || wr_wait > 0){
            pthread_cond_wait(&rd_cond, &bank_lock);
        }
        rd_count++;
        pthread_cond_broadcast(&rd_cond); //wake up the other readind threads
        pthread_mutex_unlock(&bank_lock);
    }

    void Bank::bank_rd_end(){
        pthread_mutex_lock(&bank_lock);
        rd_count--;
        if(rd_count == 0){
            pthread_cond_signal(&wr_cond);
        }
        pthread_mutex_unlock(&bank_lock);
    }

    void Bank::bank_wr_start(){
        pthread_mutex_lock(&bank_lock);
        wr_wait++;
        while(rd_cond > 0 || curr_writing){
            pthread_cond_wait(&wr_cond, &bank_lock);

        }
        curr_writing = true;
        wr_wait--;
        pthread_mutex_unlock(&bank_lock);
    }

    void Bank::bank_wr_end(){
        pthread_mutex_lock(&bank_lock);
        curr_writing = false;
        pthread_cond_broadcast(&rd_cond);
        pthread_cond_signal(&wr_cond);
        pthread_mutex_unlock(&bank_lock);
    }

int main{









    
}
