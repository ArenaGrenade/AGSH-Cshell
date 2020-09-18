char* SHELL_CWD;

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
				if(WIFEXITED(status) && WEXITSTATUS(status) == EXIT_FAILURE) printf("%s (%i) has exited by failing!\n", iter->name, iter->pid);
				else if(WIFSIGNALED(status)) printf("%s (%i) has exited with a signal!\n", iter->name, iter->pid);

				del_process(iter->pid);
			}
		}
	}
}

void setup() {
	read_history(HISTORY_PATH);
	FILE* welccf;

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
	fclose(welccf);

	SHELL_CWD = (char*) malloc(PATH_MAX * sizeof(char));
	if (getcwd(SHELL_CWD, PATH_MAX) == NULL) {
		perror(COL(ERR_COL) "AGSH shell error (shellwd)" COL_RES);
		exit(EXIT_FAILURE);
	}
}
