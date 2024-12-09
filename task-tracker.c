#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

typedef struct {
    int id;
    char task[1024];
    char status[20];
} Task;

void addTask(const char *task_desc) {
    FILE *fp;
    int last_id = 0;

    // Get last ID
    fp = fopen("task-tracker.txt", "r");
    if (fp != NULL) {
        Task task;
        char buffer[1024];
        while(fgets(buffer, sizeof(buffer), fp)) {
            sscanf(buffer, "%d %[^|]|%s", &task.id, task.task, task.status);
            if (task.id > last_id) {
                last_id = task.id;
            }
        }
        fclose(fp);
    }

    // Write new task
    fp = fopen("task-tracker.txt", "a");
    if (fp != NULL) {
        fprintf(fp, "%d %s|todo\n", last_id + 1, task_desc);
        fclose(fp);
        printf("Task added with ID: %d\n", last_id + 1);
    }
}

void removeTask(const char *id_str) {
    FILE *fp, *temp;
    Task task;
    char buffer[1024];
    int remove_id = atoi(id_str);

    fp = fopen("task-tracker.txt", "r");
    temp = fopen("temp.txt", "w");

    if (fp != NULL && temp != NULL) {
        while(fgets(buffer, sizeof(buffer), fp)) {
            sscanf(buffer, "%d %[^|]|%s", &task.id, task.task, task.status);
            if (task.id != remove_id) {
                fprintf(temp, "%d %s|%s\n", task.id, task.task, task.status);
            }
        }

        fclose(fp);
        fclose(temp);
        remove("task-tracker.txt");
        rename("temp.txt", "task-tracker.txt");
        printf("Task with ID %d removed\n", remove_id);
    }
}

void updateTask(const char *update_str) {
    FILE *fp, *temp;
    Task task;
    char buffer[1024];

    // Parse ID and new task from input string
    int update_id;
    char new_task[1024];
    sscanf(update_str, "%d %[^\n]", &update_id, new_task);

    fp = fopen("task-tracker.txt", "r");
    temp = fopen("temp.txt", "w");

    if (fp != NULL && temp != NULL) {
        while(fgets(buffer, sizeof(buffer), fp)) {
            sscanf(buffer, "%d %[^|]|%s", &task.id, task.task, task.status);
            if (task.id == update_id) {
                fprintf(temp, "%d %s|%s\n", task.id, new_task, task.status);
            } else {
                fprintf(temp, "%d %s|%s\n", task.id, task.task, task.status);
            }
        }

        fclose(fp);
        fclose(temp);
        remove("task-tracker.txt");
        rename("temp.txt", "task-tracker.txt");
        printf("Task with ID %d updated\n", update_id);
    }
}

void updateStatus(const char *id_str, const char *new_status) {
    FILE *fp, *temp;
    Task task;
    char buffer[1024];
    int update_id = atoi(id_str);

    fp = fopen("task-tracker.txt", "r");
    temp = fopen("temp.txt", "w");

    if (fp != NULL && temp != NULL) {
        while(fgets(buffer, sizeof(buffer), fp)) {
            sscanf(buffer, "%d %[^|]|%s", &task.id, task.task, task.status);
            if (task.id == update_id) {
                fprintf(temp, "%d %s|%s\n", task.id, task.task, new_status);
            } else {
                fprintf(temp, "%d %s|%s\n", task.id, task.task, task.status);
            }
        }

        fclose(fp);
        fclose(temp);
        remove("task-tracker.txt");
        rename("temp.txt", "task-tracker.txt");
        printf("Task status with ID %d updated to %s\n", update_id, new_status);
    }
}

void listTasks(const char *filter) {
    FILE *fp;
    Task task;
    char buffer[1024];
    int maxStatusLen = 0, maxTaskLen = 0, maxIdLen = 0;

    // First pass to find max lengths
    fp = fopen("task-tracker.txt", "r");
    if (fp != NULL) {
        while(fgets(buffer, sizeof(buffer), fp)) {
            sscanf(buffer, "%d %[^|]|%s", &task.id, task.task, task.status);
            if (filter == NULL || strcmp(filter, task.status) == 0) {
                int statusLen = strlen(task.status);
                int taskLen = strlen(task.task);
                int idLen = snprintf(NULL, 0, "%d", task.id);
                maxStatusLen = statusLen > maxStatusLen ? statusLen : maxStatusLen;
                maxTaskLen = taskLen > maxTaskLen ? taskLen : maxTaskLen;
                maxIdLen = idLen > maxIdLen ? idLen : maxIdLen;
            }
        }
        fclose(fp);
    }

    // Second pass to print with proper formatting
    fp = fopen("task-tracker.txt", "r");
    if (fp != NULL) {
        printf("%-*s  %-*s  %s\n", maxIdLen, "ID", maxStatusLen, "Status", "Task");
        for(int i = 0; i < maxIdLen; i++) printf("-");
        printf("  ");
        for(int i = 0; i < maxStatusLen; i++) printf("-");
        printf("  ");
        for(int i = 0; i < maxTaskLen; i++) printf("-");
        printf("\n");

        while(fgets(buffer, sizeof(buffer), fp)) {
            sscanf(buffer, "%d %[^|]|%s", &task.id, task.task, task.status);
            if (filter == NULL || strcmp(filter, task.status) == 0) {
                printf("%-*d  %-*s  %s\n", maxIdLen, task.id, maxStatusLen, task.status, task.task);
            }
        }
        fclose(fp);
    }
}

int main(int argc, char* argv[]) {
    const char *filename = "task-tracker.txt";
    if (argc < 2) {
            printf("Usage: %s <command> [arguments...]\n", argv[0]);
            printf("Commands:\n");
            printf("  add \"task description\"           Add a new task\n");
            printf("  remove <id>                        Remove a task\n");
            printf("  update <id> \"new task\"           Update a task\n");
            printf("  mark-in-progress <id>              Mark task as in-progress\n");
            printf("  mark-done <id>                     Mark task as done\n");
            printf("  mark-todo <id>                     Mark task as todo\n");
            printf("  list                               Show all tasks\n");
            printf("  list done                          Show done tasks\n");
            printf("  list todo                          Show todo tasks\n");
            printf("  list in-progress                   Show in-progress tasks\n");
            return 1;
        }
    const char *command = argv[1];

    if(strcmp(command, "add") == 0) {
        if (argc < 3) {
           printf("Error: 'add' requires task description\n");
           return 1;
        }
        addTask(argv[2]);
    } else if (strcmp(command, "remove") == 0) {
        if (argc < 3) {
            printf("Error: 'remove' requires task ID\n");
            return 1;
        }
        removeTask(argv[2]);
    } else if (strcmp(command, "update") == 0) {
        if (argc < 4) {
            printf("Error: 'update' requires ID and new task\n");
            return 1;
        }
        char update_str[1024];
        sprintf(update_str, "%s %s", argv[2], argv[3]);
        updateTask(update_str);
    } else if (strcmp(command, "mark-in-progress") == 0) {
        if (argc < 3) {
            printf("Error: 'mark-in-progress' requires task ID\n");
            return 1;
        }
        updateStatus(argv[2], "in-progress");
    } else if (strcmp(command, "mark-done") == 0) {
        if (argc < 3) {
            printf("Error: 'mark-done' requires task ID\n");
            return 1;
        }
        updateStatus(argv[2], "done");
    } else if (strcmp(command, "mark-todo") == 0) {
        if (argc < 3) {
            printf("Error: 'mark-todo' requires task ID\n");
            return 1;
        }
        updateStatus(argv[2], "todo");
    } else if (strcmp(command, "list") == 0) {
        if (argc > 2) {
            listTasks(argv[2]);
        } else {
            listTasks(NULL);
        }
    } else {
            printf("Error: Unknown command '%s'.\n", command);
            printf("Use '%s' without arguments for usage information.\n", argv[0]);
            return 1;
    }

    return 0;
}
