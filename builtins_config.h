#define num_builtins 8
char* names_builtins[] = {
	"echo",
	"pwd",
	"cd",
	"ls",
	"pinfo",
	"nightswatch",
	"history",
	"exit"
};

int agsh_echo(int argc, char** argv);
int agsh_pwd(int argc, char** argv);
int agsh_cd(int argc, char** argv);
int agsh_ls(int argc, char** argv);
int agsh_pinfo(int argc, char** argv);
int agsh_nightswatch(int argc, char** argv);
int agsh_history(int argc, char** argv);
int agsh_exit(int argc, char** argv);

int (*func_builtins[]) (int, char**) = {
	&agsh_echo,
	&agsh_pwd,
	&agsh_cd,
	&agsh_ls,
	&agsh_pinfo,
	&agsh_nightswatch,
	&agsh_history,
	&agsh_exit
};
