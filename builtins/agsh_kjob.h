int agsh_kjob(int argc, char** argv) {
    if (argc != 3 | strcmp(argv[0], "kjob") != 0) {
        printf(COL(ERR_COL) "AGSH shell error (kjob): " COL_RES "Command mismatch.\n");
        return -1;
    }

    struct child_process* job;

    if ((job = get_process(proc_list_size - strtol(argv[1], NULL, 10) + 1)) == NULL) {
        printf(COL(ERR_COL) "AGSH shell error (getjobpid): " COL_RES "Job not found.\n"); 
        return -1;
    }

    if (kill(job->pid, strtol(argv[2], NULL, 10)) < 0) {
        perror(COL(ERR_COL) "AGSH shell error (signalproc)" COL_RES);
        return -1;
    }

    return 0;
}