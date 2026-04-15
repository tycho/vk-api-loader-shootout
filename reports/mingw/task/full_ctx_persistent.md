### Full VK context (libvulkan persistent)

| Loader               |  Unpatched |    Patched | Patch speedup | vs. fastest |
|----------------------|-----------:|-----------:|--------------:|------------:|
| **Volk**             | 34081.53µs | 22701.10µs |          1.5× |        1.0× |
| Gloam (enabled-list) | 33536.03µs | 22912.20µs |          1.5× |        1.0× |
| GLAD (tycho)         | 35774.10µs | 23035.10µs |          1.6× |        1.0× |
| Gloam (discover)     | 35472.43µs | 23486.10µs |          1.5× |        1.0× |
| GLAD (dav1dde)       | 40765.50µs | 25296.33µs |          1.6× |        1.1× |

