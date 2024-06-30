//		Job.cpp
//********************************************
#include "jobs.hpp"




Job::Job(char* new_name, int new_pid, int new_jobid, bool new_is_stopped): jobid(new_jobid), pid(new_pid), is_stopped(new_is_stopped), is_fg(0){
    strcpy(name, new_name);
    if(time(&entrence_time) == -1){
        perror("smash error: time failed");
		exit(1);
    }
} 

Job::~Job(){}

Manager::Manager(): max_jobid(0), max_stopped_jobid(0), jobsCount(0), curr_foreground_pid(0), smash_pid(0){ 
     std::memset(old_path, 0, sizeof(old_path));
     std::memset(curr_foreground_cmd, 0, sizeof(curr_foreground_cmd));
     std::vector<Job> jobs;
}

Manager::~Manager(){}

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
    it = jobs.begin();
    while(it != jobs.end()){
        if(it->jobid == jobid){
            break;
        }
        it++;
    }
    /*because its a function for managing jobs, we're not supposed to not find the jobid, so we can
    just erase it after getting out of the while loop*/
    jobs.erase(it);
    /*update mas values*/
    int curr_max = 0;
    int curr_stopped_max = 0;
    if(jobid == max_jobid){//update max job id
         for(int i = 0; i < jobsCount; i++ ){
            if(curr_max < jobs[i].jobid){
                curr_max = jobs[i].jobid;
            }
        }
    max_jobid = curr_max;
    }
    if(jobid == max_stopped_jobid){//update max job id 
         for(int i = 0; i < jobsCount; i++ ){
            if(curr_stopped_max < jobs[i].jobid && jobs[i].is_stopped){
                curr_stopped_max = jobs[i].jobid;
            }
        }
    max_stopped_jobid = curr_stopped_max;
    }
}

void Manager::move_to_fg(int jobid){
    std::vector<Job>::iterator it;
    it = jobs.begin();
    while(it != jobs.end()){
        if(it->jobid == jobid){
            break;
        }
        it++;
    }
    it->is_fg = 1;

}



void Manager::addjob(char* new_name, int new_pid, bool new_is_stopped){
    //what we shall do if there more then 100 processes? waiting for answer
    /*if(jobsCount == 100){
        return
    }*/ 
    int new_jobid = max_jobid+1;
    jobs.push_back(Job(new_name, new_pid, new_jobid, new_is_stopped));
    max_jobid = new_jobid;
    if(new_is_stopped){
        max_stopped_jobid = new_jobid;
    }
    jobsCount++;
}

void Manager::deletefinished(){
	if(jobsCount == 0){
		return;
	}
	std::vector<Job>::iterator it = jobs.begin();
	while( it!=jobs.end()){
		//If the process has terminated
		int retval = waitpid(it->pid, NULL, WNOHANG);
		if(retval == -1){
			perror("smash error: waitpid failed");
			return;
		}else if(retval != 0){
            int jobid_to_delete = it->jobid;
            int curr_max = 0;
            it = jobs.erase(it); //job is finished, now 'it' pointes to the next element
            jobsCount--;
            if(jobid_to_delete == max_jobid){//update max job id
                    for(int i = 0; i < jobsCount; i++ ){
                        if(curr_max < jobs[i].jobid){
                            curr_max = jobs[i].jobid;
                         }
                        }
            }
            if(0 != curr_max){
                max_jobid = curr_max;
            }
            
		}else{
			it++; //job didnt finish, just moving forward "it"
		}	
	}
    if(jobsCount == 0){
        max_jobid = 0;
    }
}

