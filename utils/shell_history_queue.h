#define HISTORY_STORE_MAX 20
#define HISTORY_READ_MAX 10
#define HISTORY_BUF_SIZE 32


void print_queue(int);

struct queue_el {
	char* command;
	struct queue_el* next;
} *front, *back;

int history_size = 0;
int current_size = 0;

void dequeue() {
	if (front == NULL) return; // If there are no elements in queue just return.

	// store front in temp var to free later
	struct queue_el* temp = front;
	front = front->next;
	current_size--;

	// Case if there was only one element and that was deleted.
	// So we reset the queue
	if (front == NULL) back = NULL;

	free(temp);
	return;
}

void enqueue(char* word) {
	if (back != NULL && strcmp(back->command, word) == 0) return;
	struct queue_el* new_el;
	new_el = (struct queue_el*) malloc(sizeof(struct queue_el));
	new_el->command = (char*) malloc(HISTORY_BUF_SIZE * sizeof(char));
	strcpy(new_el->command, word);
	new_el->next = NULL;

	history_size++;
	current_size++;

	if (back == NULL) { // First element to be added in queue.
		front = back = new_el;
		return;
	}

	// Add at the end and set rear to the new element.
	back->next = new_el;
	back = new_el;

	if (history_size > HISTORY_STORE_MAX) dequeue();
}

void print_queue(int print_size) {
	if (front == NULL) return;
	struct queue_el* iter = front;
	int index = 0;

	if (print_size > HISTORY_READ_MAX) print_size = HISTORY_READ_MAX;

	struct queue_el** print_arr;
	print_arr = (struct queue_el**) malloc(HISTORY_STORE_MAX * sizeof(struct queue_el*));

	while(iter != NULL) {
		print_arr[index++] = iter;
		iter = iter->next;
	}

	int printed = 0;
	for(int i = index - 1; i >= 0 && printed < print_size; i--, printed++) {
		printf("%i %s\n", printed + 1, print_arr[i]->command);
	}
}

void write_history(char* path) {
	if (front == NULL) return;
	FILE* his_file;

	if ((his_file = fopen(path, "w")) == NULL) {
		perror(COL(ERR_COL) "AGSH shell error (writehistory)" COL_RES);
		return;
	}

	struct queue_el* iter = front;

	while(iter != NULL) {
		fprintf(his_file, "%s\n", iter->command);
		iter = iter->next;
	}

	fclose(his_file);
}

void read_history(char* path) {
	while (front != NULL) dequeue();

	FILE* his_file;
	char* buf;
	char* cpy;
	buf = (char*) malloc(HISTORY_BUF_SIZE * sizeof(char));
	cpy = (char*) malloc(HISTORY_BUF_SIZE * sizeof(char));

	if ((his_file = fopen(path, "r")) == NULL) {
		perror(COL(ERR_COL) "AGSH shell error (readhistory)" COL_RES);
		return;
	}

	while(fgets(buf, HISTORY_BUF_SIZE, his_file) != NULL) {
		sscanf(buf, "%s\n", buf);
		buf[strlen(buf)] = '\0';
		enqueue(buf);
	}

	fclose(his_file);
}
