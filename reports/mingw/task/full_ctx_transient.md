### Full VK context (libvulkan transient)

| Loader                   |  Unpatched |    Patched | Patch speedup | vs. fastest |
|--------------------------|-----------:|-----------:|--------------:|------------:|
| **Gloam (enabled-list)** | 36125.40µs | 35731.30µs |          1.0× |        1.0× |
| Gloam (discover)         | 39094.23µs | 36321.73µs |          1.1× |        1.0× |
| Volk                     | 37866.63µs | 37177.77µs |          1.0× |        1.0× |
| GLAD (tycho)             | 39154.10µs | 37178.27µs |          1.1× |        1.0× |
| GLAD (dav1dde)           | 44330.80µs | 38640.37µs |          1.1× |        1.1× |

