## At a glance

Lower is better. **Winner** = best patched average on this host for each task.

| Task                                   | Winner                   | Patched avg | Unpatched avg | Patch speedup |
|----------------------------------------|--------------------------|------------:|--------------:|--------------:|
| Load instance functions                | **Gloam (enabled-list)** |      1.51µs |        3.17µs |          2.1× |
| Load device functions                  | **Gloam (enabled-list)** |      5.85µs |       34.07µs |          5.8× |
| Init + load all functions              | **Gloam (enabled-list)** |      7.83µs |       38.33µs |          4.9× |
| Full VK context (libvulkan persistent) | **GLAD (tycho)**         |   5681.70µs |     8905.70µs |          1.6× |
| Full VK context (libvulkan transient)  | **GLAD (tycho)**         |   8135.77µs |     9034.50µs |          1.1× |

