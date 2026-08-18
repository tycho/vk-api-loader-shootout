## Binary sizes

All sizes in bytes. Sorted by stripped binary size. Section values come from `size`; Mach-O binaries report BSS as zero because the Mach-O segment model folds zero-init into `__DATA`.

| Loader               | Loader .o |  Binary |   text |   data | bss |
|----------------------|----------:|--------:|-------:|-------:|----:|
| GLAD (tycho)         |    53,072 |  67,072 | 17,366 | 47,404 |   0 |
| Gloam (discover)     |    53,163 |  70,144 | 23,286 | 44,820 |   0 |
| Gloam (enabled-list) |    53,163 |  71,168 | 24,326 | 44,916 |   0 |
| Volk                 |   229,029 | 108,032 | 75,206 | 31,224 |   0 |
| GLAD (dav1dde)       |   192,597 | 116,736 | 73,798 | 40,588 |   0 |

