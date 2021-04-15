Sometimes when debugging things like crashes or memory leaks you end
up doing a lot of printfs with %p to try to figure what is happening.

There are two common problems with this.

 * Pointers are very long hex-strings which are painful to read and compare.
 * If you run the same job twice the addresses will be different, so
   its hard to compare logs.

Enter `libuniqueptr.so`. If you use this from the app (using e.g. `LD_PRELOAD`),
then any call to `printf()` or `g_print()` will be formated such that the
first ever printed value is `0u1`, the second is `0u2`, etc, and if these
are ever printed again the same value is used.

For example:

```
      printf ("'%-20p' '%20p' %p\n", &a, &b, &c);
      printf ("'%-20p' '%20p' %p\n", &a, &b, &c);
```

Would normally print:

```
$ ./testprint
'0x7ffccf56607d      ' '      0x7ffccf56607e' 0x7ffccf56607f
'0x7ffccf56607d      ' '      0x7ffccf56607e' 0x7ffccf56607f
```

But with libuniqueptr.so:

```
$ LD_PRELOAD=./libuniqueptr.so ./testprint
'0u1                 ' '                 0u2' 0u3
'0u1                 ' '                 0u2' 0u3
```
