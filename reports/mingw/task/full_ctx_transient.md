### Full VK context (libvulkan transient)

| Loader               |  Unpatched |    Patched | Patch speedup | vs. fastest |
|----------------------|-----------:|-----------:|--------------:|------------:|
| **Volk**             | 34741.27µs | 33704.73µs |          1.0× |        1.0× |
| Gloam (enabled-list) | 33702.63µs | 33725.73µs |          1.0× |        1.0× |
| GLAD (tycho)         | 36492.77µs | 33859.93µs |          1.1× |        1.0× |
| Gloam (discover)     | 35863.90µs | 34128.70µs |          1.1× |        1.0× |
| GLAD (dav1dde)       | 41029.63µs | 35542.80µs |          1.2× |        1.1× |

