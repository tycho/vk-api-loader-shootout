## Binary sizes

All sizes in bytes. Sorted by stripped binary size. Section values come from `size`; Mach-O binaries report BSS as zero because the Mach-O segment model folds zero-init into `__DATA`.

| Loader               | Loader .o |  Binary |    text |  data |   bss |
|----------------------|----------:|--------:|--------:|------:|------:|
| Gloam (discover)     |    58,920 |  68,008 |  60,658 |   880 | 7,328 |
| Gloam (enabled-list) |    58,920 |  68,016 |  61,486 |   888 | 7,336 |
| GLAD (tycho)         |    67,192 |  84,408 |  67,921 | 7,416 | 7,080 |
| Volk                 |   323,312 | 108,880 |  97,555 |   856 | 6,280 |
| GLAD (dav1dde)       |   316,288 | 113,000 | 105,175 |   880 | 6,608 |

