### Full VK context (libvulkan persistent)

| Loader                   |  Unpatched |   Patched | Patch speedup | vs. fastest |
|--------------------------|-----------:|----------:|--------------:|------------:|
| **Gloam (enabled-list)** |  7265.83µs | 4380.83µs |          1.7× |        1.0× |
| GLAD (tycho)             |  8599.87µs | 4437.27µs |          1.9× |        1.0× |
| Volk                     |  7396.93µs | 4734.47µs |          1.6× |        1.1× |
| Gloam (discover)         |  8515.60µs | 4857.00µs |          1.8× |        1.1× |
| GLAD (dav1dde)           | 10139.97µs | 6069.97µs |          1.7× |        1.4× |

