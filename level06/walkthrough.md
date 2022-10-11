## 1: Analyse

### A: C'est quoi mon binaire?
* Bon vous commencez a comprendre mon workflow maintenant. Regardez les exercices precedents si vous comprenez pas.

```shell
  level05@OverRide:/tmp$ su level06
    Password: h4GtNnaMs2kZFN92ymTr2DcJHAzMfzLW25Ep59mq
    RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
    Partial RELRO   Canary found      NX enabled    No PIE          No RPATH   No RUNPATH   /home/users/level06/level06

  level06@OverRide:~$ ls -la
    total 17
    dr-xr-x---+ 1 level06 level06   80 Sep 13  2016 .
    dr-x--x--x  1 root    root     260 Oct  2  2016 ..
    -rw-r--r--  1 level06 level06  220 Sep 10  2016 .bash_logout
    lrwxrwxrwx  1 root    root       7 Sep 13  2016 .bash_profile -> .bashrc
    -rw-r--r--  1 level06 level06 3533 Sep 10  2016 .bashrc
    -rwsr-s---+ 1 level07 users   7907 Sep 10  2016 level06
    -rw-r--r--+ 1 level06 level06   41 Oct 19  2016 .pass
    -rw-r--r--  1 level06 level06  675 Sep 10  2016 .profile

  level06@OverRide:~$ ./level06
    ***********************************
    *		level06		  *
    ***********************************
    -> Enter Login: login
    ***********************************
    ***** NEW ACCOUNT DETECTED ********
    ***********************************
    -> Enter Serial: 123456789
  level06@OverRide:~$ ./level06
    ***********************************
    *		level06		  *
    ***********************************
    -> Enter Login: login
    ***********************************
    ***** NEW ACCOUNT DETECTED ********
    ***********************************
    -> Enter Serial: pf
  level06@OverRide:~$ ./level06
    ***********************************
    *		level06		  *
    ***********************************
    -> Enter Login: login trop lonnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnng
    ***********************************
    ***** NEW ACCOUNT DETECTED ********
    ***********************************
    -> Enter Serial: level06@OverRide:~$
```

  * On a un binaire appartenant a level07 dans le home avec les droits SUID...
  * ... qui demande un login et print NEW ACCOUNT DETECTED ...
  * ... qui demande un serial et quit
  * ... qui segfault pas avec un arg trop long

### B: On l'ouvre avec gdb

* Pareil pour ici regardez les exos precedents si vous comprenez pas ce que je fais

```shell
level06@OverRide:/tmp$ gdb ~/level06
  GNU gdb (Ubuntu/Linaro 7.4-2012.04-0ubuntu2.1) 7.4-2012.04
  Copyright (C) 2012 Free Software Foundation, Inc.
  License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
  This is free software: you are free to change and redistribute it.
  There is NO WARRANTY, to the extent permitted by law.  Type "show copying"
  and "show warranty" for details.
  This GDB was configured as "x86_64-linux-gnu".
  For bug reporting instructions, please see:
  <http://bugs.launchpad.net/gdb-linaro/>...
  Reading symbols from /home/users/level06/level06...(no debugging symbols found)...done.

gdb-peda$ info functions
  All defined functions:

  Non-debugging symbols:
  0x080484d0  _init
  0x08048510  printf
  0x08048510  printf@plt
  0x08048520  strcspn
  0x08048520  strcspn@plt
  0x08048530  fflush
  0x08048530  fflush@plt
  0x08048540  getchar
  0x08048540  getchar@plt
  0x08048550  fgets
  0x08048550  fgets@plt
  0x08048560  signal
  0x08048560  signal@plt
  0x08048570  alarm
  0x08048570  alarm@plt
  0x08048580  __stack_chk_fail
  0x08048580  __stack_chk_fail@plt
  0x08048590  puts
  0x08048590  puts@plt
  0x080485a0  system
  0x080485a0  system@plt
  0x080485b0  __gmon_start__
  0x080485b0  __gmon_start__@plt
  0x080485c0  __libc_start_main
  0x080485c0  __libc_start_main@plt
  0x080485d0  strnlen
  0x080485d0  strnlen@plt
  0x080485e0  __isoc99_scanf
  0x080485e0  __isoc99_scanf@plt
  0x080485f0  ptrace
  0x080485f0  ptrace@plt
  0x08048600  _start
  0x08048630  __do_global_dtors_aux
  0x08048690  frame_dummy
  0x080486b4  clear_stdin
  0x080486d7  get_unum
  0x0804870f  prog_timeout
  0x08048720  enable_timeout_cons
  0x08048748  auth
  0x08048879  main
  0x08048990  __libc_csu_init
  0x08048a00  __libc_csu_fini
  0x08048a02  __i686.get_pc_thunk.bx
  0x08048a10  __do_global_ctors_aux
  0x08048a3c  _fini

gdb-peda$ pdisas main
  Dump of assembler code for function main:
  # Initialisation, alignement et allocation de 80 octets
  0x08048879 <+0>:	push   ebp
  0x0804887a <+1>:	mov    ebp,esp
  0x0804887c <+3>:	and    esp,0xfffffff0
  0x0804887f <+6>:	sub    esp,0x50 
  
  # On stocke argv a 0x1c et la canary a 0x4c
  0x08048882 <+9>:	mov    eax,DWORD PTR [ebp+0xc] 
  0x08048885 <+12>:	mov    DWORD PTR [esp+0x1c],eax
  0x08048889 <+16>:	mov    eax,gs:0x1
  0x0804888f <+22>:	mov    DWORD PTR [esp+0x4c],eax

  # Obscures opérations menant a main+37
  0x08048893 <+26>:	xor    eax,eax
  0x08048895 <+28>:	push   eax
  0x08048896 <+29>:	xor    eax,eax
  0x08048898 <+31>:	je     0x804889d <main+36>
  0x0804889a <+33>:	add    esp,0x4
  0x0804889d <+36>:	pop    eax

  # puts * 3:
  # "***********************************"
  # "*\t\tlevel06\t\t  *"
  # "***********************************"
  0x0804889e <+37>:	mov    DWORD PTR [esp],0x8048ad4
  0x080488a5 <+44>:	call   0x8048590 <puts@plt>
  0x080488aa <+49>:	mov    DWORD PTR [esp],0x8048af8
  0x080488b1 <+56>:	call   0x8048590 <puts@plt>
  0x080488b6 <+61>:	mov    DWORD PTR [esp],0x8048ad4
  0x080488bd <+68>:	call   0x8048590 <puts@plt>

  # printf("-> Enter Login: ")
  0x080488c2 <+73>:	mov    eax,0x8048b08
  0x080488c7 <+78>:	mov    DWORD PTR [esp],eax
  0x080488ca <+81>:	call   0x8048510 <printf@plt>

  # fgets(login, 32, stdin)
  0x080488cf <+86>:	mov    eax,ds:0x804a060
  0x080488d4 <+91>:	mov    DWORD PTR [esp+0x8],eax
  0x080488d8 <+95>:	mov    DWORD PTR [esp+0x4],0x20
  0x080488e0 <+103>:	lea    eax,[esp+0x2c]
  0x080488e4 <+107>:	mov    DWORD PTR [esp],eax
  0x080488e7 <+110>:	call   0x8048550 <fgets@plt>

  # puts * 3:
  # "***********************************"
  # "***** NEW ACCOUNT DETECTED ********"
  # "***********************************"
  0x080488ec <+115>:	mov    DWORD PTR [esp],0x8048ad4
  0x080488f3 <+122>:	call   0x8048590 <puts@plt>
  0x080488f8 <+127>:	mov    DWORD PTR [esp],0x8048b1c
  0x080488ff <+134>:	call   0x8048590 <puts@plt>
  0x08048904 <+139>:	mov    DWORD PTR [esp],0x8048ad4
  0x0804890b <+146>:	call   0x8048590 <puts@plt>

  # printf("-> Enter Serial: ")
  0x08048910 <+151>:	mov    eax,0x8048b40
  0x08048915 <+156>:	mov    DWORD PTR [esp],eax
  0x08048918 <+159>:	call   0x8048510 <printf@plt>

  # scanf("%u", &serial)
  0x0804891d <+164>:	mov    eax,0x8048a60
  0x08048922 <+169>:	lea    edx,[esp+0x28]
  0x08048926 <+173>:	mov    DWORD PTR [esp+0x4],edx
  0x0804892a <+177>:	mov    DWORD PTR [esp],eax
  0x0804892d <+180>:	call   0x80485e0 <__isoc99_scanf@plt>

  # if (auth(login, serial)) != 0 on check que la stack aie pas changé et on return(1) (main+240)
  0x08048932 <+185>:	mov    eax,DWORD PTR [esp+0x28]
  0x08048936 <+189>:	mov    DWORD PTR [esp+0x4],eax
  0x0804893a <+193>:	lea    eax,[esp+0x2c]
  0x0804893e <+197>:	mov    DWORD PTR [esp],eax
  0x08048941 <+200>:	call   0x8048748 <auth>
  0x08048946 <+205>:	test   eax,eax
  0x08048948 <+207>:	jne    0x8048969 <main+240>

  # puts("Authenticated!")
  0x0804894a <+209>:	mov    DWORD PTR [esp],0x8048b52
  0x08048951 <+216>:	call   0x8048590 <puts@plt>

  # system("/bin/sh")
  0x08048956 <+221>:	mov    DWORD PTR [esp],0x8048b61
  0x0804895d <+228>:	call   0x80485a0 <system@plt>
  0x08048962 <+233>:	mov    eax,0x0
  0x08048967 <+238>:	jmp    0x804896e <main+245>

  # Check si la stack a été modifiée et return(1)
  0x08048969 <+240>:	mov    eax,0x1
  0x0804896e <+245>:	mov    edx,DWORD PTR [esp+0x4c]
  0x08048972 <+249>:	xor    edx,DWORD PTR gs:0x14
  0x08048979 <+256>:	je     0x8048980 <main+263>
  0x0804897b <+258>:	call   0x8048580 <__stack_chk_fail@plt>
  0x08048980 <+263>:	leave
  0x08048981 <+264>:	ret
```

* Commandes lancées pendant l'analyse du binaire ci-dessus
```shell
  gdb-peda$ x/s 0x8048ad4
    0x8048ad4:	 '*' <repeats 35 times>
  gdb-peda$ x/s 0x8048af8
    0x8048af8:	 "*\t\tlevel06\t\t  *"
  gdb-peda$ x/s 0x8048ad4
    0x8048ad4:	 '*' <repeats 35 times>
  gdb-peda$ x/s 0x804a060
    0x804a060 <stdin@@GLIBC_2.0>:	 ""
  gdb-peda$ x/s 0x8048b08
    0x8048b08:	 "-> Enter Login: "
  gdb-peda$ x/s 0x8048ad4
    0x8048ad4:	 '*' <repeats 35 times>
  gdb-peda$ x/s 0x8048b1c
    0x8048b1c:	 "***** NEW ACCOUNT DETECTED ********"
  gdb-peda$ x/s 0x8048ad4
    0x8048ad4:	 '*' <repeats 35 times>
  gdb-peda$ x/s 0x8048b40
    0x8048b40:	 "-> Enter Serial: "
  gdb-peda$ x/s 0x8048a60
    0x8048a60:	 "%u"
  gdb-peda$ x/s 0x8048b52
    0x8048b52:	 "Authenticated!"
  gdb-peda$ x/s 0x8048b61
    0x8048b61:	 "/bin/sh"
```

* On passe sur auth
```shell
  gdb-peda$ pdisas auth
    Dump of assembler code for function auth:
    # Initialisation, allocation de 40 octets
    0x08048748 <+0>:	push   ebp
    0x08048749 <+1>:	mov    ebp,esp
    0x0804874b <+3>:	sub    esp,0x28
    
    # index_nl = strcspn(login, "\n")
    0x0804874e <+6>:	mov    DWORD PTR [esp+0x4],0x8048a63
    0x08048756 <+14>:	mov    eax,DWORD PTR [ebp+0x8]
    0x08048759 <+17>:	mov    DWORD PTR [esp],eax
    0x0804875c <+20>:	call   0x8048520 <strcspn@plt>

    # login[index_nl] = '\0'
    0x08048761 <+25>:	add    eax,DWORD PTR [ebp+0x8]
    0x08048764 <+28>:	mov    BYTE PTR [eax],0x0
    
    # len = strnlen(login, 32)
    0x08048767 <+31>:	mov    DWORD PTR [esp+0x4],0x20
    0x0804876f <+39>:	mov    eax,DWORD PTR [ebp+0x8]
    0x08048772 <+42>:	mov    DWORD PTR [esp],eax
    0x08048775 <+45>:	call   0x80485d0 <strnlen@plt>
    0x0804877a <+50>:	mov    DWORD PTR [ebp-0xc],eax

    # obscures operations again
    0x0804877d <+53>:	push   eax
    0x0804877e <+54>:	xor    eax,eax
    0x08048780 <+56>:	je     0x8048785 <auth+61>
    0x08048782 <+58>:	add    esp,0x4
    0x08048785 <+61>:	pop    eax

    # if (len <= 5) return(1)
    0x08048786 <+62>:	cmp    DWORD PTR [ebp-0xc],0x5
    0x0804878a <+66>:	jg     0x8048796 <auth+78>
    0x0804878c <+68>:	mov    eax,0x1
    0x08048791 <+73>:	jmp    0x8048877 <auth+303>

    # if (ptrace(0, 0, 1, 0) == -1 ...
    0x08048796 <+78>:	mov    DWORD PTR [esp+0xc],0x0
    0x0804879e <+86>:	mov    DWORD PTR [esp+0x8],0x1
    0x080487a6 <+94>:	mov    DWORD PTR [esp+0x4],0x0
    0x080487ae <+102>:	mov    DWORD PTR [esp],0x0
    0x080487b5 <+109>:	call   0x80485f0 <ptrace@plt>
    0x080487ba <+114>:	cmp    eax,0xffffffff
    0x080487bd <+117>:	jne    0x80487ed <auth+165>
    
    # ... 3 puts:
    # ... "\033[32m.---------------------------."
    # ... "\033[31m| !! TAMPERING DETECTED !!  |"
    # ... "\033[32m'---------------------------'"
    # ... return(1)
    0x080487bf <+119>:	mov    DWORD PTR [esp],0x8048a68
    0x080487c6 <+126>:	call   0x8048590 <puts@plt>
    0x080487cb <+131>:	mov    DWORD PTR [esp],0x8048a8c
    0x080487d2 <+138>:	call   0x8048590 <puts@plt>
    0x080487d7 <+143>:	mov    DWORD PTR [esp],0x8048ab0
    0x080487de <+150>:	call   0x8048590 <puts@plt>
    0x080487e3 <+155>:	mov    eax,0x1
    0x080487e8 <+160>:	jmp    0x8048877 <auth+303>

    # number = login[3] ^ 4919 + 6221293
    0x080487ed <+165>:	mov    eax,DWORD PTR [ebp+0x8]
    0x080487f0 <+168>:	add    eax,0x3
    0x080487f3 <+171>:	movzx  eax,BYTE PTR [eax]
    0x080487f6 <+174>:	movsx  eax,al
    0x080487f9 <+177>:	xor    eax,0x1337
    0x080487fe <+182>:	add    eax,0x5eeded
    0x08048803 <+187>:	mov    DWORD PTR [ebp-0x10],eax

    # i = 0
    0x08048806 <+190>:	mov    DWORD PTR [ebp-0x14],0x0         ; int i @ ebp-0x14 = 0

    # while(i < len) (auth+275)
    0x0804880d <+197>:	jmp    0x804885b <auth+275>             ; jump <+275>

    # ... if login[i] <= 31   return(1)
    0x0804880f <+199>:	mov    eax,DWORD PTR [ebp-0x14]
    0x08048812 <+202>:	add    eax,DWORD PTR [ebp+0x8]
    0x08048815 <+205>:	movzx  eax,BYTE PTR [eax]
    0x08048818 <+208>:	cmp    al,0x1f                 
    0x0804881a <+210>:	jg     0x8048823 <auth+219>
    0x0804881c <+212>:	mov    eax,0x1
    0x08048821 <+217>:	jmp    0x8048877 <auth+303>

    # ... number += 0x88233b2b - (((((username[i] ^ nb) * 0x88233b2b - 0x88233b2b) >> 1) + 0x88233b2b) >> 32) * 0x539
    0x08048823 <+219>:	mov    eax,DWORD PTR [ebp-0x14]
    0x08048826 <+222>:	add    eax,DWORD PTR [ebp+0x8]  
    0x08048829 <+225>:	movzx  eax,BYTE PTR [eax]        
    0x0804882c <+228>:	movsx  eax,al
    0x0804882f <+231>:	mov    ecx,eax                  
    0x08048831 <+233>:	xor    ecx,DWORD PTR [ebp-0x10]
    0x08048834 <+236>:	mov    edx,0x88233b2b
    0x08048839 <+241>:	mov    eax,ecx
    0x0804883b <+243>:	mul    edx               
    0x0804883d <+245>:	mov    eax,ecx
    0x0804883f <+247>:	sub    eax,edx
    0x08048841 <+249>:	shr    eax,1
    0x08048843 <+251>:	add    eax,edx
    0x08048845 <+253>:	shr    eax,0xa
    0x08048848 <+256>:	imul   eax,eax,0x539
    0x0804884e <+262>:	mov    edx,ecx
    0x08048850 <+264>:	sub    edx,eax
    0x08048852 <+266>:	mov    eax,edx
    0x08048854 <+268>:	add    DWORD PTR [ebp-0x10],eax

    # ... i++
    0x08048857 <+271>:	add    DWORD PTR [ebp-0x14],0x1

    # if i < len jump auth+199  (condition de la boucle)
    0x0804885b <+275>:	mov    eax,DWORD PTR [ebp-0x14]
    0x0804885e <+278>:	cmp    eax,DWORD PTR [ebp-0xc]
    0x08048861 <+281>:	jl     0x804880f <auth+199>

    # return ((int)(number == argv[2]))
    0x08048863 <+283>:	mov    eax,DWORD PTR [ebp+0xc]
    0x08048866 <+286>:	cmp    eax,DWORD PTR [ebp-0x10]
    0x08048869 <+289>:	je     0x8048872 <auth+298>
    0x0804886b <+291>:	mov    eax,0x1
    0x08048870 <+296>:	jmp    0x8048877 <auth+303>
    0x08048872 <+298>:	mov    eax,0x0
    0x08048877 <+303>:	leave
    0x08048878 <+304>:	ret
    End of assembler dump.
```

## 2: Comportement

* Ok, donc on a un programme qui demande un login et un serial_number ...
* ... et qui lance un shell si la somme des hash des lettres des logins == serial_number

## 3: Exploit

### A: Explication

> Ici, seulement de l'interprétation et un peu de gdb.\
> On pourrait reproduire le comportement de la fonction de hash grâce à un gros calcul mais on peut aller plus vite avec gdb.\
> Or, il y a un ptrace qui protège la fonction de hash.\
> On va donc breakpoint avant le check du retour de ptrace et changer la valeur de son retour.\
> On va mettre aussi un 2eme breakpoint apres le hashage pour imprimer le serial_number voulu pour notre login.\
> On pourra lancer le programme avec ce meme login et serial_number.

### B: Creation de l'exploit

```shell
  # Premier breakpoint pour dodge ptrace
  gdb-peda$ b * 0x080487ba
    Breakpoint 1 at 0x80487ba

  # Deuxieme breakpoint pour print notre serial_number
  gdb-peda$ b * 0x08048866
    Breakpoint 2 at 0x8048866

  # Et c'est parti!
  gdb-peda$ run
    ***********************************
    *		level06		  *
    ***********************************
    -> Enter Login: jojomon
    ***********************************
    ***** NEW ACCOUNT DETECTED ********
    ***********************************
    -> Enter Serial: onsenfout
  
    [----------------------------------registers-----------------------------------]
    [...]
    # Oh non! un -1 qui va nous faire quitter! ...
    EAX: 0xffffffff
    [...]
    [-------------------------------------code-------------------------------------]
      0x80487a6 <auth+94>:	mov    DWORD PTR [esp+0x4],0x0
      0x80487ae <auth+102>:	mov    DWORD PTR [esp],0x0
      0x80487b5 <auth+109>:	call   0x80485f0 <ptrace@plt>
    => 0x80487ba <auth+114>:	cmp    eax,0xffffffff
      0x80487bd <auth+117>:	jne    0x80487ed <auth+165>
      0x80487bf <auth+119>:	mov    DWORD PTR [esp],0x8048a68
      0x80487c6 <auth+126>:	call   0x8048590 <puts@plt>
      0x80487cb <auth+131>:	mov    DWORD PTR [esp],0x8048a8c
    [...]
    Breakpoint 1, 0x080487ba in auth ()

  # ... pas de panique, changeons le!
  gdb-peda$ set $eax = 0
  gdb-peda$ c
    [...]
    Breakpoint 2, 0x08048866 in auth ()

  # On est arrivé au 2eme breakpoint! imprimons notre serial_number:
  gdb-peda$ x/d $ebp - 0x10
    0xffffd258:	6233795

  # Et voilà ! Il ne nous manque plus qu'a lancer le programme avec tout ça.
  level06@OverRide:/tmp$ ~/level06
    ***********************************
    *		level06		  *
    ***********************************
    -> Enter Login: jojomon
    ***********************************
    ***** NEW ACCOUNT DETECTED ********
    ***********************************
    -> Enter Serial: 6233795
    Authenticated!
  $ whoami
    level07
  $ cat /home/users/level07/.pass
    GbcPDRgsFK77LNnnuh7QyFYA2942Gp8yKj9KrWD8
  $
    [1]+  Stopped                 ~/level06
  
  level06@OverRide:/tmp$ su level07
    Password: GbcPDRgsFK77LNnnuh7QyFYA2942Gp8yKj9KrWD8
    RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
    Partial RELRO   Canary found      NX disabled   No PIE          No RPATH   No RUNPATH   /home/users/level07/level07
  level07@OverRide:~$
```