### Full VK context (libvulkan transient)

| Loader                   | Unpatched |   Patched | Patch speedup | vs. fastest |
|--------------------------|----------:|----------:|--------------:|------------:|
| **Gloam (enabled-list)** | 1912.87µs | 2132.43µs |          0.9× |        1.0× |
| Volk                     | 2318.80µs | 2161.77µs |          1.1× |        1.0× |
| GLAD (tycho)             | 2293.83µs | 2170.93µs |          1.1× |        1.0× |
| Gloam (discover)         | 2296.83µs | 2176.47µs |          1.1× |        1.0× |
| GLAD (dav1dde)           | 3923.67µs | 3248.93µs |          1.2× |        1.5× |

