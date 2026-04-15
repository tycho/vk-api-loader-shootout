## At a glance

Lower is better. **Winner** = best patched average on this host for each task.

| Task                                   | Winner                   | Patched avg | Unpatched avg | Patch speedup |
|----------------------------------------|--------------------------|------------:|--------------:|--------------:|
| Load instance functions                | **Gloam (enabled-list)** |      1.34µs |        5.31µs |          4.0× |
| Load device functions                  | **Gloam (enabled-list)** |      3.46µs |       45.36µs |         13.1× |
| Init + load all functions              | **Gloam (enabled-list)** |      5.43µs |       49.57µs |          9.1× |
| Full VK context (libvulkan persistent) | **Volk**                 |  22701.10µs |    34081.53µs |          1.5× |
| Full VK context (libvulkan transient)  | **Volk**                 |  33704.73µs |    34741.27µs |          1.0× |

