### Init + load all functions

| Loader                   | Unpatched |   Patched | Patch speedup | vs. fastest |
|--------------------------|----------:|----------:|--------------:|------------:|
| **Gloam (enabled-list)** |   35.23µs |    5.07µs |          6.9× |        1.0× |
| Volk                     |  379.83µs |   18.13µs |         21.0× |        3.6× |
| GLAD (tycho)             |  575.17µs |   89.30µs |          6.4× |       17.6× |
| Gloam (discover)         |  576.33µs |   90.00µs |          6.4× |       17.8× |
| GLAD (dav1dde)           | 1975.60µs | 1090.70µs |          1.8× |        215× |

