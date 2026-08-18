### Init + load all functions

| Loader                   | Unpatched |   Patched | Patch speedup | vs. fastest |
|--------------------------|----------:|----------:|--------------:|------------:|
| **Gloam (enabled-list)** |   54.20µs |    5.83µs |          9.3× |        1.0× |
| Volk                     |  695.20µs |   13.53µs |         51.4× |        2.3× |
| Gloam (discover)         | 3306.63µs |   75.27µs |         43.9× |       12.9× |
| GLAD (tycho)             | 3443.70µs |   78.00µs |         44.1× |       13.4× |
| GLAD (dav1dde)           | 6920.37µs | 1063.97µs |          6.5× |        182× |

