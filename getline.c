#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define E_LAST_CMD -2
#define E_NOK -1
#define E_OK 0

typedef signed int Std_Return;

/**
 * @param commandline double ptr to your command line
 * @return Status of the function
 *          (int)   : successfully and return size of cmd line
 *          (E_NOK) : The function faild
 * @brief commandline need to free after finish
 */
Std_Return Get_Command_Line(char **commandline)
{
    char input_char = 0;                      /* get char by char */
    int command_line_size = 0;                /* for return */
    char *buf = (char *)malloc(sizeof(char)); /*to save command line */
    if (buf == NULL)
    {
        return E_NOK;
    }

    if (*commandline == NULL)
    {
        /* Get new Command */
    }
    else /* continue last command */
    {
        command_line_size = sizeof(*commandline);
        char *temp = strdup(*commandline);
        free(*commandline);
        buf = (char *)realloc(buf, (2 + command_line_size) * sizeof(char));
        if (buf == NULL)
        {
            return E_NOK;
        }
        buf = strdup(temp);
        free(temp);
    }

    /****************** Next Char ******************/
    while (1)
    {
        input_char = getchar(); /* get char from user */

        if (input_char == '\n') /* End of line */
        {
            *commandline = (char *)malloc((command_line_size + 1) * sizeof(char));
            if (*commandline == NULL)
            {
                return E_NOK;
            }

            *commandline = strdup(buf);         /* return command line */
            commandline[command_line_size] = 0; /* end command line by '\0' */

            free(buf);
            return command_line_size;
        }
        else if (input_char == '\"') /* handle " */
        {
            buf = (char *)realloc(buf, (2 + command_line_size) * sizeof(char));
            if (buf == NULL)
            {
                return E_NOK;
            }

            buf[command_line_size] = input_char; /* save " */
            buf[command_line_size + 1] = 0;
            command_line_size++;

            input_char = getchar();
            while (input_char != '"')
            {
                buf = (char *)realloc(buf, (2 + command_line_size) * sizeof(char));
                if (buf == NULL)
                {
                    return E_NOK;
                }
                buf[command_line_size] = input_char;
                buf[command_line_size + 1] = 0;
                command_line_size++;
                input_char = getchar();
            }
            buf = (char *)realloc(buf, (2 + command_line_size) * sizeof(char));
            if (buf == NULL)
            {
                return E_NOK;
            }

            buf[command_line_size] = input_char; /* save " */
            buf[command_line_size + 1] = 0;
            command_line_size++;
        }
        else if (input_char == '\'') /* handle ' */
        {
            buf = (char *)realloc(buf, (2 + command_line_size) * sizeof(char));
            if (buf == NULL)
            {
                return E_NOK;
            }

            buf[command_line_size] = input_char; /* save ' */
            buf[command_line_size + 1] = 0;
            command_line_size++;

            input_char = getchar();
            while (input_char != '\'')
            {
                buf = (char *)realloc(buf, (2 + command_line_size) * sizeof(char));
                if (buf == NULL)
                {
                    return E_NOK;
                }

                buf[command_line_size] = input_char;
                buf[command_line_size + 1] = 0;
                command_line_size++;
                input_char = getchar();
            }

            buf = (char *)realloc(buf, (2 + command_line_size) * sizeof(char));
            if (buf == NULL)
            {
                return E_NOK;
            }

            buf[command_line_size] = input_char; /* save ' */
            buf[command_line_size + 1] = 0;
            command_line_size++;
        }
        else /* any char */
        {
            buf = (char *)realloc(buf, (2 + command_line_size) * sizeof(char));
            if (buf == NULL)
            {
                return E_NOK;
            }

            buf[command_line_size] = input_char;
            buf[command_line_size + 1] = 0;
            command_line_size++;
        }
    }
}
