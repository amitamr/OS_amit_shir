#include "BankAccount.hpp"


Account::Account(int account, int new_password, int new_balance) : acc_num(account), password(new_password), 
                                                                   balance(new_balance), rd_count (0), valid(1){
    pthread_mutex_init(&bank_acc_lock, NULL);
}

Account::~Account(){}

