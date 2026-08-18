### Full VK context (libvulkan persistent)

| Loader               | Unpatched |   Patched | Patch speedup | vs. fastest |
|----------------------|----------:|----------:|--------------:|------------:|
| **Volk**             | 1657.27µs | 1379.73µs |          1.2× |        1.0× |
| Gloam (enabled-list) | 1651.93µs | 1403.43µs |          1.2× |        1.0× |
| Gloam (discover)     | 1893.13µs | 1436.73µs |          1.3× |        1.0× |
| GLAD (tycho)         | 1883.83µs | 1487.80µs |          1.3× |        1.1× |
| GLAD (dav1dde)       | 3236.33µs | 2522.37µs |          1.3× |        1.8× |

