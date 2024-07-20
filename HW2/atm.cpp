#include "atm.hpp"

using namespace std
#define MAX_ARG 5;


Bank bank;
pthread_mutex_t log_wrt_lck;


// creting a logfile


//data structure for pthread create
struct thread_data{
    int thread_id;
    char* filename;
}


void* thread_function(void* thread){
    
    char* cargs[MAX_ARG];
    int args[MAX_ARG];
    const char* delimiters = " \t";
    FILE* file = std::fopen(thread.filename , "r");
    
    if (file == nullptr) {
        std::cerr << "Bank error: illigal arguments" << std::endl;
        exit(1);
    }

    // Buffer to hold each line
    char buffer[1024];
    char action[1024];
    // Read each line from the file
    while (fgets(buffer, sizeof(buffer), file)) {
        // Remove the newline character from the end of the line, if present
        buffer[strcspn(buffer, "\n")] = 0;
        action = strtok(buffer, delimiters);
	    if (action == NULL){ //might be that an exit thread function is needed
		    break; 
        }
        cargs[0] = action;
        args[0] = 0;
        for (i=1; i<MAX_ARG; i++)
        {
            cargs[i] = strtok(NULL, delimiters);
            if (cargs[i] != NULL){
                args[i] = atoi(cargs[i]);
            }
        }

        if (!strcmp(cargs[0], "O")){
            bank.bank_wr_start();
            if(findAccount(args[1]) != -1){
                //locking mutex
                pthread_mutex_lock(&log_wrt_lck);
                //critical section
                logfile << "Error " << thread.thread_id << ": Your transaction failed – account with the same id exists" << std::endl;
                //unlocking mutex
                pthread_mutex_lock(&log_wrt_lck);
                bank.bank_wr_end();
            }
            else{
                bank.addaccount(args[1],args[2],args[3]) //call to a function that create a new account - Shir
                //locking mutex
                pthread_mutex_lock(&log_wrt_lck);
                //critical section
                logfile << thread.thread_id << ": New account id is " << args[1] << " with password " << args[2] << " and initial balance " << args[3] << std::endl;
                //unlocking mutex
                pthread_mutex_lock(&log_wrt_lck);
                bank.bank_wr_end();
            }
            sleep(ACT_TIME);
        }

         if (!strcmp(cargs[0], "D")){
            bank.bank_rd_start();
            int acc_index = findAccount(args[1]);
            if( acc_index != -1){ //account number does not exist
                //locking mutex
                pthread_mutex_lock(&log_wrt_lck);
                //critical section
                logfile << "Error " << thread.thread_id << ": Your transaction failed – account id " << args[1] << " does not exists" << std::endl;
                //unlocking mutex
                pthread_mutex_lock(&log_wrt_lck);
                bank.bank_rd_end();
            }
            else if(bank.accounts[acc_index].password != args[2]){
                //locking mutex
                pthread_mutex_lock(&log_wrt_lck);
                //critical section
                logfile << "Error " << thread.thread_id << ": Your transaction failed – password for account id " << args[1] << " is incorrect" << std::endl;
                //unlocking mutex
                pthread_mutex_lock(&log_wrt_lck);
                bank.bank_rd_end();
            }  
            else{
                bank.bank_rd_end();
                bank.bank_wr_start();
                bank.accounts[acc_index].balance += args[3];
                //locking mutex
                pthread_mutex_lock(&log_wrt_lck);
                //critical section
                logfile << thread.thread_id << ": Account " <<  args[1] << " new balance is " << bank.accounts[acc_index].balance << " after " << args[3] << " $ was deposited" << std::endl;
                //unlocking mutex
                pthread_mutex_lock(&log_wrt_lck);
                bank.bank_wr_end();

            }
            sleep(ACT_TIME);
        }

         if (!strcmp(cargs[0], "W")){
            bank.bank_rd_start();
            int acc_index = findAccount(args[1]);
            if( acc_index != -1){ //account number does not exist
                //locking mutex
                pthread_mutex_lock(&log_wrt_lck);
                //critical section
                logfile << "Error " << thread.thread_id << ": Your transaction failed – account id " << args[1] << " does not exists" << std::endl;
                //unlocking mutex
                pthread_mutex_lock(&log_wrt_lck);
                bank.bank_rd_end();
            }
            else if(bank.accounts[acc_index].password != args[2]){
                //locking mutex
                pthread_mutex_lock(&log_wrt_lck);
                //critical section
                logfile << "Error " << thread.thread_id << ": Your transaction failed – password for account id " << args[1] << " is incorrect" << std::endl;
                //unlocking mutex
                pthread_mutex_lock(&log_wrt_lck);
                bank.bank_rd_end();
            }  
            else if(bank.accounts[acc_index].balance < args[3]) {
                //locking mutex
                pthread_mutex_lock(&log_wrt_lck);
                //critical section
                logfile << "Error " << thread.thread_id << ": Your transaction failed – account id " << args[1] << " balance is lower than " << args[3] << std::endl;
                //unlocking mutex
                pthread_mutex_lock(&log_wrt_lck);
                bank.bank_rd_end();
            }
            else{
                bank.bank_rd_end();
                bank.bank_wr_start();
                bank.accounts[acc_index].balance -= args[3];
                //locking mutex
                pthread_mutex_lock(&log_wrt_lck);
                //critical section
                logfile << thread.thread_id << ": Account " <<  args[1] << " new balance is " << bank.accounts[acc_index].balance << " after " << args[3] << " $ was withdrawn" << std::endl;
                //unlocking mutex
                pthread_mutex_lock(&log_wrt_lck);
                bank.bank_wr_end();
            }
            sleep(ACT_TIME);
        }

         if (!strcmp(cargs[0], "B")){
            bank.bank_rd_start();
            int acc_index = findAccount(args[1]);
            if( acc_index != -1){ //account number does not exist
                //locking mutex
                pthread_mutex_lock(&log_wrt_lck);
                //critical section
                logfile << "Error " << thread.thread_id << ": Your transaction failed – account id " << args[1] << " does not exists" << std::endl;
                //unlocking mutex
                pthread_mutex_lock(&log_wrt_lck);
                bank.bank_rd_end();
            }
            else if(bank.accounts[acc_index].password != args[2]){
                //locking mutex
                pthread_mutex_lock(&log_wrt_lck);
                //critical section
                logfile << "Error " << thread.thread_id << ": Your transaction failed – password for account id " << args[1] << " is incorrect" << std::endl;
                //unlocking mutex
                pthread_mutex_lock(&log_wrt_lck);
                bank.bank_rd_end();
            }  
            else{
                 //locking mutex
                pthread_mutex_lock(&log_wrt_lck);
                //critical section
                logfile << thread.thread_id << ": Account " <<  args[1] << " balance is " << bank.accounts[acc_index].balance << std::endl;
                //unlocking mutex
                pthread_mutex_lock(&log_wrt_lck);
                bank.bank_rd_end();
            }
            sleep(ACT_TIME);
        }

        if (!strcmp(cargs[0], "Q")){
            bank.bank_wr_start();
            int acc_index = findAccount(args[1]);
            if( acc_index != -1){ //account number does not exist
                //locking mutex
                pthread_mutex_lock(&log_wrt_lck);
                //critical section
                logfile << "Error " << thread.thread_id << ": Your transaction failed – account id " << args[1] << " does not exists" << std::endl;
                //unlocking mutex
                pthread_mutex_lock(&log_wrt_lck);
                bank.bank_wr_end();
            }
            else if(bank.accounts[acc_index].password != args[2]){
                //locking mutex
                pthread_mutex_lock(&log_wrt_lck);
                //critical section
                logfile << "Error " << thread.thread_id << ": Your transaction failed – password for account id " << args[1] << " is incorrect" << std::endl;
                //unlocking mutex
                pthread_mutex_lock(&log_wrt_lck);
                bank.bank_wr_end();
            }  
            else{
                int old_balance = bank.accounts[acc_index].balance;
                bank::removeaccount(acc_index); //call to a function that removes an account 
                //locking mutex
                pthread_mutex_lock(&log_wrt_lck);
                //critical section
                logfile << thread.thread_id << ": Account " <<  args[1] << " is now closed. Balance was " << old_balance << std::endl;
                //unlocking mutex
                pthread_mutex_lock(&log_wrt_lck);
                bank.bank_rd_end();
            }
            sleep(ACT_TIME);
        }

         if (!strcmp(cargs[0], "T")){
            bank.bank_rd_start();
            int src_acc_index = findAccount(args[1]);
            int dest_acc_index = findAccount(args[3]);
            if( src_acc_index != -1){ //account number does not exist
                //locking mutex
                pthread_mutex_lock(&log_wrt_lck);
                //critical section
                logfile << "Error " << thread.thread_id << ": Your transaction failed – account id " << args[1] << " does not exists" << std::endl;
                //unlocking mutex
                pthread_mutex_lock(&log_wrt_lck);
                bank.bank_rd_end();
            }
            else if( dest_acc_index != -1){ //account number does not exist
                //locking mutex
                pthread_mutex_lock(&log_wrt_lck);
                //critical section
                logfile << "Error " << thread.thread_id << ": Your transaction failed – account id " << args[3] << " does not exists" << std::endl;
                //unlocking mutex
                pthread_mutex_lock(&log_wrt_lck);
                bank.bank_rd_end();
            }
            else if(bank.accounts[src_acc_index].password != args[2]){
                //locking mutex
                pthread_mutex_lock(&log_wrt_lck);
                //critical section
                logfile << "Error " << thread.thread_id << ": Your transaction failed – password for account id " << args[1] << " is incorrect" << std::endl;
                //unlocking mutex
                pthread_mutex_lock(&log_wrt_lck);
                bank.bank_rd_end();
            }  
            else if(bank.accounts[acc_index].balance < args[4]) {
                //locking mutex
                pthread_mutex_lock(&log_wrt_lck);
                //critical section
                logfile << "Error " << thread.thread_id << ": Your transaction failed – account id " << args[1] << " balance is lower than " << args[3] << std::endl;
                //unlocking mutex
                pthread_mutex_lock(&log_wrt_lck);
                bank.bank_rd_end();
            }
            else{
                bank.bank_rd_end();
                bank.bank_wr_start();
                bank.accounts[src_acc_index].balance -= args[4];
                bank.accounts[dest_acc_index].balance += args[4];
                //locking mutex
                pthread_mutex_lock(&log_wrt_lck);
                //critical section
                logfile << thread.thread_id << ": Transfer " <<  args[4] << " from account " << args[1] << " to account " << args[3] <<
                                                                            " new account balance is " << bank.accounts[src_acc_index].balance << 
                                                                            "new target account balance is " << bank.accounts[dest_acc_index].balance 
                                                                            << std::endl;
                //unlocking mutex
                pthread_mutex_lock(&log_wrt_lck);
                bank.bank_wr_end();
            }
            sleep(ACT_TIME);
        }
        
    
	}
    sleep(WAKEUP_ATM_TIME);
    pthread_exit

}

//void openaccount(char* args){
  //  pthread_mutex_lock
//}
//void removeAccount(char* args){
//
//}