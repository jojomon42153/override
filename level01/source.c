#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char        a_user_name[100];

int         verify_user_pass(char *password)
{
    return strncmp(password, "admin", 5);
}

int         verify_user_name()
{
    puts("verifying username....\n");
    return strncmp(a_user_name, "dat_wil");
}

int         main(int argc, char **argv)
{
    char    *buffer[64];

    memset(buffer, 0, 16);
    buffer[64] = "\0";

    puts("********* ADMIN LOGIN PROMPT *********");
    printf("Enter Username: ");

    fgets(a_user_name, 256, stdin)
    if (verify_user_name() != 0)
    {
        puts("nope, incorrect username...\n");
        return (1);
    }
    puts("Enter Password: ");
    fgets(buffer, 100, stdin)
    verify_user_pass(buffer)

}