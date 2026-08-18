### Full VK context (libvulkan transient)

| Loader                   |  Unpatched |   Patched | Patch speedup | vs. fastest |
|--------------------------|-----------:|----------:|--------------:|------------:|
| **Gloam (enabled-list)** |  6920.23µs | 8060.87µs |          0.9× |        1.0× |
| Volk                     |  7763.93µs | 8201.30µs |          0.9× |        1.0× |
| GLAD (tycho)             |  8631.13µs | 8298.83µs |          1.0× |        1.0× |
| Gloam (discover)         |  8663.10µs | 8363.73µs |          1.0× |        1.0× |
| GLAD (dav1dde)           | 10336.70µs | 8900.13µs |          1.2× |        1.1× |

