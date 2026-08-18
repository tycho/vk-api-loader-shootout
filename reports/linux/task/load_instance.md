### Load instance functions

| Loader                   | Unpatched | Patched | Patch speedup | vs. fastest |
|--------------------------|----------:|--------:|--------------:|------------:|
| **Gloam (enabled-list)** |    1.17µs |  0.72µs |          1.6× |        1.0× |
| Volk                     |    7.99µs |  2.17µs |          3.7× |        3.0× |
| GLAD (tycho)             |    6.92µs |  2.88µs |          2.4× |        4.0× |
| Gloam (discover)         |  220.13µs |  9.68µs |         22.7× |       13.4× |
| GLAD (dav1dde)           |  325.82µs | 35.84µs |          9.1× |       49.8× |

