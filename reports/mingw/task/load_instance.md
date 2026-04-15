### Load instance functions

| Loader                   | Unpatched |  Patched | Patch speedup | vs. fastest |
|--------------------------|----------:|---------:|--------------:|------------:|
| **Gloam (enabled-list)** |    5.31µs |   1.34µs |          4.0× |        1.0× |
| Volk                     |   70.13µs |   2.99µs |         23.5× |        2.2× |
| GLAD (tycho)             |   28.85µs |   3.05µs |          9.5× |        2.3× |
| Gloam (discover)         | 1179.97µs |  20.36µs |         58.0× |       15.2× |
| GLAD (dav1dde)           | 1558.04µs | 100.98µs |         15.4× |       75.4× |

