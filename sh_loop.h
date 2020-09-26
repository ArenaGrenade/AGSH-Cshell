#include "defines.h"
#include "includes.h"
#include "exec_handler.h"

void prompt_edit_path(char** buf, char* usr_sys, int status) {
	char* cwd;
	cwd = (char*) malloc(PATH_MAX * sizeof(char));

	if (getcwd(cwd, PATH_MAX) == NULL) {
		perror(COL(ERR_COL) "AGSH ahhell error (getcwd)" COL_RES);
		exit(EXIT_FAILURE);
	}

	parse_path_rev(&cwd);

	sprintf(*buf, "%s" "%s" COL(PATH_COL) "%s" COL_RES "%c", (status < 0)?":'( ":":') ", usr_sys, cwd, '>');
}

void get_user_sys(char** buf) {
	struct passwd *pw;
	uid_t uid = geteuid();

	char hostname[HOST_NAME_MAX];

	// Get system host name
	if (gethostname(hostname, sizeof(hostname)) < 0) {
		perror(COL(ERR_COL) "AGSH shell error (gethostname)" COL_RES);
		exit(EXIT_FAILURE);
	}

	// Fetching the user name
	if ((pw = getpwuid(uid)) == NULL) {
		perror(COL(ERR_COL) "AGSH shell error (getusername)" COL_RES);
		exit(EXIT_FAILURE);
	}
	
	int bufsize = AGSH_TOK_BUFSIZE * 5;
	*buf = (char*) malloc(bufsize * sizeof(char));
	sprintf(*buf, "%c" COL(USR_COL) "%s" COL_RES "@" COL(SYS_COL) "%s" COL_RES ":", '<', pw->pw_name, hostname);
}

char** tokenize(int* cmd_cnt, char* cmd) {
	char* token;
	int tok_bufsize = AGSH_TOK_BUFSIZE, index = 0;
	char** tokens = (char**) malloc(tok_bufsize * sizeof(char*));

	if (!tokens) {
		printf(COL(ERR_COL) "AGSH shell error (tokbuf):" COL_RES "Memory allocation error.\n");
		exit(EXIT_FAILURE);
	}

	token = strtok(cmd, AGSH_TOK_DELIM);
	while (token != NULL) {
		tokens[index++] = token;

		if (index >= tok_bufsize) {
			tok_bufsize += AGSH_TOK_BUFSIZE;
			tokens = realloc(tokens, tok_bufsize * sizeof(char*));
			if (!tokens) {
				printf(COL(ERR_COL) "AGSH shell error (tokbuf): Memory allocation error.\n" COL_RES);
				exit(EXIT_FAILURE);
			}
		}

		token = strtok(NULL, AGSH_TOK_DELIM);
	}
	tokens[index] = NULL;
	*cmd_cnt = index;
	return tokens;
}

char** split_cmds(int* cmd_len) {
	char* line = NULL;
	ssize_t read_bufsize = 0;
	struct passwd* username;

	if(getline(&line, &read_bufsize, stdin) == -1) {
		if(feof(stdin)) {
			printf("\nEOF character detected exiting...\n");
			char** exit_com = (char**) malloc(sizeof(char*));
			exit_com[0] = (char*) malloc(5 * sizeof(char));
			strcpy(exit_com[0], "quit");
			agsh_quit(1, exit_com);
			exit(EXIT_FAILURE);
		}
		perror(COL(ERR_COL) "AGSH shell error (shellreadline)" COL_RES);
		exit(EXIT_FAILURE);
	}
	
	char* cmd;
	int cmd_bufsize = AGSH_TOK_BUFSIZE, index = 0;
	char** cmds = (char**) malloc(cmd_bufsize * sizeof(char));

	if (!cmds) {
		printf(COL(ERR_COL) "AGSH shell error (cmdbuf):" COL_RES "Memory allocation error.\n");
		exit(EXIT_FAILURE);
	}

	cmd = strtok(line, ";");
	while (cmd != NULL) {
		cmds[index++] = cmd;

		if (index >= cmd_bufsize) {
			cmd_bufsize += AGSH_TOK_BUFSIZE;
			cmds = realloc(cmds, cmd_bufsize * sizeof(char*));
			if (!cmds) {
				printf(COL(ERR_COL) "AGSH shell error (cmdbuf): Memory allocation error.\n" COL_RES);
				exit(EXIT_FAILURE);
			}
		}

		cmd = strtok(NULL, ";");
	}
	cmds[index] = NULL;
	*cmd_len = index;
	return cmds;
}

void shell_loop(void) {
	int shell_fail = 1;
	char* user_sys;
	get_user_sys(&user_sys);

	char* path_buf = (char*) malloc(AGSH_TOK_BUFSIZE * 32 * sizeof(char));

	char** cmds;
	int cmd_len;
	char** tokens;
	int tok_len;

	// Stuff to do before the shell starts - need for refactor
	signal(SIGTTIN, SIG_IGN); // stop terminal input to background process
	signal(SIGTTOU, SIG_IGN); // stop terminal output control for background process

	signal(SIGINT, SIG_IGN); // ignore CTRL + C
	signal(SIGTSTP, SIG_IGN); // ingore CTRL + Z
	signal(SIGQUIT, SIG_IGN); // ignore CTRL + \

	signal(SIGCHLD, agsh_sig_handle);

	do {
		// Print prompt
		prompt_edit_path(&path_buf, user_sys, shell_fail);
		printf(path_buf);

		// Read and tokenize command
		cmds = split_cmds(&cmd_len);
		for (int i = 0; i < cmd_len; i++) {
			tokens = tokenize(&tok_len, cmds[i]);
			shell_fail = agsh_frontend_exec(tok_len, tokens);
		}
	} while(shell_fail <= 0);
}
