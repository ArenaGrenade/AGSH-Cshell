# AGSH-Cshell
A simple linux shell in C. Has very little functionality as of now.


## Shell Look and Feel
Refer to defines.h to get the color values to write to any new theme. Just alter the color values in `config.h` under `conf/` folder. This shell has been developed with deisgn as a very important aspect. For this a color based customization is prioritized for as many commands as possible.


## The prompt
The prompt and the entire running of the shell is from `sh_loop.h`. Before the start of this loop, we have a function from the `setup.h` file that fetches the username and systemname. 
For every iteration of the shell loop, we fetch the current working directory. This is processed by the utils module to take care of the pseudo-home, which is the directory from which the shell is invoked from.


## Commands
The following commands are built into the shell.

### echo
* Echo is implemented in the `agsh_echo.h` file in the builtins folder.
* It performs all normal functions except parsing multiline strings and enviromental variables.
* Implemented the `-n` flag, which removes the endline character that is normally printed right after the given string to print.

### pwd
This command prints the current working directory after processing for the pseudo-home and can be found in `agsh_pwd.h`

### cd
This command replicates its normal behaviour and is found in `agsh_cd.h`

### ls
* This command lists all files in the current working directory.
* It can be found in `agsh_ls.h` under `builtins/`.
* It works for `-a` and `-l` flags in any combination.
* It can take multiple directories to print the file listing for all of them. Also works for `.`, `..` and `~`.

### pinfo
* When called alone without the `<pid>` parameter, it returns the info of the shell process.
* When called with the `<pid>` parmeter, it returns information of the process with pid as its process id.
* It prints the pid, Process Status, Total Virtual Memory usage, Path of the executable of the process.

### nightswatch
* Number of seconds to repeat the command for should be passed as a parameter of `-n`. The commands available are `interrupt` and `newborn`.
* When called with interrupt it returns number of `IRQ 1` interrupts for each CPU.
* When called with newborn it returns the processID of the most recently spawned process.
* For both of this, press `q` to stop nightswatch.
* It might exit with a repetition of shell prompts, you can still enter a command and use the shell normally. If this disturbs you please do a `clear` and continue.

* Prints the at max the last 20 commands. Can specify the number of commands by passed an extra parameter, like `history n`.
* Internally it is implemented as a queue on linked list for the shell to stay light on the RAM (Performance first ;)).
* For it to get stored and retrieved, we need to properly quit the terminal. (Verry Important)

### exit
* This command exits the terminal. (As the name suggests...)


## Foreground and Background processes
Foreground procoesses are what are normally executed. To perform a background process and & must be added to the end of the command.
* Vim does not work as a background process.
* A background process that ends will print that the process has ended and will not print the prompt direrctly. This is because considering a command like nightswatch, we wouold have to break the process and then prompt. This is very hard to handle, so printing the prompt right after a background process breaks is skipped.
