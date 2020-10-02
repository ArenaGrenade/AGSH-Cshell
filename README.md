# AGSH-Cshell

A simple linux shell in C. Has very little functionality as of now.


## Shell Look and Feel

Refer to defines.h to get the color values to write to any new theme. Just alter the color values in `config.h` under `conf/` folder. This shell has been developed with deisgn as a very important aspect. For this a color based customization is prioritized for as many commands as possible.

## The prompt

The prompt and the entire running of the shell is from `sh_loop.h`. Before the start of this loop, we have a function from the `setup.h` file that fetches the username and systemname.

For every iteration of the shell loop, we fetch the current working directory. This is processed by the utils module to take care of the pseudo-home, which is the directory from which the shell is invoked from.

The shell also features a command success or failure denotion by having an emoji in front of the prompt - `:')` denotes a success of the previously executed command, while `:'(` denotes the failure of a previous command.

## Commands

The following commands are built into the shell.

### echo

* Echo is implemented in the `agsh_echo.h` file in the builtins folder.
* It performs all normal functions except parsing multiline strings and enviromental variables.
* Implemented the `-n` flag, which removes the endline character that is normally printed right after the given string to print.

### pwd

This command prints the current working directory after processing for the pseudo-home and can be found in `agsh_pwd.h`

### cd

* This command replicates its normal behaviour and is found in `agsh_cd.h`
* This command also can handle `cd -` whcih would take the shell to the previous working directory.

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

### history

* Prints the at max the last 20 commands. Can specify the number of commands by passed an extra parameter, like `history n`.
* Internally it is implemented as a queue on linked list for the shell to stay light on the PC (Performance first ;)).
* For it to get stored and retrieved, we need to properly quit the terminal. (Very Important)

### setenv

Sets an environment variable that is provided as `variable value`. An environment variable `variable` is created with the value `value`.

### unsetenv

Removes the environment variable that is given as an arguement to this command.

### getenv

Returns the value of the environment variable that is given as an argument to this command.

### jobs

Prints all the background process in order of the most recently created process at the last.

### kjob

Sends a signal `signal` to the background job pointed to by `job number`. Signals need to entered as integers.

### fg

Brings the background process `job number` and runs it in the foreground.

### bg

Runs the process in the background of the job `job nuumber`.

### overkill

Kills all the background process at a time. This is also used in `quit` command to ensure that there are no orphan processes after exiting thhe shell.

### quit

This command exits the terminal. (As the name suggests...)

## Foreground and Background processes

Foreground procoesses are what are normally executed. To perform a background process and & must be added to the end of the command.

* A background process that ends will print that the process has ended and will not print the prompt direrctly. This is because considering a command like nightswatch, we wouold have to break the process and then prompt. This is very hard to handle, so printing the prompt right after a background process breaks is skipped.

## Input Output Redirection

Input and output redirection is supported by the shell. 

* You can input a file using `<`
* You can append the output to a file using `>>`
* And you can truncate the file and output to it using `>`
* A new output file will be created if that file does not exist already.
* It also takes care of multiple command rerdirections like, `command < file1.txt < file2.txt`, only the laast one is considered.

## Piping

Any number of pipes are supported by the shell. Piping and Redirection can be easily combined with each other. Though, redirection binds tighter than pipes.

## Signals

The following signals are handled by the shell to ensure that they don't kill the shell itself.

### CTRL + Z

Any running foreground process is pushed to the background.

### CTRL + C

This signals closes / terminates using SIGINT any running foreground process

Both these commands do nothing if there are no foreground processes that are running.
