### Load instance functions

| Loader                   | Unpatched |  Patched | Patch speedup | vs. fastest |
|--------------------------|----------:|---------:|--------------:|------------:|
| **Gloam (enabled-list)** |    5.08µs |   0.97µs |          5.2× |        1.0× |
| Volk                     |   75.59µs |   3.12µs |         24.2× |        3.2× |
| GLAD (tycho)             |   32.11µs |   3.17µs |         10.1× |        3.3× |
| Gloam (discover)         | 1249.52µs |  21.08µs |         59.3× |       21.7× |
| GLAD (dav1dde)           | 1691.24µs | 106.91µs |         15.8× |        110× |

