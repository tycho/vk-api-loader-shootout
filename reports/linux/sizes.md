## Binary sizes

All sizes in bytes. Sorted by stripped binary size. Section values come from `size`; Mach-O binaries report BSS as zero because the Mach-O segment model folds zero-init into `__DATA`.

| Loader               | Loader .o |  Binary |    text |  data |   bss |
|----------------------|----------:|--------:|--------:|------:|------:|
| Gloam (discover)     |    50,992 |  59,824 |  54,005 |   864 | 7,136 |
| Gloam (enabled-list) |    50,992 |  59,832 |  54,789 |   872 | 7,144 |
| GLAD (tycho)         |    67,192 |  80,320 |  66,623 | 7,400 | 7,072 |
| Volk                 |   315,096 | 100,696 |  94,069 |   832 | 6,112 |
| GLAD (dav1dde)       |   316,288 | 113,016 | 103,870 |   864 | 6,600 |

