### Load instance functions

| Loader                   | Unpatched |  Patched | Patch speedup | vs. fastest |
|--------------------------|----------:|---------:|--------------:|------------:|
| **Gloam (enabled-list)** |    5.38µs |   1.16µs |          4.6× |        1.0× |
| GLAD (tycho)             |   22.76µs |   2.74µs |          8.3× |        2.4× |
| Volk                     |   58.89µs |   4.96µs |         11.9× |        4.3× |
| Gloam (discover)         |  203.75µs |  20.06µs |         10.2× |       17.3× |
| GLAD (dav1dde)           |  468.01µs | 219.82µs |          2.1× |        190× |

