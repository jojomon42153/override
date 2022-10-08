## 0: Setup du projet
  ```shell
  scp -r -P 4242 peda .gdbinit level00@127.0.0.1:/tmp
       ____                  ____  _     __
      / __ \_   _____  _____/ __ \(_)___/ /__
     / / / / | / / _ \/ ___/ /_/ / / __  / _ \
    / /_/ /| |/ /  __/ /  / _, _/ / /_/ /  __/
    \____/ |___/\___/_/  /_/ |_/_/\__,_/\___/

                        Good luck & Have fun

    To start, ssh with level00/level00 on 10.0.2.15:4242
    level00@127.0.0.1's password: level00
    LICENSE                              100%  157   307.9KB/s   00:00
    README                               100% 1162     2.4MB/s   00:00
    README.md                            100% 1939     4.0MB/s   00:00
    .gitignore                           100%  232   415.7KB/s   00:00
    config.py                            100% 3456     5.3MB/s   00:00
    skeleton.py                          100% 6857    11.0MB/s   00:00
    nasm.py                              100% 3263     4.0MB/s   00:00
    utils.py                             100%   23KB  18.0MB/s   00:00
    six.py                               100%   29KB  30.7MB/s   00:00
    shellcode.py                         100%   18KB  25.0MB/s   00:00
    python23-compatibility.md            100% 2078     3.4MB/s   00:00
    peda.py                              100%  195KB  37.8MB/s   00:00
    .gdbinit                             100%   25    66.7KB/s   00:00
  
  $ ssh level00@127.0.0.1 -p 4242
    level00@127.0.0.1's password: level00
  
  $ chmod 777 /tmp/peda /tmp/.gdbinit
  ```

## 1: Analyse

### A: C'est quoi mon binaire?

  ```shell
  # On est déja connecté post setup
  
  # On regarde ce qu'on a
  level00@OverRide:~$ ls -la
    total 13
    dr-xr-x---+ 1 level01 level01   60 Sep 13  2016 .
    dr-x--x--x  1 root    root     260 Oct  2  2016 ..
    -rw-r--r--  1 level01 level01  220 Sep 10  2016 .bash_logout
    lrwxrwxrwx  1 root    root       7 Sep 13  2016 .bash_profile -> .bashrc
    -rw-r--r--  1 level00 level00 3533 Sep 10  2016 .bashrc
    -rwsr-s---+ 1 level01 users   7280 Sep 10  2016 level00
    -rw-r--r--  1 level01 level01  675 Sep 10  2016 .profile

  # On teste les arguments
  level00@OverRide:~$ ./level00
    ***********************************
    * 	          -Level00 -	  	    *
    ***********************************
    Password:SUPER LONG PASSWORD VRAIMENT C'EST TROP LONG C'EST POUR ESSAYER DE FAIRE SEGFAULT LE PASSWORD C'EST POUR CA QUE C'EST LONG ON ESSAYE CE QU'ON PEUT HEIN!

    Invalid Password!
  ```
  * On a un binaire appartenant a level01 dans le home avec les droits SUID...
  * ... qui demande un password ...
  * ... qui segfault pas avec un input trop long.

### B: On l'ouvre avec gdb
  * On va dans tmp pour lancer avec peda qu'on a copié avec le setup.sh
  ```shell
    level00@RainFall:~$ cd /tmp
  ```

  * On lance gdb avec notre binaire
  ```shell
    level00@RainFall:/tmp$ gdb ~/level00
      [...]
      Reading symbols from /home/users/level00/level00...(no debugging symbols found)...done.
  ```

  * Qu'est-ce qu'on a comme fonctions?
  ```shell
    gdb-peda$ info function
      All defined functions:

      Non-debugging symbols:
      0x08048338  _init
      0x08048380  printf
      0x08048380  printf@plt
      0x08048390  puts
      0x08048390  puts@plt
      0x080483a0  system
      0x080483a0  system@plt
      0x080483b0  __gmon_start__
      0x080483b0  __gmon_start__@plt
      0x080483c0  __libc_start_main
      0x080483c0  __libc_start_main@plt
      0x080483d0  __isoc99_scanf
      0x080483d0  __isoc99_scanf@plt
      0x080483e0  _start
      0x08048410  __do_global_dtors_aux
      0x08048470  frame_dummy
      0x08048494  main        # Exactement comme rainfall du coup je mettrai juste ce qu'il y a de new dans cette partie a partir de maintenant
      0x08048520  __libc_csu_init
      0x08048590  __libc_csu_fini
      0x08048592  __i686.get_pc_thunk.bx
      0x080485a0  __do_global_ctors_aux
      0x080485cc  _fini
  ```

  * Commandes lancées pendant l'analyse du binaire ci-dessous
  ```shell
    gdb-peda$ x/s 0x80485f0
      0x80485f0:	 '*' <repeats 35 times>
    gdb-peda$ x/s 0x8048614
      0x8048614:	 "* \t     -Level00 -\t\t  *"
    gdb-peda$ x/s 0x804862c
      0x804862c:	 "Password:"
    gdb-peda$ x/s 0x8048636
      0x8048636:	 "%d"
    gdb-peda$ x/s 0x8048651
      0x8048651:	 "\nInvalid Password!"
    gdb-peda$ x/s 0x8048639
      0x8048639:	 "\nAuthenticated!"
    gdb-peda$ x/s 0x8048649
      0x8048649:	 "/bin/sh"
  ```

  * On disassemble main pour regarder le code
  ```shell
    gdb-peda$ pdisas main
    Dump of assembler code for function main:
      # Initialisation, alignement et allocation de 32 octets
      0x08048494 <+0>:	push   ebp
      0x08048495 <+1>:	mov    ebp,esp
      0x08048497 <+3>:	and    esp,0xfffffff0
      0x0804849a <+6>:	sub    esp,0x20

      # Call 3 fois puts pour afficher le premier output du programme
      0x0804849d <+9>:	mov    DWORD PTR [esp],0x80485f0
      0x080484a4 <+16>:	call   0x8048390 <puts@plt>
      0x080484a9 <+21>:	mov    DWORD PTR [esp],0x8048614
      0x080484b0 <+28>:	call   0x8048390 <puts@plt>
      0x080484b5 <+33>:	mov    DWORD PTR [esp],0x80485f0
      0x080484bc <+40>:	call   0x8048390 <puts@plt>

      # On call printf pour afficher "Password:"
      0x080484c1 <+45>:	mov    eax,0x804862c
      0x080484c6 <+50>:	mov    DWORD PTR [esp],eax
      0x080484c9 <+53>:	call   0x8048380 <printf@plt>

      # On call scanf pour demander un int en input.
      # L'int est stocké a ESP +28
      0x080484ce <+58>:	mov    eax,0x8048636
      0x080484d3 <+63>:	lea    edx,[esp+0x1c]
      0x080484d7 <+67>:	mov    DWORD PTR [esp+0x4],edx
      0x080484db <+71>:	mov    DWORD PTR [esp],eax
      0x080484de <+74>:	call   0x80483d0 <__isoc99_scanf@plt>

      # Si l'input != 5276 print Invalid password et return 1 
      0x080484e3 <+79>:	mov    eax,DWORD PTR [esp+0x1c]
      0x080484e7 <+83>:	cmp    eax,0x149c
      0x080484ec <+88>:	jne    0x804850d <main+121>

      # Sinon puts Authenticated!, lance un shell et return 0
      0x080484ee <+90>:	mov    DWORD PTR [esp],0x8048639
      0x080484f5 <+97>:	call   0x8048390 <puts@plt>
      0x080484fa <+102>:	mov    DWORD PTR [esp],0x8048649
      0x08048501 <+109>:	call   0x80483a0 <system@plt>
      0x08048506 <+114>:	mov    eax,0x0
      0x0804850b <+119>:	jmp    0x804851e <main+138>

      # Puts "Invalid Password !\n" et return 
      0x0804850d <+121>:	mov    DWORD PTR [esp],0x8048651
      0x08048514 <+128>:	call   0x8048390 <puts@plt>
      0x08048519 <+133>:	mov    eax,0x1
      0x0804851e <+138>:	leave
      0x0804851f <+139>:	ret
    End of assembler dump.
  ```

## 2: Comportement
  * Ok, donc on a un programme qui demande un mdp, et lance un shell s'il est égal a 5276. L'exploit est fait.

## 3: Exploit
  > Pas besoin d'explication ici.
  ```shell
    level00@OverRide:/tmp$ ~/level00
      ***********************************
      * 	        -Level00 -	      	  *
      ***********************************
      Password:5276

      Authenticated!
    $ cat /home/users/level01/.pass
      uSq2ehEGT6c9S24zbshexZQBXUGrncxn5sD5QfGL
  ```
