#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "tasc.h"

/**
 * @brief task manager app using c
 * @author Obaydur Rahman (213902018) <obayed.opu@gmail.com>
 * @date 2022-02-19 (21:56)
 *
 * @returns int
 */

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        echo("No arguments passed.\n");
        echo("Current binary location: \n");
        echo(argv[0]);

        print_help();

        return 0;
    }

    char *action = argv[1]; // action to perform

    // generate date
    char date[STR_SIZE];
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(date, sizeof(date) - 1, "%Y-%m-%d", t);

    if (strcmp(action, "add") == 0) // add task
    {
        task new_task;                              // new task
        new_task.id = generate_task_id();           // generate id incrementally
        scan("Task: ", new_task.task);              // scan task
        scan("Project: ", new_task.project);        // scan project
        scan("Repeat [yes,no]: ", new_task.repeat); // scan repeat
        if (strcmp(new_task.repeat, "yes") == 0)    // if task is repeating
        {
            scan("Repeat every [day, week, month]: ", new_task.repeat_every); // reapeat frequency
            strcpy(new_task.due_date, " ");
        }
        else
        {
            // set due date
            scan("Due date [yyyy-mm-dd]: ", new_task.due_date); // if not repeating, set due date
            strcpy(new_task.repeat_every, " ");
        }

        // we don't know the date yet
        strcpy(new_task.complete_date, " ");

        strcpy(new_task.add_date, date);

        add_task(new_task);
        echo("Task added successfully.\n");
    }
    else if (strcmp(action, "list") == 0) // list tasks
    {
        if (argc == 3)
        {
            search_task(argv[2], 1);
        }
        else
        {
            get_task_list();
        }
    }
    else if (strcmp(action, "search") == 0) // search tasks
    {
        if (argc == 3)
        {
            search_task(argv[2], 0);
        }
        else
        {
            echo("Please enter a search string.\n");
            print_help();
        }
    }
    else if (strcmp(action, "delete") == 0) // delete task
    {
        if (argc == 3)
        {
            delete_task(argv[2]);
        }
        else
        {
            echo("Please specify the task id.\n");
            print_help();
        }
    }
    else if (strcmp(action, "done") == 0) // mark task as complete
    {
        if (argc == 3)
        {
            done_task(argv[2], date);
        }
        else
        {
            echo("Please specify the task id.\n");
            print_help();
        }
    }
    else if (strcmp(action, "undone") == 0) // mark task as incomplete
    {
        if (argc == 3)
        {
            undone_task(argv[2]);
        }
        else
        {
            echo("Please specify the task id.\n");
            print_help();
        }
    }
    else if (strcmp(action, "due") == 0) // list tasks due on a date
    {
        if (argc == 3)
        {
            list_due_tasks(argv[2]);
        }
        else
        {
            list_due_tasks(date);
        }
    }
    else if (strcmp(action, "help") == 0) // show help
    {
        print_help();
    }
    else
    {
        echo("Invalid action.\n");
        print_help();
    }

    return 0;
}

void echo(char *s)
{
    printf("%s\n", s);
}

int generate_task_id()
{
    // get last id from file (create if not exist)
    FILE *fp = fopen("task_id.txt", "r+");
    int id;
    if (fp == NULL)
    {
        fp = fopen("task_id.txt", "w+");
        fprintf(fp, "0");
        fclose(fp);
        fp = fopen("task_id.txt", "r+");
    }
    fscanf(fp, "%d", &id);
    fseek(fp, 0, SEEK_SET);
    fprintf(fp, "%d", id + 1);
    fclose(fp);
    return id + 1;
}

task parse_task(char *line)
{
    char *token = strtok(line, "|");
    task t;
    int i = 0;

    while (token != NULL)
    {
        switch (i)
        {
        case 0:
            t.id = atoi(token);
            break;
        case 1:
            strcpy(t.task, token);
            break;
        case 2:
            strcpy(t.project, token);
            break;
        case 3:
            strcpy(t.repeat, token);
            break;
        case 4:
            strcpy(t.repeat_every, token);
            break;
        case 5:
            strcpy(t.due_date, token);
            break;
        case 6:
            strcpy(t.add_date, token);
            break;
        case 7:
            strcpy(t.complete_date, token);
            break;
        }
        token = strtok(NULL, "|");
        i++;
    }

    return t;
}

void print_task(task t)
{
    printf("ID: %d\n", t.id);
    printf("Task: %s\n", t.task);
    printf("Project: %s\n", t.project);
    printf("Repeat: %s\n", t.repeat);
    if (strcmp(t.repeat, "yes") == 0)
    {
        printf("Repeat every: %s\n", t.repeat_every);
    }
    else
    {
        printf("Due date: %s\n", t.due_date);
    }
    if (strcmp(t.add_date, "") != 0)
    {
        printf("Add date: %s\n", t.add_date);
    }
    // if complete_date is not empty, print it
    if (strlen(t.complete_date) > 5)
    {
        printf("Complete date: %s\n", t.complete_date);
    }
}

int scan(char *str, char *to)
{
    // clear the string
    to[0] = '\0';

    // while the user doesn't enter anything keep asking
    while (strlen(to) == 0)
    {
        printf("%s", str);
        fgets(to, STR_SIZE, stdin);
        to[strlen(to) - 1] = '\0';
    }
    return 0;
}

int add_task(task t)
{
    FILE *fp;
    fp = fopen("tasks.txt", "a");
    if (fp == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }
    // concatenate all the strings including id
    char *str = (char *)malloc(STR_SIZE * sizeof(char) * 8);
    char id_str[STR_SIZE];
    sprintf(id_str, "%d", t.id);
    strcpy(str, id_str);
    strcat(str, "|");
    strcat(str, t.task);
    strcat(str, "|");
    strcat(str, t.project);
    strcat(str, "|");
    strcat(str, t.repeat);
    strcat(str, "|");
    strcat(str, t.repeat_every);
    strcat(str, "|");
    strcat(str, t.due_date);
    strcat(str, "|");
    strcat(str, t.add_date);
    strcat(str, "|");
    strcat(str, t.complete_date);
    strcat(str, "\n");
    fputs(str, fp);
    fclose(fp);
    return 0;
}

void get_task_list()
{
    FILE *fp;
    char buffer[STR_SIZE];

    fp = fopen("tasks.txt", "r");

    while (fgets(buffer, STR_SIZE, fp))
    {
        // read each line
        char *line = strdup(buffer);
        task t = parse_task(line);

        print_task(t);
        printf("\n");
    }

    fclose(fp);
}

void search_task(char *search_str, int type)
{
    // type 1: search only project
    FILE *fp;
    char buffer[STR_SIZE];
    int found = 0;

    fp = fopen("tasks.txt", "r");

    while (fgets(buffer, STR_SIZE, fp))
    {
        // read each line
        char *line = strdup(buffer);
        task t;
        t = parse_task(line);
        if (type == 1)
        {
            if (strcmp(t.project, search_str) == 0)
            {
                print_task(t);
                printf("\n");
                found = 1;
            }
        }
        else
        {
            if (strstr(t.task, search_str) != NULL)
            {
                print_task(t);
                found++;
            }
            else if (strstr(t.project, search_str) != NULL)
            {
                print_task(t);
                found++;
            }
            else if (t.id == atoi(search_str))
            {
                print_task(t);
                found++;
            }
        }
    }

    if (found == 0)
    {
        printf("No results found!\n");
    }

    fclose(fp);
}

void print_help()
{
    echo("\nUsage: ");
    echo("tasc add \t\t: Add new task");
    echo("tasc list \t\t: List all tasks");
    echo("tasc list <project name>: List all tasks of a project");
    echo("tasc search <keyword> \t: Search for a task");
    echo("tasc due \t: List all tasks due today");
    echo("tasc due <date> \t: List all tasks due on a date (YYYY-MM-DD)");
    echo("tasc delete <task id> \t: Delete the task");
    echo("tasc done <task id> \t: Mark the task as complete");
    echo("tasc undone <task id> \t: Mark the task as incomplete");
    echo("tasc help \t\t: Show this help");
}

void delete_task(char *id)
{
    FILE *fp;
    FILE *temp;
    char buffer[STR_SIZE];
    int found = 0;

    fp = fopen("tasks.txt", "r");
    temp = fopen("temp.txt", "w");

    while (fgets(buffer, STR_SIZE, fp))
    {
        // read each line
        char *line = strdup(buffer);
        task t;
        t = parse_task(line);
        if (t.id == atoi(id))
        {
            found = 1;
        }
        else
        {
            fputs(buffer, temp);
        }
    }

    fclose(fp);
    fclose(temp);

    remove("tasks.txt");
    rename("temp.txt", "tasks.txt");

    if (found == 0)
    {
        printf("No task with id %s found!\n", id);
    }
    else
    {
        printf("Task with id %s deleted!\n", id);
    }
}

void done_task(char *id, char *date)
{
    FILE *fp;
    FILE *temp;
    char buffer[STR_SIZE];
    int found = 0;

    fp = fopen("tasks.txt", "r");
    temp = fopen("temp.txt", "w");

    while (fgets(buffer, STR_SIZE, fp))
    {
        // read each line
        char *line = strdup(buffer);
        task t;
        t = parse_task(line);
        if (t.id == atoi(id))
        {
            found = 1;
            if (strlen(t.complete_date) < 5)
            {
                strcpy(t.complete_date, date);
                // concatenate all the strings including id
                char *str = (char *)malloc(STR_SIZE * sizeof(char) * 8);
                char id_str[STR_SIZE];
                sprintf(id_str, "%d", t.id);
                strcpy(str, id_str);
                strcat(str, "|");
                strcat(str, t.task);
                strcat(str, "|");
                strcat(str, t.project);
                strcat(str, "|");
                strcat(str, t.repeat);
                strcat(str, "|");
                strcat(str, t.repeat_every);
                strcat(str, "|");
                strcat(str, t.due_date);
                strcat(str, "|");
                strcat(str, t.add_date);
                strcat(str, "|");
                strcat(str, t.complete_date);
                strcat(str, "\n");
                fputs(str, temp);
            }
            else
            {
                echo("Task already marked as complete!");
            }
        }
        else
        {
            fputs(buffer, temp);
        }
    }

    fclose(fp);
    fclose(temp);

    remove("tasks.txt");
    rename("temp.txt", "tasks.txt");

    if (found == 0)
    {
        printf("No task with id %s found!\n", id);
    }
    else
    {
        printf("Task with id %s marked as complete!\n", id);
    }
}

void undone_task(char *id)
{
    FILE *fp;
    FILE *temp;
    char buffer[STR_SIZE];
    int found = 0;

    fp = fopen("tasks.txt", "r");
    temp = fopen("temp.txt", "w");

    while (fgets(buffer, STR_SIZE, fp))
    {
        // read each line
        char *line = strdup(buffer);
        task t;
        t = parse_task(line);
        if (t.id == atoi(id))
        {
            found = 1;
            if (strlen(t.complete_date) > 5)
            {
                strcpy(t.complete_date, "");
                // concatenate all the strings including id
                char *str = (char *)malloc(STR_SIZE * sizeof(char) * 8);
                char id_str[STR_SIZE];
                sprintf(id_str, "%d", t.id);
                strcpy(str, id_str);
                strcat(str, "|");
                strcat(str, t.task);
                strcat(str, "|");
                strcat(str, t.project);
                strcat(str, "|");
                strcat(str, t.repeat);
                strcat(str, "|");
                strcat(str, t.repeat_every);
                strcat(str, "|");
                strcat(str, t.due_date);
                strcat(str, "|");
                strcat(str, t.add_date);
                strcat(str, "|");
                strcat(str, t.complete_date);
                strcat(str, "\n");
                fputs(str, temp);
            }
        }
        else
        {
            fputs(buffer, temp);
        }
    }

    fclose(fp);
    fclose(temp);

    remove("tasks.txt");
    rename("temp.txt", "tasks.txt");

    if (found == 0)
    {
        printf("No task with id %s found!\n", id);
    }
    else
    {
        printf("Task with id %s marked as incomplete!\n", id);
    }
}

void list_due_tasks(char *date)
{
    char *str = (char *)malloc(STR_SIZE * sizeof(char) * 8);
    FILE *fp;
    fp = fopen("tasks.txt", "r");
    int found = 0;

    printf("Tasks due on %s:\n\n", date);

    while (fgets(str, STR_SIZE, fp))
    {
        task t;
        t = parse_task(str);
        if (strlen(t.due_date) > 5)
        {
            if (strcmp(t.due_date, date) == 0)
            {
                if (strlen(t.complete_date) < 5)
                {
                    print_task(t);
                    echo("\n");
                    found++;
                }
            }
        }
        else if (strlen(t.repeat_every) > 2)
        {
            if (t.repeat_every[0] == 'd')
            {
                print_task(t);
                echo("\n");
                found++;
            }
            else if (t.repeat_every[0] == 'w')
            {
                // TODO: implement weekly tasks
                // parse/split the date by '-' and add 7 days to the add date
                // and compare it to today's date
                // if it is equal, print the task and update the due date next week
                // else, do nothing
                // also if the date becomes greater than number of days in this month,
                // increment the month and set the date to remaining days in this month
                // if the month is greater than 12,
                // increment the year and set the month to remaining months in this year
            }
            else if (t.repeat_every[0] == 'm')
            {
                // TODO: implement monthly tasks
                // similar to weekly tasks
            }
        }
    }

    if (found == 0)
    {
        printf("No tasks due on %s!\n", date);
    }

    fclose(fp);
}