#define AGSH_STAT_SIZE 32

int print_dir(char* path, int flaga, int flagl) {
	path = parse_path(&path);

	DIR* path_dir;
	if ((path_dir = opendir(path)) == NULL) {
		perror(COL(ERR_COL) "AGSH shell error (lsdiropen)" COL_RES);
		return -1;
	}

	struct stat stat_buf;

	int stat_arr_size = AGSH_STAT_SIZE, index = 0, max_digits = 0, cur_digits = 0, fil_siz = 0;
	struct stat* stat_arr = malloc(stat_arr_size * sizeof(struct stat));

	int user_max_size = 0, user_cur_size = 0, group_max_size = 0, group_cur_size = 0;
	struct passwd* pwd_buf;
	struct group* grp_buf;

	char** filenames = (char**) malloc(stat_arr_size * sizeof(char*));

	int sum_blocks = 0, siz_blocks = 0;

	char fp_buf[PATH_MAX];
	struct dirent* entry;
	while ((entry = readdir(path_dir)) != NULL) {
		if (!flaga && entry->d_name[0] == '.') continue;

		sprintf(fp_buf, "%s/%s", path, entry->d_name);
		if(stat(fp_buf, &stat_buf) == -1) {
			perror(COL(ERR_COL) "AGSH shell error (lsstat)" COL_RES);
		}
		
		if (!flagl) {
			if (S_ISDIR(stat_buf.st_mode)) printf(COL(LS_FOLDER_BG) COL(LS_FOLDER_FG) "%s  " COL_RES, entry->d_name);
			else printf(COL(LS_FILE_NAME_BG) COL(LS_FILE_NAME_BG)"%s  " COL_RES, entry->d_name);
		}
		else {
			filenames[index] = entry->d_name;
			stat_arr[index++] = stat_buf;
			
			cur_digits = 0;
			fil_siz = stat_buf.st_size;
			do {
				cur_digits++;
				fil_siz /= 10;
			} while(fil_siz != 0);

			max_digits = ((max_digits >= cur_digits)? max_digits : cur_digits);

			pwd_buf = getpwuid(stat_buf.st_uid);
			user_cur_size = strlen(pwd_buf->pw_name);
			user_max_size = ((user_max_size >= user_cur_size)? user_max_size: user_cur_size);

			grp_buf = getgrgid(stat_buf.st_gid);
			group_cur_size = strlen(grp_buf->gr_name);
			group_max_size = ((group_max_size >= group_cur_size)? group_max_size: group_cur_size);

			sum_blocks += stat_buf.st_blocks;

			if (index >= stat_arr_size) {
				stat_arr_size += AGSH_STAT_SIZE;
				stat_arr = realloc(stat_arr, stat_arr_size * sizeof(struct stat));
				filenames = realloc(filenames, stat_arr_size * sizeof(char*));
				if (!stat_arr) printf(COL(ERR_COL) "AGSH shell error (statarray): Memory allocation error.\n" COL_RES);
				if (!filenames) printf(COL(ERR_COL) "AGSH shell error (filenames): Memory allocation error.\n" COL_RES);
			}
		}
	}

	char time_str[1000];
	struct tm* mod_time;
	int is_dir = 0;


	if (flagl) printf("total %i\n", sum_blocks / 2);

	for(int i = 0; i < index; i++) {
		// Is a Directory
		is_dir = S_ISDIR(stat_arr[i].st_mode);
		printf("%c", is_dir? 'd': '-');
		
		// User permissions
		printf("%c", (stat_arr[i].st_mode & S_IRUSR)? 'r': '-');
		printf("%c", (stat_arr[i].st_mode & S_IWUSR)? 'w': '-');
		printf("%c", (stat_arr[i].st_mode & S_IXUSR)? 'x': '-');

		// Group permissions
		printf("%c", (stat_arr[i].st_mode & S_IRGRP)? 'r': '-');
		printf("%c", (stat_arr[i].st_mode & S_IWGRP)? 'w': '-');
		printf("%c", (stat_arr[i].st_mode & S_IXGRP)? 'x': '-');

		// Others permissions
		printf("%c", (stat_arr[i].st_mode & S_IROTH)? 'r': '-');
		printf("%c", (stat_arr[i].st_mode & S_IWOTH)? 'w': '-');
		printf("%c", (stat_arr[i].st_mode & S_IXOTH)? 'x': '-');

		printf(" ");

		// Number of symbolic links
		printf(COL(LS_SYM_LINK_BG) COL(LS_SYM_LINK_FG) "%i" COL_RES " ", stat_arr[i].st_nlink);

		// User owner
		pwd_buf = getpwuid(stat_arr[i].st_uid);
		printf(COL(LS_USR_OWN_BG) COL(LS_USR_OWN_FG) "%*s" COL_RES " ", user_max_size, pwd_buf->pw_name);

		// Group Owner
		grp_buf = getgrgid(stat_arr[i].st_gid);
		printf(COL(LS_GRP_OWN_BG) COL(LS_GRP_OWN_BG) "%*s" COL_RES " ", group_max_size, grp_buf->gr_name);

		// Size of files in bytes
		printf(COL(LS_SIZE_BG) COL(LS_SIZE_FG) "%*i" COL_RES " ", max_digits, stat_arr[i].st_size);
	
		// Time of last modification
		mod_time = localtime(&stat_arr[i].st_mtime);
		strftime(time_str, 1000, "%b %d %H:%M ", mod_time);
		printf(COL(LS_MOD_TIME_BG) COL(LS_MOD_TIME_BG) "%s" COL_RES " ", time_str);
		
		// Prints the name of the current file
		if (is_dir) printf(COL(LS_FOLDER_BG) COL(LS_FOLDER_FG) "%s" COL_RES "\n", filenames[i]);
		else printf(COL(LS_FILE_NAME_BG) COL(LS_FILE_NAME_FG) "%s" COL_RES "\n", filenames[i]);
	}

	free(stat_arr);
	free(filenames);

	if(!flagl) printf("\n");
	
	return 0;
}

int agsh_ls(int argc, char** argv) {
	if(argc == 0 || strcmp(argv[0], "ls") != 0) {
		printf(COL(ERR_COL) "AGSH shell error: Command mismatch.\n" COL_RES);
		return -1;
	}

	int c = 0;

	int aflag = 0;
	int lflag = 0;
	optind = 0;

	while((c = getopt(argc, argv, "al")) != -1) {
		switch(c) {
			case 'a':
				aflag = 1;
				break;
			case 'l':
				lflag = 1;
				break;
			default:
				printf(COL(ERR_COL) "AGSH shell error: " COL_RES "One or more of the flags was not recognized\n");
				return 0;
		}
	}

	int count = 0;
	int dir_count = 0;
	for(int i = 1; i < argc; i++) if (argv[i][0] != '-') dir_count++;

	int dir_printed = 0;
	for(int i = 1 ; i < argc; i++) {
		if (argv[i][0] != '-') {
			if (dir_count > 1) printf("%s:\n", argv[i]);

			if(print_dir(argv[i], aflag, lflag) == -1) return -1;
			dir_printed++;

			if (dir_count - dir_printed != 0) printf("\n");

			count++;
		}
	}

	if(!count) print_dir(".", aflag, lflag);
	return 0;
}
