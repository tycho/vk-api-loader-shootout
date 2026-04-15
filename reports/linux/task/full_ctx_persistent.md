### Full VK context (libvulkan persistent)

| Loader               |  Unpatched |   Patched | Patch speedup | vs. fastest |
|----------------------|-----------:|----------:|--------------:|------------:|
| **GLAD (tycho)**     |  8905.70µs | 5681.70µs |          1.6× |        1.0× |
| Volk                 |  8759.50µs | 5724.77µs |          1.5× |        1.0× |
| Gloam (discover)     |  8905.17µs | 5798.67µs |          1.5× |        1.0× |
| Gloam (enabled-list) |  8187.17µs | 5905.70µs |          1.4× |        1.0× |
| GLAD (dav1dde)       | 10469.83µs | 6570.27µs |          1.6× |        1.2× |

