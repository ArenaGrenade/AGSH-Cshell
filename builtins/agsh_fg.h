int agsh_fg(int argc, char** argv) {
    if (argc != 2 || strcmp(argv[0], "fg") != 0) {
        printf(COL(ERR_COL) "AGSH shell error (fg): " COL_RES "Command mismatch.\n");
    }

    struct child_process* job;

    if ((job = get_process(strtol(argv[1], NULL, 10))) == NULL) {
        printf(COL(ERR_COL) "AGSH shell error (getjobpid): " COL_RES "Job not found.\n"); 
        return -1;
    }

    int status;

    fgproc_pid = job->pid;
    del_process(fgproc_pid);
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);

    tcsetpgrp(STDIN_FILENO, fgproc_pid);

    kill(job->pid, SIGCONT);

    waitpid(job->pid, &status, WUNTRACED);

    tcsetpgrp(STDIN_FILENO, getpgrp());

    signal(SIGTTOU, SIG_DFL);
    signal(SIGTTIN, SIG_DFL);

    if(!WIFSTOPPED(status)) del_process(job->pid);
    else printf("%s (%i) has stopped!\n", argv[0], job->pid);

    fgproc_pid = -1;
    
    tcsetpgrp(shell_fd, shell_pgid);

    return 0;
}