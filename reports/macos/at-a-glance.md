## At a glance

Lower is better. **Winner** = best patched average on this host for each task.

| Task                                   | Winner                   | Patched avg | Unpatched avg | Patch speedup |
|----------------------------------------|--------------------------|------------:|--------------:|--------------:|
| Load instance functions                | **Gloam (enabled-list)** |      0.83µs |        1.13µs |          1.4× |
| Load device functions                  | **Gloam (enabled-list)** |      3.42µs |        5.43µs |          1.6× |
| Init + load all functions              | **Gloam (enabled-list)** |      5.00µs |        7.20µs |          1.4× |
| Full VK context (libvulkan persistent) | **Volk**                 |   1379.73µs |     1657.27µs |          1.2× |
| Full VK context (libvulkan transient)  | **Gloam (enabled-list)** |   2247.83µs |     1944.57µs |          0.9× |

