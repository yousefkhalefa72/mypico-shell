#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define COMMAND tokens[0]
#define E_LAST_CMD -2
#define E_NOK -1
#define E_OK 0

typedef signed int Std_Return;

Std_Return myecho(char *tokens[]);
Std_Return mypwd();
Std_Return mycd(char *tokens[]);
void sigintHandler(int sig_num);
Std_Return Get_Command_Line(char **commandline);
Std_Return printPrompt();
Std_Return parsing(char *command, char ***tokens);

int main()
{
    signal(SIGINT, sigintHandler); /*** stop (ctrl + c) ***/

    char *command_line = NULL;        /*** input command line ***/
    signed int command_line_size = 0; /*** recive from getline func ***/
    signed int tokens_num = 0;        /*** recive from parsing func ***/
    char **tokens = NULL;             /*** array of strings (tokens) ***/
    int pipefd[2];
    int num_of_local_vars = 1; /*** number of local vars ***/
    char **local_vars =
        (char **)malloc(2 * sizeof(char *)); /** array of local vars **/

    char child_flag = 0;           /** flag to kill child after end proc **/
    char parent_flag = 0;          /** flag to continue parent due to child do his job **/
    char local_variable_found = 0; /** flag to find local var **/

    local_vars[0] = strdup("joe");    // variable name
    local_vars[1] = strdup("412003"); // variable value

    while (1)
    {
        /**************** Get Command Line ****************/
        printPrompt();
        command_line_size = Get_Command_Line(&command_line);

        if (command_line_size == 0) /* empty line */
        {
            continue;
        }
        else if (command_line_size == E_NOK) /* can't allocate memory */
        {
            printf("Can't get your command\n");
            continue;
        }

        /*************** Parsing *****************/
        tokens_num = parsing(command_line, &tokens);
        if (tokens_num == E_NOK)
        {
            printf("Parsing failed\n");
            continue;
        }

        /*** Checking of $ for local var ***/
        for (int i = 0; tokens[i] != NULL; ++i)
        {
            /* if first char of tokens[i] = $ */
            if (strncmp(tokens[i], "$", 1) == 0)
            {
                char *temp = strdup(tokens[i]);
                temp++;
                for (int y = 1; y < num_of_local_vars + 1; ++y)
                {
                    /* if found var_name */
                    if (strcmp(temp, local_vars[(y * 2) - 2]) == 0)
                    {
                        /* replace tokens[i] (var_name) by his value */
                        free(tokens[i]);
                        tokens[i] = strdup(local_vars[(y * 2) - 1]);
                    }
                }
            }
        }

        /*************** REDIRECTION **********************/
        for (int i = 0; tokens[i] != NULL; ++i)
        {
            if (strcmp(tokens[i], "<") == 0) /* input redirection*/
            {
                char file_from_redirection = open(tokens[i + 1], O_RDONLY);
                if (-1 == file_from_redirection)
                {
                    printf(">>>> can't open file %s\n", tokens[i + 1]);
                    break;
                }
                else
                {
                    /** remove (<) & (file name) from tokens */
                    free(tokens[i]);
                    tokens[i] = NULL;
                    free(tokens[i + 1]);
                    tokens[i + 1] = NULL;

                    pid_t returned_pid = fork();
                    if (returned_pid > 0) /*** PARENT ***/
                    {
                        int wstatus;
                        wait(&wstatus);
                        parent_flag = 1;
                    }
                    else if (returned_pid == 0) /*** child ***/
                    {
                        /* make stdin -> file */
                        dup2(file_from_redirection, 0);
                        child_flag = 1;
                    }
                    break;
                }
            }
            else if (strcmp(tokens[i], ">") == 0) /* output redirection*/
            {
                char file_from_redirection =
                    open(tokens[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (-1 == file_from_redirection)
                {
                    printf(">>>> can't open file %s\n", tokens[i + 1]);
                    continue;
                }

                pid_t returned_pid = fork();
                if (returned_pid > 0) /*** PARENT ***/
                {
                    int wstatus;
                    wait(&wstatus);
                    parent_flag = 1;
                }
                else if (returned_pid == 0) /*** child ***/
                {
                    /* make stdout -> file */
                    dup2(file_from_redirection, 1);
                    child_flag = 1;

                    /* remove (>) & (file name) from tokents */
                    free(tokens[i]);
                    tokens[i] = NULL;
                    free(tokens[i + 1]);
                    tokens[i + 1] = NULL;
                }
                break;
            }
            else if (strcmp(tokens[i], "|") == 0) /* pipe redirection */
            {
                if (pipe(pipefd) == -1)
                {
                    printf("can't pipe\n");
                    return 0;
                }

                pid_t returned_pid = fork();
                if (returned_pid > 0) /* parent reads from pipe */
                {
                    int wstatus;
                    wait(&wstatus);
                    close(pipefd[1]); /* Close write end */

                    int y = 0;
                    i++;                                     // tokens after pipe (|)
                    for (y = 0; tokens[i] != NULL; ++i, ++y) /* remove (CMD1) */
                    {
                        free(tokens[y]);
                        tokens[y] = strdup(tokens[i]);
                    }
                    for (int free_i = y; tokens[free_i] != NULL;
                         ++free_i) /* rearange tokens */
                    {
                        free(tokens[free_i]);
                        tokens[free_i] = NULL;
                    }

                    pid_t returned_pid = fork();
                    if (returned_pid > 0) /*** PARENT ***/
                    {
                        int wstatus;
                        wait(&wstatus);
                        parent_flag = 1;
                    }
                    else if (returned_pid == 0) /*** child ***/
                    {
                        /* make stdin -> file */
                        dup2(pipefd[0], 0);
                        child_flag = 1;
                    }
                    close(pipefd[0]); /* close file after write in */
                    break;
                }
                else if (returned_pid == 0) /* child writes to pipe */
                {
                    close(pipefd[0]);              /* Close read end */
                    for (; tokens[i] != NULL; ++i) /* remove (|) & (CMD2) from tokens*/
                    {
                        free(tokens[i]);
                        tokens[i] = NULL;
                    }
                    dup2(pipefd[1], 1); /* write the output in write end */
                    child_flag = 1;
                }
                break;
            }
        }

        /****** if you are parent you will free and start
                again becouse your child do your job */
        if (parent_flag == 1 && child_flag == 0)
        {
            free(command_line);
            command_line = NULL;
            for (int i = 0; tokens[i] != NULL; ++i)
            {
                free(tokens[i]);
            }
            free(tokens);
            parent_flag = 0;
            continue;
        }

        /************** local variables *********/
        if (tokens[1] != NULL && strcmp(tokens[1], "=") == 0)
        {
            char variable_found = 0;
            for (int i = 1; i < num_of_local_vars + 1; ++i)
            {
                // if var_name already exist
                if (strcmp(tokens[0], local_vars[(i * 2) - 2]) == 0)
                {
                    /* updata tokens[2] (var_value)*/
                    free(local_vars[(i * 2) - 1]);
                    local_vars[(i * 2) - 1] = strdup(tokens[2]);
                    variable_found = 1;
                }
            }
            if (variable_found == 0)
            {
                num_of_local_vars++;
                local_vars = (char **)realloc(local_vars,
                                              (num_of_local_vars * 2) * sizeof(char *));

                local_vars[(num_of_local_vars * 2) - 2] =
                    strdup(tokens[0]); // variable name
                local_vars[(num_of_local_vars * 2) - 1] =
                    strdup(tokens[2]); // variable value
            }
            variable_found = 0;
        }

        /*************** empty command ***************/
        else if (COMMAND == NULL)
        {
            continue;
        }

        /*************** set **********************/
        else if (strcmp(COMMAND, "set") == 0)
        {
            for (int i = 1; i < num_of_local_vars + 1; ++i)
            {
                printf("%s = ", local_vars[(i * 2) - 2]); // variable name
                printf("%s\n", local_vars[(i * 2) - 1]);  // variable value
            }
        }

        /*************** myecho **********************/
        else if (strcmp(COMMAND, "myecho") == 0)
        {
            myecho(tokens);
        }

        /*************** mypwd **********************/
        else if (strcmp(COMMAND, "mypwd") == 0)
        {
            mypwd();
        }

        /*************** mycd **********************/
        else if (strcmp(COMMAND, "mycd") == 0)
        {
            mycd(tokens);
        }

        /*************** EXIT **********************/
        else if (strcmp(COMMAND, "exit") == 0)
        {
            free(command_line);
            command_line = NULL;
            for (int i = 0; tokens[i] != NULL; ++i)
            {
                free(tokens[i]);
            }
            free(tokens);

            printf(">Goodbye\n");
            return E_OK;
        }

        /*************** unknown command ***************/
        else
        {
            local_variable_found = 0;
            for (int i = 1; i < num_of_local_vars + 1; ++i)
            {
                if (strcmp(COMMAND, local_vars[(i * 2) - 2]) == 0)
                {
                    printf("%s = ", local_vars[(i * 2) - 2]); // variable name
                    printf("%s\n", local_vars[(i * 2) - 1]);  // variable value

                    local_variable_found = 1;
                    break;
                }
            }

            if (local_variable_found)
            {
                local_variable_found = 0;
            }
            else
            {
                // printf(">this command isn't exist in my shell\n");
                pid_t returned_pid = fork();
                if (returned_pid > 0) /** PARENT **/
                {
                    int wstatus;
                    wait(&wstatus);
                }
                else if (returned_pid == 0) /** CHILD **/
                {
                    execvp(COMMAND, tokens);
                    /***************** bad child *****************/
                    printf("bash-> couldn't find \"%s\"\n", COMMAND);
                    return E_OK;
                }
                else /** didn't Fork **/
                {
                    printf("busy to execute out of myshell\n");
                }
            }
        }

        /***************** Free Memory **********************/
        free(command_line);
        command_line = NULL;
        for (int i = 0; tokens[i] != NULL; ++i)
        {
            free(tokens[i]);
            tokens[i] = NULL;
        }
        free(tokens);

        /******************* thanks child u do ur job **************/
        if (child_flag == 1)
        {
            return 0;
        }
        if (child_flag == 2) /*child from pipe */
        {
            close(pipefd[1]); /* close file after write in */
            return 0;
        }
    }
}

void sigintHandler(int sig_num)
{
    signal(SIGINT, sigintHandler);
    printf("\n(ctrl+C) Cannot be kill \"$*#@!!\"\n");
    printf("\nJoe_shell->");
    fflush(stdout);
}
