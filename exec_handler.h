int  redirection_setup(int* argc, char** argv, int* stdfds);

int print_arr(int size, char** arr) {
	for (int i = 0 ; i < size; i++) {
		if (arr[i] != NULL) printf("%s\n", arr[i]);
		else printf("That was a NULL\n");
	}
}

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

	int status, was_builtin = 0;

	// Redirection Setup
	int stdfds[2];
	if (redirection_setup(&argc, argv, stdfds) < 0) return -1;

	for (int i = 0; i < num_builtins; i++) {
		if(strcmp(argv[0], names_builtins[i]) == 0) {
			status = (func_builtins[i])(argc, argv);
			was_builtin = 1;
			break;
		}
	}
	
	if (!was_builtin) status = agsh_spawn_process(argc, argv);

	// Rediretion Reset
	dup2(stdfds[0], STDIN_FILENO);
	dup2(stdfds[1], STDOUT_FILENO);

	return status;
}


int redirection_setup(int* argc, char **argv, int* stdfds) {
	int open_flags = -1, redfile = 0;

	stdfds[0] = dup(STDIN_FILENO);
	stdfds[1] = dup(STDOUT_FILENO);

	for (int i = 1 ; i < *argc; i++) {
		if (strcmp(argv[i], ">>") == 0) open_flags = O_WRONLY | O_TRUNC | O_CREAT;
		else if (strcmp(argv[i], ">") == 0) open_flags = O_WRONLY | O_CREAT;
		else if (strcmp(argv[i], "<") == 0) open_flags = O_RDONLY;

		if (open_flags != -1) {
			// Check for existence of the correspoding file
			if (i + 1 >= *argc) {
				printf(COL(ERR_COL) "AGSH shell error: " COL_RES "Input or output file not specified.\n");
				return -1;
			}

			// Open the file and duplicate the filestream for stdout
			redfile = open(argv[i + 1], open_flags, S_IWUSR | S_IRUSR | S_IWGRP | S_IRGRP);

			// Handle for output file
			if (open_flags != O_RDONLY && dup2(redfile, STDOUT_FILENO) < 0) {
				perror(COL(ERR_COL) "AGSH shell error(outdup)" COL_RES);
				return -1;
			}
			

			// Handle for input file
			if (open_flags == O_RDONLY && dup2(redfile, STDIN_FILENO) < 0) {
				perror(COL(ERR_COL) "AGSH shell error(indup)" COL_RES);
				return -1;
			}

			// Close the file descriptor and clean up
			close(redfile);
			open_flags = -1;

			// Clean up the redirection part of the prompt
			argv[i] = NULL;
			argv[i + 1] = NULL;
			i++;
		}
	}

	int after_size = *argc;
	for (int i = (*argc) - 1; i >= 0; i--) {
		if (argv[i] == NULL) {
			for (int j = i + 1; j < after_size; j++) argv[j - 1] = argv[j];
			(*argc)--;
		}
	}
	return 0;
}
