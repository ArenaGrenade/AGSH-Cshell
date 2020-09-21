int agsh_setenv(int argc, char** argv) {
    if (argc < 2 || argc > 3 || strcmp(argv[0], "setenv") != 0) {
        printf(COL(ERR_COL) "AGSH shell error: " COL_RES "Command mismatch.\n");
    }
    
    
    if (setenv(argv[1], ((argc == 3)?argv[2]:""), 0) < 0) {
        perror(COL(ERR_COL) "AGSH shell error (setenv)" COL_RES);
        return -1;
    }
    
    return 0;
}