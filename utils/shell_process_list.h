struct child_process {
	char* name;
	pid_t pid;
	struct child_process* next;
};

struct child_process* head_lis = NULL;
struct child_process* current = NULL;

int proc_list_size = 0;

void add_process(pid_t pid, char* name) {
	struct child_process* new_head = (struct child_process*) malloc(sizeof(struct child_process));

	new_head->pid = pid;
	new_head->name = strdup(name);

	new_head->next = head_lis;
	head_lis = new_head;
	proc_list_size++;
}

void del_process(pid_t pid) {
	if (head_lis == NULL) return;
	struct child_process* curr = head_lis;
	struct child_process* prev = NULL;

	while(curr->pid != pid) {
		if(curr->next == NULL) {
			return;
		} else {
			prev = curr;
			curr = curr->next;
		}
	}

	if(curr == head_lis) {
		head_lis = head_lis->next;
	} else {
		prev->next = curr->next;
	}

	proc_list_size--;
}

pid_t get_process(int job) {
	if (head_lis == NULL) return -1;
	struct child_process* iter = head_lis;
	int iter_position = 1;

	while (iter_position < job && iter != NULL) {
		iter = iter->next;
		iter_position++;
	}
	if (iter == NULL) return -1;
	return iter->pid;
}

int print_processes() {
	if (head_lis == NULL) return 0;
	struct child_process* last_printed = NULL;
	struct child_process* print_iter = head_lis;
	int iter_position = 1;

	char path[PATH_MAX];
	char* buf;
	buf = (char*) malloc(AGSH_PINFO_BUF * sizeof(char));

	FILE* file;

	// Variables to read value to
	char state;

	while (last_printed != head_lis) {
		while (print_iter->next != last_printed) print_iter = print_iter->next;

		sprintf(path, "/proc/%i/status", print_iter->pid);
		
		if(!(file = fopen(path, "r"))) {
			perror(COL(ERR_COL) "AGSH shell error (statusbgproc)" COL_RES);
			return -1;
		} else {
			while(fgets(buf, AGSH_PINFO_BUF, file) != NULL) 
				if (sscanf(buf, "State:\t%c", &state) == 1) 
					break;
		}

		printf("[%i] %s %s [%i]", iter_position, (state == 'R')? "Running" : "Stopped", print_iter->name, print_iter->pid);
		last_printed = print_iter;
		iter_position++;
	}

	fclose(file);

	return 0;
}
