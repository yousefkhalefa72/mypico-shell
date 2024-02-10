#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

#define COMMAND      tokens[0]
#define E_LAST_CMD  -2
#define E_NOK       -1
#define E_OK         0

typedef signed int Std_Return;

Std_Return myecho(char* tokens[]);
Std_Return mypwd();
Std_Return mycd(char* tokens[]);


void sigintHandler(int sig_num)
{
    signal(SIGINT, sigintHandler);
    printf("\n(ctrl+C) Cannot be kill \"$*#@!!\"\n");
    printf("\nJoe_shell->");
    fflush(stdout);
}

/**
 * @param commandline double ptr to your command line
 * @return Status of the function
 *          (int)   : successfully and return size of cmd line
 *          (E_NOK) : The function faild
 * @brief commandline need to free after finish
 */
Std_Return Get_Command_Line(char** commandline)
{
    char input_char=0;
    int command_line_size = 0;
    char* buf = (char *)malloc(sizeof(char));
    if(buf == NULL)
    {
        return E_NOK;
    }
    if(*commandline == NULL)
    {

    }
    else
    {
        command_line_size = sizeof(*commandline);
        char* temp = strdup(*commandline);
        free(*commandline);

        buf = (char *)realloc(buf,(2+command_line_size)*sizeof(char));
        if(buf == NULL)
        {
            return E_NOK;
        }
        buf = strdup(temp);
        free(temp);
    }

    /****************** Next Char ******************/
    while(1)
    {
        input_char = getchar();
        /*********************** End of line *******/
        if(input_char == '\n')
        {
            *commandline =(char *)malloc((command_line_size+1)*sizeof(char));
            if(*commandline == NULL)
            {
                return E_NOK;
            }

            *commandline = strdup(buf);
            commandline[command_line_size]=0;
            free(buf);
            return command_line_size;
        }
        /**************** handle "" *****************/
        else if(input_char == '\"')
        {
            buf = (char *)realloc(buf,(2+command_line_size)*sizeof(char));
            if(buf == NULL)
            {
                return E_NOK;
            }
            buf[command_line_size] = input_char;
            buf[command_line_size+1] = 0;
            command_line_size++;
            input_char = getchar();

            while(input_char != '"')
            {
                buf = (char *)realloc(buf,(2+command_line_size)*sizeof(char));
                if(buf == NULL)
                {
                    return E_NOK;
                }
                buf[command_line_size] = input_char;
                buf[command_line_size+1] = 0;
                command_line_size++;
                input_char = getchar();
            }
            buf = (char *)realloc(buf,(2+command_line_size)*sizeof(char));
            if(buf == NULL)
            {
                return E_NOK;
            }
            buf[command_line_size] = input_char;
            buf[command_line_size+1] = 0;
            command_line_size++;
        }
        /**************** handle '' *****************/
        else if(input_char == '\'')
        {
            buf = (char *)realloc(buf,(2+command_line_size)*sizeof(char));
            if(buf == NULL)
            {
                return E_NOK;
            }
            buf[command_line_size] = input_char;
            buf[command_line_size+1] = 0;
            command_line_size++;
            input_char = getchar();

            while(input_char != '\'')
            {
                buf = (char *)realloc(buf,(2+command_line_size)*sizeof(char));
                if(buf == NULL)
                {
                    return E_NOK;
                }
                buf[command_line_size] = input_char;
                buf[command_line_size+1] = 0;
                command_line_size++;
                input_char = getchar();
            }
            buf = (char *)realloc(buf,(2+command_line_size)*sizeof(char));
            if(buf == NULL)
            {
                return E_NOK;
            }
            buf[command_line_size] = input_char;
            buf[command_line_size+1] = 0;
            command_line_size++;
        }
        /*************** any char *******************/
        else
        {
            buf = (char *)realloc(buf,(2+command_line_size)*sizeof(char));
            if(buf == NULL)
            {
                return E_NOK;
            }
            buf[command_line_size] = input_char;
            buf[command_line_size+1] = 0;
            command_line_size++;
        }
    }
}

Std_Return printPrompt()
{
    printf("\nJoe_Shell->");
}

/**
 * @param command command line that will be tokens
 * @param tokens array of strins(tokens)
 * @return Status of the function
 *          (int)   : successfully and return num of tokens
 *          (E_NOK) : The function faild
 * @brief tokens[...,NULL] & tokens need to free after finish
 */
Std_Return parsing(char* command, char*** tokens)
{
	int token_num = 0;
	char** dynamicArray = (char**)malloc(sizeof(char *));
	if (dynamicArray == NULL)
	{
		return E_NOK;
	}
	while((*command != '\0'))
	{
		if(*command == '\'')
		{
			//take new token
			int i=0;
			char* buf = (char*)malloc(sizeof(char));
			if (buf == NULL)
			{
				return E_NOK;
			}
			buf[0]='\'';
			i++;
			command++;
			while(*command != '\'')
			{
				buf = (char*)realloc(buf,(2+i)*sizeof(char));
				if (buf == NULL)
				{
				    return E_NOK;
				}
				buf[i] = *command++;
				i++;
			}
			command++;
			buf[i]='\'';
			buf[i+1]='\0';
			if(token_num == 0)
			{
				dynamicArray[0]=strdup(buf);
			}
			else
			{
				dynamicArray = (char**)realloc(dynamicArray, (token_num+1)*sizeof(char*));
				if (dynamicArray == NULL)
				{
				    return E_NOK;
				}
				dynamicArray[token_num] =strdup(buf);
			}
			token_num++;
		}
		else if(*command == '\"')
		{
			//take new token
			int i=0;
			char* buf = (char*)malloc(sizeof(char));
			if (buf == NULL)
			{
				return E_NOK;
			}
			buf[0]='"';
			i++;
			command++;
			while(*command != '\"')
			{
				buf = (char*)realloc(buf,(2+i)*sizeof(char));
				if (buf == NULL)
				{
				    return E_NOK;
				}
				buf[i] = *command++;
				i++;
			}
			command++;
			buf[i]='\"';
			buf[i+1]='\0';
			if(token_num == 0)
			{
				dynamicArray[0]=strdup(buf);
			}
			else
			{
				dynamicArray = (char**)realloc(dynamicArray, (token_num+1)*sizeof(char*));
				if (dynamicArray == NULL)
				{
				    return E_NOK;
				}
				dynamicArray[token_num] =strdup(buf);
			}
			token_num++;
       		}
		else if(*command == ' ')
		{
		    command++;
		}
		else if(*command == '<')
		{	
			//take new token
			if(token_num == 0)
			{
				dynamicArray[0]=strdup("<");
			}
			else
			{
				dynamicArray = (char**)realloc(dynamicArray, (token_num+1)*sizeof(char*));
				if (dynamicArray == NULL)
				{
				    return E_NOK;
				}
				dynamicArray[token_num] =strdup("<");
			}
			token_num++;
			command++;
       		}
		else if(*command == '>')
		{	
			//take new token
			if(token_num == 0)
			{
				dynamicArray[0]=strdup(">");
			}
			else
			{
				dynamicArray = (char**)realloc(dynamicArray, (token_num+1)*sizeof(char*));
				if (dynamicArray == NULL)
				{
				    return E_NOK;
				}
				dynamicArray[token_num] =strdup(">");
			}
			token_num++;
			command++;
       		}
		else if(*command == '|')
		{	
			//take new token
			if(token_num == 0)
			{
				dynamicArray[0]=strdup("|");
			}
			else
			{
				dynamicArray = (char**)realloc(dynamicArray, (token_num+1)*sizeof(char*));
				if (dynamicArray == NULL)
				{
				    return E_NOK;
				}
				dynamicArray[token_num] =strdup("|");
			}
			token_num++;
			command++;
       		}
		else if(*command == '=')
		{	
			//take new token
			if(token_num == 0)
			{
				dynamicArray[0]=strdup("=");
			}
			else
			{
				dynamicArray = (char**)realloc(dynamicArray, (token_num+1)*sizeof(char*));
				if (dynamicArray == NULL)
				{
				    return E_NOK;
				}
				dynamicArray[token_num] =strdup("=");
			}
			token_num++;
			command++;
       		}
		else
		{
		    	//take new token
		    	int i=0;
		    	char buf[20];

		    	while((*command != ' ' ) && (*command != '\0' ) && (*command != '=' ))
		    	{
		        	buf[i] = *command++;
		        	i++;
		    	}
		    	buf[i]='\0';
		    	if(token_num == 0)
		    	{
		        	dynamicArray[0]=strdup(buf);
		    	}
		    	else
	    		{
	        		dynamicArray = (char**)realloc(dynamicArray, (token_num+1)*sizeof(char*));
	        		if (dynamicArray == NULL)
	        		{
	            			return E_NOK;
	        		}
	        		dynamicArray[token_num] =strdup(buf);
	    		}
	    		token_num++;
		}
    	}
    	/***************** END DYNAMIC ARR BY NULL ******************************/
    	dynamicArray = (char**)realloc(dynamicArray, (token_num+1)*sizeof(char*));
    	if (dynamicArray == NULL)
    	{
        	return E_NOK;
    	}
    	dynamicArray[token_num] =NULL;
    	/******************** copy in original arr ************************/
    	(*tokens) = (char**)malloc((token_num+1)*sizeof(char*)); // Allocate memory for tokens
    	if ((*tokens) == NULL)
    	{
        	return E_NOK;
    	}

    	for(int i=0; dynamicArray[i] != NULL ; ++i)
    	{
        	(*tokens)[i]=strdup(dynamicArray[i]);
    	}
    	(*tokens)[token_num] =NULL;
    	/*********************** free **********************/
    	for(int i=0; dynamicArray[i] != NULL; ++i)
    	{
        	free(dynamicArray[i]);
    	}
	free(dynamicArray);
	return token_num;
}

int main()
{
	signal(SIGINT, sigintHandler);
    	
    	char* command_line = NULL;
    	signed int command_line_size = 0;
    	signed int tokens_num = 0;
    	char** tokens = NULL;
    	
    	char child_flag=0;
    	char parent_flag=0;
    	int pipefd[2];
    	
    	int num_of_local_vars = 1;
    	char local_variable_found = 0;
    	char** local_vars = (char**)malloc(2*sizeof(char*));
    	local_vars[0] = strdup("joe");   //variable name
    	local_vars[1] = strdup("412003");//variable value
    	
	while(1)
    	{
		/**************** Get Command Line ****************/
		printPrompt();
		command_line_size = Get_Command_Line(&command_line);
		/* empty line */
		if(command_line_size == 0)
		{
		    continue;
		}
		/* can't allocate memory */
		else if(command_line_size == E_NOK)
		{
		    printf("Can't get your command\n");
		    continue;
		}

		/*************** parsing ***************************/
		tokens_num = parsing(command_line, &tokens);
		
		if (tokens_num == E_NOK)
		{
		    printf("Parsing failed\n");
		    continue;
		}
		
		for(int i=0; tokens[i]!=NULL; ++i)
		{
			/* if first char of tokens[i] = $ */
			if(strncmp(tokens[i],"$",1)==0)
			{
				char* temp = strdup(tokens[i]);
				temp++;
		       		for(int y=1; y<num_of_local_vars+1;++y)
		       		{
		       			/* if found var_name */
		       			if(strcmp(temp,local_vars[(y*2)-2]) == 0)
		       			{
		       				/* replace tokens[i] (var_name) by his value */
						free(tokens[i]);
						tokens[i]=(char *)malloc(sizeof(local_vars[(y*2)-2]+1));
						tokens[i]=strdup(local_vars[(y*2)-1]);
	       				}
		       		}
			}
		}
		/*************** REDIRECTION **********************/
		for(int i =0; tokens[i] != NULL;++i)
		{
			if(strcmp(tokens[i], "<") == 0)
			{
				char file_from_redirection=open(tokens[i+1],O_RDONLY);
				if(-1 == file_from_redirection)
				{
					printf(">>>> can't open file %s\n",tokens[i+1]);
					break;
				}
				else
				{
					/********* replace tokens[i] (<) by what in file(tokens[i+1]) *****/
					free(tokens[i]);
					tokens[i]=(char *)malloc(sizeof(char));
					tokens[i]=strdup("\0");
					
					unsigned char buf[20];
					int num_read =0;
					int read_times = 0;
					
					while((num_read = read(file_from_redirection, buf, 20)) != 0)
					{
						read_times++;
						//check reading file
						if(num_read < 0)
						{
							printf(">>>> can't read file %s\n",tokens[i+1]);
							for(int i=0; tokens[i] != NULL; ++i)
							{
							    free(tokens[i]);
							}
							tokens[0] = NULL;
							break;
						}
						tokens[i]=(char *)realloc(tokens[i],(sizeof(char)*20*read_times));
						strcat(tokens[i],buf);
					}
					/******************* remove file name (tokens[i+1]) ******************/
					free(tokens[i+1]);
					tokens[i+1]=NULL;
					break;
				}
			}
			else if(strcmp(tokens[i], ">") == 0)
			{
				char file_from_redirection=open(tokens[i+1],O_WRONLY|O_CREAT|O_TRUNC, 0644);
				if(-1 == file_from_redirection)
				{
					printf(">>>> can't open file %s\n",tokens[i+1]);
					continue;
				}
				pid_t returned_pid = fork();
				/********************* PARENT ******************************************/
				if (returned_pid > 0)
				{
					int wstatus;
					wait(&wstatus);
					parent_flag = 1;
				}
				/****************** child *******************************************/
				else if (returned_pid == 0)
				{
					/* make stdout -> file */
					dup2(file_from_redirection,1);
					child_flag = 1;
					
					/* remove tokens[i] (>) and tokens[i+1] (file name) */
					free(tokens[i]);
					tokens[i]=NULL;
					free(tokens[i+1]);
					tokens[i+1]=NULL;
				}
				/*************** can't give child ***********************************/
				else
				{
					printf("I could not redirection\n");
				}
				break;
			}
			else if(strcmp(tokens[i], "|") == 0)
			{
				if (pipe(pipefd) == -1) 
				{
					printf("can't pipe\n");
					return 0;
				}

				pid_t returned_pid = fork();
				if (returned_pid > 0)
				{
					/* parent reads from pipe */
					int wstatus;
					wait(&wstatus);
					close(pipefd[1]);/* Close write end */
					
					++i; //tokens after pipe (|)
					int y=0;
					for(y=0;tokens[i] != NULL; ++i,++y)
					{
						free(tokens[y]);
						tokens[y]=strdup(tokens[i]);
					}
					
					for(int h=y;tokens[h] != NULL; ++h)
					{
						free(tokens[h]);
						tokens[h]=NULL;
					}
					tokens[y]=(char *)malloc(sizeof(char)*2);
					tokens[y]=strdup("\0");

					unsigned char buf[21];
					int num_read =0;
					int read_times = 0;
					while((num_read = read(pipefd[0], buf, 20)) != 0)
					{
						buf[21]=0;
						read_times++;
						tokens[y]=(char *)realloc(tokens[y],(sizeof(char)*20*read_times)+1);
						strncat(tokens[y],buf,num_read);
					}
					tokens[y][strlen(tokens[y])-1]=0;
					close(pipefd[0]);/* close file after write in */
					break;
				}
				else if (returned_pid == 0)
				{
					/* child writes to pipe */
					close(pipefd[0]); /* Close read end */
					for(;tokens[i] != NULL ;++i)
					{
						free(tokens[i]);
						tokens[i] = NULL;
					}
					dup2(pipefd[1],1);
					child_flag=2;		
				}
				else
				{
					printf("ERROR: I could not get a child\n");
				}
				break;
			}
		}
			
		/****** if you are parent you will free and start again 
			becouse your child do your job */
		if(parent_flag == 1 && child_flag == 0)
		{
			free(command_line);
			command_line = NULL;
			for(int i=0; tokens[i] != NULL; ++i)
			{
			    free(tokens[i]);
			}
			free(tokens);
			parent_flag=0;
			continue;
		}

		/************** local variables *********/
		if(tokens[1] != NULL && strcmp(tokens[1],"=") == 0)
		{
			char variable_found = 0;
			for(int i=1; i<num_of_local_vars+1;++i)
	       		{
	       			// if var_name already exist
       				if(strcmp(tokens[0],local_vars[(i*2)-2]) == 0)
				{
					/* updata tokens[2] (var_value)*/
					free(local_vars[(i*2)-1]);
					local_vars[(i*2)-1]=strdup(tokens[2]);
					variable_found = 1;
				}
	       		}
	       		if(variable_found == 0)
	       		{
				num_of_local_vars++;
				local_vars = (char**)realloc(local_vars,(num_of_local_vars*2)*sizeof(char*));
				local_vars[(num_of_local_vars*2)-2] = strdup(tokens[0]);   //variable name
				local_vars[(num_of_local_vars*2)-1] = strdup(tokens[2]);//variable value
			}
			variable_found = 0;
		}
		
		/*************** empty command ***************/
		else if(COMMAND == NULL)
		{
			//nothing
		}
		
		/*************** set **********************/
		else if(strcmp(COMMAND,"set") == 0)
		{
			for(int i=1; i<num_of_local_vars+1;++i)
	       		{
       				printf("%s = ",local_vars[(i*2)-2]);   //variable name
				printf("%s\n",local_vars[(i*2)-1]);     //variable value
	       		}
		}
		
		/*************** myecho **********************/
		else if(strcmp(COMMAND,"myecho") == 0)
		{
			myecho(tokens);
		}

		/*************** mypwd **********************/
	       	else if(strcmp(COMMAND,"mypwd") == 0)
	       	{
	       		mypwd();
	       	}

		/*************** mycd **********************/
	       	else if(strcmp(COMMAND,"mycd") == 0)
	       	{
	       	    	mycd(tokens);
	       	}

		/*************** EXIT **********************/
	       	else if(strcmp(COMMAND,"exit") == 0)
	       	{
			free(command_line);
			command_line = NULL;
			for(int i=0; tokens[i] != NULL; ++i)
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
	       		for(int i=1; i<num_of_local_vars+1;++i)
	       		{
	       			if(strcmp(COMMAND,local_vars[(i*2)-2]) == 0)
	       			{
	       				
	       				printf("%s = ",local_vars[(i*2)-2]);   //variable name
					printf("%s\n",local_vars[(i*2)-1]);     //variable value
					local_variable_found = 1;
					break;
       				}
	       		}
	       		if(local_variable_found)
	       		{
	       			local_variable_found = 0;
	       		}
	       		else
	       		{
	       			//printf(">this command isn't exist in my shell\n");
				pid_t returned_pid = fork();
				/********************* PARENT *********************/
				if (returned_pid > 0)
				{
					int wstatus;
					wait(&wstatus);
				}
				/*********************** CHILD *********************/
				else if (returned_pid == 0)
				{	
					execvp(COMMAND, tokens);
					/***************** bad child *****************/
					printf("bash-> couldn't find \"%s\"\n",COMMAND);
					return E_OK;
				}
				/******************** didn't Fork *******************/
				else
				{
					printf("busy to execute out of myshell\n");
				}	
	       		}
	       	}

		/***************** Free Memory **********************/
		free(command_line);
		command_line = NULL;
		for(int i=0; tokens[i] != NULL; ++i)
		{
		    	free(tokens[i]);
		    	tokens[i]=NULL;
		}
		free(tokens);
		
		/********** if you are child you will die here ******/
		if(child_flag == 1)
		{
			return 0;
		}
		if(child_flag == 2)
		{
			close(pipefd[1]);/* close file after write in */
			return 0;
		}
    	}
}

Std_Return myecho(char* tokens[])
{
	char counter = 1;
	printf(">");
	while(tokens[counter] != NULL)
	{
		printf("%s ",tokens[counter]);
		counter++;
	}
	printf("\n");
}

Std_Return mypwd()
{
 	char buf[500]; //buf for the directory path

	//handle getcwd error
	if(getcwd(buf,sizeof(buf)) == NULL)
       	{
		printf(">can't get current directory\n");
	}
	else
	{
        	printf(">Current directory : %s\n",buf);
	}
}

Std_Return mycd(char* tokens[])
{
        char cd_ret = chdir(tokens[1]);
	if(cd_ret != E_OK)
	{
		printf(">failed to chanage dir\n");
	}
}
