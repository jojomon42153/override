#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

void         decrypt(int key)
{
    int     number1 = 1971092817;   // "Q}|u"
    int     number2 = 1734767456;   // "`sfg"
    int     number3 = 2070311806;   // "~sf{"
    int     number4 = 862026877;    // "}|a3"
    char    char0 = 0;              // '\0'
    // Ces 4 initialisations se suivent dans la mémoire.
    // Cela équivaut donc à déclarer la string: "Q}|u`sfg~sf{}|a3\0"
    int     len_str = strlen(&number1);
    int     number5 = -1;
    int     index = 0;

    while (index < len_str)
    {
        ((char *)&number1)[index] = ((char *)&number1)[index] ^ key;
        index++;
    }
    if (!strncmp(&number1, "Congratulations!", 17))
        puts("\nInvalid Password");
    else
        system("/bin/sh");
    // Avant le return, il y a une protection de la stack.
    return;
}

void        test(char *input, int number)
{
    u_int   diff = 322424845 - atoi(number);

    if (diff > 21){
        decrypt(rand())
        return
    }
    // Ici il se passe des bails chelou avec une addresse d'instruction à laquelle on rajoute diff * 4, qu'on va call, etc ...
    // Mais ce qu'il y a a retenir, c'est qu'un de ces return sera appelé.
    return(decrypt(diff));
    return(decrypt(diff));
    return(decrypt(diff));
    return(decrypt(diff));
    return(decrypt(diff));
    return(decrypt(diff));
    return(decrypt(diff));
    return(decrypt(diff));
    return(decrypt(diff));
    return(decrypt(diff));
    return(decrypt(diff));
    return(decrypt(diff));
    return(decrypt(diff));
    return(decrypt(diff));
    return(decrypt(diff));
}

int         main(int argc, char **argv)
{
    char    *input;

    time(0);
    srand(0);
    puts("***********************************");
    puts("*            level03             **");
    puts("***********************************");
    printf("Password:");
    scanf("%d", input);
    test(322424845, input);
    return(0);
}