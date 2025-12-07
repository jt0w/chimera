# Chimera
Chimera is my shot at implementing a standard c library. Of course there is libc but that isn't enough for me.
Chimera includes a lot of utils (like macros for shifting arrays, etc,...) and overall things the "normal" c dev will need in almost every project.

This is heavily inspired by [nob.h](https://github.com/tsoding/nob.h) from rexim (tsoding).

## Building
You don't need to build chimera.h you can just import it. But there is `showcase.c` which shows you some of the things chimera wcan do.
To build said file you only need your trusted c compile **once**;
So you just build `showcase.c` like this:
```shell
gcc -o showcase showcase.c
```
In this exampe I used gcc but you should be able to use any compiler.
After this to build `showcase.c` (if you for example modified it) you can just do `./showcase build` to make the showcase rebuild itself.

