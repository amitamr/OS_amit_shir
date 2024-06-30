// signals.c
// contains signal handler funtions
// contains the function/s that set the signal handlers

/*******************************************/
/* Name: handler_cntlc
   Synopsis: handle the Control-C */
#include "signals.hpp"


extern Manager manager;

void Ctrl_C_handler(int signal){
   std::cout << "smash: caught ctrl-C" << std::endl;
   //check that it's not the smash process
   if(manager.curr_foreground_pid != manager.smash_pid){
      if(kill(manager.curr_foreground_pid , SIGKILL) == -1 ){
         perror("smash error: kill failed");
      }
      else{
         std::cout << "smash: process " << manager.curr_foreground_pid << " was killed" << std::endl;
         manager.curr_foreground_pid = manager.smash_pid;
         getcwd(manager.curr_foreground_cmd, sizeof(manager.curr_foreground_cmd));
      }

   }
}

void Ctrl_Z_handler(int signal){
   std::cout << "smash: caught ctrl-Z" << std::endl;
   if(manager.curr_foreground_pid != manager.smash_pid){
      if(kill(manager.curr_foreground_pid , SIGSTOP) == -1 ){
         perror("smash error: kill failed");
      }
      else{
         manager.deletefinished();
         std::vector<Job>::iterator it;
         it = manager.jobs.begin();
         while(it != manager.jobs.end()){
         if(it->pid == manager.curr_foreground_pid){
            break;
         }
         it++;
         }
         if(it == manager.jobs.end()){
            manager.addjob(manager.curr_foreground_cmd, manager.curr_foreground_pid, 1);
         }
         else{
            it->is_fg = 0;
            it->is_stopped = 1;
            if(time(&(it->entrence_time)) == -1){
               perror("smash error: time failed");
		      exit(1);
            }
            if(manager.max_stopped_jobid < it->jobid){
               manager.max_stopped_jobid = it->jobid;
            }
         }
         std::cout << "smash: process " << manager.curr_foreground_pid << " was stopped" << std::endl;
         manager.curr_foreground_pid = manager.smash_pid;
         getcwd(manager.curr_foreground_cmd, sizeof(manager.curr_foreground_cmd));
      }  
   }
}

