#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define E_LAST_CMD -2
#define E_NOK -1
#define E_OK 0

typedef signed int Std_Return;

Std_Return myecho(char *tokens[])
{
    char counter = 1;
    printf(">");
    while (tokens[counter] != NULL)
    {
        printf("%s ", tokens[counter]);
        counter++;
    }
    printf("\n");
}

Std_Return mypwd()
{
    char buf[500]; // buf for the directory path
    if (getcwd(buf, sizeof(buf)) == NULL)
    {
        printf(">can't get current directory\n");
    }
    else
    {
        printf(">Current directory : %s\n", buf);
    }
}

Std_Return mycd(char *tokens[])
{
    char cd_ret = chdir(tokens[1]);
    if (cd_ret != E_OK)
    {
        printf(">failed to chanage dir\n");
    }
}

Std_Return printPrompt() { printf("\nJoe->"); }
