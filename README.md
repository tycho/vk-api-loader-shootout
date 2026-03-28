# Vulkan API Loader Shootout

A benchmark comparing Vulkan API function-pointer loaders:
[GLAD (upstream)](https://github.com/Dav1dde/glad),
[GLAD (tycho)](https://github.com/tycho/glad),
[Gloam](https://github.com/tycho/gloam), and
[Volk](https://github.com/zeux/volk).

GLAD (tycho) is a fork of upstream GLAD that re-added global multi-context
support, introduced xxHash-based extension matching, and added a PFN range
feature for loading feature levels and extensions by numeric index. Gloam is a
from-scratch Rust rewrite based on the generated output design of the GLAD
fork. It generates all supported loaders (Vulkan, OpenGL, OpenGL ES, EGL, WGL,
GLX) in ~0.2s versus ~13s for the Python-based GLAD, and adds the Vulkan
enabled-list loading mode and optional compatibility with the upstream Vulkan
headers (making it a practical drop-in replacement for Volk in existing
projects).

Each loader is benchmarked on five tasks:

1. **Load instance functions** / **Load device functions** — re-loading
   function pointers into an already-initialized context (measures the raw
   `vkGet*ProcAddr` path).
2. **Init + load all functions** — tearing down the API loader and
   reinitializing it against an existing VkInstance and VkDevice. This
   isolates the loader's own overhead from Vulkan context creation.
3. **Full VK context (libvulkan persistent)** — destroying the VkDevice and
   VkInstance, then recreating them from scratch and reloading all function
   pointers. libvulkan stays loaded across iterations.
4. **Full VK context (libvulkan transient)** — same as above, but libvulkan
   is also unloaded and reloaded each iteration.

Gloam is tested in two modes: **discover** (automatic extension detection, like
GLAD) and **enabled-list** (the caller specifies what was enabled). The
benchmark creates a VK 1.3 instance and device with no extensions enabled
beyond `VK_KHR_portability_enumeration` (when available).


## Building

```
$ git submodule update --init --recursive
$ make build
$ make run
```

This should just work on Windows and Linux. On macOS, you will need to install
MoltenVK via the Vulkan SDK.


## Dependencies

You'll want the following tools installed to build:

* Python 3.x (for GLAD)
* virtualenv (for GLAD)
* util-linux (for formatting the `make run` outputs)
* sed (for formatting the `make run` outputs)
* a Rust toolchain (for Gloam)
* Clang or GCC (you can specify a compiler with `make CC=gcc CXX=g++`)

And of course you'll need a system-wide installation of libvulkan and
a functional Vulkan driver.


## Results

Each platform is tested with two builds of the Vulkan loader library: the
**unpatched** stock release, and a **patched** build with two changes:

1. **[Hash-based command name lookups](#libvulkan-is-actually-just-a-strcmp-benchmark)**
   — replaces the loader's internal `strcmp()` chain with XXH3 hash-guarded
   comparisons in `vkGetInstanceProcAddr` and `vkGetDeviceProcAddr`.
2. **[ICD and layer manifest caching](#icd-and-layer-manifest-caching)** —
   preloads ICD and layer metadata on first use and serves subsequent queries
   from an in-process cache, replacing the per-instance filesystem/registry
   scanning and JSON parsing with cheap stat()-based revalidation.

Both patches are explained in detail at the end of this document. The patched
source is available at [github.com/tycho/Vulkan-Loader](https://github.com/tycho/Vulkan-Loader/)
and passes the full Vulkan-Loader test suite on Linux, Windows, and macOS.

### Linux

| Project        | Version (git describe)      |
|----------------|-----------------------------|
| GLAD (dav1dde) | `2.0.8-8-ga4ca574` |
| GLAD (tycho)   | `2.0.8-91-g8092eae`   |
| gloam          | `0.4.6`        |
| Volk           | `1.4.341.0-21-g78463da`         |
| xxHash         | `0.8.3`       |
| Vulkan-Headers | `1.4.347-1-gf07ffc2`   |

| Tool         | Name             | Version             |
|--------------|------------------|---------------------|
| Kernel       | `Linux`   | `6.19.8-1-hsw`      |
| Architecture | `x86_64`   | `AMD Ryzen AI 9 HX 370 w/ Radeon 890M`    |
| CC           | `clang`        | `22.1.1`   |
| CXX          | `clang++`       | `22.1.1`  |

| Variable     | Value         |
|--------------|---------------|
| `OPTFLAGS`   | `-O2 -fno-unroll-loops -march=x86-64-v2 -mtune=znver3 -g0` |
| `CFLAGS`     | `-std=c17` |
| `CXXFLAGS`   | `-std=c++20` |

#### Unpatched libvulkan

[GLAD (dav1dde)](https://github.com/Dav1dde/glad)
| Task                                       | Iterations | Total Time (µs) | Average Time (µs) |
|--------------------------------------------|------------|-----------------|-------------------|
| Load instance functions                    |        300 |          131985 |            439.95 |
| Load device functions                      |        300 |          662356 |           2207.85 |
| Init + load all functions                  |         30 |           95199 |           3173.30 |
| Full VK context (libvulkan persistent)     |         30 |          296979 |           9899.30 |
| Full VK context (libvulkan transient)      |         30 |          295954 |           9865.13 |

[GLAD (tycho)](https://github.com/tycho/glad)
| Task                                       | Iterations | Total Time (µs) | Average Time (µs) |
|--------------------------------------------|------------|-----------------|-------------------|
| Load instance functions                    |        300 |           11501 |             38.34 |
| Load device functions                      |        300 |          123917 |            413.06 |
| Init + load all functions                  |         30 |           30112 |           1003.73 |
| Full VK context (libvulkan persistent)     |         30 |          209579 |           6985.97 |
| Full VK context (libvulkan transient)      |         30 |          212722 |           7090.73 |

[Gloam (discover API)](https://github.com/tycho/gloam)
| Task                                       | Iterations | Total Time (µs) | Average Time (µs) |
|--------------------------------------------|------------|-----------------|-------------------|
| Load instance functions                    |        300 |           84656 |            282.19 |
| Load device functions                      |        300 |          127428 |            424.76 |
| Init + load all functions                  |         30 |           30673 |           1022.43 |
| Full VK context (libvulkan persistent)     |         30 |          206029 |           6867.63 |
| Full VK context (libvulkan transient)      |         30 |          207126 |           6904.20 |

[Gloam (enabled-list API)](https://github.com/tycho/gloam)
| Task                                       | Iterations | Total Time (µs) | Average Time (µs) |
|--------------------------------------------|------------|-----------------|-------------------|
| Load instance functions                    |        300 |            1546 |              5.15 |
| Load device functions                      |        300 |           17556 |             58.52 |
| Init + load all functions                  |         30 |            1926 |             64.20 |
| Full VK context (libvulkan persistent)     |         30 |          173208 |           5773.60 |
| Full VK context (libvulkan transient)      |         30 |          183514 |           6117.13 |

[Volk](https://github.com/zeux/volk)
| Task                                       | Iterations | Total Time (µs) | Average Time (µs) |
|--------------------------------------------|------------|-----------------|-------------------|
| Load instance functions                    |        300 |           18032 |             60.11 |
| Load device functions                      |        300 |          144114 |            480.38 |
| Init + load all functions                  |         30 |           16320 |            544.00 |
| Full VK context (libvulkan persistent)     |         30 |          193046 |           6434.87 |
| Full VK context (libvulkan transient)      |         30 |          193523 |           6450.77 |

#### [Patched libvulkan](#libvulkan-is-actually-just-a-strcmp-benchmark)

[GLAD (dav1dde)](https://github.com/Dav1dde/glad)
| Task                                       | Iterations | Total Time (µs) | Average Time (µs) |
|--------------------------------------------|------------|-----------------|-------------------|
| Load instance functions                    |        300 |           21165 |             70.55 |
| Load device functions                      |        300 |          298206 |            994.02 |
| Init + load all functions                  |         30 |           36384 |           1212.80 |
| Full VK context (libvulkan persistent)     |         30 |          181078 |           6035.93 |
| Full VK context (libvulkan transient)      |         30 |          213964 |           7132.13 |

[GLAD (tycho)](https://github.com/tycho/glad)
| Task                                       | Iterations | Total Time (µs) | Average Time (µs) |
|--------------------------------------------|------------|-----------------|-------------------|
| Load instance functions                    |        300 |            3116 |             10.39 |
| Load device functions                      |        300 |           20893 |             69.64 |
| Init + load all functions                  |         30 |            3885 |            129.50 |
| Full VK context (libvulkan persistent)     |         30 |          131024 |           4367.47 |
| Full VK context (libvulkan transient)      |         30 |          192259 |           6408.63 |

[Gloam (discover API)](https://github.com/tycho/gloam)
| Task                                       | Iterations | Total Time (µs) | Average Time (µs) |
|--------------------------------------------|------------|-----------------|-------------------|
| Load instance functions                    |        300 |            7746 |             25.82 |
| Load device functions                      |        300 |           25386 |             84.62 |
| Init + load all functions                  |         30 |            3851 |            128.37 |
| Full VK context (libvulkan persistent)     |         30 |          128862 |           4295.40 |
| Full VK context (libvulkan transient)      |         30 |          181069 |           6035.63 |

[Gloam (enabled-list API)](https://github.com/tycho/gloam)
| Task                                       | Iterations | Total Time (µs) | Average Time (µs) |
|--------------------------------------------|------------|-----------------|-------------------|
| Load instance functions                    |        300 |             809 |              2.70 |
| Load device functions                      |        300 |            3068 |             10.23 |
| Init + load all functions                  |         30 |             419 |             13.97 |
| Full VK context (libvulkan persistent)     |         30 |          118096 |           3936.53 |
| Full VK context (libvulkan transient)      |         30 |          166981 |           5566.03 |

[Volk](https://github.com/zeux/volk)
| Task                                       | Iterations | Total Time (µs) | Average Time (µs) |
|--------------------------------------------|------------|-----------------|-------------------|
| Load instance functions                    |        300 |            3947 |             13.16 |
| Load device functions                      |        300 |           22154 |             73.85 |
| Init + load all functions                  |         30 |            2647 |             88.23 |
| Full VK context (libvulkan persistent)     |         30 |          125695 |           4189.83 |
| Full VK context (libvulkan transient)      |         30 |          178109 |           5936.97 |

#### Binary sizes

Loader object sizes in bytes:
```
316288  obj/loader-glad-dav1dde.o
 67192  obj/loader-glad-tycho.o
 50488  obj/loader-gloam-discover.o
 50488  obj/loader-gloam.o
312912  obj/loader-volk.o
```

Test program sizes in bytes:
```
112992  bin/test-glad-dav1dde
 80288  bin/test-glad-tycho
 59808  bin/test-gloam
 59792  bin/test-gloam-discover
100664  bin/test-volk
```

Section sizes:
```
  text  data  bss   dec     hex    filename
101267  832   6568  108667  1a87b  bin/test-glad-dav1dde
 63900  7360  7040  78300   131dc  bin/test-glad-tycho
 51890  840   7064  59794   e992   bin/test-gloam
 50937  824   7056  58817   e5c1   bin/test-gloam-discover
 91170  792   6040  98002   17ed2  bin/test-volk
```

#### Test hardware

vulkaninfo
```
Devices:
========
GPU0:
	apiVersion         = 1.4.346
	driverVersion      = 26.0.99
	vendorID           = 0x1002
	deviceID           = 0x150e
	deviceType         = PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU
	deviceName         = AMD Radeon 890M Graphics (RADV STRIX1)
	driverID           = DRIVER_ID_MESA_RADV
	driverName         = radv
	driverInfo         = Mesa 26.1.0-devel (git-ee031d67b4)
	conformanceVersion = 1.4.0.0
	deviceUUID         = 00000000-c100-0000-0000-000000000000
	driverUUID         = 414d442d-4d45-5341-2d44-525600000000
```

### MinGW

| Project        | Version (git describe)      |
|----------------|-----------------------------|
| GLAD (dav1dde) | `2.0.8-8-ga4ca574` |
| GLAD (tycho)   | `2.0.8-91-g8092eae`   |
| gloam          | `0.4.6`        |
| Volk           | `1.4.341.0-21-g78463da`         |
| xxHash         | `0.8.3`       |
| Vulkan-Headers | `1.4.347-1-gf07ffc2`   |

| Tool         | Name             | Version             |
|--------------|------------------|---------------------|
| Kernel       | `MINGW64_NT-10.0-26200`   | `3.6.7-6e9c4de9.x86_64`      |
| Architecture | `x86_64`   | `AMD Ryzen 9 9950X 16-Core Processor`    |
| CC           | `clang`        | `21.1.8`   |
| CXX          | `clang++`       | `21.1.8`  |

| Variable     | Value         |
|--------------|---------------|
| `OPTFLAGS`   | `-O2 -fno-unroll-loops -march=x86-64-v2 -mtune=znver3 -g0` |
| `CFLAGS`     | `-std=c17` |
| `CXXFLAGS`   | `-std=c++20` |

#### Unpatched libvulkan

[GLAD (dav1dde)](https://github.com/Dav1dde/glad)
| Task                                       | Iterations | Total Time (µs) | Average Time (µs) |
|--------------------------------------------|------------|-----------------|-------------------|
| Load instance functions                    |        300 |          653799 |           2179.33 |
| Load device functions                      |        300 |         1185605 |           3952.02 |
| Init + load all functions                  |         30 |          247942 |           8264.73 |
| Full VK context (libvulkan persistent)     |         30 |         1365128 |          45504.27 |
| Full VK context (libvulkan transient)      |         30 |         1466317 |          48877.23 |

[GLAD (tycho)](https://github.com/tycho/glad)
| Task                                       | Iterations | Total Time (µs) | Average Time (µs) |
|--------------------------------------------|------------|-----------------|-------------------|
| Load instance functions                    |        300 |            8477 |             28.26 |
| Load device functions                      |        300 |          110480 |            368.27 |
| Init + load all functions                  |         30 |          126871 |           4229.03 |
| Full VK context (libvulkan persistent)     |         30 |         1166991 |          38899.70 |
| Full VK context (libvulkan transient)      |         30 |         1187219 |          39573.97 |

[Gloam (discover API)](https://github.com/tycho/gloam)
| Task                                       | Iterations | Total Time (µs) | Average Time (µs) |
|--------------------------------------------|------------|-----------------|-------------------|
| Load instance functions                    |        300 |          488904 |           1629.68 |
| Load device functions                      |        300 |          122468 |            408.23 |
| Init + load all functions                  |         30 |          123900 |           4130.00 |
| Full VK context (libvulkan persistent)     |         30 |         1169809 |          38993.63 |
| Full VK context (libvulkan transient)      |         30 |         1189663 |          39655.43 |

[Gloam (enabled-list API)](https://github.com/tycho/gloam)
| Task                                       | Iterations | Total Time (µs) | Average Time (µs) |
|--------------------------------------------|------------|-----------------|-------------------|
| Load instance functions                    |        300 |            1416 |              4.72 |
| Load device functions                      |        300 |           12038 |             40.13 |
| Init + load all functions                  |         30 |            1372 |             45.73 |
| Full VK context (libvulkan persistent)     |         30 |         1093681 |          36456.03 |
| Full VK context (libvulkan transient)      |         30 |         1108127 |          36937.57 |

[Volk](https://github.com/zeux/volk)
| Task                                       | Iterations | Total Time (µs) | Average Time (µs) |
|--------------------------------------------|------------|-----------------|-------------------|
| Load instance functions                    |        300 |           20096 |             66.99 |
| Load device functions                      |        300 |          156321 |            521.07 |
| Init + load all functions                  |         30 |           18837 |            627.90 |
| Full VK context (libvulkan persistent)     |         30 |         1100724 |          36690.80 |
| Full VK context (libvulkan transient)      |         30 |         1131219 |          37707.30 |

#### [Patched libvulkan](#libvulkan-is-actually-just-a-strcmp-benchmark)

[GLAD (dav1dde)](https://github.com/Dav1dde/glad)
| Task                                       | Iterations | Total Time (µs) | Average Time (µs) |
|--------------------------------------------|------------|-----------------|-------------------|
| Load instance functions                    |        300 |           30988 |            103.29 |
| Load device functions                      |        300 |          220176 |            733.92 |
| Init + load all functions                  |         30 |           29325 |            977.50 |
| Full VK context (libvulkan persistent)     |         30 |          935127 |          31170.90 |
| Full VK context (libvulkan transient)      |         30 |         1267788 |          42259.60 |

[GLAD (tycho)](https://github.com/tycho/glad)
| Task                                       | Iterations | Total Time (µs) | Average Time (µs) |
|--------------------------------------------|------------|-----------------|-------------------|
| Load instance functions                    |        300 |            2026 |              6.75 |
| Load device functions                      |        300 |           14464 |             48.21 |
| Init + load all functions                  |         30 |            3179 |            105.97 |
| Full VK context (libvulkan persistent)     |         30 |          886126 |          29537.53 |
| Full VK context (libvulkan transient)      |         30 |         1234728 |          41157.60 |

[Gloam (discover API)](https://github.com/tycho/gloam)
| Task                                       | Iterations | Total Time (µs) | Average Time (µs) |
|--------------------------------------------|------------|-----------------|-------------------|
| Load instance functions                    |        300 |            6924 |             23.08 |
| Load device functions                      |        300 |           17728 |             59.09 |
| Init + load all functions                  |         30 |            3174 |            105.80 |
| Full VK context (libvulkan persistent)     |         30 |          811194 |          27039.80 |
| Full VK context (libvulkan transient)      |         30 |         1130850 |          37695.00 |

[Gloam (enabled-list API)](https://github.com/tycho/gloam)
| Task                                       | Iterations | Total Time (µs) | Average Time (µs) |
|--------------------------------------------|------------|-----------------|-------------------|
| Load instance functions                    |        300 |             704 |              2.35 |
| Load device functions                      |        300 |            3044 |             10.15 |
| Init + load all functions                  |         30 |             394 |             13.13 |
| Full VK context (libvulkan persistent)     |         30 |          888302 |          29610.07 |
| Full VK context (libvulkan transient)      |         30 |         1148792 |          38293.07 |

[Volk](https://github.com/zeux/volk)
| Task                                       | Iterations | Total Time (µs) | Average Time (µs) |
|--------------------------------------------|------------|-----------------|-------------------|
| Load instance functions                    |        300 |            3391 |             11.30 |
| Load device functions                      |        300 |           17315 |             57.72 |
| Init + load all functions                  |         30 |            2080 |             69.33 |
| Full VK context (libvulkan persistent)     |         30 |          889205 |          29640.17 |
| Full VK context (libvulkan transient)      |         30 |         1201134 |          40037.80 |

#### Binary sizes

Loader object sizes in bytes:
```
192545  obj/loader-glad-dav1dde.o
 52988  obj/loader-glad-tycho.o
 46689  obj/loader-gloam.o
 46689  obj/loader-gloam-discover.o
221552  obj/loader-volk.o
```

Test program sizes in bytes:
```
114688  bin/test-glad-dav1dde.exe
 66048  bin/test-glad-tycho.exe
 64512  bin/test-gloam.exe
 64000  bin/test-gloam-discover.exe
103936  bin/test-volk.exe
```

Section sizes:
```
 text  data   bss  dec     hex    filename
72438  39600  0    112038  1b5a6  bin/test-glad-dav1dde.exe
16422  46424  0    62846   f57e   bin/test-glad-tycho.exe
20374  41504  0    61878   f1b6   bin/test-gloam.exe
19766  41432  0    61198   ef0e   bin/test-gloam-discover.exe
72166  29536  0    101702  18d46  bin/test-volk.exe
```

#### Test hardware

vulkaninfo
```
Devices:
========
GPU0:
        apiVersion         = 1.4.315
        driverVersion      = 2.0.353
        vendorID           = 0x1002
        deviceID           = 0x13c0
        deviceType         = PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU
        deviceName         = AMD Radeon(TM) Graphics
        driverID           = DRIVER_ID_AMD_PROPRIETARY
        driverName         = AMD proprietary driver
        driverInfo         = 26.4.1 (AMD proprietary shader compiler)
        conformanceVersion = 1.4.0.0
        deviceUUID         = 00000000-7900-0000-0000-000000000000
        driverUUID         = 414d442d-5749-4e2d-4452-560000000000
```

### macOS

| Project        | Version (git describe)      |
|----------------|-----------------------------|
| GLAD (dav1dde) | `2.0.8-8-ga4ca574` |
| GLAD (tycho)   | `2.0.8-91-g8092eae`   |
| gloam          | `0.4.6`        |
| Volk           | `1.4.341.0-21-g78463da`         |
| xxHash         | `0.8.3`       |
| Vulkan-Headers | `1.4.347-1-gf07ffc2`   |

| Tool         | Name             | Version             |
|--------------|------------------|---------------------|
| Kernel       | `Darwin`   | `25.4.0`      |
| Architecture | `arm64`   | `Apple M5`    |
| CC           | `clang`        | `21.0.0`   |
| CXX          | `clang++`       | `21.0.0`  |

| Variable     | Value         |
|--------------|---------------|
| `OPTFLAGS`   | `-O2 -fno-unroll-loops -mcpu=native -g0` |
| `CFLAGS`     | `-std=c17` |
| `CXXFLAGS`   | `-std=c++20` |

#### Unpatched libvulkan

[GLAD (dav1dde)](https://github.com/Dav1dde/glad)
| Task                                       | Iterations | Total Time (µs) | Average Time (µs) |
|--------------------------------------------|------------|-----------------|-------------------|
| Load instance functions                    |        300 |          160976 |            536.59 |
| Load device functions                      |        300 |          362970 |           1209.90 |
| Init + load all functions                  |         30 |           68031 |           2267.70 |
| Full VK context (libvulkan persistent)     |         30 |          124029 |           4134.30 |
| Full VK context (libvulkan transient)      |         30 |          134474 |           4482.47 |

[GLAD (tycho)](https://github.com/tycho/glad)
| Task                                       | Iterations | Total Time (µs) | Average Time (µs) |
|--------------------------------------------|------------|-----------------|-------------------|
| Load instance functions                    |        300 |            6615 |             22.05 |
| Load device functions                      |        300 |           41091 |            136.97 |
| Init + load all functions                  |         30 |           22935 |            764.50 |
| Full VK context (libvulkan persistent)     |         30 |           69459 |           2315.30 |
| Full VK context (libvulkan transient)      |         30 |           78212 |           2607.07 |

[Gloam (discover API)](https://github.com/tycho/gloam)
| Task                                       | Iterations | Total Time (µs) | Average Time (µs) |
|--------------------------------------------|------------|-----------------|-------------------|
| Load instance functions                    |        300 |           90102 |            300.34 |
| Load device functions                      |        300 |           44001 |            146.67 |
| Init + load all functions                  |         30 |           22613 |            753.77 |
| Full VK context (libvulkan persistent)     |         30 |           69779 |           2325.97 |
| Full VK context (libvulkan transient)      |         30 |           78634 |           2621.13 |

[Gloam (enabled-list API)](https://github.com/tycho/gloam)
| Task                                       | Iterations | Total Time (µs) | Average Time (µs) |
|--------------------------------------------|------------|-----------------|-------------------|
| Load instance functions                    |        300 |            1308 |              4.36 |
| Load device functions                      |        300 |            8805 |             29.35 |
| Init + load all functions                  |         30 |            1006 |             33.53 |
| Full VK context (libvulkan persistent)     |         30 |           54899 |           1829.97 |
| Full VK context (libvulkan transient)      |         30 |           64270 |           2142.33 |

[Volk](https://github.com/zeux/volk)
| Task                                       | Iterations | Total Time (µs) | Average Time (µs) |
|--------------------------------------------|------------|-----------------|-------------------|
| Load instance functions                    |        300 |           17161 |             57.20 |
| Load device functions                      |        300 |          100753 |            335.84 |
| Init + load all functions                  |         30 |           11457 |            381.90 |
| Full VK context (libvulkan persistent)     |         30 |           67539 |           2251.30 |
| Full VK context (libvulkan transient)      |         30 |           76867 |           2562.23 |

#### [Patched libvulkan](#libvulkan-is-actually-just-a-strcmp-benchmark)

[GLAD (dav1dde)](https://github.com/Dav1dde/glad)
| Task                                       | Iterations | Total Time (µs) | Average Time (µs) |
|--------------------------------------------|------------|-----------------|-------------------|
| Load instance functions                    |        300 |           61025 |            203.42 |
| Load device functions                      |        300 |          173327 |            577.76 |
| Init + load all functions                  |         30 |           31313 |           1043.77 |
| Full VK context (libvulkan persistent)     |         30 |           74452 |           2481.73 |
| Full VK context (libvulkan transient)      |         30 |          100349 |           3344.97 |

[GLAD (tycho)](https://github.com/tycho/glad)
| Task                                       | Iterations | Total Time (µs) | Average Time (µs) |
|--------------------------------------------|------------|-----------------|-------------------|
| Load instance functions                    |        300 |             830 |              2.77 |
| Load device functions                      |        300 |            2605 |              8.68 |
| Init + load all functions                  |         30 |            2534 |             84.47 |
| Full VK context (libvulkan persistent)     |         30 |           43787 |           1459.57 |
| Full VK context (libvulkan transient)      |         30 |           68205 |           2273.50 |

[Gloam (discover API)](https://github.com/tycho/gloam)
| Task                                       | Iterations | Total Time (µs) | Average Time (µs) |
|--------------------------------------------|------------|-----------------|-------------------|
| Load instance functions                    |        300 |            6628 |             22.09 |
| Load device functions                      |        300 |            3901 |             13.00 |
| Init + load all functions                  |         30 |            2480 |             82.67 |
| Full VK context (libvulkan persistent)     |         30 |           43446 |           1448.20 |
| Full VK context (libvulkan transient)      |         30 |           68269 |           2275.63 |

[Gloam (enabled-list API)](https://github.com/tycho/gloam)
| Task                                       | Iterations | Total Time (µs) | Average Time (µs) |
|--------------------------------------------|------------|-----------------|-------------------|
| Load instance functions                    |        300 |             326 |              1.09 |
| Load device functions                      |        300 |             937 |              3.12 |
| Init + load all functions                  |         30 |             148 |              4.93 |
| Full VK context (libvulkan persistent)     |         30 |           43004 |           1433.47 |
| Full VK context (libvulkan transient)      |         30 |           66503 |           2216.77 |

[Volk](https://github.com/zeux/volk)
| Task                                       | Iterations | Total Time (µs) | Average Time (µs) |
|--------------------------------------------|------------|-----------------|-------------------|
| Load instance functions                    |        300 |             910 |              3.03 |
| Load device functions                      |        300 |            4061 |             13.54 |
| Init + load all functions                  |         30 |             517 |             17.23 |
| Full VK context (libvulkan persistent)     |         30 |           42016 |           1400.53 |
| Full VK context (libvulkan transient)      |         30 |           67531 |           2251.03 |

#### Binary sizes

Loader object sizes in bytes:
```
275592  obj/loader-glad-dav1dde.o
 74024  obj/loader-glad-tycho.o
 47432  obj/loader-gloam-discover.o
 47432  obj/loader-gloam.o
257360  obj/loader-volk.o
```

Test program sizes in bytes:
```
161344  bin/test-glad-dav1dde
 70032  bin/test-glad-tycho
 86760  bin/test-gloam
 70208  bin/test-gloam-discover
119208  bin/test-volk
```

Section sizes:
```
__TEXT  __DATA  __OBJC  others      dec         hex        
114688  16384   0       4295016448  4295147520  10002c000  bin/test-glad-dav1dde
 49152  16384   0       4295000064  4295065600  100018000  bin/test-glad-tycho
 65536  16384   0       4295000064  4295081984  10001c000  bin/test-gloam
 49152  16384   0       4295000064  4295065600  100018000  bin/test-gloam-discover
 98304  16384   0       4295000064  4295114752  100024000  bin/test-volk
```

#### Test hardware

vulkaninfo
```
Devices:
========
GPU0:
	apiVersion         = 1.4.334
	driverVersion      = 0.2.2209
	vendorID           = 0x106b
	deviceID           = 0x1a04020a
	deviceType         = PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU
	deviceName         = Apple M5
	driverID           = DRIVER_ID_MOLTENVK
	driverName         = MoltenVK
	driverInfo         = 1.4.1
	conformanceVersion = 1.4.4.0
	deviceUUID         = 0000106b-1a04-020a-0000-000000000000
	driverUUID         = 4d564b00-0000-28a1-1a04-020a00000000
GPU1:
	apiVersion         = 1.3.340
	driverVersion      = 26.0.99
	vendorID           = 0x106b
	deviceID           = 0x0064
	deviceType         = PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU
	deviceName         = Apple M5
	driverID           = DRIVER_ID_MESA_KOSMICKRISP
	driverName         = KosmicKrisp
	driverInfo         = vulkan-sdk-1.4.341.1
	conformanceVersion = 1.4.3.2
	deviceUUID         = 60050000-0100-0000-0000-000000000000
	driverUUID         = 258801e9-778a-3af1-b03f-46cef7603a2a
```

## Analysis

### The cost of extension detection

GLAD and Gloam's discover mode provide a feature that Volk does not: extension
presence flags. After loading, you can query whether a specific extension is
available:

```c
if (GLAD_VK_EXT_graphics_pipeline_library)
    ...
```

This is also available in the other GLAD/Gloam API loaders (EGL, GLX, WGL,
OpenGL, etc).

To provide this, GLAD and Gloam's discover mode call
`vkEnumerateInstanceExtensionProperties` and
`vkEnumerateDeviceExtensionProperties` at load time. These APIs are
unfortunately **very** expensive — the Vulkan loader implementation ends up
loading and unloading every ICD and scanning all implicit layers each time they
are called.

The impact is visible in the "Init + load all functions" row (Linux,
unpatched): GLAD (tycho) at ~1004µs and Gloam discover at ~1022µs, versus
~64µs for Gloam enabled-list which skips enumeration entirely. The
device-load numbers are similarly inflated because
`vkEnumerateDeviceExtensionProperties` runs during that phase: Gloam discover
at ~425µs versus ~59µs for Gloam enabled-list.

Volk sidesteps this cost entirely because it does no extension detection at
all — but this comes with trade-offs described below.

### Trade-offs in the load-everything approach

Volk is the most widely used Vulkan function-pointer loader, and for good
reason: it's fast, simple, header-only, and has zero dependencies beyond the
Vulkan headers. Its design — call `vkGetInstanceProcAddr` or
`vkGetDeviceProcAddr` for every function that compiles in on the target
platform, guarded only by compile-time platform macros like
`VK_USE_PLATFORM_METAL_EXT` — avoids the expensive enumeration calls entirely
and keeps the implementation straightforward.

This approach does come with two trade-offs that are worth being aware of:

**Ambiguous non-NULL pointers.** The Vulkan spec
[requires](https://docs.vulkan.org/spec/latest/chapters/initialization.html)
`vkGetInstanceProcAddr` to return NULL for extension commands that are not
enabled (instance extensions) or not available on any physical device (device
extensions). In practice, the Vulkan loader has a
[long-standing bug](https://github.com/KhronosGroup/Vulkan-Loader/issues/1443)
where it returns non-NULL trampoline stubs for any extension it knows about at
build time, regardless of availability. Calling through these stubs is
undefined behavior. Since Volk requests every slot unconditionally, some
populated pointers may not be safe to call, and users need to track their
enabled extensions separately to know which slots are valid.

**Manual alias resolution.** When Vulkan extensions are promoted to core, the
extension function gets a new name (e.g. `vkCmdDrawIndirectCountKHR` became
`vkCmdDrawIndirectCount` in VK 1.2). Some drivers only recognize the core
name, others only the extension name. Because each name is loaded
independently, one slot may be populated and the other NULL. Users must
manually copy pointers between the aliased slots — something the ANGLE project
has [documented](https://chromium.googlesource.com/angle/angle/+/refs/heads/main/src/common/vulkan/vk_headers.h)
as a recurring maintenance task.

### Gloam's two loading modes

Gloam offers two Vulkan loading workflows to address these trade-offs:

**Discovery mode** (`gloamLoaderLoadVulkan`) works like GLAD: it calls the
Vulkan enumeration APIs, detects available extensions, sets presence flags, and
loads the corresponding PFNs. This gives you the same extension-query
capability as GLAD, at the same cost — and with the same correctness caveat:
presence flags indicate what the driver *supports*, not what the application
*enabled*, so the caller still needs to track enabled extensions separately.

**Enabled-list mode** (`gloamVulkanInitialize` / `gloamVulkanLoadInstance` /
`gloamVulkanLoadDevice`) takes a different approach. The caller creates their
Vulkan instance and device however they like, then tells gloam what API version
and extensions were actually enabled. Gloam loads only those PFNs, sets the
corresponding presence flags, and runs alias resolution. No enumeration calls,
no heap allocation for hash arrays, and no bogus pointers.

The difference is substantial. Using the Linux unpatched-libvulkan numbers:

| Metric                           | Gloam discover | Gloam enabled-list | Speedup |
|----------------------------------|---------------:|-------------------:|--------:|
| Load instance functions (avg)    | 282.19µs       | 5.15µs             | ~55×    |
| Load device functions (avg)      | 424.76µs       | 58.52µs            | ~7×     |
| Init + load all functions (avg)  | 1022.43µs      | 64.20µs            | ~16×    |

The performance gap comes from loading only the PFNs for enabled features
and extensions rather than the entire spec. The init improvement
comes from skipping `vkEnumerateInstanceExtensionProperties` and
`vkEnumerateDeviceExtensionProperties` entirely — `gloamVulkanFinalize` is
just a conditional `dlclose` and a `memset`, and `gloamVulkanInitialize` only
loads the handful of global-scope bootstrap functions needed to create an
instance.

Enabled-list mode also provides guarantees that neither Volk nor discovery mode
can: slots for functions you didn't enable stay NULL (clean and unambiguous),
and alias resolution runs automatically so both the core and extension names
work regardless of which name the driver resolves.


### Volk vs. Gloam enabled-list

Beyond the design trade-offs discussed above, the enabled-list numbers are
also faster than Volk across the board. The difference comes down to how many
`vkGet*ProcAddr` calls each loader makes.

Volk calls `vkGetInstanceProcAddr` or `vkGetDeviceProcAddr` for every function
that compiles in on the target platform — hundreds of entry points spanning all
compiled-in versions and extensions, guarded only by compile-time platform
macros. This is simple, but it means every load cycle pays for the entire spec
even when the application only uses a small subset — and as noted above, some
of the returned pointers may not be safe to call.

Gloam's enabled-list mode loads only the PFNs for the API version and
extensions the caller actually enabled. A `gloamVulkanLoadDevice` call for a VK
1.3 device with no device extensions only loads the device-scope functions from
VK 1.0 through 1.3 core — a fraction of the full spec.

Using the Linux numbers:

| Metric                              | Volk     | Gloam enabled | Speedup |
|-------------------------------------|---------:|-------------:|--------:|
| Load device (avg, unpatched)        | 480.38µs | 58.52µs       | ~8×     |
| Load device (avg, patched)          | 73.85µs  | 10.23µs       | ~7×     |
| Init + load all (avg, unpatched)    | 544.00µs | 64.20µs       | ~8×     |
| Init + load all (avg, patched)      | 88.23µs  | 13.97µs       | ~6×     |

The gap is consistent across patched and unpatched builds: fewer lookups is
fewer lookups regardless of how fast each individual lookup is. With the
patched libvulkan, each lookup is cheaper, so both loaders improve — but the
ratio holds because Volk is still making lookups for functions the application
never asked for.


## libvulkan is actually just a strcmp() benchmark

This section is about the Vulkan loader library itself (`libvulkan.so` /
`vulkan-1.dll`) — the shared infrastructure that sits between every API loader
and the ICD. The data above shows that libvulkan is the single largest
contributor to function-loading latency for *every* loader tested, and the
problem gets worse with every Vulkan release.

### The problem

When any loader calls `vkGetInstanceProcAddr(instance, "vkCmdDrawIndirect")`,
the Vulkan loader library resolves the name to a function pointer. The current
implementation does this with a linear chain of `strcmp()` calls against a
table of known function names. For the 400+ function names in a modern Vulkan
build, this means:

* Hundreds of `strcmp()` calls per lookup in the worst case — either because
  the requested function falls near the end of the chain, or because the
  name is unknown and the entire chain must be traversed before returning
  NULL.
* Each `strcmp()` touches the name string and the candidate entry — cold memory
  on the first access, and still cache-unfriendly on repeated lookups because
  the access pattern doesn't match the table layout.
* Branch predictor misses on nearly every comparison, because the comparison
  order (sorted by name) has no correlation with the call order.
* The cost scales linearly with the size of the spec. Every new Vulkan release
  adds more extensions and more function names, making every lookup slower for
  every application.

This is why the "unpatched" numbers for every loader in the results above are
so much higher than the "patched" numbers. The per-lookup cost inside
libvulkan dominates the total loading time — it dwarfs anything the API loader
itself is doing.

On embedded and mobile devices, this problem is compounded: weaker CPUs, slower
memory, smaller caches, and less capable branch predictors all amplify the cost
of a `strcmp()`-heavy inner loop.

### The fix

The patch replaces each `strcmp()` with a hash-guarded comparison. The
existing if-chain structure is unchanged — the incoming function name is hashed
once at the top of the lookup with XXH3, and then each branch compares the
64-bit hash against a build-time constant. On a hash match, a `strcmp()`
confirms the result to guard against collisions:

```c
if (nameHash == XXH3_vkCmdCopyImage && !strcmp(name, "vkCmdCopyImage"))
    return table->CmdCopyImage;
```

The `strcmp()` only runs for the one entry whose hash matches — effectively
once per lookup — so the cost is negligible. The hash constants are generated
by a Python script at build time, which also verifies that no collisions exist
among the current set of Vulkan command names.

The improvement comes from skipping the non-matching comparisons. In the stock
loader, `strcmp()` is a loop: each byte is a branch ("equal so far? keep
going"), and the branch predictor has no useful pattern to work with because
the point of divergence changes depending on which names are being compared.
Looking up `vkCmdDrawIndirect` after `vkCmdDrawIndexed` means the first 10
bytes all match — the predictor learns "keep going" — and then mispredicts at
the divergence point. The next lookup against a different name diverges at a
different position, and the predictor is wrong again. With hundreds of lookups
per load call, this generates a steady stream of mispredicts in a tight inner
loop.

The hash comparison replaces all of that with a single 64-bit
compare-and-branch per candidate. The predictor only needs to learn "not this
one" (not-taken), which it gets right almost every time since only one branch
in the chain will match. One predictable branch per candidate versus a loop of
unpredictable branches per candidate — that's where the speedup comes from.

Both patches are maintained in a
[fork of the Vulkan-Loader](https://github.com/tycho/Vulkan-Loader/) that
passes the full upstream test suite on Windows, Linux, and macOS.

### Impact

Every loader benefits. The following table shows the speedup on the
function-loading benchmarks (Linux), which isolate the `vkGet*ProcAddr` path
where the hash lookup applies:

| Loader                | Metric                            | Unpatched    | Patched     | Speedup |
|-----------------------|-----------------------------------|-------------:|------------:|--------:|
| GLAD (dav1dde)        | Load device functions (avg)       | 2207.85µs    | 994.02µs    | ~2×     |
| GLAD (dav1dde)        | Init + load all functions (avg)   | 3173.30µs    | 1212.80µs   | ~3×     |
| GLAD (tycho)          | Load device functions (avg)       | 413.06µs     | 69.64µs     | ~6×     |
| GLAD (tycho)          | Init + load all functions (avg)   | 1003.73µs    | 129.50µs    | ~8×     |
| Gloam (discover)      | Load device functions (avg)       | 424.76µs     | 84.62µs     | ~5×     |
| Gloam (discover)      | Init + load all functions (avg)   | 1022.43µs    | 128.37µs    | ~8×     |
| Gloam (enabled-list)  | Load device functions (avg)       | 58.52µs      | 10.23µs     | ~6×     |
| Gloam (enabled-list)  | Init + load all functions (avg)   | 64.20µs      | 13.97µs     | ~5×    |
| Volk                  | Load device functions (avg)       | 480.38µs     | 73.85µs     | ~7×     |
| Volk                  | Init + load all functions (avg)   | 544.00µs     | 88.23µs     | ~6×     |

The improvement is universal. Even the fastest loader (Gloam enabled-list),
which makes the fewest calls into libvulkan, sees a 5–6× speedup. Loaders that
make more calls see proportionally larger absolute savings, because every call
was expensive and now isn't.

Vulkan is increasingly deployed on mobile, embedded, and XR platforms where
startup latency directly impacts user experience. A hash-based lookup in
libvulkan would benefit every Vulkan application on every platform, regardless
of which API loader or loading strategy they use.


## ICD and layer manifest caching

The hash-based lookup patch addresses the cost of individual `vkGet*ProcAddr`
calls, but the "Full VK context" benchmark rows reveal a second bottleneck:
the I/O the Vulkan loader performs every time a VkInstance is created or
destroyed.

### The problem

Each time `vkCreateInstance` is called, the stock Vulkan loader rediscovers
every ICD and layer on the system from scratch: it enumerates registry keys
(Windows) or scans filesystem search paths (Linux/macOS), reads and parses
the JSON manifest for each one, and builds its internal ICD and layer lists.
`vkDestroyInstance` tears all of this down, so the next `vkCreateInstance`
pays the full cost again.

For applications that create and destroy Vulkan contexts in a loop — or
libraries that initialize Vulkan on demand — this repeated I/O adds up. On
Windows, where registry enumeration is particularly expensive, the manifest
scanning cost is a significant fraction of the total context creation time.

### The fix

The patch adds a lazy-initialized, in-process cache for ICD and layer manifest
metadata. On the first pre-instance API call (`vkEnumerateInstanceExtensionProperties`,
`vkCreateInstance`, etc.), manifests are scanned once and cached. Each cache
entry stores the manifest file path, file size, and modification time. All
subsequent calls serve directly from the cache with no I/O.

When `vkDestroyInstance` is called, the caches are marked stale but not
immediately rescanned. On the next pre-instance call, revalidation runs:
manifest paths are re-discovered using the current environment state, and each
cached entry is checked with a cheap `stat()` call (file size + mtime). Only
new or modified manifests are re-parsed; unchanged entries are served from
cache. If a manifest has been removed, its cache entry is evicted.

When the library's reference count drops to zero (i.e. it is unloaded), the
caches are fully cleared.

### Persistent vs transient

This caching behavior explains the gap between the "libvulkan persistent" and
"libvulkan transient" rows in the patched results.

When libvulkan stays loaded between context cycles ("persistent"), the cache
survives `vkDestroyInstance` and revalidation on the next `vkCreateInstance` is
a quick metadata check — no JSON parsing, no full directory scan, no registry
enumeration. When libvulkan is unloaded and reloaded each cycle ("transient"),
the cache is destroyed on unload and rebuilt from scratch on reload, paying the
full scan cost every time.

In the unpatched results, the persistent/transient gap is minimal because the
stock loader was already doing a full rescan on every `vkCreateInstance`
regardless — there was no cache to benefit from.

Using the Linux numbers (Volk, as a representative loader):

| Metric                               | Unpatched    | Patched     |
|--------------------------------------|-------------:|------------:|
| Full VK context, persistent (avg)    | 6434.87µs    | 4189.83µs   |
| Full VK context, transient (avg)     | 6450.77µs    | 5936.97µs   |
| Persistent vs transient gap          | ~16µs        | ~1747µs     |

The unpatched gap of ~16µs is noise — the scan happens either way. The patched
gap of ~1747µs is the cost of rebuilding the manifest cache from scratch versus
a cheap revalidation.

On Windows, the effect is more pronounced due to the higher cost of registry
enumeration. On macOS, driver overhead (MoltenVK context creation) tends to
dominate, so the cache improvement is proportionally smaller.

### A note about driver overhead

The "Full VK context" numbers include not just loader overhead but also
driver-side costs for `vkCreateInstance`, `vkCreateDevice`, `vkDestroyDevice`,
and `vkDestroyInstance`. These are fixed costs that neither loader patch
addresses, and they vary significantly across drivers. On the NVIDIA
proprietary driver, for example, context creation and destruction takes roughly
2–3× as long as on Mesa/RADV for the same hardware class, pushing the total
well above what is visible in the results above (which use AMD/Mesa systems).
On such drivers, loader optimizations — while still beneficial — are a smaller
fraction of the total.
