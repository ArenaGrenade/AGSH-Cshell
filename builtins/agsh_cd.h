int agsh_cd(int argc, char** argv) {
	if (argc != 2 || strcmp(argv[0], "cd")) {
		printf(COL(ERR_COL) "AGSH shell error: Command mismatch.\n" COL_RES);
		return -1;
	}

	if (chdir(parse_path(&argv[1])) == -1) {
		perror(COL(ERR_COL) "AGSH shell error (changedir)" COL_RES);
		return -1;
	}
	return 0;
}
