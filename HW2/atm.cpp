#include "atm.hpp"
#define MAX_ARG 5

using namespace std;

extern Bank bank;
extern pthread_mutex_t log_wrt_lck;
extern ofstream logfile;

/*Bank bank;
ofstream logfile;
pthread_mutex_t log_wrt_lck;*/ 

//data structure for pthread create

void* thread_function(void* thread){
    
    char* cargs[MAX_ARG];
    int args[MAX_ARG];
    const char* delimiters = " \t";
    atm_data* curr_atm = static_cast<atm_data*>(thread);
    FILE* file = std::fopen(curr_atm->filename , "r");
    
    if (file == nullptr) {
        std::cerr << "Bank error: illigal arguments" << std::endl;
        exit(1);
    }

    // Buffer to hold each line
    char buffer[1024];
    char* action;
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
        for (int i=1; i<MAX_ARG; i++)
        {
            cargs[i] = strtok(NULL, delimiters);
            if (cargs[i] != NULL){
                args[i] = atoi(cargs[i]);
            }
        }

        if (!strcmp(cargs[0], "O")){
            bank.bank_wr_start(curr_atm->thread_id); //to prevent doubling on the bank account
            if(bank.findAccount(args[1]) != -1){
                //locking mutex
                pthread_mutex_lock(&log_wrt_lck);
                //critical section
                logfile << "Error " << curr_atm->thread_id << ": Your transaction failed – account with the same id exists" << std::endl;
                //unlocking mutex
                pthread_mutex_unlock(&log_wrt_lck);
                bank.bank_wr_end(curr_atm->thread_id);
            }
            else{
                bank.addaccount(args[1],args[2],args[3]); //call to a function that create a new account
               // int index_for_print = bank.findAccount(args[1]);
                //locking mutex
                pthread_mutex_lock(&log_wrt_lck);
                //critical section
                logfile << curr_atm->thread_id << ": New account id is " << args[1] << " with password " << args[2] << " and initial balance " << args[3] << std::endl;
                //logfile << "print index " << index_for_print << " acc num " << bank.accounts[index_for_print].acc_num << 
                //   " balance " << bank.accounts[index_for_print].balance << " valid " << bank.accounts[index_for_print].valid << endl;
                //unlocking mutex
                pthread_mutex_unlock(&log_wrt_lck);
                bank.bank_wr_end(curr_atm->thread_id);
            }
            sleep(ACT_TIME);
        }

         if (!strcmp(cargs[0], "D")){
            bank.bank_rd_start(curr_atm->thread_id);
            int acc_index = bank.findAccount(args[1]);
            if(acc_index == -1){ //account number does not exist
                //locking mutex
                pthread_mutex_lock(&log_wrt_lck);
                //critical section
                logfile << "Error " << curr_atm->thread_id << ": Your transaction failed – account id " << args[1] << " does not exists" << std::endl;
                //unlocking mutex
                pthread_mutex_unlock(&log_wrt_lck);
                bank.bank_rd_end(curr_atm->thread_id);
            }
            else if(bank.accounts[acc_index].password != args[2]){
                //locking mutex
                pthread_mutex_lock(&log_wrt_lck);
                //critical section
                logfile << "Error " << curr_atm->thread_id << ": Your transaction failed – password for account id " << args[1] << " is incorrect" << std::endl;
                //unlocking mutex
                pthread_mutex_unlock(&log_wrt_lck);
                bank.bank_rd_end(curr_atm->thread_id);
            }  
            else{
                bank.accounts[acc_index].acc_wr_start(curr_atm->thread_id, args[1]); //lock the account for only one writer
                bank.accounts[acc_index].balance += args[3];
                //locking mutex
                pthread_mutex_lock(&log_wrt_lck);
                //critical section
                logfile << curr_atm->thread_id << ": Account " <<  args[1] << " new balance is " << bank.accounts[acc_index].balance << " after " << args[3] << " $ was deposited" << std::endl;
                //unlocking mutex
                pthread_mutex_unlock(&log_wrt_lck);
                bank.accounts[acc_index].acc_wr_end(curr_atm->thread_id, args[1]); //unlock the account
                bank.bank_rd_end(curr_atm->thread_id);

            }
            sleep(ACT_TIME);
        }

         if (!strcmp(cargs[0], "W")){
            bank.bank_rd_start(curr_atm->thread_id);
            int acc_index = bank.findAccount(args[1]);
            if( acc_index == -1){ //account number does not exist
                //locking mutex
                pthread_mutex_lock(&log_wrt_lck);
                //critical section
                logfile << "Error " << curr_atm->thread_id << ": Your transaction failed – account id " << args[1] << " does not exists" << std::endl;
                //unlocking mutex
                pthread_mutex_unlock(&log_wrt_lck);
                bank.bank_rd_end(curr_atm->thread_id);
            }
            else if(bank.accounts[acc_index].password != args[2]){
                //locking mutex
                pthread_mutex_lock(&log_wrt_lck);
                //critical section
                logfile << "Error " << curr_atm->thread_id << ": Your transaction failed – password for account id " << args[1] << " is incorrect" << std::endl;
                //unlocking mutex
                pthread_mutex_unlock(&log_wrt_lck);
                bank.bank_rd_end(curr_atm->thread_id);
            }  
            else {
                bank.accounts[acc_index].acc_wr_start(curr_atm->thread_id, args[1]); //lock the account for only one writer
                if(bank.accounts[acc_index].balance < args[3]) {
                    //locking mutex
                    pthread_mutex_lock(&log_wrt_lck);
                    //critical section
                    logfile << "Error " << curr_atm->thread_id << ": Your transaction failed – account id " << args[1] << " balance is lower than " << args[3] << std::endl;
                    //unlocking mutex
                    pthread_mutex_unlock(&log_wrt_lck);
                }
                else{
                    bank.accounts[acc_index].balance -= args[3];
                    //locking mutex
                    pthread_mutex_lock(&log_wrt_lck);
                    //critical section
                    logfile << curr_atm->thread_id << ": Account " <<  args[1] << " new balance is " << bank.accounts[acc_index].balance << " after " << args[3] << " $ was withdrawn" << std::endl;
                    //unlocking mutex
                    pthread_mutex_unlock(&log_wrt_lck);
                }
                bank.accounts[acc_index].acc_wr_end(curr_atm->thread_id, args[1]); //unlock the account
                bank.bank_rd_end(curr_atm->thread_id);
            }
            sleep(ACT_TIME);
        }

         if (!strcmp(cargs[0], "B")){
            bank.bank_rd_start(curr_atm->thread_id);
            int acc_index = bank.findAccount(args[1]);
            if( acc_index == -1){ //account number does not exist
                //locking mutex
                pthread_mutex_lock(&log_wrt_lck);
                //critical section
                logfile << "Error " << curr_atm->thread_id << ": Your transaction failed – account id " << args[1] << " does not exists" << std::endl;
                //unlocking mutex
                pthread_mutex_unlock(&log_wrt_lck);
                bank.bank_rd_end(curr_atm->thread_id);
            }
            else if(bank.accounts[acc_index].password != args[2]){
                //locking mutex
                pthread_mutex_lock(&log_wrt_lck);
                //critical section
                logfile << "Error " << curr_atm->thread_id << ": Your transaction failed – password for account id " << args[1] << " is incorrect" << std::endl;
                //unlocking mutex
                pthread_mutex_unlock(&log_wrt_lck);
                bank.bank_rd_end(curr_atm->thread_id);
            }  
            else{
                bank.accounts[acc_index].acc_rd_start(curr_atm->thread_id, args[1]); //lock the account for readers
                //locking mutex
                pthread_mutex_lock(&log_wrt_lck);
                //critical section
                logfile << curr_atm->thread_id << ": Account " <<  args[1] << " balance is " << bank.accounts[acc_index].balance << std::endl;
                //unlocking mutex
                pthread_mutex_unlock(&log_wrt_lck);
                bank.accounts[acc_index].acc_rd_end(curr_atm->thread_id, args[1]); //unlock the account
                bank.bank_rd_end(curr_atm->thread_id);
            }
            sleep(ACT_TIME);
        }

        if (!strcmp(cargs[0], "Q")){
            bank.bank_wr_start(curr_atm->thread_id);
            int acc_index = bank.findAccount(args[1]);
            if( acc_index == -1){ //account number does not exist
                //locking mutex
                pthread_mutex_lock(&log_wrt_lck);
                //critical section
                logfile << "Error " << curr_atm->thread_id << ": Your transaction failed – account id " << args[1] << " does not exists" << std::endl;
                //unlocking mutex
                pthread_mutex_unlock(&log_wrt_lck);
                bank.bank_wr_end(curr_atm->thread_id);
            }
            else if(bank.accounts[acc_index].password != args[2]){
                //locking mutex
                pthread_mutex_lock(&log_wrt_lck);
                //critical section
                logfile << "Error " << curr_atm->thread_id << ": Your transaction failed – password for account id " << args[1] << " is incorrect" << std::endl;
                //unlocking mutex
                pthread_mutex_unlock(&log_wrt_lck);
                bank.bank_wr_end(curr_atm->thread_id);
            }
            else{
                bank.accounts[acc_index].valid = false;
                //bank.bank_rd_start(curr_atm->thread_id); 
                int old_balance = bank.accounts[acc_index].balance;
                bank.removeaccount(acc_index); //call to a function that removes an account 
                //locking mutex
                pthread_mutex_lock(&log_wrt_lck);
                //critical section
                logfile << curr_atm->thread_id << ": Account " <<  args[1] << " is now closed. Balance was " << old_balance << std::endl;
                //unlocking mutex
                pthread_mutex_unlock(&log_wrt_lck);
                //bank.bank_rd_end(curr_atm->thread_id);
                bank.bank_wr_end(curr_atm->thread_id);
            }
            sleep(ACT_TIME);
        }

         if (!strcmp(cargs[0], "T")){
            bank.bank_rd_start(curr_atm->thread_id);
            int src_acc_index = bank.findAccount(args[1]);
            int dest_acc_index = bank.findAccount(args[3]);
            if(src_acc_index == -1){ //account number does not exist
                //locking mutex
                pthread_mutex_lock(&log_wrt_lck);
                //critical section
                logfile << "Error " << curr_atm->thread_id << ": Your transaction failed – account id " << args[1] << " does not exists" << std::endl;
                //unlocking mutex
                pthread_mutex_unlock(&log_wrt_lck);
                bank.bank_rd_end(curr_atm->thread_id);
            }
            else if(dest_acc_index == -1){ //account number does not exist
                //locking mutex
                pthread_mutex_lock(&log_wrt_lck);
                //critical section
                logfile << "Error " << curr_atm->thread_id << ": Your transaction failed – account id " << args[3] << " does not exists" << std::endl;
                //unlocking mutex
                pthread_mutex_unlock(&log_wrt_lck);
                bank.bank_rd_end(curr_atm->thread_id);
            }
            else if(bank.accounts[src_acc_index].password != args[2]){
                //locking mutex
                pthread_mutex_lock(&log_wrt_lck);
                //critical section
                logfile << "Error " << curr_atm->thread_id << ": Your transaction failed – password for account id " << args[1] << " is incorrect" << std::endl;
                //unlocking mutex
                pthread_mutex_unlock(&log_wrt_lck);
                bank.bank_rd_end(curr_atm->thread_id);
            }  
            else{
                if(bank.accounts[src_acc_index].acc_num < bank.accounts[dest_acc_index].acc_num){
                    bank.accounts[src_acc_index].acc_wr_start(curr_atm->thread_id, args[1]); //lock the account for only one writer
                    bank.accounts[dest_acc_index].acc_wr_start(curr_atm->thread_id, args[3]); //lock the account for only one writer
                }
                else{
                    bank.accounts[dest_acc_index].acc_wr_start(curr_atm->thread_id, args[3]); //lock the account for only one writer
                    bank.accounts[src_acc_index].acc_wr_start(curr_atm->thread_id, args[1]); //lock the account for only one writer
                }
                if(bank.accounts[src_acc_index].balance < args[4]) {
                    //locking mutex
                    pthread_mutex_lock(&log_wrt_lck);
                    //critical section
                    logfile << "Error " << curr_atm->thread_id << ": Your transaction failed – account id " << args[1] << " balance is lower than " << args[3] << std::endl;
                    //unlocking mutex
                    pthread_mutex_unlock(&log_wrt_lck);
                }
                else{
                    bank.accounts[src_acc_index].balance -= args[4];
                    bank.accounts[dest_acc_index].balance += args[4];
                    //locking mutex
                    pthread_mutex_lock(&log_wrt_lck);
                    //critical section
                    logfile << curr_atm->thread_id << ": Transfer " <<  args[4] << " from account " << args[1] << " to account " << args[3] <<
                                                                                " new account balance is " << bank.accounts[src_acc_index].balance << 
                                                                                " new target account balance is " << bank.accounts[dest_acc_index].balance 
                                                                                << std::endl;
                    //unlocking mutex
                    pthread_mutex_unlock(&log_wrt_lck);
                }
                if(bank.accounts[src_acc_index].acc_num < bank.accounts[dest_acc_index].acc_num){
                    bank.accounts[src_acc_index].acc_wr_end(curr_atm->thread_id, args[1]); //lock the account for only one writer
                    bank.accounts[dest_acc_index].acc_wr_end(curr_atm->thread_id, args[3]); //lock the account for only one writer
                }
                else{
                    bank.accounts[dest_acc_index].acc_wr_end(curr_atm->thread_id, args[3]); //lock the account for only one writer
                    bank.accounts[src_acc_index].acc_wr_end(curr_atm->thread_id, args[1]); //lock the account for only one writer
                }
                bank.bank_rd_end(curr_atm->thread_id);
            }
            sleep(ACT_TIME);
        }
        sleep(WAKEUP_ATM_TIME);
	}

    pthread_exit(NULL);
    fclose(file);
}


//void openaccount(char* args){
  //  pthread_mutex_lock
//}
//void removeAccount(char* args){
//
//}