// signals.c
// contains signal handler funtions
// contains the function/s that set the signal handlers

/*******************************************/
/* Name: handler_cntlc
   Synopsis: handle the Control-C */
#include "signals.hpp"


void Ctrl_C_handler(Manager& manager){
   std::cout << "smash: caught ctrl-C" << std::endl;
   //check that it's not the smash process
   if(manager.curr_foreground_pid != manager.smash_pid){
      if(kill(manager.curr_foreground_pid , SIGKILL) == -1 ){
         perror("smash error: kill failed");
      }
      else{
         std::cout << "smash: process " << manager.curr_foreground_pid << " was killed";
         manager.curr_foreground_pid = manager.smash_pid;
         getcwd(manager.curr_foreground_cmd, sizeof(manager.curr_foreground_cmd));
      }
      
   }
}

void Ctrl_Z_handler(Manager& manager){
   std::cout << "smash: caught ctrl-Z" << std::endl;
   if(manager.curr_foreground_pid != manager.smash_pid){
      if(kill(manager.curr_foreground_pid , SIGSTOP) == -1 ){
         perror("smash error: kill failed");
      }
      else{
         manager.deletefinished();
         manager.addjob(manager.curr_foreground_cmd, manager.curr_foreground_pid, 1);
         std::cout << "smash: process " << manager.curr_foreground_pid << " was stopped";
         manager.curr_foreground_pid = manager.smash_pid;
         getcwd(manager.curr_foreground_cmd, sizeof(manager.curr_foreground_cmd));
      }  
   }
}

