#ifndef AGSH_WATCH_BUF
#define AGSH_WATCH_BUF 512
#endif

int agsh_nightswatch(int argc, char** argv) {
	if (argc != 4 || strcmp(argv[0], "nightswatch") != 0) {
		printf(COL(ERR_COL) "AGSH shell error (nightswatch): " COL_RES "Command mismatch.\n");
		return -1;
	}

	// Flags
	int nsecs = -1;

	optind = 0;
	char c;
	while ((c = getopt(argc, argv, "n:")) != -1) {
		switch (c) {
			case 'n':
				nsecs = atoi(optarg);
				break;
			default:
				printf(COL(ERR_COL) "AGSH shell error: " COL_RES "Invalid Arguments.\n");
				return -1;
		}
	}

	if (nsecs == -1) {
		printf(COL(ERR_COL) "AGSH shell error: " COL_RES "Parameter seconds is not supplied.\n");
		return -1;
	}

	if ((strcmp(argv[3], "interrupt") != 0) && (strcmp(argv[3], "newborn") != 0)) {
		printf(COL(ERR_COL) "AGSHS shell error: " COL_RES "Nightswatch command not found.\n");
		return -1;
	}

	pid_t child;

	child = fork();

	if (child == 0) {
		signal(SIGTTIN, SIG_IGN);
		signal(SIGTTOU, SIG_IGN);
		signal(SIGHUP, SIG_IGN);

		if (strcmp(argv[3], "interrupt") == 0) {
			char path[] = "/proc/interrupts";
			char buf[AGSH_WATCH_BUF];
			FILE* file;

			if (!(file = fopen(path, "r"))) {
				perror(COL(ERR_COL) "AGSH shell error (fopenchild)" COL_RES);
				exit(EXIT_FAILURE);
			}

			int column_count = 0;
			char* token;

			fgets(buf, AGSH_WATCH_BUF, file);
			fseek(file, 0, SEEK_SET);
		
			token = strtok(buf, " ");
			while (token != NULL) {
				printf("%s\t", token);
				column_count++;
				token = strtok(NULL, " ");
			}
			column_count--;
			printf("\n");

			int tok_num = 0;

			while (1) {
				fgets(buf, AGSH_WATCH_BUF, file);
				fgets(buf, AGSH_WATCH_BUF, file);
				fgets(buf, AGSH_WATCH_BUF, file);
				token = strtok(buf, " ");
				token = strtok(NULL, " ");
				while (token != NULL && tok_num++ < column_count) {
					printf("%s\t", token);
					token = strtok(NULL, " ");
				}
				printf("\n");
				fseek(file, 0, SEEK_SET);
				tok_num = 0;
				sleep(nsecs);
			}

			fclose(file);
		}
		else if (strcmp(argv[3], "newborn") == 0) {
			char path[] = "/proc";
			char buf[AGSH_WATCH_BUF];
			DIR* proc_dir;

			if ((proc_dir = opendir(path)) == NULL) {
				perror(COL(ERR_COL) "AGSH shell error (procdir)" COL_RES);
				printf("Press 'q' followed by 'return' key to continue.\n");
				exit(EXIT_FAILURE);
			}

			struct stat stat_buf;
			struct dirent* entry;
			char pid_path[PATH_MAX];

			time_t min_time = 0;
			pid_t pid;

			while (1) {
				while ((entry = readdir(proc_dir)) != NULL) {
					if (strlen(entry->d_name) == strspn(entry->d_name, "0123456789")) {
						memset(pid_path, 0, PATH_MAX);
						sprintf(pid_path, "%s/%s", path, entry->d_name);

						if (stat(pid_path, &stat_buf) == -1) {
							closedir(proc_dir);
							perror(COL(ERR_COL) "AGSH shell error (pidread)" COL_RES);
							printf("Press 'q' followed by 'return' key to continue.\n");
							exit(EXIT_FAILURE);
						}

						if (min_time < stat_buf.st_atime) {
							min_time = stat_buf.st_atime;
							pid = atoi(entry->d_name);
						}
					}
				}
				printf("%i\n", pid);
				rewinddir(proc_dir);
				sleep(nsecs);
			}

			closedir(proc_dir);
		}
		exit(EXIT_SUCCESS);
	}
	else if(child < 0) {
		perror(COL(ERR_COL) "AGSH shell error (forkfail)" COL_RES);
		return -1;
	}
	else {
		int status;
		char input = 0;
		do {
			waitpid(child, &status, WNOHANG | WUNTRACED);
			scanf("%c", &input);
		} while((input != 'q') && (!WIFEXITED(status)) && (!WIFSIGNALED(status)) && (!WIFSTOPPED(status)));
		if (input == 'q') kill(child, SIGTERM);
		printf(COL(ERR_COL) "Nightswatch Stopped\n" COL_RES);
	}
	return 0;
}
