#define num_builtins 11
char* names_builtins[] = {
	"echo",
	"pwd",
	"cd",
	"ls",
	"pinfo",
	"nightswatch",
	"history",
	"exit",
	"setenv",
	"unsetenv",
	"getenv",
};

int agsh_echo(int argc, char** argv);
int agsh_pwd(int argc, char** argv);
int agsh_cd(int argc, char** argv);
int agsh_ls(int argc, char** argv);
int agsh_pinfo(int argc, char** argv);
int agsh_nightswatch(int argc, char** argv);
int agsh_history(int argc, char** argv);
int agsh_exit(int argc, char** argv);
int agsh_setenv(int argc, char** argv);
int agsh_unsetenv(int argc, char** argv);
int agsh_getenv(int argc, char** argv);

int (*func_builtins[]) (int, char**) = {
	&agsh_echo,
	&agsh_pwd,
	&agsh_cd,
	&agsh_ls,
	&agsh_pinfo,
	&agsh_nightswatch,
	&agsh_history,
	&agsh_exit,
	&agsh_setenv,
	&agsh_unsetenv,
	&agsh_getenv,
};
