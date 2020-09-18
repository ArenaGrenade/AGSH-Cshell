int agsh_history(int argc, char** argv) {
	if (argc == 0 || strcmp(argv[0], "history") != 0) {
		printf(COL(ERR_COL) "AGSH shell error (history): " COL_RES "Command mismatch.\n");
		return -1;
	}
	
	if (argc == 2) print_queue(atoi(argv[1]));
	else print_queue(HISTORY_READ_MAX);

	return 0;
}
