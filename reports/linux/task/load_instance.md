### Load instance functions

| Loader                   | Unpatched | Patched | Patch speedup | vs. fastest |
|--------------------------|----------:|--------:|--------------:|------------:|
| **Gloam (enabled-list)** |    3.17µs |  1.51µs |          2.1× |        1.0× |
| GLAD (tycho)             |   23.27µs |  6.43µs |          3.6× |        4.3× |
| Volk                     |   38.87µs |  7.88µs |          4.9× |        5.2× |
| Gloam (discover)         |  239.85µs | 15.20µs |         15.8× |       10.1× |
| GLAD (dav1dde)           |  352.94µs | 43.57µs |          8.1× |       28.9× |

