Overview
========

Building
--------

To build and run this:

```
$ git submodule update --init --recursive
$ make build
$ make run
```

This should just work on Windows and Linux. On macOS, you will need to install
MoltenVK via the Vulkan SDK.


Dependencies
------------

You'll want the following tools installed to build:

* Python 3.x (for GLAD)
* virtualenv (for GLAD)
* util-linux (for formatting the `make run` outputs)
* sed (for formatting the `make run` outputs)
* a Rust toolchain (for Gloam)
* Clang or GCC (you can specify a compiler with `make CC=gcc CXX=g++`)

And of course you'll need a system-wide installation of libvulkan and
a functional Vulkan driver.


Results
=======

Linux
-----
| Project        | Version (git describe)      |
|----------------|-----------------------------|
| GLAD (dav1dde) | `2.0.8-8-ga4ca574` |
| GLAD (tycho)   | `2.0.8-91-g8092eae`   |
| gloam          | `0.1.5-1-g3be2e6f`        |
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

[GLAD (dav1dde)](https://github.com/Dav1dde/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 761621          | 3808.11            |
| Load device functions   | 200 | 1209947          | 6049.73            |
| Teardown and full init  | 20 | 270205          | 13510.2            |

[GLAD (tycho)](https://github.com/tycho/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 12565          | 62.825            |
| Load device functions   | 200 | 141251          | 706.255            |
| Teardown and full init  | 20 | 158354          | 7917.7            |

[Gloam](https://github.com/tycho/gloam)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 14070          | 70.35            |
| Load device functions   | 200 | 147855          | 739.275            |
| Teardown and full init  | 20 | 159596          | 7979.8            |

[Volk](https://github.com/zeux/volk)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 17170          | 85.85            |
| Load device functions   | 200 | 150557          | 752.785            |
| Teardown and full init  | 20 | 16900          | 845            |

Loader object sizes in bytes:
```
316288  obj/loader-glad-dav1dde.o
 67192  obj/loader-glad-tycho.o
 67664  obj/loader-gloam.o
312912  obj/loader-volk.o
```

Test program sizes in bytes:
```
104800  bin/test-glad-dav1dde
 80288  bin/test-glad-tycho
 76176  bin/test-gloam
 96568  bin/test-volk
```

Section sizes:
```
 text  data  bss   dec     hex    filename
99060  832   6568  106460  19fdc  bin/test-glad-dav1dde
62043  7360  7040  76443   12a9b  bin/test-glad-tycho
62622  7344  7032  76998   12cc6  bin/test-gloam
88967  792   6040  95799   17637  bin/test-volk
```

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

MinGW
-----
| Project        | Version (git describe)      |
|----------------|-----------------------------|
| GLAD (dav1dde) | `2.0.8-8-ga4ca574` |
| GLAD (tycho)   | `2.0.8-91-g8092eae`   |
| gloam          | `0.1.5-1-g3be2e6f`        |
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

[GLAD (dav1dde)](https://github.com/Dav1dde/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 609064          | 3045.32            |
| Load device functions   | 200 | 1102829          | 5514.15            |
| Teardown and full init  | 20 | 226066          | 11303.3            |

[GLAD (tycho)](https://github.com/tycho/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 9452          | 47.26            |
| Load device functions   | 200 | 118856          | 594.28            |
| Teardown and full init  | 20 | 122058          | 6102.9            |

[Gloam](https://github.com/tycho/gloam)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 9505          | 47.525            |
| Load device functions   | 200 | 108450          | 542.25            |
| Teardown and full init  | 20 | 109506          | 5475.3            |

[Volk](https://github.com/zeux/volk)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 12834          | 64.17            |
| Load device functions   | 200 | 98764          | 493.82            |
| Teardown and full init  | 20 | 12541          | 627.05            |

Loader object sizes in bytes:
```
192545  obj/loader-glad-dav1dde.o
 52988  obj/loader-glad-tycho.o
 53567  obj/loader-gloam.o
221552  obj/loader-volk.o
```

Test program sizes in bytes:
```
112128  bin/test-glad-dav1dde.exe
 64000  bin/test-glad-tycho.exe
 64000  bin/test-gloam.exe
101888  bin/test-volk.exe
```

Section sizes:
```
 text  data   bss  dec     hex    filename
70630  39320  0    109950  1ad7e  bin/test-glad-dav1dde.exe
14870  46172  0    61042   ee72   bin/test-glad-tycho.exe
14598  47244  0    61842   f192   bin/test-gloam.exe
70310  29244  0    99554   184e2  bin/test-volk.exe
```

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
```

macOS
-----
| Project        | Version (git describe)      |
|----------------|-----------------------------|
| GLAD (dav1dde) | `2.0.8-8-ga4ca574` |
| GLAD (tycho)   | `2.0.8-91-g8092eae`   |
| gloam          | `0.1.5-1-g3be2e6f`        |
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

[GLAD (dav1dde)](https://github.com/Dav1dde/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 112349          | 561.745            |
| Load device functions   | 200 | 239689          | 1198.44            |
| Teardown and full init  | 20 | 45823          | 2291.15            |

[GLAD (tycho)](https://github.com/tycho/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 4670          | 23.35            |
| Load device functions   | 200 | 27285          | 136.425            |
| Teardown and full init  | 20 | 15866          | 793.3            |

[Gloam](https://github.com/tycho/gloam)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 5023          | 25.115            |
| Load device functions   | 200 | 28537          | 142.685            |
| Teardown and full init  | 20 | 16395          | 819.75            |

[Volk](https://github.com/zeux/volk)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 11055          | 55.275            |
| Load device functions   | 200 | 66567          | 332.835            |
| Teardown and full init  | 20 | 8546          | 427.3            |

Loader object sizes in bytes:
```
275560  obj/loader-glad-dav1dde.o
 75640  obj/loader-glad-tycho.o
 74656  obj/loader-gloam.o
250024  obj/loader-volk.o
```

Test program sizes in bytes:
```
161344  bin/test-glad-dav1dde
 70032  bin/test-glad-tycho
 69880  bin/test-gloam
119144  bin/test-volk
```

Section sizes:
```
__TEXT  __DATA  __OBJC  others      dec         hex
114688  16384   0       4295016448  4295147520  10002c000  bin/test-glad-dav1dde
 49152  16384   0       4295000064  4295065600  100018000  bin/test-glad-tycho
 49152  16384   0       4295000064  4295065600  100018000  bin/test-gloam
 98304  16384   0       4295000064  4295114752  100024000  bin/test-volk
```

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
        deviceUUID         = 74040000-0100-0000-0000-000000000000
        driverUUID         = 258801e9-778a-3af1-b03f-46cef7603a2a
```

Commentary
==========

libvulkan is slow
-----------------

The majority of the time in Gloam is taken up by `strcmp()` calls inside
libvulkan. This, frustratingly, is how libvulkan decides which function pointer
is being requested by the application. I wrote a patch for libvulkan which uses
the same kind of strategy I used in Gloam and my GLAD fork: precomputed XXH3
64-bit values and runtime comparison against the precomputed values. This ends
up being dramatically faster than repeated string comparisons. The libvulkan
patch is available
[here](https://github.com/tycho/vk-api-loader-shootout/blob/master/experiments/0001-use-xxhash-for-function-name-lookups.patch).

Here's the Linux build from the previous section, run with a patched libvulkan
dropped into the `bin/` directory (`make run` adds the folder to
`LD_LIBRARY_PATH`):

[GLAD (dav1dde)](https://github.com/Dav1dde/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 127423          | 637.115            |
| Load device functions   | 200 | 356760          | 1783.8            |
| Teardown and full init  | 20 | 61296          | 3064.8            |

[GLAD (tycho)](https://github.com/tycho/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 2316          | 11.58            |
| Load device functions   | 200 | 15694          | 78.47            |
| Teardown and full init  | 20 | 22755          | 1137.75            |

[Gloam](https://github.com/tycho/gloam)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 2513          | 12.565            |
| Load device functions   | 200 | 16255          | 81.275            |
| Teardown and full init  | 20 | 22877          | 1143.85            |

[Volk](https://github.com/zeux/volk)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 2482          | 12.41            |
| Load device functions   | 200 | 14613          | 73.065            |
| Teardown and full init  | 20 | 1726          | 86.3            |

Note in particular how much faster volk is with that patch (compared to
[above](#linux)). We cut around 90% of our `Teardown and full init` test time
by using a patched libvulkan.

This approach helps on Windows too. Here's MinGW with a patched libvulkan DLL
added to the `bin` directory:

[GLAD (dav1dde)](https://github.com/Dav1dde/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 526716          | 2633.58            |
| Load device functions   | 200 | 708675          | 3543.38            |
| Teardown and full init  | 20 | 171186          | 8559.3            |

[GLAD (tycho)](https://github.com/tycho/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 3009          | 15.045            |
| Load device functions   | 200 | 16631          | 83.155            |
| Teardown and full init  | 20 | 101231          | 5061.55            |

[Gloam](https://github.com/tycho/gloam)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 3047          | 15.235            |
| Load device functions   | 200 | 16640          | 83.2            |
| Teardown and full init  | 20 | 93009          | 4650.45            |

[Volk](https://github.com/zeux/volk)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 3417          | 17.085            |
| Load device functions   | 200 | 14496          | 72.48            |
| Teardown and full init  | 20 | 1803          | 90.15            |

And on macOS with a patched libvulkan:

[GLAD (dav1dde)](https://github.com/Dav1dde/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 53403          | 267.015            |
| Load device functions   | 200 | 128056          | 640.28            |
| Teardown and full init  | 20 | 24360          | 1218            |

[GLAD (tycho)](https://github.com/tycho/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 423          | 2.115            |
| Load device functions   | 200 | 1373          | 6.865            |
| Teardown and full init  | 20 | 4006          | 200.3            |

[Gloam](https://github.com/tycho/gloam)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 478          | 2.39            |
| Load device functions   | 200 | 1391          | 6.955            |
| Teardown and full init  | 20 | 3980          | 199            |

[Volk](https://github.com/zeux/volk)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 503          | 2.515            |
| Load device functions   | 200 | 2150          | 10.75            |
| Teardown and full init  | 20 | 275          | 13.75            |

Why is Volk so fast?
------------------------

There's a major notable feature GLAD/Gloam have that volk does not provide:
extension detection. With GLAD and Gloam, you can do things after the API
loader has initialized, like this:

```
if (GLAD_VK_EXT_graphics_pipeline_library)
    ...
```

This feature is also available in the other GLAD/Gloam API loaders (EGL, GLX,
WGL, OpenGL, etc).

Extension detection is implemented on the GLAD/Gloam Vulkan API loaders by
using these two functions:

```
vkEnumerateInstanceExtensionProperties
vkEnumerateDeviceExtensionProperties
```

These APIs are unfortunately **very** expensive to call, because they end up
loading and unloading ICDs each time they are called. It also scans all the
implicit layers.
