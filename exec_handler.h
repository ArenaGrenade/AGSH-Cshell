int check_bg(int argc, char** args) {
	for (int i = 0 ; i < argc; i++) {
		if (strcmp(args[i], "&") == 0) {
			return 0;
		}
	}

	return 1;
}

int agsh_spawn_process(int argc, char** argv) {
	pid_t child;

	int do_wait = check_bg(argc, argv);
	
	child = fork();
	if (child == 0) {
		setpgid(0, 0);

		if (execvp(argv[0], argv) < 0) {
			perror(COL(ERR_COL) "AGSH shell error (execvpchild)" COL_RES);
			printf(COL(FGB_BLACK) "Unable to launch the command you have entered.\n" COL_RES);
			exit(EXIT_FAILURE);
		}
		exit(EXIT_SUCCESS);
	} 
	else if(child < 0) {
		perror(COL(ERR_COL) "AGSH shell error (par-chld:forkfail)" COL_RES);
		return -1;
	} 
	else {
		// Parent Process Control
		if (do_wait) {
			int status;
			waitpid(child, &status, WUNTRACED);

			if(!WIFSTOPPED(status)) del_process(child);
			else printf("%s (%i) has stopped!\n", argv[0], child);
			return 0;
		}
		else {
			printf("[%i] %i\n", proc_list_size, child);
			add_process(child, argv[0]);
			return 0;
		}
	}

	return 0;
}

int agsh_frontend_exec(int argc, char **argv) {
	if (argv[0] == NULL) return 0;
	enqueue(argv[0]);

	for (int i = 0; i < num_builtins; i++) {
		if(strcmp(argv[0], names_builtins[i]) == 0) {
			return (func_builtins[i])(argc, argv);
		}
	}
	
	return agsh_spawn_process(argc, argv);
}
