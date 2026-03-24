# Vulkan API Loader Shootout

A benchmark comparing Vulkan API function-pointer loaders:
[GLAD (upstream)](https://github.com/Dav1dde/glad),
[GLAD (tycho)](https://github.com/tycho/glad),
[Gloam](https://github.com/tycho/gloam), and
[Volk](https://github.com/zeux/volk).

Each loader is benchmarked on three tasks: loading instance-scope functions,
loading device-scope functions, and a full teardown-and-reinitialize cycle.
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
**unpatched** stock release, and
a **[patched](#libvulkan-is-actually-just-a-strcmp-benchmark)** build that
replaces the loader's internal `strcmp()` chain with XXH3 hash lookups. The
patch is explained in detail [at the end of this
document](#libvulkan-is-actually-just-a-strcmp-benchmark); the short version is
that the stock Vulkan loader uses linear string comparison to resolve function
names, and this is the dominant cost in every benchmark below.

### Linux

| Project        | Version (git describe)      |
|----------------|-----------------------------|
| GLAD (dav1dde) | `2.0.8-8-ga4ca574` |
| GLAD (tycho)   | `2.0.8-91-g8092eae`   |
| gloam          | `0.4.0`        |
| Volk           | `1.4.341.0-21-g78463da`         |
| xxHash         | `0.8.3`       |
| Vulkan-Headers | `1.4.347-1-gf07ffc2`   |

| Tool         | Name             | Version             |
|--------------|------------------|---------------------|
| Kernel       | `Linux`   | `6.19.8-1-hsw`      |
| Architecture | `x86_64`   | `AMD Ryzen 9 3950X 16-Core Processor`    |
| CC           | `clang`        | `22.1.1`   |
| CXX          | `clang++`       | `22.1.1`  |

| Variable     | Value         |
|--------------|---------------|
| `OPTFLAGS` | `-O2 -fno-unroll-loops -march=x86-64-v2 -mtune=znver3 -g0` |
| `CFLAGS`   | `-std=c17` |
| `CXXFLAGS` | `-std=c++20` |

#### Unpatched libvulkan

[GLAD (dav1dde)](https://github.com/Dav1dde/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 950266          | 4751.33            |
| Load device functions   | 200 | 1416296          | 7081.48            |
| Teardown and full init  | 20 | 328740          | 16437            |

[GLAD (tycho)](https://github.com/tycho/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 12979          | 64.895            |
| Load device functions   | 200 | 147642          | 738.21            |
| Teardown and full init  | 20 | 196591          | 9829.55            |

[Gloam (discover API)](https://github.com/tycho/gloam)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 904481          | 4522.4            |
| Load device functions   | 200 | 160814          | 804.07            |
| Teardown and full init  | 20 | 197911          | 9895.55            |

[Gloam (enabled-list API)](https://github.com/tycho/gloam)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 15004          | 75.02            |
| Load device functions   | 200 | 15010          | 75.05            |
| Teardown and full init  | 20 | 2971          | 148.55            |

[Volk](https://github.com/zeux/volk)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 17822          | 89.11            |
| Load device functions   | 200 | 155333          | 776.665            |
| Teardown and full init  | 20 | 17515          | 875.75            |

#### [Patched libvulkan](#libvulkan-is-actually-just-a-strcmp-benchmark)

[GLAD (dav1dde)](https://github.com/Dav1dde/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 128295          | 641.475            |
| Load device functions   | 200 | 357590          | 1787.95            |
| Teardown and full init  | 20 | 61730          | 3086.5            |

[GLAD (tycho)](https://github.com/tycho/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 2275          | 11.375            |
| Load device functions   | 200 | 15657          | 78.285            |
| Teardown and full init  | 20 | 23317          | 1165.85            |

[Gloam (discover API)](https://github.com/tycho/gloam)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 93763          | 468.815            |
| Load device functions   | 200 | 21033          | 105.165            |
| Teardown and full init  | 20 | 23274          | 1163.7            |

[Gloam (enabled-list API)](https://github.com/tycho/gloam)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 2633          | 13.165            |
| Load device functions   | 200 | 1571          | 7.855            |
| Teardown and full init  | 20 | 443          | 22.15            |

[Volk](https://github.com/zeux/volk)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 2368          | 11.84            |
| Load device functions   | 200 | 30593          | 152.965            |
| Teardown and full init  | 20 | 3261          | 163.05            |

#### Binary sizes

Loader object sizes in bytes:
```
316288  obj/loader-glad-dav1dde.o
 67192  obj/loader-glad-tycho.o
 48104  obj/loader-gloam-discover.o
 48104  obj/loader-gloam.o
312912  obj/loader-volk.o
```

Test program sizes in bytes:
```
104800  bin/test-glad-dav1dde
 80288  bin/test-glad-tycho
 55712  bin/test-gloam
 55696  bin/test-gloam-discover
 96568  bin/test-volk
```

Section sizes:
```
 text  data  bss   dec     hex    filename
99092  832   6568  106492  19ffc  bin/test-glad-dav1dde
62043  7360  7040  76443   12a9b  bin/test-glad-tycho
48092  840   7040  55972   daa4   bin/test-gloam
47476  824   7032  55332   d824   bin/test-gloam-discover
88993  792   6040  95825   17651  bin/test-volk
```

#### Test hardware

vulkaninfo
```
Devices:
========
GPU0:
        apiVersion         = 1.4.325
        driverVersion      = 590.48.1.0
        vendorID           = 0x10de
        deviceID           = 0x2204
        deviceType         = PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
        deviceName         = NVIDIA GeForce RTX 3090
        driverID           = DRIVER_ID_NVIDIA_PROPRIETARY
        driverName         = NVIDIA
        driverInfo         = 590.48.01
        conformanceVersion = 1.4.3.0
        deviceUUID         = 8389ef2b-954d-4c2e-1e19-49bc92e5c34f
        driverUUID         = cacc304f-7167-5cf5-a6bd-293f0657ef84
```

### MinGW

| Project        | Version (git describe)      |
|----------------|-----------------------------|
| GLAD (dav1dde) | `2.0.8-8-ga4ca574` |
| GLAD (tycho)   | `2.0.8-91-g8092eae`   |
| gloam          | `0.4.0`        |
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
| `OPTFLAGS` | `-O2 -fno-unroll-loops -march=x86-64-v2 -mtune=znver3 -g0` |
| `CFLAGS`   | `-std=c17` |
| `CXXFLAGS` | `-std=c++20` |

#### Unpatched libvulkan

[GLAD (dav1dde)](https://github.com/Dav1dde/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 597580          | 2987.9            |
| Load device functions   | 200 | 1041536          | 5207.68            |
| Teardown and full init  | 20 | 212371          | 10618.5            |

[GLAD (tycho)](https://github.com/tycho/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 9270          | 46.35            |
| Load device functions   | 200 | 107470          | 537.35            |
| Teardown and full init  | 20 | 106370          | 5318.5            |

[Gloam (discover API)](https://github.com/tycho/gloam)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 444608          | 2223.04            |
| Load device functions   | 200 | 127019          | 635.095            |
| Teardown and full init  | 20 | 111299          | 5564.95            |

[Gloam (enabled-list API)](https://github.com/tycho/gloam)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 10236          | 51.18            |
| Load device functions   | 200 | 7267          | 36.335            |
| Teardown and full init  | 20 | 1754          | 87.7            |

[Volk](https://github.com/zeux/volk)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 13069          | 65.345            |
| Load device functions   | 200 | 98229          | 491.145            |
| Teardown and full init  | 20 | 12757          | 637.85            |

#### [Patched libvulkan](#libvulkan-is-actually-just-a-strcmp-benchmark)

[GLAD (dav1dde)](https://github.com/Dav1dde/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 514906          | 2574.53            |
| Load device functions   | 200 | 716663          | 3583.32            |
| Teardown and full init  | 20 | 175565          | 8778.25            |

[GLAD (tycho)](https://github.com/tycho/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 2883          | 14.415            |
| Load device functions   | 200 | 15960          | 79.8            |
| Teardown and full init  | 20 | 94003          | 4700.15            |

[Gloam (discover API)](https://github.com/tycho/gloam)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 402281          | 2011.4            |
| Load device functions   | 200 | 18633          | 93.165            |
| Teardown and full init  | 20 | 91944          | 4597.2            |

[Gloam (enabled-list API)](https://github.com/tycho/gloam)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 4015          | 20.075            |
| Load device functions   | 200 | 2317          | 11.585            |
| Teardown and full init  | 20 | 650          | 32.5            |

[Volk](https://github.com/zeux/volk)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 3243          | 16.215            |
| Load device functions   | 200 | 13721          | 68.605            |
| Teardown and full init  | 20 | 1712          | 85.6            |

#### Binary sizes

Loader object sizes in bytes:
```
192545  obj/loader-glad-dav1dde.o
 52988  obj/loader-glad-tycho.o
 44580  obj/loader-gloam.o
 44580  obj/loader-gloam-discover.o
221552  obj/loader-volk.o
```

Test program sizes in bytes:
```
112128  bin/test-glad-dav1dde.exe
 64000  bin/test-glad-tycho.exe
 60928  bin/test-gloam.exe
 60928  bin/test-gloam-discover.exe
101888  bin/test-volk.exe
```

Section sizes:
```
 text  data   bss  dec     hex    filename
70646  39320  0    109966  1ad8e  bin/test-glad-dav1dde.exe
14886  46172  0    61058   ee82   bin/test-glad-tycho.exe
17270  41136  0    58406   e426   bin/test-gloam.exe
16966  41096  0    58062   e2ce   bin/test-gloam-discover.exe
70326  29244  0    99570   184f2  bin/test-volk.exe
```

#### Test hardware

vulkaninfo
```
Devices:
========
GPU0:
        apiVersion         = 1.4.329
        driverVersion      = 595.71.0.0
        vendorID           = 0x10de
        deviceID           = 0x2b85
        deviceType         = PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
        deviceName         = NVIDIA GeForce RTX 5090
        driverID           = DRIVER_ID_NVIDIA_PROPRIETARY
        driverName         = NVIDIA
        driverInfo         = 595.71
        conformanceVersion = 1.4.3.3
        deviceUUID         = c630cdf6-46dc-2c45-c099-c9c455afb590
        driverUUID         = d7ce4bc1-acf9-5be0-875a-ebb2a6300feb
GPU1:
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
| gloam          | `0.4.0`        |
| Volk           | `1.4.341.0-21-g78463da`         |
| xxHash         | `0.8.3`       |
| Vulkan-Headers | `1.4.347-1-gf07ffc2`   |

| Tool         | Name             | Version             |
|--------------|------------------|---------------------|
| Kernel       | `Darwin`   | `25.3.0`      |
| Architecture | `arm64`   | `Apple M5`    |
| CC           | `clang`        | `17.0.0`   |
| CXX          | `clang++`       | `17.0.0`  |

| Variable     | Value         |
|--------------|---------------|
| `OPTFLAGS` | `-O2 -fno-unroll-loops -mcpu=native -g0` |
| `CFLAGS`   | `-std=c17` |
| `CXXFLAGS` | `-std=c++20` |

#### Unpatched libvulkan

[GLAD (dav1dde)](https://github.com/Dav1dde/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 107312          | 536.56            |
| Load device functions   | 200 | 231218          | 1156.09            |
| Teardown and full init  | 20 | 44049          | 2202.45            |

[GLAD (tycho)](https://github.com/tycho/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 4285          | 21.425            |
| Load device functions   | 200 | 27329          | 136.645            |
| Teardown and full init  | 20 | 15577          | 778.85            |

[Gloam (discover API)](https://github.com/tycho/gloam)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 60047          | 300.235            |
| Load device functions   | 200 | 28361          | 141.805            |
| Teardown and full init  | 20 | 15267          | 763.35            |

[Gloam (enabled-list API)](https://github.com/tycho/gloam)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 8775          | 43.875            |
| Load device functions   | 200 | 4996          | 24.98            |
| Teardown and full init  | 20 | 1400          | 70            |

[Volk](https://github.com/zeux/volk)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 11069          | 55.345            |
| Load device functions   | 200 | 61813          | 309.065            |
| Teardown and full init  | 20 | 7358          | 367.9            |

#### [Patched libvulkan](#libvulkan-is-actually-just-a-strcmp-benchmark)

[GLAD (dav1dde)](https://github.com/Dav1dde/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 52605          | 263.025            |
| Load device functions   | 200 | 123350          | 616.75            |
| Teardown and full init  | 20 | 23667          | 1183.35            |

[GLAD (tycho)](https://github.com/tycho/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 439          | 2.195            |
| Load device functions   | 200 | 1389          | 6.945            |
| Teardown and full init  | 20 | 3783          | 189.15            |

[Gloam (discover API)](https://github.com/tycho/gloam)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 13648          | 68.24            |
| Load device functions   | 200 | 2588          | 12.94            |
| Teardown and full init  | 20 | 3746          | 187.3            |

[Gloam (enabled-list API)](https://github.com/tycho/gloam)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 859          | 4.295            |
| Load device functions   | 200 | 321          | 1.605            |
| Teardown and full init  | 20 | 131          | 6.55            |

[Volk](https://github.com/zeux/volk)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 503          | 2.515            |
| Load device functions   | 200 | 2130          | 10.65            |
| Teardown and full init  | 20 | 276          | 13.8            |

#### Binary sizes

Loader object sizes in bytes:
```
275560  obj/loader-glad-dav1dde.o
 75640  obj/loader-glad-tycho.o
 44880  obj/loader-gloam-discover.o
 44880  obj/loader-gloam.o
250024  obj/loader-volk.o
```

Test program sizes in bytes:
```
161344  bin/test-glad-dav1dde
 70032  bin/test-glad-tycho
 69992  bin/test-gloam
 70000  bin/test-gloam-discover
119144  bin/test-volk
```

Section sizes:
```
__TEXT  __DATA  __OBJC  others      dec         hex
114688  16384   0       4295016448  4295147520  10002c000  bin/test-glad-dav1dde
 49152  16384   0       4295000064  4295065600  100018000  bin/test-glad-tycho
 49152  16384   0       4295000064  4295065600  100018000  bin/test-gloam
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
        deviceID           = 0x1a03020a
        deviceType         = PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU
        deviceName         = Apple M5
        driverID           = DRIVER_ID_MOLTENVK
        driverName         = MoltenVK
        driverInfo         = 1.4.1
        conformanceVersion = 1.4.4.0
        deviceUUID         = 0000106b-1a03-020a-0000-000000000000
        driverUUID         = 4d564b00-0000-28a1-1a03-020a00000000
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
        deviceUUID         = fb030000-0100-0000-0000-000000000000
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

This is why GLAD and Gloam's discover mode show such high teardown-and-reinit
numbers (Linux, unpatched): GLAD (tycho) at ~9830µs and Gloam discover at
~9896µs. The device-load numbers are similarly inflated because
`vkEnumerateDeviceExtensionProperties` runs during that phase.

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

**Ambiguous non-NULL pointers.** The Vulkan spec allows
`vkGetInstanceProcAddr` to return a non-NULL function pointer for *any* known
command name, regardless of whether the corresponding extension was enabled at
instance or device creation. The returned pointer is typically a loader
trampoline stub, and calling it is undefined behavior per the spec. Since
every slot is requested unconditionally, some populated pointers may not be
safe to call. Users need to track their enabled extensions separately to know
which slots are valid.

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
capability as GLAD, at the same cost.

**Enabled-list mode** (`gloamVulkanInitialize` / `gloamVulkanLoadInstance` /
`gloamVulkanLoadDevice`) takes a different approach. The caller creates their
Vulkan instance and device however they like, then tells gloam what API version
and extensions were actually enabled. Gloam loads only those PFNs, sets the
corresponding presence flags, and runs alias resolution. No enumeration calls,
no heap allocation for hash arrays, and no bogus pointers.

The difference is substantial. Using the Linux unpatched-libvulkan numbers:

| Metric                        | Gloam discover | Gloam enabled-list | Speedup |
|-------------------------------|---------------:|-------------------:|--------:|
| Load instance functions (avg) | 4522.4µs       | 75.02µs            | ~60×    |
| Load device functions (avg)   | 804.07µs       | 75.05µs            | ~10.7×  |
| Teardown and full init (avg)  | 9895.55µs      | 148.55µs           | ~66×    |

The device-load improvement comes from loading only the PFNs for enabled
features and extensions rather than the entire spec. The teardown improvement
comes from skipping `vkEnumerateInstanceExtensionProperties` and
`vkEnumerateDeviceExtensionProperties` entirely — `gloamVulkanFinalize` is
just a conditional `dlclose` and a `memset`, and `gloamVulkanInitialize` only
loads the handful of global-scope bootstrap functions needed to create an
instance.

Enabled-list mode also provides guarantees that neither Volk nor discovery mode
can: slots for functions you didn't enable stay NULL (clean and unambiguous),
and alias resolution runs automatically so both the core and extension names
work regardless of which name the driver resolves.


### Why Gloam's enabled-list mode is faster than Volk

Beyond the design trade-offs discussed above, the enabled-list numbers are
also faster than Volk across the board.

**Volk loads everything.** `volkLoadInstanceOnly(instance)` calls
`vkGetInstanceProcAddr` for every instance-scope function that compiles in on
the target platform — hundreds of entry points spanning all compiled-in
versions and extensions, regardless of what the application actually enabled.

**Gloam loads only what you enabled.** A `gloamVulkanLoadDevice` call for a VK
1.3 device with no device extensions only loads the device-scope functions from
VK 1.0 through 1.3 core — a fraction of the full spec.

Using the Linux numbers:

| Metric                        | Gloam enabled | Volk    | Speedup |
|-------------------------------|-------------:|--------:|--------:|
| Load device (avg, unpatched)  | 75.05µs      | 776.67µs | ~10×   |
| Load device (avg, patched)    | 7.86µs       | 152.97µs | ~19×   |
| Teardown+init (avg, unpatched)| 148.55µs     | 875.75µs | ~6×    |
| Teardown+init (avg, patched)  | 22.15µs      | 163.05µs | ~7×    |

The gap widens with the patched libvulkan because the per-lookup cost drops for
both loaders, but Volk still has to make hundreds of lookups for functions the
application never asked for. Once the individual lookups are cheap, the *count*
of lookups dominates — and that's where loading only what you need wins
decisively.

There's also a secondary effect: Gloam's enabled-path dispatch doesn't use a
callback at all. It calls `vkGetInstanceProcAddr` and `vkGetDeviceProcAddr`
directly from the context struct, avoiding the function-pointer indirection
that callback-based loaders pay on every lookup.


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

* Hundreds of `strcmp()` calls per lookup in the worst case.
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

The patch replaces each `strcmp()` with a 64-bit integer comparison against a
precomputed XXH3 hash constant. The existing if-chain structure is unchanged
— the incoming function name is hashed once at the top of the lookup, and then
each branch compares the hash against a build-time constant instead of calling
`strcmp()`.

The improvement comes from the comparison itself. `strcmp()` is a loop: each
byte is a branch ("equal so far? keep going"), and the branch predictor has no
useful pattern to work with because the point of divergence changes depending
on which names are being compared. Looking up `vkCmdDrawIndirect` after
`vkCmdDrawIndexed` means the first 10 bytes all match — the predictor learns
"keep going" — and then mispredicts at the divergence point. The next lookup
against a different name diverges at a different position, and the predictor is
wrong again. With hundreds of lookups per load call, this generates a steady
stream of mispredicts in a tight inner loop.

The hash comparison replaces all of that with a single 64-bit
compare-and-branch per candidate. The predictor only needs to learn "not this
one" (not-taken), which it gets right almost every time since only one branch
in the chain will match. One predictable branch per candidate versus a loop of
unpredictable branches per candidate — that's where the 5–10× comes from.

The patch is available
[here](https://github.com/tycho/vk-api-loader-shootout/blob/master/experiments/0001-use-xxhash-for-function-name-lookups.patch).

### Impact

Every loader benefits. The following table shows the speedup from the patched
libvulkan on Linux, holding everything else constant:

| Loader                | Metric                       | Unpatched   | Patched    | Speedup |
|-----------------------|------------------------------|------------:|-----------:|--------:|
| GLAD (dav1dde)        | Load device functions (avg)  | 7081.48µs   | 1787.95µs  | ~4×     |
| GLAD (dav1dde)        | Teardown and full init (avg) | 16437µs     | 3086.5µs   | ~5×     |
| GLAD (tycho)          | Load device functions (avg)  | 738.21µs    | 78.29µs    | ~9×     |
| GLAD (tycho)          | Teardown and full init (avg) | 9829.55µs   | 1165.85µs  | ~8×     |
| Gloam (discover)      | Load device functions (avg)  | 804.07µs    | 105.17µs   | ~8×     |
| Gloam (discover)      | Teardown and full init (avg) | 9895.55µs   | 1163.7µs   | ~9×     |
| Gloam (enabled-list)  | Load device functions (avg)  | 75.05µs     | 7.86µs     | ~10×    |
| Gloam (enabled-list)  | Teardown and full init (avg) | 148.55µs    | 22.15µs    | ~7×     |
| Volk                  | Load device functions (avg)  | 776.67µs    | 152.97µs   | ~5×     |
| Volk                  | Teardown and full init (avg) | 875.75µs    | 163.05µs   | ~5×     |

The improvement is universal and significant. Even the fastest loader (Gloam
enabled-list), which makes the fewest calls into libvulkan, still sees a 7–10×
speedup from the patched loader. Loaders that make more calls see
proportionally larger absolute savings, because every call was expensive and
now isn't.

Vulkan is increasingly deployed on mobile, embedded, and XR platforms where
startup latency directly impacts user experience. A hash-based lookup in
libvulkan would benefit every Vulkan application on every platform, regardless
of which API loader or loading strategy they use.
