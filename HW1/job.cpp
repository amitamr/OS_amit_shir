//		Job.cpp
//********************************************
#include "jobs.hpp"

bool Job::comapreByJobID(const Job &a, const Job &b){
    return a.jobid < b.jobid;
}

Job::Job(char* new_name, int new_pid, int new_jobid): is_stopped(0), pid(new_pid), jobid(new_jobid){
    strcpy(name, new_name);
    if(time_t(&entrence_time) == -1){
        perror("smash error: time failed");
		exit(1);
    }
} 

int Manager::find(int jobtofind){ // if the job found in Jobs returns it's index, else return -1
    for(int i = 0; i < jobsCount; i++ ){
        if(jobs[i].jobid == jobtofind)
            return i;
    }
    return -1;
}

void Manager::erasejob(int jobid){
    jobsCount --;
    std::vector<Job>::iterator it;
    while(it != jobs.end()){
        it->jobid == jobid;
        break;
    }
    jobs.erase(it);
    int curr_max = 0;
    int curr_stopped_max = 0;
    if(jobid == max_jobid){//update max job id
         for(int i = 0; i < jobsCount; i++ ){
            if(curr_max < jobs[i].jobid){
                curr_max = jobs[i].jobid;
            }
        }
    }
    if(jobid == max_stopped_jobid){//update max job id 
         for(int i = 0; i < jobsCount; i++ ){
            if(curr_stopped_max < jobs[i].jobid && jobs[i].is_stopped){
                curr_stopped_max = jobs[i].jobid;
            }
        }
    }
    max_jobid = curr_max;
    max_stopped_jobid = curr_stopped_max;
}
void Manager::addjob(char* new_name, int new_pid){
    
}