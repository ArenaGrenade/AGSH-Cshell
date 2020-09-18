char* parse_path(char** path) {
	char* subs = (char*) malloc(PATH_MAX * sizeof(char));

	if ((*path)[0] == '~') {
		if (strlen(*path) > 1)  sprintf(subs, "%s%s", SHELL_CWD, *path + 1);
		else strcpy(subs, SHELL_CWD);
		*path = subs;
	}
	return *path;
}

char* parse_path_rev(char* path[PATH_MAX]) {
	char* subs = (char*) malloc(PATH_MAX * sizeof(char));

	for(int i = 0; i < strlen(SHELL_CWD); i++) {
		if (SHELL_CWD[i] != (*path)[i]) {
			return *path;
		}
	}
	sprintf(subs, "~%s", (*path) + strlen(SHELL_CWD));
	*path = subs;

	return *path;
}
