### Init + load all functions

| Loader                   | Unpatched |  Patched | Patch speedup | vs. fastest |
|--------------------------|----------:|---------:|--------------:|------------:|
| **Gloam (enabled-list)** |    7.27µs |   4.47µs |          1.6× |        1.0× |
| Volk                     |   25.10µs |  10.17µs |          2.5× |        2.3× |
| Gloam (discover)         |  544.60µs |  34.40µs |         15.8× |        7.7× |
| GLAD (tycho)             |  547.17µs |  39.23µs |         13.9× |        8.8× |
| GLAD (dav1dde)           | 1691.70µs | 722.27µs |          2.3× |        162× |

