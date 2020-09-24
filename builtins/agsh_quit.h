int agsh_quit(int argc, char** argv) {
	if (argc != 1 || strcmp(argv[0], "quit") != 0) {
		printf(COL(ERR_COL) "AGSH shell error (exit): " COL_RES "Command Mismatch.\n");
		return -1;
	}

	printf("\nThanks for using AGSH shell! Good Bye! :)\n\n");
	write_history(HISTORY_PATH);

	exit(EXIT_SUCCESS);
}
