#include <stdio.h>
#include <stdlib.h>
#include <strings.h>


typedef struct s_message
{
    char    msg[140];
    char    user_name[40];
    int     len_msg;
}           message;


void        secret_backdoor(void)
{
    char    buffer[128];

    fgets(buffer, 128,  stdin);
    system(buffer);
    return;
}

void        set_msg(message *s_msg_ptr)
{
    char    buffer[1024];

    bzero(buffer, 1024);
    puts(">: Msg @Unix-Dude");
    printf(">>: ");
    fgets(buffer, 1024, stdin);
    // C'est ce strncpy qui va réécrire sur le premier octet de len_msg.
    strncpy(s_msg_ptr->msg, buffer, s_msg_ptr->len_msg);
    return;
}

void        set_username(message *s_msg_ptr)
{
    char    buffer[128];
    int     i = 0;

    bzero(buffer, 128);
    puts(">: Enter your username");
    printf(">>: ");
    fgets(buffer, 128, stdin);

    // Ici on va écrire jusqu'a l'index 40 dans le user_name
    // l'index 40, c'est le 41ème caractère.
    // On fait donc un buffer overflow de 1 qu'on va exploiter.
    // En effet, on va pouvoir écrire sur le premier octet de len_msg.
    while(i <= 40 && buffer[i] == 0)
    {
        s_msg_ptr->user_name[i] = buffer[i];
        i++;
    }
    printf(">: Welcome, %s", s_msg_ptr->user_name);
    return;
}

void        handle_msg(void)
{
    message s_msg;

    bzero(s_msg.user_name, 40);
    s_msg.len_msg = 140;
    set_username(&s_msg);
    set_message(&s_msg);
    puts(">: Msg sent!");
    return;
}

int         main(int argc, char**argv)
{
    puts(
        "--------------------------------------------\n"
        "|   ~Welcome to l33t-m$n ~    v1337        |\n"
        "--------------------------------------------"
    );
    handle_msg();
    return(0);
}