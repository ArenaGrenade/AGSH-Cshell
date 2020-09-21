int agsh_unsetenv(int argc, char** argv) {
    if (argc != 2 || strcmp(argv[0], "unsetenv") != 0) {
        printf(COL(ERR_COL) "AGSH shell error: " COL_RES "Command mismatch.\n");
		return -1;
    }

    if (unsetenv(argv[1]) < 0) {
		perror(COL(ERR_COL) "AGSH shell error (unsetenv)" COL_RES);
		return -1;
	}

    return 0;
}