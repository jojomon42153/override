#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int         get_unum()
{
    int     number = 0;

    fflush(stdout);
    scanf("%u", &number);
    return(number);
}

int         read_number(int *array)
{
    u_int   index = 0;

    printf(" Index: ");
    index = get_unum();
    printf(" Number at data[%u] is %u\n", index, array[index * 4]);
    return(0);
}

int         store_number(int *array)
{
    int     number = 0;
    int     index = 0;

    printf(" Number: ");
    number = get_unum();
    printf(" Index: ");
    index = get_unum();
    if (index % 3 != 0 || number >> 24 == 183)
    {
        puts(" *** ERROR! ***");
        puts("   This index is reserved for wil!")
        puts(" *** ERROR! ***");
        return(1);
    }
    array[index] = number
    return(0);
}

int         main(int argc, char **argv)
{
    char    *argv_ptr = argv[0];
    char    *argv_1 = argv[1];
    int     error = 0;
    int     array[100];
    char    command[20];

    memset(array, 0, 100);
    bzero(command, 24);
    // Les 2 while suivants font un bzero sur les arguments.
    // On ne peut donc pas mettre "/bin/sh" dedans
    while (*argv_ptr != 0)
    {
        memset(argv_ptr, 0, strlen(argv_ptr));
        argv_ptr++;
    }
    while (*argv_1 != 0)
    {
        memset(argv_1, 0, strlen(argv_1);
        argv_1++;
    }

    printf(
        "----------------------------------------------------\n"
        " Welcome to wil\'s crappy number storage service!   \n"
        "----------------------------------------------------\n"
        " Commands:                                          \n"
        " store - store a number into the data storage    \n"
        " read  - read a number from the data storage     \n"
        " quit  - exit the program                        \n"
        "----------------------------------------------------\n"
        " wil has reserved some storage :>                 \n"
        "----------------------------------------------------\n"
    );

    while(1)
    {
        printf("Input command: ");
        fgets(command, 20, stdin);
        command[strlen(command)] = '\0';

        if strncmp(command, "store", 5)
            error = store_number(array);
        else if strncmp(command, "read", 4)
            error = read_number(array);
        else if strncmp(command, "quit", 4)
            break;
        if error != 0
            printf(" Failed to do %s command\n", command);
        else
            printf(" Completed %s command successfully\n", command);
        bzero(command, 24);
    }
    // check stack integrity
    return (0);
}