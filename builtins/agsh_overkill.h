int agsh_overkill(int argc, char** argv) {
    if (argc != 1 || strcmp(argv[0], "overkill") != 0) {
        printf(COL(ERR_COL) "AGSH shell error (overkill): " COL_RES "Command mismatch.\n");
        return -1;
    }

    struct child_process* to_kill;
    while (proc_list_size != 0) {
        if ((to_kill = get_process(proc_list_size)) == NULL) return -1;

        if (kill(to_kill->pid, SIGKILL) < 0) {
            perror(COL(ERR_COL) "AGSH shell error (killproc): " COL_RES);
            printf(COL(FGB_BLACK) "Unable to kill %s (%i).\n" COL_RES, to_kill->name, to_kill->pid);
        }

        del_process(to_kill->pid);
    }

    return 0;
}