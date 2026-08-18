### Full VK context (libvulkan persistent)

| Loader               |  Unpatched |    Patched | Patch speedup | vs. fastest |
|----------------------|-----------:|-----------:|--------------:|------------:|
| **Gloam (discover)** | 39178.07µs | 23913.73µs |          1.6× |        1.0× |
| Volk                 | 36831.83µs | 24580.90µs |          1.5× |        1.0× |
| Gloam (enabled-list) | 36001.67µs | 25128.03µs |          1.4× |        1.1× |
| GLAD (tycho)         | 38912.97µs | 25175.33µs |          1.5× |        1.1× |
| GLAD (dav1dde)       | 43235.43µs | 26652.23µs |          1.6× |        1.1× |

