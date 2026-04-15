## Binary sizes

All sizes in bytes. Sorted by stripped binary size. Section values come from `size`; Mach-O binaries report BSS as zero because the Mach-O segment model folds zero-init into `__DATA`.

| Loader               | Loader .o |  Binary |    text |   data | bss |
|----------------------|----------:|--------:|--------:|-------:|----:|
| GLAD (tycho)         |    74,024 |  70,608 |  49,152 | 16,384 |   0 |
| Gloam (discover)     |    47,912 |  87,360 |  65,536 | 16,384 |   0 |
| Gloam (enabled-list) |    47,912 |  87,360 |  65,536 | 16,384 |   0 |
| Volk                 |   259,216 | 136,312 | 114,688 | 16,384 |   0 |
| GLAD (dav1dde)       |   275,592 | 161,856 | 114,688 | 16,384 |   0 |

