//		commands.c
//********************************************
#include "commands.hpp"

//********************************************
// function name: ExeCmd
// Description: interperts and executes built-in commands
// Parameters: pointer to jobs, command string
// Returns: 0 - success,1 - failure
//**************************************************************************************
int ExeCmd(Manager& manager, char* lineSize, char* cmdString)
{
	char* cmd; 
	char* args[MAX_ARG];
	char pwd[MAX_LINE_SIZE]; // for pwd built in commands
	char* delimiters = " \t\n";  
	int i = 0, num_arg = 0;
	bool illegal_cmd = FALSE; // illegal command
    	cmd = strtok(lineSize, delimiters);
	if (cmd == NULL)
		return 0; 
   	args[0] = cmd;
	for (i=1; i<MAX_ARG; i++)
	{
		args[i] = strtok(NULL, delimiters); 
		if (args[i] != NULL) 
			num_arg++; 
 
	}
/*************************************************/
// Built in Commands PLEASE NOTE NOT ALL REQUIRED
// ARE IN THIS CHAIN OF IF COMMANDS. PLEASE ADD
// MORE IF STATEMENTS AS REQUIRED
/*************************************************/
	if (!strcmp(cmd, "cd")) 
	{
		// need to check with GAL what is considered as old path if the argument is the current pwd
		if(num_arg > 1){//wrong arguments, error
			perror("smash error: cd: too many arguments");
			return 1;
		}
		char current_pwd[MAX_LINE_SIZE] = ""; // saving the current cwd for later
		if(NULL == getcwd(current_pwd, sizeof(current_pwd))){
			perror("smash error: getcwd failed");
			return 1;
		}
		if(num_arg){ //num arg == 1
			if(!strcmp(arg[1],"-")){
				if(NULL == manager.old_path){
					perror("smash error: cd: OLDPWD not set");
					return 1;
				}
				if(chdir(manager.old_path)){ //here we change dir to old pwd (if success)
					perror("smash error: chdir failed");
					return 1;
				}
			}
			else if(chdir(arg[1])){
					perror("smash error: chdir failed");
					return 1;
			}
		}
		else{// num args == 0
			const char *home = getenv("HOME");
			if (NULL == home){
				perror("smash error: getenv failed");
				return 1;
			}
			else (chdir(home)){ //error
					perror("smash error: chdir failed (home)");
					return 1;
			}
		}
		strcpy(manager.old_path, (const)current_pwd); // change old_path after secceed with cd
	} 
	
	/*************************************************/
	else if (!strcmp(cmd, "pwd")) 
	{
		if(NULL != getcwd(pwd, sizeof(pwd))){
			std::cout << pwd << std::endl;
		}
		else{
			perror("smash error: pwd failed");
			return 1;
		}
	}
	
	/*************************************************/
	else if (!strcmp(cmd, "kill"))
	{
		if(num_arg != 2 || args[1].compare(0,1,"-")!=0){
			perror("smash error: kill: invalid arguments");
			return 1;
		}
		int jobid = atoi(args[2]);
		(string)args[1].erase(args[1].begin()); // if fails do varible of string and then string.c_str
		int signum = atoi(args[1]);

		if(jobid == 0 || signum == 0) 
		{
		//at least one argument is not an integer
			perror("smash error: kill: invalid arguments");
			return 1;
		}
		int jobindex = manager.find(jobid);
		if( jobindex == -1){
			std::cerr << "smash error: kill: job-id " << jobid << " does not exist" << std::endl;
			return 1;
		}
		
		std::cout << "signal number " << signum << " was sent to pid" <<  manager.jobs[jobindex].pid << std::endl;

	}
	
	/*************************************************/
	else if (!strcmp(cmd, "diff"))
	{
		if(num_arg != 2){
			perror("smash error: diff: invalid arguments");
			return 1;
		}
 		FILE *fp1 = fopen(args[1], "r");
        FILE *fp2 = fopen(args[2], "r");
		if (fp1 == NULL || fp2 == NULL){
			perror("smash error: open failed");
			return 1;
		}
		char c1, c2;
		while( (c1 = fgetc(fp1)) != EOF && (c2= fgetc(fp2)) != EOF)
		{
			if(c1 != c2){
				std::cout << "1" <<std::endl;
				break;
			}
		}
		if(c1 == EOF && c2 == EOF){
			std::cout << "0" <<std::endl;
		}
		else
			std::cout << "1" <<std::endl;
	}
	/*************************************************/
	
	else if (!strcmp(cmd, "jobs")) 
	{	//check for ended processes
		time_t curr_time;
		time(&curr_time);
 		std::sort(manager.jobs.begin(), manager.jobs.end(), compareByJobID);
		for(int i=0; i < manager.jobsCount; i++){
			std::cout << "[" << manager.jobs[i].jobid << "] " << manager.jobs[i].name << ": " 
							 <<manager.jobs[i].pid << " " << `time(curr_time,manager.jobs[i].entrence_time) 
							 << " secs" ;
			if(manager.jobs[i].is_stopped){
				std::cout << " (stopped)";
			}
			std::cout << std::endl;				 
		}
	}
	/*************************************************/
	else if (!strcmp(cmd, "showpid")) 
	{
		std::cout << smash pid is << getpid() << std::endl;
	}
	/*************************************************/
	else if (!strcmp(cmd, "fg")) 
	{
		int jobid = atoi (args[1]);
		if(num_arg > 1 || jobid == 0){ // format or num arguments are invalid
			perror("smash error: fg: invalid arguments");
			return 1;
		}
		if(num_arg == 0 && manager.jobsCount == 0){
			perror("smash error: fg: jobs list is empty");
			return 1;
		}
		int jobindex;
		if (num_arg == 0){
			jobindex = manager.find(manager.max_jobid);
		}
		else{
			jobindex = manager.find(jobid);
		}
		if (jobindex == -1){
			std::cerr << "smash error: fg: job-id " << jobid << " does not exist" << std::endl;
			return 1;
		}

		int pid_fg = manager.jobs[jobindex].pid;
		std::cout << manager.jobs[jobindex].name << " : " << pid_fg << std::endl;
		if(kill(pid_fg, SIGCONT) == -1){
			perror("smash error: kill failed");
			return 1;
		}
		manager.erasejob(jobid);
		if(waitpid(pid_fg, NULL, WUNTRACED) == -1){
			perror("smash error: waitpid failed");
			return 1;
		}

	} 
	/*************************************************/
	else if (!strcmp(cmd, "bg")) 
	{
  		int jobid = atoi (args[1]);
		if(num_arg > 1 || jobid == 0){ // format or num arguments are invalid
			perror("smash error: bg: invalid arguments");
			return 1;
		}
		if(num_arg == 0 && manager.max_stopped_jobid == 0){ //no stopped jobs
			perror("smash error: bg: there are no stopped jobs to resume");
			return 1;
		}
		int jobindex;
		if (num_arg == 0){
			jobindex = manager.find(manager.max_stopped_jobid);
		}
		else{
			jobindex = manager.find(jobid);
		}
		if(jobindex == -1){
			std::cerr << "smash error: bg: job-id " << jobid << " does not exist" << std::endl;
			return 1;
		}
		if(!manager.jobs[jobindex].is_stopped){
			std::cerr << "smash error: bg: job-id " << jobid << " is already running in the background" << std::endl;
			return 1;
		}
		int pid_bg = manager.jobs[jobindex].pid;
		std::cout << manager.jobs[jobindex].name << " : " << pid_bg << std::endl;
		if(kill(pid_bg, SIGCONT) == -1){
			perror("smash error: kill failed");
			return 1;
		}
		manager.jobs[jobindex].is_stopped = 0;
	}
	/*************************************************/
	else if (!strcmp(cmd, "quit"))
	{
   		if(num_arg != 1){
			exit(0);
		}
		if (strcmp(args[1],"kill")){
			exit(0);
		}
		std::vector<Job>::iterator it;
		for(it = manager.jobs.begin(); it != manager.jobs.end(); it++){
			if(kill(it->pid , SIGTERM) == -1){
				perror("smash error: kill failed");
				return 1;
			}
			std::cout << "[" << it->jobid << "] " << it->name << " - Sending SIGTERM...";
			std::cout.flush();
			sleep(5);				
			if(waitpid(it->pid,NULL,WNOHANG) == 0){
				std::cout << " (5 sec passed) Sending SIGKILL...";
				if(kill(it->pid, SIGKILL) == ERROR){
					perror("smash error: kill failed");
					return FAILURE;
				}
			}
			std::cout << " Done." << std::endl;
		}
		exit(0);
	} 
	/*************************************************/
	else // external command
	{
 		ExeExternal(args, cmd);
	 	return 0;
	}
	if (illegal_cmd == TRUE)
	{
		perror("smash error: > \"%s\"\n", cmdString);
		return 1;
	}
    return 0;
}
//**************************************************************************************
// function name: ExeExternal
// Description: executes external command
// Parameters: external command arguments, external command string
// Returns: void
//**************************************************************************************
void ExeExternal(char *args[MAX_ARG], char* cmdString)
{
	int pID;
    	switch(pID = fork()) 
	{
    		case -1: 
					// Add your code here (error)
					perror("smash error: fork failed");
					exit(1);
        	case 0 :
                	// Child Process
               		setpgrp();
			        // Add your code here (execute an external command)
					if(execv(cmdString, args) == -1){
						perror("smash error: execv failed");
						exit(1);
					}
			
			default:
                	// Add your code here
				if(wait(pID) == -1){
					perror("smash error: wait failed");
					exit(1);
				}
	}
}
//**************************************************************************************
// function name: ExeComp
// Description: executes complicated command
// Parameters: command string
// Returns: 0- if complicated -1- if not
//**************************************************************************************
int ExeComp(char* lineSize)
{
	char ExtCmd[MAX_LINE_SIZE+2];
	char *args[MAX_ARG];
    if ((strstr(lineSize, "|")) || (strstr(lineSize, "<")) || (strstr(lineSize, ">")) || (strstr(lineSize, "*")) || (strstr(lineSize, "?")) || (strstr(lineSize, ">>")) || (strstr(lineSize, "|&")))
    {
		// Add your code here (execute a complicated command)
					
		/* 
		your code
		*/
	} 
	return -1;
}
//**************************************************************************************
// function name: BgCmd
// Description: if command is in background, insert the command to jobs
// Parameters: command string, pointer to jobs
// Returns: 0- BG command -1- if not
//**************************************************************************************
int BgCmd(char* lineSize, Manager& manager)
{
	char* cmd;
	char* delimiters = " \t\n";
	char *args[MAX_ARG];
	if (lineSize[strlen(lineSize)-2] == '&')
	{
		lineSize[strlen(lineSize)-2] = '\0';
		int i = 0, num_arg = 0;
		cmd = strtok(lineSize, delimiters);
		if (cmd == NULL){
			return 0;
		}
   		args[0] = cmd;
		for (i=1; i<MAX_ARG; i++){
			args[i] = strtok(NULL, delimiters); 
			if (args[i] != NULL){
				num_arg++;
			}
		}
		if(is_built_in(cmd)){
			return -1;		
		}

		int pID;
    	switch(pID = fork()) 
		{
    		case -1: 
					perror("smash error: fork failed");
					exit(1);
        	case 0 :
                	// Child Process
               		setpgrp();
					if(execv(cmdString, args) == -1){
						perror("smash error: execv failed");
						exit(1);
					}
			default:
				// delete jobs that finished - add function here
				manager.addjob(cmd, pID);
	}
	return 0;
	}
	return -1;
}

bool is_built_in(char* cmd){
	return (!strcmp(cmd, "cd") || !strcmp(cmd, "kill") || !strcmp(cmd, "diff") || 
			!strcmp(cmd, "fg") || !strcmp(cmd, "bg") || !strcmp(cmd, "showpid") || !strcmp(cmd, "pwd") || 
			!strcmp(cmd, "jobs") || !strcmp(cmd, "quit"));
}
