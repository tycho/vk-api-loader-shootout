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
* Clang or GCC (you can specify a compiler with `make CC=gcc CXX=g++`)

And of course you'll need a system-wide installation of libvulkan and
a functional Vulkan driver.


Results
=======

Linux
-----
| Project        | Version (git describe)      |
|----------------|-----------------------------|
| GLAD (dav1dde) | `2.0.8-1-gf492295` |
| GLAD (tycho)   | `2.0.8-59-gf805540`   |
| Volk           | `1.4.321.0-23-g1e0ec16`         |
| xxHash         | `0.7.4-976-gc961fbe`       |
| Vulkan-Headers | `1.4.327`   |

| Tool         | Name             | Version             |
|--------------|------------------|---------------------|
| Kernel       | `Linux`   | `6.16.5-zen-1-zen-ge8be1c8993bf`      |
| Architecture | `x86_64`   | `AMD Ryzen 9 3950X 16-Core Processor`    |
| CC           | `clang`        | `20.1.8`   |
| CXX          | `clang++`       | `20.1.8`  |

| Variable     | Value         |
|--------------|---------------|
| `OPTFLAGS` | `-O2 -march=x86-64-v2 -mtune=znver3 -g0` |
| `CFLAGS`   | `-std=c17` |
| `CXXFLAGS` | `-std=c++20` |

[GLAD (dav1dde)](https://github.com/Dav1dde/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 855120          | 4275.6            |
| Load device functions   | 200 | 1334528          | 6672.64            |
| Teardown and full init  | 20 | 301516          | 15075.8            |

[GLAD (tycho)](https://github.com/tycho/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 167786          | 838.93            |
| Load device functions   | 200 | 150302          | 751.51            |
| Teardown and full init  | 20 | 179442          | 8972.1            |

[Volk](https://github.com/zeux/volk)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 16092          | 80.46            |
| Load device functions   | 200 | 130575          | 652.875            |
| Teardown and full init  | 20 | 14868          | 743.4            |

Loader object sizes in bytes:
```
349968  obj/loader-glad-dav1dde.o
 69464  obj/loader-glad-tycho.o
283040  obj/loader-volk.o
```

Test program sizes in bytes:
```
121184  bin/test-glad-dav1dde
 81392  bin/test-glad-tycho
 92472  bin/test-volk
```

Section sizes:
```
  text  data   bss   dec     hex    filename
111112  832    7160  119104  1d140  bin/test-glad-dav1dde
 59903  12088  6552  78543   132cf  bin/test-glad-tycho
 81842  792    5608  88242   158b2  bin/test-volk
```

vulkaninfo
```
Devices:
========
GPU0:
        apiVersion         = 1.4.312
        driverVersion      = 580.82.9.0
        vendorID           = 0x10de
        deviceID           = 0x2204
        deviceType         = PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
        deviceName         = NVIDIA GeForce RTX 3090
        driverID           = DRIVER_ID_NVIDIA_PROPRIETARY
        driverName         = NVIDIA
        driverInfo         = 580.82.09
        conformanceVersion = 1.4.1.3
        deviceUUID         = 8389ef2b-954d-4c2e-1e19-49bc92e5c34f
        driverUUID         = c642c5f0-0715-5db7-85d2-8c160b823f7d
```


MinGW
-----
| Project        | Version (git describe)      |
|----------------|-----------------------------|
| GLAD (dav1dde) | `2.0.8-1-gf492295` |
| GLAD (tycho)   | `2.0.8-59-gf805540`   |
| Volk           | `1.4.321.0-23-g1e0ec16`         |
| xxHash         | `0.7.4-976-gc961fbe`       |
| Vulkan-Headers | `1.4.327`   |

| Tool         | Name             | Version             |
|--------------|------------------|---------------------|
| Kernel       | `MINGW64_NT-10.0-26100`   | `3.6.4-23a25d49.x86_64`      |
| Architecture | `x86_64`   | `AMD Ryzen 9 9950X 16-Core Processor`    |
| CC           | `clang`        | `21.1.1`   |
| CXX          | `clang++`       | `21.1.1`  |

| Variable     | Value         |
|--------------|---------------|
| `OPTFLAGS` | `-O2 -march=x86-64-v2 -mtune=znver3 -g0` |
| `CFLAGS`   | `-std=c17` |
| `CXXFLAGS` | `-std=c++20` |

[GLAD (dav1dde)](https://github.com/Dav1dde/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 361945          | 1809.72            |
| Load device functions   | 200 | 840562          | 4202.81            |
| Teardown and full init  | 20 | 156109          | 7805.45            |

[GLAD (tycho)](https://github.com/tycho/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 143061          | 715.305            |
| Load device functions   | 200 | 106871          | 534.355            |
| Teardown and full init  | 20 | 76289          | 3814.45            |

[Volk](https://github.com/zeux/volk)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 11361          | 56.805            |
| Load device functions   | 200 | 80222          | 401.11            |
| Teardown and full init  | 20 | 10266          | 513.3            |

Loader object sizes in bytes:
```
213436  obj/loader-glad-dav1dde.o
 55432  obj/loader-glad-tycho.o
200853  obj/loader-volk.o
```

Test program sizes in bytes:
```
123904  bin/test-glad-dav1dde.exe
 67584  bin/test-glad-tycho.exe
 94720  bin/test-volk.exe
```

Section sizes:
```
 text  data   bss  dec     hex    filename
78470  42609  0    121079  1d8f7  bin/test-glad-dav1dde.exe
16390  47969  0    64359   fb67   bin/test-glad-tycho.exe
64262  27485  0    91747   16663  bin/test-volk.exe
```

vulkaninfo
```
Devices:
========
GPU0:
        apiVersion         = 1.4.312
        driverVersion      = 581.29.0.0
        vendorID           = 0x10de
        deviceID           = 0x2b85
        deviceType         = PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
        deviceName         = NVIDIA GeForce RTX 5090
        driverID           = DRIVER_ID_NVIDIA_PROPRIETARY
        driverName         = NVIDIA
        driverInfo         = 581.29
        conformanceVersion = 1.4.1.3
        deviceUUID         = c630cdf6-46dc-2c45-c099-c9c455afb590
        driverUUID         = d12cc906-29d2-5c75-90d9-1e427e2c380e
```


macOS
-----
| Project        | Version (git describe)      |
|----------------|-----------------------------|
| GLAD (dav1dde) | `2.0.8-1-gf492295` |
| GLAD (tycho)   | `2.0.8-59-gf805540`   |
| Volk           | `vulkan-sdk-1.4.321.0-23-g1e0ec16`         |
| xxHash         | `0.7.4-976-gc961fbe`       |
| Vulkan-Headers | `1.4.327`   |

| Tool         | Name             | Version             |
|--------------|------------------|---------------------|
| Kernel       | `Darwin`   | `24.6.0`      |
| Architecture | `arm64`   | `Apple M4 Pro`    |
| CC           | `clang`        | `17.0.0`   |
| CXX          | `clang++`       | `17.0.0`  |

| Variable     | Value         |
|--------------|---------------|
| `OPTFLAGS` | `-O2 -mcpu=native -g0` |
| `CFLAGS`   | `-std=c17` |
| `CXXFLAGS` | `-std=c++20` |

[GLAD (dav1dde)](https://github.com/Dav1dde/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 86036          | 430.18            |
| Load device functions   | 200 | 220765          | 1103.83            |
| Teardown and full init  | 20 | 38145          | 1907.25            |

[GLAD (tycho)](https://github.com/tycho/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 49837          | 249.185            |
| Load device functions   | 200 | 31766          | 158.83            |
| Teardown and full init  | 20 | 12508          | 625.4            |

[Volk](https://github.com/zeux/volk)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 13408          | 67.04            |
| Load device functions   | 200 | 63583          | 317.915            |
| Teardown and full init  | 20 | 7778          | 388.9            |

Loader object sizes in bytes:
```
304096  obj/loader-glad-dav1dde.o
 76176  obj/loader-glad-tycho.o
225864  obj/loader-volk.o
```

Test program sizes in bytes:
```
180224  bin/test-glad-dav1dde
 86576  bin/test-glad-tycho
119128  bin/test-volk
```

Section sizes:
```
__TEXT  __DATA  __OBJC  others      dec         hex
131072  16384   0       4295016448  4295163904  100030000  bin/test-glad-dav1dde
 49152  16384   0       4295000064  4295065600  100018000  bin/test-glad-tycho
 98304  16384   0       4295000064  4295114752  100024000  bin/test-volk
```

vulkaninfo
```
Devices:
========
GPU0:
        apiVersion         = 1.3.313
        driverVersion      = 0.2.2108
        vendorID           = 0x106b
        deviceID           = 0xf060209
        deviceType         = PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU
        deviceName         = Apple M4 Pro
        driverID           = DRIVER_ID_MOLTENVK
        driverName         = MoltenVK
        driverInfo         = 1.3.0
        conformanceVersion = 1.3.8.0
        deviceUUID         = 0000106b-0f06-0209-0000-000000000000
        driverUUID         = 4d564b00-0000-283c-0f06-020900000000
```

Commentary
==========

libvulkan is slow
-----------------

The majority of the time in my GLAD fork is taken up by `strcmp()` calls inside
libvulkan. This, frustratingly, is how libvulkan decides which function pointer
is being requested by the application. I wrote a patch for libvulkan which uses
the same kind of strategy I used in my GLAD fork: precomputed XXH3 64-bit values
and runtime comparison against the precomputed values. This ends up being
dramatically faster than repeated string comparisons. The libvulkan patch is
available [here](https://github.com/tycho/vk-api-loader-shootout/blob/master/experiments/0001-use-xxhash-for-function-name-lookups.patch).

Here's the Linux build from the previous section, run with a patched libvulkan
dropped into the `bin/` directory (`make run` adds the folder to
`LD_LIBRARY_PATH`):

[GLAD (dav1dde)](https://github.com/Dav1dde/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 858742          | 4293.71            |
| Load device functions   | 200 | 1103971          | 5519.85            |
| Teardown and full init  | 20 | 282758          | 14137.9            |

[GLAD (tycho)](https://github.com/tycho/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 23533          | 117.665            |
| Load device functions   | 200 | 20342          | 101.71            |
| Teardown and full init  | 20 | 168166          | 8408.3            |

[Volk](https://github.com/zeux/volk)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 2127          | 10.635            |
| Load device functions   | 200 | 13363          | 66.815            |
| Teardown and full init  | 20 | 1550          | 77.5            |

Note in particular how much faster volk is with that patch (compared to
[above](#linux)). We cut around 90% of our `Teardown and full init` test time
by using a patched libvulkan.

The difference is even more stark for Windows. Here's MinGW with a patched
libvulkan DLL added to the `bin` directory:

[GLAD (dav1dde)](https://github.com/Dav1dde/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 304378          | 1521.89            |
| Load device functions   | 200 | 488919          | 2444.59            |
| Teardown and full init  | 20 | 108323          | 5416.15            |

[GLAD (tycho)](https://github.com/tycho/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 22844          | 114.22            |
| Load device functions   | 200 | 16501          | 82.505            |
| Teardown and full init  | 20 | 55404          | 2770.2            |

[Volk](https://github.com/zeux/volk)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 3145          | 15.725            |
| Load device functions   | 200 | 12611          | 63.055            |
| Teardown and full init  | 20 | 1594          | 79.7            |

So on MinGW, it's more than 98% reduction for `Teardown and full init` on both
my GLAD fork and volk!

On macOS we see similar performance gains compared to the Windows build, when
using a patched libvulkan:

[GLAD (dav1dde)](https://github.com/Dav1dde/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 73078          | 365.39            |
| Load device functions   | 200 | 158000          | 790            |
| Teardown and full init  | 20 | 31421          | 1571.05            |

[GLAD (tycho)](https://github.com/tycho/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 2386          | 11.93            |
| Load device functions   | 200 | 1930          | 9.65            |
| Teardown and full init  | 20 | 7784          | 389.2            |

[Volk](https://github.com/zeux/volk)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 478          | 2.39            |
| Load device functions   | 200 | 1912          | 9.56            |
| Teardown and full init  | 20 | 249          | 12.45            |

volk vs GLAD performance
------------------------

There are two main reasons GLAD ends up slower than volk.

There's a major notable feature GLAD has that volk does not provide: extension
detection. With GLAD you can do things after the API loader has initialized,
like this:

```
if (GLAD_VK_EXT_graphics_pipeline_library)
    ...
```

This feature is also available in the other GLAD API loaders (EGL, GLX, WGL,
OpenGL, etc).

Extension detection is implemented on the GLAD Vulkan API loader by using these
two functions:

```
vkEnumerateInstanceExtensionProperties
vkEnumerateDeviceExtensionProperties
```

These APIs are unfortunately **very** expensive to call, because they end up
loading and unloading ICDs each time they are called. It also scans all the
implicit layers.

The second reason GLAD ends up slower is because the generated Volk loader
splits out the device and instance function loads, so it doesn't need to do
a run-time selection deciding whether to use vkGetInstanceProcAddr or
vkGetDeviceProcAddr.
