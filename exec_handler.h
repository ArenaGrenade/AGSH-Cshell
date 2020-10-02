int redirection_setup(int* argc, char** argv, int* stdfds);

int print_arr(int size, char** arr) {
	for (int i = 0 ; i < size; i++) {
		if (arr[i] != NULL) printf("%s\n", arr[i]);
		else printf("That was a NULL\n");
	}
}

int agsh_spawn_process(int argc, char** argv, int is_piped) {
	pid_t child;

	int do_wait = 1;
	if (strcmp(argv[argc - 1], "&") == 0) {
		argv[argc - 1] = NULL;
		argc--;
		do_wait = 0;
	}
	
	child = fork();
	if (!do_wait) setpgid(0, 0);
	if (child == 0) {	

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
		int status;
		if (is_piped) {
			waitpid(child, &status, WUNTRACED);
			return 0;
		}
		if (do_wait) {

			fgproc_pid = child;
			signal(SIGTTOU, SIG_IGN);
			setpgid(child, 0);
			tcsetpgrp(STDIN_FILENO, child);
			add_process(child, argv[0]);
			waitpid(child, &status, WUNTRACED);

			tcsetpgrp(STDIN_FILENO, getpgrp());
			signal(SIGTTOU, SIG_DFL);

			if(!WIFSTOPPED(status)) del_process(child);
			else printf("%s (%i) has stopped!\n", argv[0], child);
			
			tcsetpgrp(shell_fd, shell_pgid);

			return 0;
		}
		else {
			waitpid(child, &status, WNOHANG);
			printf("[%i] %i\n", proc_list_size, child);
			add_process(child, argv[0]);
			return 0;
		}
	}

	return 0;
}

void decide_front_back(int argc, char **argv, int* status, int is_piped) {
	int was_builtin = 0;

	for (int i = 0; i < num_builtins; i++) {
		if(strcmp(argv[0], names_builtins[i]) == 0) {
			(*status) = (func_builtins[i])(argc, argv);
			was_builtin = 1;
			break;
		}
	}
	if (!was_builtin) (*status) = agsh_spawn_process(argc, argv, is_piped);
}

int piping_and_exec(int argc, char** argv) {
	// Creating a matrix of size argc * argc and then entering the indices of the commands correspondingly.
	int** cmd_indices = (int**) malloc(argc * sizeof(int*));
	cmd_indices[0] = (int*) malloc(argc * sizeof(int));
	for (int j = 0 ; j < argc; j++) cmd_indices[0][j] = -1;

	int cnt_cmds = 0;
	for (int i = 0, col_ind = 0; i < argc; i++) {
		if (strcmp(argv[i], "|") == 0) {
			if (i == argc - 1){
				printf(COL(ERR_COL) "AGSH shell error: " COL_RES "Unable to process command.\n");
				return -1;
			}
			cnt_cmds++;
			cmd_indices[cnt_cmds] = (int*) malloc(argc * sizeof(int));
			for (int j = 0 ; j < argc; j++) cmd_indices[cnt_cmds][j] = -1;
			col_ind = 0;

			continue;
		}
		cmd_indices[cnt_cmds][col_ind] = i;
		col_ind++;
	}

	int status = 0;
	if (cnt_cmds == 0) {
		// Creating the redirection now
		int stdfds[2];
		if (redirection_setup(&argc, argv, stdfds) < 0) return -1;

		decide_front_back(argc, argv, &status, 0);

		// Rediretion Reset
		dup2(stdfds[0], STDIN_FILENO);
		dup2(stdfds[1], STDOUT_FILENO);
	} else {
		int num_pipes = cnt_cmds + 2;
		int num_cmds = cnt_cmds + 1;
		
		// Create a list of pids of all pipe proocesses
		pid_t* pipepids = (pid_t*) malloc(num_cmds * sizeof(pid_t));

		// Create a list for the pipefds
		int** pipefds = (int**) malloc(num_pipes * sizeof(int*));

		for (int i = 0; i < num_pipes; i++) {
			// Create pipes here and associate them with fds.
			pipefds[i] = (int*) malloc(2 * sizeof(int));
			pipefds[i][0] = -1;
			pipefds[i][0] = -1;
			if (pipe(pipefds[i]) < 0) {
				perror(COL(ERR_COL) "AGSH shell error (pipecreate)" COL_RES);
				// Close every other opened pipe on any failure
				for (int j = 0; j < num_pipes; j++) {
					if (pipefds[j][0] != -1) close(pipefds[i][0]);
					if (pipefds[j][1] != -1) close(pipefds[i][1]);
				}
				return -1;
			}
		}

		// Create the processes and handle pipes for them.
		for (int i = 0 ; i < num_cmds; i++) pipepids[i] = -2;
		for (int k = 0 ; k < num_cmds; k++) {
			// Fork a child
			if ((pipepids[k] = fork()) < 0) {
				perror(COL(ERR_COL) "AGSH shell error (forkpipe)" COL_RES);
				exit(EXIT_FAILURE);
			}

			if (pipepids[k] == 0) {
				// Close all non-relevant pipe endings here.
				for (int i = 0; i < num_pipes; i++) {
					for (int j = 0; j < 2; j++) {
						if ((i == k && j == 0) || (i == k + 1 && j == 1)) continue;
						close(pipefds[i][j]);
					}
				}

				// Dup the pipe that needs to be used here.
				dup2(pipefds[k][0], STDIN_FILENO); // dup the input
				if (k != (num_cmds - 1)) dup2(pipefds[k + 1][1], STDOUT_FILENO); // dup the output

				// Recreate the command list here
				int cur_size = 0;
				char** command = (char**) malloc(sizeof(char*));
				while (cmd_indices[k][cur_size] != -1) {
					command = (char**) realloc(command, (cur_size + 2) * sizeof(char));
					command[cur_size] = argv[cmd_indices[k][cur_size]];
					cur_size++;
				}
				command[cur_size] = NULL;


				// Creating the redirection now
				int stdfds[2];
				if (redirection_setup(&cur_size, command, stdfds) < 0) return -1;

				// Testing code for finding the process id and the fds as well as the pipings.
				// printf("%s at pid %i with pipe co-ordinates (%i, %i) and (%i, %i) and having fds (in: %i, out: %i)\n", command[0], getpid(), k, 0, k + 1, 1, pipefds[k][0], pipefds[k + 1][1]);

				// Execute the command
				int status;
				decide_front_back(cur_size, command, &status, 1);
				if (status < 0) exit(EXIT_FAILURE);

				// Rediretion Reset
				dup2(stdfds[0], STDIN_FILENO);
				dup2(stdfds[1], STDOUT_FILENO);

				// Clean and exit this child
				exit(EXIT_SUCCESS);
			}
		}

		// Close all pipes in the parent process
		for (int i = 0; i < num_pipes; i++)
			for (int j = 0; j < 2; j++)
				close(pipefds[i][j]);

		int child_status = 0;
		// Wait for all the children to complete and handle errors
		for (int i = 0 ; i < num_cmds; i++) {
			if ((waitpid(pipepids[i], &child_status, 0) < 0) || (!WIFEXITED(child_status) && (WEXITSTATUS(child_status) == EXIT_FAILURE))) {
				perror(COL(ERR_COL) "AGSH shell error (waitforpipes)" COL_RES);
				// Close all other opened processes and the pipe created for the parent comms.
				for (int j = 0 ; j < num_cmds; j++) {
					if (pipepids[j] != -1) {
						kill(pipepids[j], SIGTERM);
					}
				}
				return -1;
			}
		}
	}
	return status;
}

int agsh_frontend_exec(int argc, char **argv) {
	if (argv[0] == NULL) return 0;
	enqueue(argv[0]);

	int status;	
	status = piping_and_exec(argc, argv);

	return status;
}


int redirection_setup(int* argc, char **argv, int* stdfds) {
	int open_flags = -1, redfile = 0;

	stdfds[0] = dup(STDIN_FILENO);
	stdfds[1] = dup(STDOUT_FILENO);

	for (int i = 1 ; i < *argc; i++) {
		if (strcmp(argv[i], ">") == 0) open_flags = O_WRONLY | O_TRUNC | O_CREAT;
		else if (strcmp(argv[i], ">>") == 0) open_flags = O_WRONLY | O_CREAT | O_APPEND;
		else if (strcmp(argv[i], "<") == 0) open_flags = O_RDONLY;

		if (open_flags != -1) {
			// Check for existence of the correspoding file
			if (i + 1 >= *argc) {
				printf(COL(ERR_COL) "AGSH shell error: " COL_RES "Input or output file not specified.\n");
				return -1;
			}

			// Open the file and duplicate the filestream for stdout
			redfile = open(argv[i + 1], open_flags, S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH);

			// Handle for output file
			if (open_flags != O_RDONLY && dup2(redfile, STDOUT_FILENO) < 0) {
				perror(COL(ERR_COL) "AGSH shell error(outdupopen)" COL_RES);
				return -1;
			}
			

			// Handle for input file
			if (open_flags == O_RDONLY && dup2(redfile, STDIN_FILENO) < 0) {
				perror(COL(ERR_COL) "AGSH shell error(indupopen)" COL_RES);
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
