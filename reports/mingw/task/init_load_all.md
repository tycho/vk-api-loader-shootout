### Init + load all functions

| Loader                   | Unpatched |  Patched | Patch speedup | vs. fastest |
|--------------------------|----------:|---------:|--------------:|------------:|
| **Gloam (enabled-list)** |   49.57µs |   5.43µs |          9.1× |        1.0× |
| Volk                     |  623.83µs |  18.70µs |         33.4× |        3.4× |
| GLAD (tycho)             | 3195.53µs |  73.17µs |         43.7× |       13.5× |
| Gloam (discover)         | 3100.57µs |  73.83µs |         42.0× |       13.6× |
| GLAD (dav1dde)           | 6731.70µs | 977.23µs |          6.9× |        180× |

