## At a glance

Lower is better. **Winner** = best patched average on this host for each task.

| Task                                   | Winner                   | Patched avg | Unpatched avg | Patch speedup |
|----------------------------------------|--------------------------|------------:|--------------:|--------------:|
| Load instance functions                | **Gloam (enabled-list)** |      0.97µs |        5.08µs |          5.2× |
| Load device functions                  | **Gloam (enabled-list)** |      4.28µs |       46.21µs |         10.8× |
| Init + load all functions              | **Gloam (enabled-list)** |      5.83µs |       54.20µs |          9.3× |
| Full VK context (libvulkan persistent) | **Gloam (discover)**     |  23913.73µs |    39178.07µs |          1.6× |
| Full VK context (libvulkan transient)  | **Gloam (enabled-list)** |  35731.30µs |    36125.40µs |          1.0× |

