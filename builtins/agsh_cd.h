int agsh_cd(int argc, char** argv) {
	if (argc != 2 || strcmp(argv[0], "cd")) {
		printf(COL(ERR_COL) "AGSH shell error: " COL_RES "Command mismatch.\n");
		return -1;
	}

	

	char* oldwd;
	oldwd = (char*) malloc(PATH_MAX * sizeof(char));
	if (getcwd(oldwd, PATH_MAX) == NULL) {
		perror(COL(ERR_COL) "AGSH ahhell error (getoldwd)" COL_RES);
		exit(EXIT_FAILURE);
	}

	if (strcmp(argv[1], "-") == 0) {
		if (getenv("OLDPWD") == NULL) {
			printf(COL(ERR_COL) "AGSH shell error (OLDPWDretrieve): " COL_RES "The previous directory is not set.\n");
			return -1;
		}
		strcpy(argv[1], getenv("OLDPWD"));
		printf("%s\n", argv[1]);
	}
	if (chdir(parse_path(&argv[1])) == -1) {
		perror(COL(ERR_COL) "AGSH shell error (changedir)" COL_RES);
		return -1;
	}

	setenv("OLDPWD", oldwd, 1);
	return 0;
}
