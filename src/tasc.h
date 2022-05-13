#define STR_SIZE 256 // size for strings
/**
 * @brief struct for task (Type definition)
 * @details This struct is used to store the information of a task.
 */
typedef struct task
{
    int id;
    char task[STR_SIZE];
    char project[STR_SIZE];
    char repeat[STR_SIZE];
    char repeat_every[STR_SIZE];
    char due_date[STR_SIZE];
    char add_date[STR_SIZE];
    char complete_date[STR_SIZE];
} task;

/**
 * @brief short way to print string
 *
 * @param s string to print
 */
void echo(char *s);

/**
 * @brief short way to scan string
 *
 * @param str print label
 * @param to store the scanned string here
 * @return int
 */
int scan(char *str, char *to);

/**
 * @brief add a task to the task list
 *
 * @param t task to add
 * @return int
 */
int add_task(task t);

/**
 * @brief print all the tasks in the task list
 *
 * @return int
 */
void get_task_list();

/**
 * @brief print the task
 *
 * @param t task
 */
void print_task(task t);

/**
 * @brief generate a new task id
 *
 * @return char*
 */
int generate_task_id();

/**
 * @brief search task by string
 *
 * @param str string to search
 * @param type type of search
 *
 * @return void
 */
void search_task(char *str, int type);

/**
 * @brief print help
 *
 * @return void
 */
void print_help();

/**
 * @brief delete task
 *
 * @param id task id
 *
 * @return void
 */
void delete_task(char *id);

/**
 * @brief mark task as complete
 *
 * @param id task id
 * @param date date
 *
 * @return void
 */
void done_task(char *id, char *date);

/**
 * @brief mark task as incomplete
 *
 * @param id task id
 *
 * @return void
 */
void undone_task(char *id);

/**
 * @brief due by date
 *
 * @param date date
 *
 * @return void
 */
void list_due_tasks(char *date);