#include "bank.hpp"
using namespace std;

Bank::Bank(): bank_balance(0), accounts_cnt(0), rd_count(0), wr_wait(0), curr_writing(false){
    std::vector<Account> accounts;
    pthread_mutex_init(&bank_lock, NULL);
    pthread_cond_init(&rd_cond, NULL);
    pthread_cond_init(&wr_cond, NULL);
}

Bank::~Bank(){
    pthread_mutex_destroy(&bank_lock);
    pthread_cond_destroy(&rd_cond);
    pthread_cond_destroy(&wr_cond);
}

int Bank::findAccount(int account){
    for(int i = 0; i < accounts_cnt; i ++){
        if ((accounts[i].acc_num == account) && (accounts[i].valid == true)){
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
    accounts.erase(accounts.begin() + acc_index);
    accounts_cnt--;
}

    void Bank::bank_rd_start(){
        pthread_mutex_lock(&bank_lock);
        while(curr_writing || wr_wait > 0){ //the logic rd_count != 0 is not included to prevent writer starvation- if there is a writer waiting then stop entering reader
            pthread_cond_wait(&rd_cond, &bank_lock);
        }
        rd_count++;
        pthread_cond_broadcast(&rd_cond); //wake up the other reading threads
        pthread_mutex_unlock(&bank_lock);
    }

    void Bank::bank_rd_end(){
        pthread_mutex_lock(&bank_lock);
        rd_count--;
        if(rd_count == 0){
            pthread_cond_signal(&wr_cond); //if we want several writers shpuld change to broadcast
        }
        pthread_mutex_unlock(&bank_lock);
    }

    void Bank::bank_wr_start(){
        pthread_mutex_lock(&bank_lock);
        wr_wait++;
        while(rd_count > 0 || curr_writing){
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


Bank bank;
ofstream logfile;
pthread_mutex_t log_wrt_lck;

vector<atm_data*> all_atms;
bool atms_done = false;

void* thread_function(void* thread);


int main(int argc, char *argv[]){
    pthread_mutex_init(&log_wrt_lck, NULL);
    logfile.open("log.txt" , ios::out);
    if (!logfile.is_open()) {
        cerr << "Bank error: illegal arguments" << endl;
        return 1;
    }
    int num_atms = argc - 1;
    //char* filename;
    pthread_t* atms = new pthread_t[num_atms];
    //atm_data* atm = new atm_data;
    for (int i=0; i < num_atms; i++){
        atm_data* atm = new atm_data;
        atm->filename = new char[strlen(argv[i+1]) + 1];
        //filename = new char[strlen(argv[i+1]+1)];
        strcpy(atm->filename, argv[i+1]);
        atm->thread_id = i+1;
        all_atms.push_back(atm);
        if (pthread_create(&atms[i], NULL, thread_function, atm) != 0) {
            perror("Bank error: pthread_create failed");
            return 1;
        }
    }

    pthread_t commission_thread;
    if (pthread_create(&commission_thread, NULL, commission , NULL) != 0) {
            perror("Bank error: pthread_create failed");
            return 1;
    }

    pthread_t print_acc_thread;
    if (pthread_create(&print_acc_thread, NULL, printAccounts, NULL) != 0) {
            perror("Bank error: pthread_create failed");
            return 1;
    }

    // Join threads
    for (int i = 0; i < num_atms; i++) {
        if(pthread_join(atms[i], NULL)){
            perror("Error: pthread_join failed");
        }
    }

    atms_done = true;

    //join bank functions
    if (pthread_join(commission_thread, NULL)) {
    	perror("Error: pthread_join failed");
    }
    if (pthread_join(print_acc_thread, NULL)) {
        perror("Error: pthread_join failed");
    }


    logfile.close();
    delete[] atms;
    for (auto atm : all_atms) {
        delete[] atm->filename;
        delete atm;
    }
    pthread_mutex_destroy(&log_wrt_lck);
    return 0;
}


void* commission(void* args){
    while(1)
    {
        if(atms_done){
            pthread_exit(NULL);
            break;
        }
        srand(static_cast<unsigned int>(time(nullptr)));
        int precentage = (rand()% 5) + 1;
        bank.bank_wr_start();
        for(int i=0; i < bank.accounts_cnt; i++){
            int curr_balance = bank.accounts[i].balance;
            int comm = static_cast<int>(curr_balance * (precentage/100.00));
            bank.bank_balance += comm;
            bank.accounts[i].balance = curr_balance - comm;
            pthread_mutex_lock(&log_wrt_lck);
            //critical section
            logfile << "Bank: commissions of " << precentage << " % were charged, bank gained " << comm << " from account " << bank.accounts[i].acc_num << std::endl;
            //unlocking mutex
            pthread_mutex_unlock(&log_wrt_lck);
        }
        bank.bank_wr_end();
        sleep(3);
    }
}
void* printAccounts(void* args){
    while(1)
    {
        if(atms_done){
            pthread_exit(NULL);
            break;
        }
        bank.bank_rd_start();
        printf("\033[2J");
        printf("\033[1;1H");
        cout << "Current Bank Status" << endl;
        for(int i=0; i < bank.accounts_cnt; i++){
            bank.accounts[i].acc_rd_start();
            cout << "Account " << bank.accounts[i].acc_num << ": Balance - " << bank.accounts[i].balance << 
                    " $, Account Password - " << bank.accounts[i].password << endl;   
            bank.accounts[i].acc_rd_end();      
        }
        bank.bank_rd_end();
        usleep(500000);
    }
}