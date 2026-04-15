### Load device functions

| Loader                   | Unpatched |  Patched | Patch speedup | vs. fastest |
|--------------------------|----------:|---------:|--------------:|------------:|
| **Gloam (enabled-list)** |   30.04µs |   3.25µs |          9.2× |        1.0× |
| GLAD (tycho)             |  141.58µs |   8.80µs |         16.1× |        2.7× |
| Gloam (discover)         |  148.56µs |  13.63µs |         10.9× |        4.2× |
| Volk                     |  338.01µs |  15.54µs |         21.8× |        4.8× |
| GLAD (dav1dde)           | 1091.54µs | 595.26µs |          1.8× |        183× |

