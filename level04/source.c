#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

int         main(void)
{
    int 	child_pid = fork();
    char 	buffer[136];
    int 	ret = 0;
    int	    status = 0;

    memset(buffer, 0, 128);
    buffer[136] = '\0';

    if (child_pid == 0) {
        prctl(1, 1);
        ptrace(0, 0, 0, 0);
        puts("Give me some shellcode, k");
        gets(buffer);
        return(0);
    }
    while(ptrace(3, child_pid, 44, 0) != 11) // tant que l'enfant ne fait pas d'execv
    {
        wait(&status);
        if (status and 0x7f) // if status == WIFEXITED => signal de l'exit de l'enfant
        {
            puts("child is exiting...");
            return;
        }
    }
    puts("no exec() for you")
    kill(pid, 9)
    return(0);
}