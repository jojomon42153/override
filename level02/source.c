#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int         main(int argc, char **argv)
{
    FILE    *fd;
    int     nb_char_read;
    char    username[100];
    char    flag[44];
    char    password[100];

    memset(username, '\0', 96);
    memset(flag, '\0', 40);
    memset(password, '\0', 96);

    fd = fopen("/home/users/level03/.pass", "r");
    if (!fd)
    {
        fwrite("ERROR: failed to open password file\n", 1, 36, stderr);
        exit(1);
    }
    nb_char_read = fread(flag, 1, 41, fd);
    flag[strcspn(flag, "\n")] = '\0';
    if (nb_char_read != 41)
    {
        fwrite("ERROR: failed to read password file\n", 1, 36, stderr);
        fwrite("ERROR: failed to read password file\n", 1, 36, stderr);
        exit(1);
    }
    fclose(fd);
    puts("===== [ Secure Access System v1.0 ] =====");
    puts("/***************************************\\");
    puts("| You must login to access this system. |");
    puts("\\***************************************/");

    printf("--[ Username: ");
    fgets(username, 100, stdin);
    username[strcspn(username, "\n")] = '\0';

    printf("--[ Password: ");
    fgets(password, 100, stdin);
    password[strcspn(password, "\n")] = '\0';

    puts("*****************************************");

    if (!strncmp(flag, password))
    {
        printf(username);
        puts(" does not have access!");
        exit(1);
    }

    printf("Greetings, %s!\n", username);
    system("/bin/sh");
    return(0);
}