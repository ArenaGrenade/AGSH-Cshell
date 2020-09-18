int agsh_pwd(int argc, char** argv){
	if (argc == 0 || strcmp(argv[0], "pwd") != 0) {
		printf(COL(ERR_COL) "AGSH shell error: Command mismatch.\n" COL_RES);
		return -1;
	}

	char* cwd;
	cwd = (char*) malloc(PATH_MAX * sizeof(char));

	if(getcwd(cwd, PATH_MAX) == NULL) {
		perror(COL(ERR_COL) "AGSH shell error" COL_RES);
		return -1;
	}

	printf("%s\n", cwd);

	free(cwd);
	return 0;
}
