### Load device functions

| Loader                   | Unpatched |  Patched | Patch speedup | vs. fastest |
|--------------------------|----------:|---------:|--------------:|------------:|
| **Gloam (enabled-list)** |   46.21µs |   4.28µs |         10.8× |        1.0× |
| Volk                     |  541.21µs |  10.15µs |         53.3× |        2.4× |
| GLAD (tycho)             |  433.67µs |  11.07µs |         39.2× |        2.6× |
| Gloam (discover)         |  511.23µs |  23.67µs |         21.6× |        5.5× |
| GLAD (dav1dde)           | 3634.71µs | 764.79µs |          4.8× |        179× |

