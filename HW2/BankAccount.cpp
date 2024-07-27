#include "BankAccount.hpp"
using namespace std;

extern pthread_mutex_t log_wrt_lck;
extern ofstream logfile;

Account::Account(int account, int new_password, int new_balance) : acc_num(account), password(new_password), balance(new_balance),
                                                                   rd_count (0), curr_writing(false), wr_wait(0), valid(true){
    pthread_mutex_init(&bank_acc_lock, NULL);
    pthread_cond_init(&acc_rd_cond, NULL);
    pthread_cond_init(&acc_wr_cond, NULL);
}

Account::~Account(){
    pthread_mutex_destroy(&bank_acc_lock);
    pthread_cond_destroy(&acc_rd_cond);
    pthread_cond_destroy(&acc_wr_cond);
}

void Account::acc_rd_start(int thread_id, int acc_num){
        pthread_mutex_lock(&bank_acc_lock);
        // printing for understanding the locks
        pthread_mutex_lock(&log_wrt_lck);
        logfile << "thread number " << thread_id << " catch account " << acc_num << " lock, start reading, rd_count = " << rd_count << endl;
        pthread_mutex_unlock(&log_wrt_lck);

        while(curr_writing || wr_wait > 0){ //the logic rd_count != 0 is not included to prevent writer starvation- if there is a writer waiting then stop entering reader
            
            pthread_mutex_lock(&log_wrt_lck);
            logfile << "thread number " << thread_id << " is about to wait and unlock acc " << acc_num << " lock, start reading, wr_wait = "<< wr_wait << endl;
            pthread_mutex_unlock(&log_wrt_lck);
            
            pthread_cond_wait(&acc_rd_cond, &bank_acc_lock);
        }
        rd_count++;
        pthread_cond_broadcast(&acc_rd_cond); //wake up the other reading threads

        pthread_mutex_lock(&log_wrt_lck);
        logfile << "thread number " << thread_id << " is about to unlock acc " << acc_num << " lock, start reading" << endl;
        pthread_mutex_unlock(&log_wrt_lck);

        pthread_mutex_unlock(&bank_acc_lock);
    }

    void Account::acc_rd_end(int thread_id, int acc_num){
        pthread_mutex_lock(&bank_acc_lock);
        
        pthread_mutex_lock(&log_wrt_lck);
        logfile << "thread number " << thread_id << " catch acc " << acc_num << " lock, end reading , rd_count = " << rd_count << " wr_wait = " << wr_wait << endl;
        pthread_mutex_unlock(&log_wrt_lck);

        rd_count--;
        if(rd_count == 0){
            pthread_cond_signal(&acc_wr_cond); //if we want several writers shpuld change to broadcast
        }

        pthread_mutex_lock(&log_wrt_lck);
        logfile << "thread number " << thread_id << " is about to unlock acc " << acc_num << " lock, end reading, rd_count = " << rd_count << " wr_wait " << wr_wait << endl;
        pthread_mutex_unlock(&log_wrt_lck);

        pthread_mutex_unlock(&bank_acc_lock);
    }

    void Account::acc_wr_start(int thread_id, int acc_num){
        pthread_mutex_lock(&bank_acc_lock);

        pthread_mutex_lock(&log_wrt_lck);
        logfile << "thread number " << thread_id << " catch acc "<< acc_num << " lock, start writing" << endl;
        pthread_mutex_unlock(&log_wrt_lck);

        wr_wait++;
        while(rd_count > 0 || curr_writing){
            pthread_mutex_lock(&log_wrt_lck);
            logfile << "thread number " << thread_id << " is about to wait and unlock acc "<< acc_num<< " lock, start writing, rd_count = " << rd_count << endl;
            pthread_mutex_unlock(&log_wrt_lck);

            pthread_cond_wait(&acc_wr_cond, &bank_acc_lock);

        }
        curr_writing = true;
        wr_wait--;

        pthread_mutex_lock(&log_wrt_lck);
        logfile << "thread number " << thread_id << " is about to unlock acc "<< acc_num << " lock, start writing" << endl;
        pthread_mutex_unlock(&log_wrt_lck);

        pthread_mutex_unlock(&bank_acc_lock);
    }

    void Account::acc_wr_end(int thread_id, int acc_num){
        pthread_mutex_lock(&bank_acc_lock);
        curr_writing = false;
        pthread_cond_broadcast(&acc_rd_cond);
        pthread_cond_signal(&acc_wr_cond);
        pthread_mutex_unlock(&bank_acc_lock);
    }

