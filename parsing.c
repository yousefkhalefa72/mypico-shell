#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define E_LAST_CMD -2
#define E_NOK -1
#define E_OK 0

typedef signed int Std_Return;

/**
 * @param command command line that will be tokens
 * @param tokens array of strins(tokens)
 * @return Status of the function
 *          (int)   : successfully and return num of tokens
 *          (E_NOK) : The function faild
 * @brief tokens[...,NULL] & tokens need to free after finish
 */
Std_Return parsing(char *command, char ***tokens)
{
    int token_num = 0;
    char **dynamicArray = (char **)malloc(sizeof(char *));
    if (dynamicArray == NULL)
    {
        return E_NOK;
    }

    while ((*command != '\0')) /* parsing till '\0' */
    {
        if (*command == '\'') /* handle ' */
        {
            // take new token
            int i = 0;
            char *buf = (char *)malloc(sizeof(char));
            if (buf == NULL)
            {
                return E_NOK;
            }

            buf[0] = '\'';
            i++;
            command++;
            while (*command != '\'')
            {
                buf = (char *)realloc(buf, (2 + i) * sizeof(char));
                if (buf == NULL)
                {
                    return E_NOK;
                }

                buf[i] = *command++;
                i++;
            }
            command++;
            buf[i] = '\'';
            buf[i + 1] = '\0';

            if (token_num == 0)
            {
                dynamicArray[0] = strdup(buf);
            }
            else
            {
                dynamicArray =
                    (char **)realloc(dynamicArray, (token_num + 1) * sizeof(char *));
                if (dynamicArray == NULL)
                {
                    return E_NOK;
                }
                dynamicArray[token_num] = strdup(buf);
            }
            token_num++;
        }
        else if (*command == '\"') /* handle " */
        {
            // take new token
            int i = 0;
            char *buf = (char *)malloc(sizeof(char));
            if (buf == NULL)
            {
                return E_NOK;
            }
            buf[0] = '"';
            i++;

            command++;
            while (*command != '\"')
            {
                buf = (char *)realloc(buf, (2 + i) * sizeof(char));
                if (buf == NULL)
                {
                    return E_NOK;
                }
                buf[i] = *command++;
                i++;
            }
            command++;
            buf[i] = '\"';
            buf[i + 1] = '\0';
            if (token_num == 0)
            {
                dynamicArray[0] = strdup(buf);
            }
            else
            {
                dynamicArray =
                    (char **)realloc(dynamicArray, (token_num + 1) * sizeof(char *));
                if (dynamicArray == NULL)
                {
                    return E_NOK;
                }
                dynamicArray[token_num] = strdup(buf);
            }
            token_num++;
        }
        else if (*command == ' ') /* skip spaces */
        {
            command++;
        }
        else if (*command == '<') /* take (<) in one token*/
        {
            // take new token
            if (token_num == 0)
            {
                dynamicArray[0] = strdup("<");
            }
            else
            {
                dynamicArray =
                    (char **)realloc(dynamicArray, (token_num + 1) * sizeof(char *));
                if (dynamicArray == NULL)
                {
                    return E_NOK;
                }
                dynamicArray[token_num] = strdup("<");
            }
            token_num++;
            command++;
        }
        else if (*command == '>') /* take (>) in one token*/
        {
            // take new token
            if (token_num == 0)
            {
                dynamicArray[0] = strdup(">");
            }
            else
            {
                dynamicArray =
                    (char **)realloc(dynamicArray, (token_num + 1) * sizeof(char *));
                if (dynamicArray == NULL)
                {
                    return E_NOK;
                }
                dynamicArray[token_num] = strdup(">");
            }
            token_num++;
            command++;
        }
        else if (*command == '|') /* take (|) in one token*/
        {
            // take new token
            if (token_num == 0)
            {
                dynamicArray[0] = strdup("|");
            }
            else
            {
                dynamicArray =
                    (char **)realloc(dynamicArray, (token_num + 1) * sizeof(char *));
                if (dynamicArray == NULL)
                {
                    return E_NOK;
                }
                dynamicArray[token_num] = strdup("|");
            }
            token_num++;
            command++;
        }
        else if (*command == '=') /* take (=) in one token*/
        {
            // take new token
            if (token_num == 0)
            {
                dynamicArray[0] = strdup("=");
            }
            else
            {
                dynamicArray =
                    (char **)realloc(dynamicArray, (token_num + 1) * sizeof(char *));
                if (dynamicArray == NULL)
                {
                    return E_NOK;
                }
                dynamicArray[token_num] = strdup("=");
            }
            token_num++;
            command++;
        }
        else
        {
            // take new token
            int i = 0;
            char buf[20];
            while ((*command != ' ') && (*command != '\0') && (*command != '='))
            {
                buf[i] = *command++;
                i++;
            }
            buf[i] = '\0';
            if (token_num == 0)
            {
                dynamicArray[0] = strdup(buf);
            }
            else
            {
                dynamicArray =
                    (char **)realloc(dynamicArray, (token_num + 1) * sizeof(char *));
                if (dynamicArray == NULL)
                {
                    return E_NOK;
                }
                dynamicArray[token_num] = strdup(buf);
            }
            token_num++;
        }
    }

    /***************** END DYNAMIC ARR BY NULL ******************************/
    dynamicArray =
        (char **)realloc(dynamicArray, (token_num + 1) * sizeof(char *));
    if (dynamicArray == NULL)
    {
        return E_NOK;
    }
    dynamicArray[token_num] = NULL;

    /******************** copy in original arr ************************/
    (*tokens) = (char **)malloc((token_num + 1) *
                                sizeof(char *)); // Allocate memory for tokens
    if ((*tokens) == NULL)
    {
        return E_NOK;
    }
    for (int i = 0; dynamicArray[i] != NULL; ++i)
    {
        (*tokens)[i] = strdup(dynamicArray[i]);
    }
    (*tokens)[token_num] = NULL;

    /*********************** free **********************/
    for (int i = 0; dynamicArray[i] != NULL; ++i)
    {
        free(dynamicArray[i]);
    }
    free(dynamicArray);
    return token_num;
}
