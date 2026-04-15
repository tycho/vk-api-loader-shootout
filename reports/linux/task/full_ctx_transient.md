### Full VK context (libvulkan transient)

| Loader               |  Unpatched |   Patched | Patch speedup | vs. fastest |
|----------------------|-----------:|----------:|--------------:|------------:|
| **GLAD (tycho)**     |  9034.50µs | 8135.77µs |          1.1× |        1.0× |
| Volk                 |  8349.53µs | 8238.67µs |          1.0× |        1.0× |
| Gloam (discover)     |  9083.13µs | 8301.73µs |          1.1× |        1.0× |
| Gloam (enabled-list) |  8303.40µs | 8428.87µs |          1.0× |        1.0× |
| GLAD (dav1dde)       | 10561.17µs | 9168.83µs |          1.2× |        1.1× |

