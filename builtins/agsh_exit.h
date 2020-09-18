int agsh_exit(int argc, char** argv) {
	if (argc != 1 || strcmp(argv[0], "exit") != 0) {
		printf(COL(ERR_COL) "AGSH shell error (exit): " COL_RES "Command Mismatch.\n");
		return -1;
	}
	return 1;
}
