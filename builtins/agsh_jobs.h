int agsh_jobs(int argc, char** argv) {
    if (argc != 1 || strcmp(argv[0], "jobs") != 0) {
        printf(COL(ERR_COL) "AGSH shell error (jobs): " COL_RES "Command mismatch.\n");
    }

    int return_status = print_processes();
    if (return_status < 0) return -1;
    
    return 0;
}