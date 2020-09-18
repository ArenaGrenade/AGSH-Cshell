int agsh_echo(int argc, char** argv) {
	if(argc == 0) {
		return -1;
	}
	
	if (strcmp(argv[0], "echo") != 0) {
		printf(COL(ERR_COL) "AGSH shell error: " COL_RES "Command Mismatch.\n");
		return -1;
	}

	// Flags
	//
	int flagn = 0;
	optind = 0;
	char c;
	while ((c = getopt(argc, argv, "n")) != -1) {
		if (c == 'n') {
			flagn = 1;
		} else {
			printf(COL(ERR_COL) "AGSH shell error: " COL_RES "One or more of the arguements that you have entered is incorrect.\n");
			return -1;
		}
	}

	for (int i = 1; i < argc; i++) {
		if (argv[i][0] != '-') printf("%s ", argv[i]);
	}
	if (!flagn) printf("\n");

	return 0;
}
