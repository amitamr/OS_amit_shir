//		commands.c
//********************************************
#include "commands.hpp"

//********************************************
// function name: ExeCmd
// Description: interperts and executes built-in commands
// Parameters: pointer to jobs, command string
// Returns: 0 - success,1 - failure
//**************************************************************************************
bool compareByJobID(const Job &a, const Job &b){
    return a.jobid < b.jobid;
}

int ExeCmd(Manager& manager, char* lineSize, char* cmdString)
{
	char* cmd; 
	char* args[MAX_ARG];
	char pwd[MAX_LINE_SIZE]; // for pwd built in commands
	const char* delimiters = " \t\n";  
	int i = 0, num_arg = 0;
	//bool illegal_cmd = FALSE; // illegal command
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
	manager.deletefinished();
	if (!strcmp(cmd, "cd")) 
	{
		if(num_arg > 1){//wrong arguments, error
			std::cerr << "smash error: cd: too many arguments" << std::endl;
			return 1;
		}
		char current_pwd[MAX_LINE_SIZE] = ""; // saving the current cwd for later
		if(NULL == getcwd(current_pwd, sizeof(current_pwd))){
			perror("smash error: getcwd failed");
			return 1;
		}
		if(num_arg){ //num arg == 1
			if(!strcmp(args[1],"-")){
				if(!strcmp("", manager.old_path)){
					std::cerr << "smash error: cd: OLDPWD not set" << std::endl;
					return 1;
				}
				if(chdir(manager.old_path)){ //here we change dir to old pwd (if success)
					perror("smash error: chdir failed");
					return 1;
				}
			}
			else if(chdir(args[1])){
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
			else if(chdir(home)){ //error
					perror("smash error: chdir failed (home)");
					return 1;
			}
		}
		strcpy(manager.old_path, current_pwd); // change old_path after secceed with cd
	} 
	
	/*************************************************/
	else if (!strcmp(cmd, "pwd")) 
	{
		if(NULL != getcwd(pwd, sizeof(pwd))){
			std::cout << pwd << std::endl;
		}
		else{
			perror("smash error: getcwd failed");
			return 1;
		}
	}
	
	/*************************************************/
	else if (!strcmp(cmd, "kill"))
	{
		if(num_arg != 2 || strncmp(args[1], "-", 1) != 0){
			std::cerr << "smash error: kill: invalid arguments" << std::endl;
			return 1;
		}
		int jobid = atoi(args[2]);
		int signum = atoi(args[1]+1);

		if(jobid == 0 || signum == 0) 
		{
		//at least one argument is not an integer
			std::cerr << "smash error: kill: invalid arguments" << std::endl;
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
			std::cerr << "smash error: diff: invalid arguments" <<std::endl;
			return 1;
		}
 		int file1;
		int file2;
		if ((file1 = open(args[1], O_RDONLY)) == -1){
			perror("smash error: open failed");
			return 1;
		}
		if ((file2 = open(args[2], O_RDONLY)) == -1){
			perror("smash error: open failed");
			return 1;
		}
		const size_t bufferSize = 4096;
    	char buffer1[bufferSize], buffer2[bufferSize];
    	ssize_t bytesRead1, bytesRead2;
		do {
        	bytesRead1 = read(file1, buffer1, bufferSize);
        	bytesRead2 = read(file2, buffer2, bufferSize);

        	if (bytesRead1 == -1 || bytesRead2 == -1) {
				perror("smash error: open failed");
            	close(file1);
            	close(file2);
            	return 1;
        	}

        	if (bytesRead1 != bytesRead2 || memcmp(buffer1, buffer2, bytesRead1) != 0) {
            	close(file1);
            	close(file2);
				std::cout << "1" <<std::endl;
            	break;  // Files are different
        }
    	} while (bytesRead1 > 0 && bytesRead2 > 0);
	if(bytesRead1 == 0 && bytesRead2 == 0){
    	close(file1);
    	close(file2);
     	std::cout << "0" <<std::endl;// Files are identical
	}
}

		/*char c1, c2;
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
		*/
	
	/*************************************************/
	
	else if (!strcmp(cmd, "jobs")) 
	{	//check for ended processes
		time_t curr_time = time(NULL);
		//first, delete all finished jobs
		manager.deletefinished();
		//now sort for printing 
 		std::sort(manager.jobs.begin(), manager.jobs.end(), compareByJobID);
		for(int i=0; i < manager.jobsCount; i++){
			std::cout << "[" << manager.jobs[i].jobid << "] " << manager.jobs[i].name << " : " 
							 <<manager.jobs[i].pid << " " << difftime(curr_time,manager.jobs[i].entrence_time) 
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
		std::cout << "smash pid is " << getpid() << std::endl;
	}
	/*************************************************/
	else if (!strcmp(cmd, "fg")) 
	{
		if(num_arg == 0 && manager.jobsCount == 0){

			std::cerr << "smash error: fg: jobs list is empty" <<std::endl;
			return 1;
		}
		int jobid; 
		int jobindex;
		if (num_arg == 0){
			jobindex = manager.find(manager.max_jobid);
		}
		else{
			jobid = atoi (args[1]);
			if(num_arg > 1 || jobid == 0){ // format or num arguments are invalid
				std::cerr << "smash error: fg: invalid arguments" << std::endl;
				return 1;
			}
			jobindex = manager.find(jobid);
		}
		if (jobindex == -1){
			std::cerr << "smash error: fg: job-id " << jobid << " does not exist" << std::endl;
			return 1;
		}
		
		manager.curr_foreground_pid = manager.jobs[jobindex].pid;
		strcpy(manager.curr_foreground_cmd, manager.jobs[jobindex].name);
		std::cout << manager.jobs[jobindex].name << " : " << manager.curr_foreground_pid << std::endl;
		if(kill(manager.curr_foreground_pid, SIGCONT) == -1){
			perror("smash error: kill failed");
			return 1;
		} 
		//manager.erasejob(jobid); trying to fix problen of bg -> fg -> bg/stopped
		manager.move_to_fg(jobid);
		if(waitpid(manager.curr_foreground_pid, NULL, WUNTRACED) == -1){
			perror("smash error: waitpid failed");
			return 1;
		}
		manager.curr_foreground_pid = getpid();
		getcwd(manager.curr_foreground_cmd, sizeof(manager.curr_foreground_cmd));

	} 
	/*************************************************/
	else if (!strcmp(cmd, "bg")) 
	{
		if(num_arg == 0 && manager.max_stopped_jobid == 0){ //no stopped jobs
			std::cerr << "smash error: bg: there are no stopped jobs to resume" << std::endl;
			return 1;
		}
		int jobindex;
		int jobid;
		if (num_arg == 0){
			jobindex = manager.find(manager.max_stopped_jobid);
		}
		else{
			jobid = atoi(args[1]);
			if(num_arg > 1 || jobid == 0){ // format or num arguments are invalid
				std::cerr << "smash error: bg: invalid arguments" << std::endl;
				return 1;
			}
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
				if(kill(it->pid, SIGKILL) == -1){
					perror("smash error: kill failed");
					return 1;
				}
			}
			std::cout << " Done." << std::endl;
		}
		exit(0);
	} 
	/*************************************************/
	else // external command
	{
 		ExeExternal(args, cmd ,cmdString, manager);
	 	return 0;
	}
    return 0;
}
//**************************************************************************************
// function name: ExeExternal
// Description: executes external command
// Parameters: external command arguments, external command string
// Returns: void
//**************************************************************************************
void ExeExternal(char *args[MAX_ARG], char* cmd, char* cmdString , Manager& manager)
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
					if(execv(cmd, args) == -1){
						perror("smash error: execv failed");
						exit(1);
					}
			
			default:
				/*new process in forground so we need to update*/
				manager.curr_foreground_pid = pID;
				strcpy(manager.curr_foreground_cmd, cmdString);
                	// Add your code here
				if(waitpid(pID, NULL, WUNTRACED | WCONTINUED) == -1){
					perror("smash error: wait failed");
					exit(1);
				}
				manager.curr_foreground_pid = getpid();
				getcwd(manager.curr_foreground_cmd, sizeof(manager.curr_foreground_cmd));

	}
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
	const char* delimiters = " \t\n";
	char *args[MAX_ARG];
	if (lineSize[strlen(lineSize)-2] == '&')
	{
		char cmdName[MAX_LINE_SIZE];
		strcpy(cmdName, lineSize);
		//cmdName will hold the name including the '&'
		cmdName[strlen(cmdName)-1] = '\0';
		// extract command and arguments.
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
					if(execv(cmd, args) == -1){
						perror("smash error: execv failed");
						exit(1);
					}
			default:
				// delete jobs that finished - add function here
				manager.deletefinished();
				manager.addjob(cmdName, pID, 0); //0 is for is_stopped=0
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
