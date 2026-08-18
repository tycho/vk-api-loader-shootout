## At a glance

Lower is better. **Winner** = best patched average on this host for each task.

| Task                                   | Winner                   | Patched avg | Unpatched avg | Patch speedup |
|----------------------------------------|--------------------------|------------:|--------------:|--------------:|
| Load instance functions                | **Gloam (enabled-list)** |      0.72µs |        1.17µs |          1.6× |
| Load device functions                  | **Gloam (enabled-list)** |      3.31µs |        5.60µs |          1.7× |
| Init + load all functions              | **Gloam (enabled-list)** |      4.47µs |        7.27µs |          1.6× |
| Full VK context (libvulkan persistent) | **Gloam (enabled-list)** |   4380.83µs |     7265.83µs |          1.7× |
| Full VK context (libvulkan transient)  | **Gloam (enabled-list)** |   8060.87µs |     6920.23µs |          0.9× |

