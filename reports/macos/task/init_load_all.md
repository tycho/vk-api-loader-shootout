### Init + load all functions

| Loader                   | Unpatched |   Patched | Patch speedup | vs. fastest |
|--------------------------|----------:|----------:|--------------:|------------:|
| **Gloam (enabled-list)** |    7.20µs |    5.00µs |          1.4× |        1.0× |
| Volk                     |   28.40µs |   13.97µs |          2.0× |        2.8× |
| Gloam (discover)         |  422.80µs |   86.17µs |          4.9× |       17.2× |
| GLAD (tycho)             |  427.90µs |   89.13µs |          4.8× |       17.8× |
| GLAD (dav1dde)           | 1555.03µs | 1083.13µs |          1.4× |        217× |

