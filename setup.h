char* SHELL_CWD;
int shell_fd;
pid_t shell_pid;
pid_t shell_pgid;
pid_t fgproc_pid;

void agsh_sig_handle(int sig) {
	if(sig == SIGCHLD) {
		int status;
		pid_t dead;
		struct child_process* iter;
		while((dead = waitpid(-1, &status, WNOHANG)) > 0) {
			iter = head_lis;
			while (iter != NULL) {
				if (iter->pid == dead) break;
				iter = iter->next;
			}
			if (iter != NULL) {
				if(WIFEXITED(status) && WEXITSTATUS(status) == EXIT_SUCCESS) printf("%s (%i) has exited succesfully!\n", iter->name, iter->pid);
				else if(WIFEXITED(status) && WEXITSTATUS(status) == EXIT_FAILURE) printf("%s (%i) has exited by failing!\n", iter->name, iter->pid);
				else if(WIFSIGNALED(status)) printf("%s (%i) has exited with a signal!\n", iter->name, iter->pid);
				else if(kill(dead, 0) < 0) printf("%s (%i) has terminated with SIGTERM!\n", iter->name, iter->pid);
				else return;
				del_process(iter->pid);
			}
		}
	} else if (sig == SIGTSTP) {
		pid_t curr_proc = getpid();

		if (curr_proc != shell_pid) return;
		if (fgproc_pid == -1) return;

		raise(SIGTSTP);
	} else if (sig == SIGINT) {
		pid_t curr_proc = getpid();

		if (curr_proc != shell_pid) return;
		if (fgproc_pid == -1) return;

		raise(SIGINT);
	}
}

void setup() {
	read_history(HISTORY_PATH);
	/*FILE* welccf;

	if ((welccf = fopen(WELCC_PATH, "r")) == NULL) {
		printf(COL(ERR_COL) "AGSH shell error (fetchwelcc): " COL_RES "Unable to fetch welcome message\n");
		return;
	}
	char c = fgetc(welccf);
	printf(COL(WELCC_COL_BG) COL(WELCC_COL_FG));
	while (c != EOF) {
		printf("%c", c);
		c = fgetc(welccf);
	}
	printf(COL_RES);
	fclose(welccf);

	if ((welccf = fopen(WELCC_INFO_PATH, "r")) == NULL) {
		printf(COL(ERR_COL) "AGSH shell error (fetchwelccmsg): " COL_RES "Unable to fetch welcome message\n");
		return;
	}
	printf(COL(WELCC_INFO_COL_BG) COL(WELCC_INFO_COL_FG));
	c = fgetc(welccf);
	while (c != EOF) {
		printf("%c", c);
		c = fgetc(welccf);
	}
	printf(COL_RES);
	fclose(welccf);*/

	shell_fd = STDIN_FILENO;

	// Signal stuff
	signal(SIGTTIN, SIG_IGN); // stop terminal input to background process
	signal(SIGTTOU, SIG_IGN); // stop terminal output control for background process

	signal(SIGCHLD, agsh_sig_handle);
	signal(SIGTSTP, agsh_sig_handle);
	signal(SIGINT, agsh_sig_handle);

	shell_pgid = shell_pid = getpid();
	setpgid(shell_pid, shell_pgid);
	tcsetpgrp(shell_fd, shell_pgid);
	fgproc_pid = -1;

	SHELL_CWD = (char*) malloc(PATH_MAX * sizeof(char));
	if (getcwd(SHELL_CWD, PATH_MAX) == NULL) {
		perror(COL(ERR_COL) "AGSH shell error (shellwd)" COL_RES);
		exit(EXIT_FAILURE);
	}

	// Setting and Getting Environment Variables
	if (unsetenv("OLDPWD") < 0) {
		perror(COL(ERR_COL) "AGSH shell error (clearoldpwd)" COL_RES);
		exit(EXIT_FAILURE);
	}
}
