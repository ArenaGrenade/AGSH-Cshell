#ifndef AGSH_PINFO_BUF
#define AGSH_PINFO_BUF 256
#endif

int agsh_pinfo(int argc, char** argv) {
	if (argc == 0 || strcmp(argv[0], "pinfo") != 0) {
		printf(COL(ERR_COL) "AGSH shell error: " COL_RES "Command Mismatch.\n");
		return -1;
	}
	
	// To print the pid
	pid_t pid = getpid();
	if (argv[1] != NULL) pid = atoi(argv[1]);

	printf("PID -- %d\n", pid);

	// File requirements
	char path[PATH_MAX];
	char* buf;
	buf = (char*) malloc(AGSH_PINFO_BUF * sizeof(char));

	FILE* file;

	// Variables to read values from
	char state;
	int memsize;

	sprintf(path, "/proc/%i/status", pid);
	
	if(!(file = fopen(path, "r"))) {
		perror(COL(ERR_COL) "AGSH shell error (pinfostatus)" COL_RES);
		return -1;
	} else {
		while(fgets(buf, AGSH_PINFO_BUF, file) != NULL) {
			if (sscanf(buf, "State:\t%c", &state) == 1) {
				printf("Process Status -- %c\n", state);
			}
			if (sscanf(buf, "VmSize:\t%d kB", &memsize) == 1) {

				printf("Memory -- %i\n", memsize);
			}
		}
	}

	// Reading proc-exe for executable path
	sprintf(path, "/proc/%i/exe", pid);
	buf = (char*) malloc(PATH_MAX * sizeof(char));
	memset(buf, 0, PATH_MAX);

	int size = -1;
	if ((size = readlink(path, buf, PATH_MAX - 1) == -1)) {
		perror(COL(ERR_COL) "AGSH shell error (pinforeadlink)" COL_RES);
		return -1;
	} else {
		buf = parse_path_rev(&buf);
		printf("Executable Path -- %s\n", buf);
	}

	return 0;
}
