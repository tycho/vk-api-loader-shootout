## Binary sizes

All sizes in bytes. Sorted by stripped binary size. Section values come from `size`; Mach-O binaries report BSS as zero because the Mach-O segment model folds zero-init into `__DATA`.

| Loader               | Loader .o |  Binary |   text |   data | bss |
|----------------------|----------:|--------:|-------:|-------:|----:|
| Gloam (discover)     |    46,745 |  69,120 | 22,742 | 43,508 |   0 |
| Gloam (enabled-list) |    46,745 |  70,144 | 23,782 | 43,604 |   0 |
| GLAD (tycho)         |    53,072 |  70,656 | 19,574 | 48,220 |   0 |
| Volk                 |   223,294 | 110,592 | 75,830 | 31,532 |   0 |
| GLAD (dav1dde)       |   192,597 | 120,320 | 76,006 | 41,372 |   0 |

