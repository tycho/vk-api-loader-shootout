### Init + load all functions

| Loader                   | Unpatched |  Patched | Patch speedup | vs. fastest |
|--------------------------|----------:|---------:|--------------:|------------:|
| **Gloam (enabled-list)** |   38.33µs |   7.83µs |          4.9× |        1.0× |
| Volk                     |  390.50µs |  54.63µs |          7.1× |        7.0× |
| GLAD (tycho)             |  792.57µs |  76.13µs |         10.4× |        9.7× |
| Gloam (discover)         |  800.73µs |  76.20µs |         10.5× |        9.7× |
| GLAD (dav1dde)           | 2219.37µs | 784.20µs |          2.8× |        100× |

