### Load device functions

| Loader                   | Unpatched |  Patched | Patch speedup | vs. fastest |
|--------------------------|----------:|---------:|--------------:|------------:|
| **Gloam (enabled-list)** |    5.43µs |   3.42µs |          1.6× |        1.0× |
| GLAD (tycho)             |   12.69µs |   7.08µs |          1.8× |        2.1× |
| Volk                     |   16.99µs |  10.62µs |          1.6× |        3.1× |
| Gloam (discover)         |   17.41µs |  11.72µs |          1.5× |        3.4× |
| GLAD (dav1dde)           |  762.92µs | 597.76µs |          1.3× |        175× |

