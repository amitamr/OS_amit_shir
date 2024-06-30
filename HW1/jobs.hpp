#ifndef JOB_H
#define JOB_H

#include <stdio.h>
#include <time.h>
#include <string>
#include <cstring>
#include <vector>
#include <iostream>
#include <algorithm>
#include <sys/wait.h>
#include <unistd.h> 

#define MAX_LINE_SIZE 80
#define MAX_ARG 20

using std::string;

/*this class is the Job struct*/
class Job {
public:
    int jobid;
    int pid;
    bool is_stopped;
    time_t entrence_time;
    char name[MAX_LINE_SIZE];
    bool is_fg;
   // bool signals[SIGSIZE];

// constructor
    Job(char* new_name, int new_pid, int new_jobid, bool new_is_stopped);
// distractor
    ~Job();
/*need to add here methods of class Job*/
};

/*this class is the job's manager of the smash*/
class Manager{
public:
std::vector<Job> jobs;
int max_jobid;
int max_stopped_jobid;
int jobsCount;
//string old_path;
char old_path[MAX_LINE_SIZE];
//string curr_foreground_cmd;
int curr_foreground_pid; 
int smash_pid; 
char curr_foreground_cmd[MAX_LINE_SIZE];

// constructor
Manager();
// distractor 
~Manager();

int find(int jobid);
void erasejob(int jobid);
void move_to_fg(int jobid);
void addjob(char* new_name, int new_pid, bool new_is_stopped);
void deletefinished();
};

#endif