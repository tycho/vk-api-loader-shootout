### Load device functions

| Loader                   | Unpatched |  Patched | Patch speedup | vs. fastest |
|--------------------------|----------:|---------:|--------------:|------------:|
| **Gloam (enabled-list)** |   34.07µs |   5.85µs |          5.8× |        1.0× |
| GLAD (tycho)             |  253.82µs |  39.85µs |          6.4× |        6.8× |
| Volk                     |  347.14µs |  45.47µs |          7.6× |        7.8× |
| Gloam (discover)         |  280.25µs |  49.87µs |          5.6× |        8.5× |
| GLAD (dav1dde)           | 1427.07µs | 640.17µs |          2.2× |        109× |

