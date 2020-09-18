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
