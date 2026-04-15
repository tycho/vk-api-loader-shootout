## At a glance

Lower is better. **Winner** = best patched average on this host for each task.

| Task                                   | Winner                   | Patched avg | Unpatched avg | Patch speedup |
|----------------------------------------|--------------------------|------------:|--------------:|--------------:|
| Load instance functions                | **Gloam (enabled-list)** |      1.16µs |        5.38µs |          4.6× |
| Load device functions                  | **Gloam (enabled-list)** |      3.25µs |       30.04µs |          9.2× |
| Init + load all functions              | **Gloam (enabled-list)** |      5.07µs |       35.23µs |          6.9× |
| Full VK context (libvulkan persistent) | **Volk**                 |   1313.57µs |     1992.53µs |          1.5× |
| Full VK context (libvulkan transient)  | **Gloam (enabled-list)** |   2132.43µs |     1912.87µs |          0.9× |

