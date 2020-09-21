int agsh_getenv(int argc, char** argv) {
    if (argc != 2 || strcmp(argv[0], "getenv") != 0) {
        printf(COL(ERR_COL) "AGSH shell error: " COL_RES "Command mismatch.\n");
        return -1;
    }
    
    if (getenv(argv[1]) == NULL) {
        printf(COL(ERR_COL) "AGSH shell error (getenv): " COL_RES "Environemnt variable not found.\n");
        return -1;
    }

    printf("%s\n", getenv(argv[1]));

    return 0;
}