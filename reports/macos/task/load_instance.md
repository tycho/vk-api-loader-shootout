### Load instance functions

| Loader                   | Unpatched |  Patched | Patch speedup | vs. fastest |
|--------------------------|----------:|---------:|--------------:|------------:|
| **Gloam (enabled-list)** |    1.13µs |   0.83µs |          1.4× |        1.0× |
| GLAD (tycho)             |    4.90µs |   2.57µs |          1.9× |        3.1× |
| Volk                     |    9.66µs |   4.64µs |          2.1× |        5.6× |
| Gloam (discover)         |  181.21µs |  19.00µs |          9.5× |       22.9× |
| GLAD (dav1dde)           |  400.87µs | 222.95µs |          1.8× |        269× |

