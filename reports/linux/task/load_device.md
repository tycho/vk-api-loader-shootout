### Load device functions

| Loader                   | Unpatched |  Patched | Patch speedup | vs. fastest |
|--------------------------|----------:|---------:|--------------:|------------:|
| **Gloam (enabled-list)** |    5.60µs |   3.31µs |          1.7× |        1.0× |
| Volk                     |   16.50µs |   7.51µs |          2.2× |        2.3× |
| GLAD (tycho)             |   19.94µs |   8.93µs |          2.2× |        2.7× |
| Gloam (discover)         |   27.23µs |  16.28µs |          1.7× |        4.9× |
| GLAD (dav1dde)           |  977.43µs | 598.40µs |          1.6× |        181× |

