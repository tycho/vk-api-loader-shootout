### Full VK context (libvulkan persistent)

| Loader               | Unpatched |   Patched | Patch speedup | vs. fastest |
|----------------------|----------:|----------:|--------------:|------------:|
| **Volk**             | 1992.53µs | 1313.57µs |          1.5× |        1.0× |
| Gloam (enabled-list) | 1630.13µs | 1371.50µs |          1.2× |        1.0× |
| Gloam (discover)     | 2010.17µs | 1385.27µs |          1.5× |        1.1× |
| GLAD (tycho)         | 2006.93µs | 1418.60µs |          1.4× |        1.1× |
| GLAD (dav1dde)       | 3582.93µs | 2474.70µs |          1.4× |        1.9× |

