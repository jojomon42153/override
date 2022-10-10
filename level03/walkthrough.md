## 1: Analyse

### A: C'est quoi mon binaire?


  ```shell
  # On se connecte si c'est pas déja fait via le level02
  $ ssh level03@127.0.0.1 -p 4242
        _____       _       ______    _ _
      |  __ \     (_)     |  ____|  | | |
      | |__) |__ _ _ _ __ | |__ __ _| | |
      |  _  /  _` | | '_ \|  __/ _` | | |
      | | \ \ (_| | | | | | | | (_| | | |
      |_|  \_\__,_|_|_| |_|_|  \__,_|_|_|

                    Good luck & Have fun

      To start, ssh with level0/level0 on 10.0.2.15:4242

  # On rentre le password
  level03@127.0.0.1's password: Hh74RPnuQ9sa5JAEXgNWCqz7sXGnh5J5M9KfPg3H
    RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
    Partial RELRO   Canary found      NX enabled    No PIE          No RPATH   No RUNPATH   /home/users/level03/level03
  level03@OverRide:~$

  # On regarde ce qu'on a
  level03@OverRide:~$ ls -la
    total 17
    dr-xr-x---+ 1 level03 level03   80 Sep 13  2016 .
    dr-x--x--x  1 root    root     260 Oct  2  2016 ..
    -rw-r--r--  1 level03 level03  220 Sep 10  2016 .bash_logout
    lrwxrwxrwx  1 root    root       7 Sep 13  2016 .bash_profile -> .bashrc
    -rw-r--r--  1 level03 level03 3533 Sep 10  2016 .bashrc
    -rwsr-s---+ 1 level04 users   7677 Sep 10  2016 level03
    -rw-r--r--+ 1 level03 level03   41 Oct 19  2016 .pass
    -rw-r--r--  1 level03 level03  675 Sep 10  2016 .profile

  # On teste les arguments
  level03@OverRide:~$ ~/level03
    ***********************************
    *	          	level03	           **
    ***********************************
    Password:PASSWORD SUPER LONG DE LA MORT QUI TUE TROP FORT TELLEMENT IL EST LONG %s%s%s%s%s%s

    Invalid Password
  ```
  * On a un binaire appartenant a level03 dans le home avec les droits SUID...
  * ... qui demande un password ...
  * ... qui segfault pas

### B: On l'ouvre avec gdb
  * On va dans tmp pour lancer avec peda qu'on a copié avec le setup.sh
  ```shell
    level03@RainFall:~$ cd /tmp
  ```

  * On lance gdb avec notre binaire
  ```shell
    level03@RainFall:/tmp$ gdb ~/level03
      [...]
      Reading symbols from /home/user/level03/level03...(no debugging symbols found)...done.
  ```

  * Qu'est-ce qu'on a comme fonctions?
  ```shell
    gdb-peda$ info function
      [...]
      0x080485d0  frame_dummy
      0x080485f4  clear_stdin
      0x08048617  get_unum
      0x0804864f  prog_timeout
      0x08048660  decrypt
      0x08048747  test
      0x0804885a  main
      [...]
  ```

  * Commandes lancées pendant l'analyse du binaire ci-dessous
  ```shell
    gdb-peda$ x/s 0x8048a48
      0x8048a48:	 '*' <repeats 35 times>
    gdb-peda$ x/s 0x8048a6c
      0x8048a6c:	 "*\t\tlevel03\t\t**"
    gdb-peda$ x/s 0x8048a48
      0x8048a48:	 '*' <repeats 35 times>
    gdb-peda$ x/s 0x8048a7b
      0x8048a7b:	 "Password:"
    gdb-peda$ x/s 0x8048a85
      0x8048a85:	 "%d"
    gdb-peda$ x/i 0x80489f0
      0x80489f0:	dec    edx
    gdb-peda$ x/s 0x80489c3
      0x80489c3:	 "Congratulations!"
  ```

  * On disassemble main pour regarder le code
  ```shell
    gdb-peda$ pdisas main
      Dump of assembler code for function main:
      # Initialisation, alignement, allocation de 32 octets.
      0x0804885a <+0>:	push   ebp
      0x0804885b <+1>:	mov    ebp,esp
      0x0804885d <+3>:	and    esp,0xfffffff0
      0x08048860 <+6>:	sub    esp,0x20

      # initialise eax à 0, puis reprend sa valeur d'avant et le reste on s'en fout le code est jamais atteint.
      0x08048863 <+9>:	push   eax
      0x08048864 <+10>:	xor    eax,eax
      0x08048866 <+12>:	je     0x804886b <main+17>
      0x08048868 <+14>:	add    esp,0x4
      0x0804886b <+17>:	pop    eax

      # time(0)
      0x0804886c <+18>:	mov    DWORD PTR [esp],0x0
      0x08048873 <+25>:	call   0x80484b0 <time@plt>

      # srand(0)
      0x08048878 <+30>:	mov    DWORD PTR [esp],eax
      0x0804887b <+33>:	call   0x8048500 <srand@plt>

      # puts * 3 + printf
      # '***********************************'
      # "*            level03             **"
      # '***********************************'
      # "Password:"
      0x08048880 <+38>:	mov    DWORD PTR [esp],0x8048a48
      0x08048887 <+45>:	call   0x80484d0 <puts@plt>
      0x0804888c <+50>:	mov    DWORD PTR [esp],0x8048a6c
      0x08048893 <+57>:	call   0x80484d0 <puts@plt>
      0x08048898 <+62>:	mov    DWORD PTR [esp],0x8048a48
      0x0804889f <+69>:	call   0x80484d0 <puts@plt>
      0x080488a4 <+74>:	mov    eax,0x8048a7b
      0x080488a9 <+79>:	mov    DWORD PTR [esp],eax
      0x080488ac <+82>:	call   0x8048480 <printf@plt>

      # scanf("%d", input)
      0x080488b1 <+87>:	mov    eax,0x8048a85
      0x080488b6 <+92>:	lea    edx,[esp+0x1c]
      0x080488ba <+96>:	mov    DWORD PTR [esp+0x4],edx
      0x080488be <+100>:	mov    DWORD PTR [esp],eax
      0x080488c1 <+103>:	call   0x8048530 <__isoc99_scanf@plt>

      # test(input, 322424845)
      0x080488c6 <+108>:	mov    eax,DWORD PTR [esp+0x1c]
      0x080488ca <+112>:	mov    DWORD PTR [esp+0x4],0x1337d00d
      0x080488d2 <+120>:	mov    DWORD PTR [esp],eax
      0x080488d5 <+123>:	call   0x8048747 <test>

      # return(0)
      0x080488da <+128>:	mov    eax,0x0
      0x080488df <+133>:	leave
      0x080488e0 <+134>:	ret
      End of assembler dump.
  ```

  ```shell
  gdb-peda$ pdisas test
    Dump of assembler code for function test:
    # Initialisation, allocation de 40 octets
    0x08048747 <+0>:	push   ebp
    0x08048748 <+1>:	mov    ebp,esp
    0x0804874a <+3>:	sub    esp,0x28

    # diff = 322424845 - atoi(number)
    0x0804874d <+6>:	mov    eax,DWORD PTR [ebp+0x8]
    0x08048750 <+9>:	mov    edx,DWORD PTR [ebp+0xc]
    0x08048753 <+12>:	mov    ecx,edx
    0x08048755 <+14>:	sub    ecx,eax
    0x08048757 <+16>:	mov    eax,ecx
    0x08048759 <+18>:	mov    DWORD PTR [ebp-0xc],eax

    # if (unsigned_int)diff > 21  decrypt(rand()) & return
    0x0804875c <+21>:	cmp    DWORD PTR [ebp-0xc],0x15
    0x08048760 <+25>:	ja     0x804884a <test+259>

    # diff << 2 => diff * 2^2
    0x08048766 <+31>:	mov    eax,DWORD PTR [ebp-0xc]
    0x08048769 <+34>:	shl    eax,0x2

    # eax + &(dec edx) = sub edx, 1  https://www.gladir.com/LEXIQUE/ASM/dec.htm
    0x0804876c <+37>:	add    eax,0x80489f0

    # jump to dec edx + diff * 4
    0x08048771 <+42>:	mov    eax,DWORD PTR [eax]
    0x08048773 <+44>:	jmp    eax

    # return decrypt(diff)
    0x08048775 <+46>:	mov    eax,DWORD PTR [ebp-0xc]
    0x08048778 <+49>:	mov    DWORD PTR [esp],eax
    0x0804877b <+52>:	call   0x8048660 <decrypt>
    0x08048780 <+57>:	jmp    0x8048858 <test+273>

    # return decrypt(diff)
    0x08048785 <+62>:	mov    eax,DWORD PTR [ebp-0xc]
    0x08048788 <+65>:	mov    DWORD PTR [esp],eax
    0x0804878b <+68>:	call   0x8048660 <decrypt>
    0x08048790 <+73>:	jmp    0x8048858 <test+273>

    [...] repeat many times [...]
  
    # return decrypt(diff)
    0x0804883d <+246>:	mov    eax,DWORD PTR [ebp-0xc]
    0x08048840 <+249>:	mov    DWORD PTR [esp],eax
    0x08048843 <+252>:	call   0x8048660 <decrypt>
    0x08048848 <+257>:	jmp    0x8048858 <test+273>
  
    # decrypt(rand())
    0x0804884a <+259>:	call   0x8048520 <rand@plt>
    0x0804884f <+264>:	mov    DWORD PTR [esp],eax
    0x08048852 <+267>:	call   0x8048660 <decrypt>
    0x08048857 <+272>:	nop

    # return
    0x08048858 <+273>:	leave
    0x08048859 <+274>:	ret
    End of assembler dump.
  ```

  ```shell
    gdb-peda$ pdisas decrypt
      Dump of assembler code for function decrypt:
      # Initialise, push edi/esi, alloue 64 octets
      0x08048660 <+0>:	push   ebp
      0x08048661 <+1>:	mov    ebp,esp
      0x08048663 <+3>:	push   edi
      0x08048664 <+4>:	push   esi
      0x08048665 <+5>:	sub    esp,0x40

      # canary value stocked to ebp - 12
      0x08048668 <+8>:	mov    eax,gs:0x14
      0x0804866e <+14>:	mov    DWORD PTR [ebp-0xc],eax
      0x08048671 <+17>:	xor    eax,eax

      # initialise number1 1971092817 => "Q}|u"
      0x08048673 <+19>:	mov    DWORD PTR [ebp-0x1d],0x757c7d51
      # initialise number2 1734767456 => "`sfg"
      0x0804867a <+26>:	mov    DWORD PTR [ebp-0x19],0x67667360
      # initialise number3 2070311806 => "~sf{"
      0x08048681 <+33>:	mov    DWORD PTR [ebp-0x15],0x7b66737e
      # initialise number4 862026877 => "}|a3"
      0x08048688 <+40>:	mov    DWORD PTR [ebp-0x11],0x33617c7d
      # initialise char0 to 0
      0x0804868f <+47>:	mov    BYTE PTR [ebp-0xd],0x0
      # Ces 4 initialisations se suivent dans la mémoire.
      # On déclare donc la string: "Q}|u`sfg~sf{}|a3\0"

      # Ok. pareil que pour test, c'est inutile
      0x08048693 <+51>:	push   eax
      0x08048694 <+52>:	xor    eax,eax
      0x08048696 <+54>:	je     0x804869b <decrypt+59>
      0x08048698 <+56>:	add    esp,0x4
      0x0804869b <+59>:	pop    eax

      # len_str = strlen(&number1)
      0x0804869c <+60>:	lea    eax,[ebp-0x1d]
      0x0804869f <+63>:	mov    DWORD PTR [ebp-0x2c],0xffffffff # number5 = -1
      0x080486a6 <+70>:	mov    edx,eax
      0x080486a8 <+72>:	mov    eax,0x0
      0x080486ad <+77>:	mov    ecx,DWORD PTR [ebp-0x2c]
      0x080486b0 <+80>:	mov    edi,edx
      0x080486b2 <+82>:	repnz scas al,BYTE PTR es:[edi]
      0x080486b4 <+84>:	mov    eax,ecx
      0x080486b6 <+86>:	not    eax
      0x080486b8 <+88>:	sub    eax,0x1
      0x080486bb <+91>:	mov    DWORD PTR [ebp-0x24],eax

      # index = 0
      0x080486be <+94>:	mov    DWORD PTR [ebp-0x28],0x0

      # While decrypt+133 => index++ < len_str...
      0x080486c5 <+101>:	jmp    0x80486e5 <decrypt+133>

      # ... &number1 + index = (&number1 + index) ^ key (argv[0])
      0x080486c7 <+103>:	lea    eax,[ebp-0x1d]
      0x080486ca <+106>:	add    eax,DWORD PTR [ebp-0x28]
      0x080486cd <+109>:	movzx  eax,BYTE PTR [eax]
      0x080486d0 <+112>:	mov    edx,eax
      0x080486d2 <+114>:	mov    eax,DWORD PTR [ebp+0x8]
      0x080486d5 <+117>:	xor    eax,edx
      0x080486d7 <+119>:	mov    edx,eax
      0x080486d9 <+121>:	lea    eax,[ebp-0x1d]
      0x080486dc <+124>:	add    eax,DWORD PTR [ebp-0x28]
      0x080486df <+127>:	mov    BYTE PTR [eax],dl

      # if index++ < len_str retour en haut de la boucle
      0x080486e1 <+129>:	add    DWORD PTR [ebp-0x28],0x1
      0x080486e5 <+133>:	mov    eax,DWORD PTR [ebp-0x28]
      0x080486e8 <+136>:	cmp    eax,DWORD PTR [ebp-0x24]
      0x080486eb <+139>:	jb     0x80486c7 <decrypt+103>

      # if !strncmp(&number1, "Congratulations!", 17) print "\nInvalid Password" et return
      0x080486ed <+141>:	lea    eax,[ebp-0x1d]
      0x080486f0 <+144>:	mov    edx,eax
      0x080486f2 <+146>:	mov    eax,0x80489c3
      0x080486f7 <+151>:	mov    ecx,0x11
      0x080486fc <+156>:	mov    esi,edx
      0x080486fe <+158>:	mov    edi,eax
      0x08048700 <+160>:	repz cmps BYTE PTR ds:[esi],BYTE PTR es:[edi]
      0x08048702 <+162>:	seta   dl
      0x08048705 <+165>:	setb   al
      0x08048708 <+168>:	mov    ecx,edx
      0x0804870a <+170>:	sub    cl,al
      0x0804870c <+172>:	mov    eax,ecx
      0x0804870e <+174>:	movsx  eax,al
      0x08048711 <+177>:	test   eax,eax
      0x08048713 <+179>:	jne    0x8048723 <decrypt+195>

      # system("/bin/sh"), check stack et return
      0x08048715 <+181>:	mov    DWORD PTR [esp],0x80489d4
      0x0804871c <+188>:	call   0x80484e0 <system@plt>
      0x08048721 <+193>:	jmp    0x804872f <decrypt+207>

      # puts ("\nInvalid Password")
      0x08048723 <+195>:	mov    DWORD PTR [esp],0x80489dc
      0x0804872a <+202>:	call   0x80484d0 <puts@plt>

      # Check que la stack a pas été modifiée, return
      0x0804872f <+207>:	mov    esi,DWORD PTR [ebp-0xc]
      0x08048732 <+210>:	xor    esi,DWORD PTR gs:0x14
      0x08048739 <+217>:	je     0x8048740 <decrypt+224>
      0x0804873b <+219>:	call   0x80484c0 <__stack_chk_fail@plt>

      # Return
      0x08048740 <+224>:	add    esp,0x40
      0x08048743 <+227>:	pop    esi
      0x08048744 <+228>:	pop    edi
      0x08048745 <+229>:	pop    ebp
      0x08048746 <+230>:	ret
      End of assembler dump.
  ```

## 2: Comportement
> Une fois recomposé, on comprend que le programme demande un password (int)
> Il va ensuite faire un calcul avec cet int qui va sortir une clef de cryptage
> Il va ensuite faire un [xor cypher](https://en.wikipedia.org/wiki/XOR_cipher) grâce a cette clef de cryptage sur la string "Q}|u`sfg~sf{}|a3"
> Si après ce décryptage la string devient "Congratulations!", alors il lance un shell


## 3: Exploit

### A: Explication

> Il n'y a pas de technique particulière ici si ce n'est la compréhension du binaire.\
> Malgré le flou total sur la fonction test, on comprend qu'elle appelle decrypt.\
> Le parametre pour decrypt est soit un nombre random soit 322424845 - password.\
> Or on voit que dans decrypt, key (le parametre qu'on lui passe) ^ 'Q' doit etre égal à 'C', puisque l'on applique ce xor sur tous les caracteres de "Q}|u`sfg~sf{}|a3" et qu'elle doit devenir "Congratulations!".\
> Essayons donc de trouver le password tel que (322424845 - password) ^ 'Q' = 'C'.

### B: Creation de l'exploit

* Le calcul est symple:
> (322424845 - password) ^ 'Q' = 'C' <=> (322424845 - password) = 'C' ^ 'Q'\
> 'C' ^ 'Q' = 18 = 322424845 - password <=> password = 322424845 - 18 = 322424827

```shell
  level03@OverRide:/tmp$ ~/level03
    ***********************************
    *		          level03	           **
    ***********************************
    Password:322424827
  $ whoami
    level04
  $ cat /home/users/level04/.pass
    kgv3tkEb9h2mLkRsPkXRfc2mHbjMxQzvb2FrgKkf
  level03@OverRide:/tmp$ su level04
    Password:
    RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
    Partial RELRO   No canary found   NX disabled   No PIE          No RPATH   No RUNPATH   /home/users/level04/level04
  level04@OverRide:~$
```