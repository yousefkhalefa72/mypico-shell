# mypico-shell
in mypico shell it's asimple shell that can do

  1- get command line from user by
  
          a-Get_Command_Line function that takes (double ptr to your command line) as argument 
          and return on success (int) : size of cmd line, on fail return : -1 .
        
          b-it work by memory allocation (malloc & realloc) you can get any command line with
          any size you want and don't worry there are no memory leak or fixed size but you
          must free commandline after finish it.
        
          c-it can handle (") and (') if you need to save newline in your command line.

  2- parsing your command line by
  
          a-parsing function that takes (command line that will be tokens) and (address of array of
          strins(tokens)) as arguments and return on success (int) : num of tokens, on fail return : -1 .
        
          b-it work by memory allocation (malloc & realloc) you can get any num of tokens with
          any size you want and don't worry there are no memory leak or fixed size but you
          must free tokens and (tokens[0] -> tokens[i] == NULL) after finish it.
        
          c-it can handle (") and (') if you need to save newline in your command line, and (<) ,
          (>) ,(|) for redirection it put them in sperated token, and (=) for local variables 

  3- builtin_commands :-
  
        a- myecho takes (double ptr) your array of token as argument and it print arguments of command.
        
        b- mypwd it show you current dir.
        
        c- mycd takes (ptr -> dir) as argument and it change dir.
        
        d- printPrompt simple out to get enter your command easly.

  4- local vars :-
  
          a-it work by memory allocation (malloc & realloc) you can save any num of vars 
          with any size you want and don't worry there are no memory leak or fixed size. 
        
          b-you  can create local var by (var_name = var_value).
          
          c-you can display all vars by cmd (set) or display one var by his name (ex-> x).
          
  5- you can use redirection operations like (>),(<) and (|) 
  
      ex1-> CMD > file       (out redirection)
      
      ex2-> CMD < file       (in redirection)
      
      ex3-> CMD1 | CMD2      (pipe redirection)

  6- i stoped killed signal so if you want to exit of my shell enter command (exit)

  7- if you input non builtin command it will be exeute by bash by (execvp).

  8- remember that my shell will not die till you enter (exit) command.

  9-i made for you make file to easy compile.

  10- thanks for your time 
  
        

        
        
          
