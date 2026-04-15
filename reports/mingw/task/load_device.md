### Load device functions

| Loader                   | Unpatched |  Patched | Patch speedup | vs. fastest |
|--------------------------|----------:|---------:|--------------:|------------:|
| **Gloam (enabled-list)** |   45.36µs |   3.46µs |         13.1× |        1.0× |
| GLAD (tycho)             |  389.72µs |  12.28µs |         31.7× |        3.5× |
| Volk                     |  491.41µs |  14.57µs |         33.7× |        4.2× |
| Gloam (discover)         |  423.43µs |  23.99µs |         17.7× |        6.9× |
| GLAD (dav1dde)           | 3544.73µs | 724.25µs |          4.9× |        209× |

