#include <stdio.h>
#include <stdlib.b>
#include <string.h>

int     main(void) {
    int   i = 0;
    char  input[100];

    fgets(input, 100, stdin);
    while (i < strlen(input)) {
        if (input[i] > 64 && input[i] <= 90) {
            input[i] = input[i] ^ 32;
        }
        i++;
    }

    printf(input);
    exit(0);
}