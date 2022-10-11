#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ptrace.h>
#include <string.h>
#include <stdint.h>

int         auth(char *login, u_int serial_number)
{
    int     len;
    int     mask = 0x88233b2b;
    u_int   hash = login[3] ^ 4919 + 6221293;
    int     i;

    login[strcspn(username, "\n")] = '\0';
    
    if (len = strnlen(username, 32) <= 5)
        return(1);

    if (ptrace(0, 0, 1, 0) == -1) {
        puts("\033[32m.---------------------------.");
        puts("\033[31m| !! TAMPERING DETECTED !!  |");
        puts("\033[32m'---------------------------'");
        return(1);
    }
    
    while(i < len)
    {
        if (login[i] <= 31)
            return(1);
        hash += 0x88233b2b - (((((login[i] ^ nb) * 0x88233b2b - 0x88233b2b) >> 1) + 0x88233b2b) >> 32) * 0x539;
    }
    
    return((int)(serial_number == hash));
}

int         main(int argc, char **argv)
{
    char    *argv_0 = argv[0];
    char    login[32];
    u_int   serial_number;

    puts("***********************************");
    puts("*\t\tlevel06\t\t  *");
    puts("***********************************");
    printf("-> Enter Login: ");
    fgets(login, 32, stdin);

    puts("***********************************");
    puts("***** NEW ACCOUNT DETECTED ********");
    puts("***********************************");
    printf("-> Enter Serial: ");
    scanf("%u", &serial_number);

    if (auth(login, serial_number) != 0)
        return(1);

    puts("Authenticated!");
    system("/bin/sh");
    return(1);
}