# STC
STC is my shot at implementing a standard c library. Of course there is libc but that isn't enough for me.
STC includes a lot of utils (like macros for shifting arrays, etc,...) and overall things the "normal" c dev will need in almost every project.

## Building
You don't need to build stc.h you can just import it. But there is `showcase.c` which shows you some of the things stc can do.
To build said file you only need your trusted c compile **once**;
So you just build `showcase.c` like this:
```shell
gcc -o showcase showcase.c
```
In this exampe I used gcc but you should be able to use any compiler.
After this to build `showcase.c` (if you for example modified it) you can just do `./showcase build` to make the showcase rebuild itself.
