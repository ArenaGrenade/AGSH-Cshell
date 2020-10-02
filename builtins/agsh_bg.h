int agsh_bg(int argc, char** argv) {
    if (argc != 2 | strcmp(argv[0], "bg") != 0) {
        printf(COL(ERR_COL) "AGSH shell error (bg): " COL_RES "Command mismatch.\n");
        return -1;
    }

    struct child_process* job;

    if ((job = get_process(strtol(argv[1], NULL, 10))) == NULL) {
        printf(COL(ERR_COL) "AGSH shell error (getjobpid): " COL_RES "Job not found.\n"); 
        return -1;
    }

    if (kill(job->pid, SIGCONT) < 0) {
        perror(COL(ERR_COL) "AGSH shell error (signalproc)" COL_RES);
        return -1;
    }

    printf("[%d] %s %d running in background\n", strtol(argv[1], NULL, 10), job->name, job->pid);

    return 0;
}