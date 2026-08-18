### Full VK context (libvulkan transient)

| Loader                   | Unpatched |   Patched | Patch speedup | vs. fastest |
|--------------------------|----------:|----------:|--------------:|------------:|
| **Gloam (enabled-list)** | 1944.57µs | 2247.83µs |          0.9× |        1.0× |
| Volk                     | 1968.47µs | 2257.23µs |          0.9× |        1.0× |
| GLAD (tycho)             | 2159.80µs | 2282.53µs |          0.9× |        1.0× |
| Gloam (discover)         | 2176.73µs | 2283.10µs |          1.0× |        1.0× |
| GLAD (dav1dde)           | 3556.23µs | 3352.43µs |          1.1× |        1.5× |

