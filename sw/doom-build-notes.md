## Linux X11 compilation

Steps taken to setup compliation environment:
```shell
$ distrobox create -i ubuntu:24.04 --name doom-linux-x11
$ distrobox enter doom-linux-x11
$ cd src/linux-x11
$ sudo dpkg --add-architecture i386
$ sudo apt update
$ sudo apt upgrade
$ sudo apt install build-essential
$ sudo apt install gcc-multilib
$ sudo apt install libxext-dev:i386
$ make
```

In addition, the DOOM.WAD file needs to be provided. There's one in archive.org: https://archive.org/details/DOOMIWADFILE

