#ifndef JOB_H
#define JOB_H

#include <time.h>
#include <string>
#include <vector>
#include <iostream>
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
   // bool signals[SIGSIZE];

// constructor
    Job(char* new_name, int new_pid, int new_jobid);
// distractor
    ~Job();
bool comapreByJobID(const Job &a, const Job &b);
/*need to add here methods of class Job*/
};


/*this class is the job's manager of the smash*/
class Manager{
public:
std::vector<Job> jobs;
int max_jobid;
int max_stopped_jobid;
int jobsCount;
string old_path;

// constructor
Manager();
// distractor 
~Manager();

int find(int jobid);
void erasejob(int jobid);
};

#endif