#include <stdio.h>
#include <stdlib.h>

int     main(int argc, char **argv)
{
    int input;

    puts("***********************************");
    puts("* \t     -Level00 -\t\t  *");
    puts("***********************************");
    printf("Password:")

    scanf("%d", &input);
    if (input != 5276)
    {
        puts("\nInvalid Password!");
        return (1);
    }
    puts("\nAuthenticated!");
    system("/bin/sh");
    return (0);
}