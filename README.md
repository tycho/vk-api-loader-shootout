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
| Load instance functions | 200 | 870473          | 4352.36            |
| Load device functions   | 200 | 1317183          | 6585.91            |
| Teardown and full init  | 20 | 298984          | 14949.2            |

[GLAD (tycho)](https://github.com/tycho/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 28075          | 140.375            |
| Load device functions   | 200 | 176476          | 882.38            |
| Teardown and full init  | 20 | 20408          | 1020.4            |

[Volk](https://github.com/zeux/volk)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 15621          | 78.105            |
| Load device functions   | 200 | 127472          | 637.36            |
| Teardown and full init  | 20 | 14483          | 724.15            |

Loader object sizes in bytes:
```
349968  obj/loader-glad-dav1dde.o
 54456  obj/loader-glad-tycho.o
283040  obj/loader-volk.o
```

Test program sizes in bytes:
```
121184  bin/test-glad-dav1dde
 67976  bin/test-glad-tycho
 92472  bin/test-volk
```

Section sizes:
```
  text  data  bss   dec     hex    filename
111112  832   7160  119104  1d140  bin/test-glad-dav1dde
 52798  6872  6112  65782   100f6  bin/test-glad-tycho
 81842  792   5608  88242   158b2  bin/test-volk
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
| Load instance functions | 200 | 367923          | 1839.62            |
| Load device functions   | 200 | 826420          | 4132.1            |
| Teardown and full init  | 20 | 153278          | 7663.9            |

[GLAD (tycho)](https://github.com/tycho/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 18003          | 90.015            |
| Load device functions   | 200 | 121098          | 605.49            |
| Teardown and full init  | 20 | 14441          | 722.05            |

[Volk](https://github.com/zeux/volk)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 11320          | 56.6            |
| Load device functions   | 200 | 77323          | 386.615            |
| Teardown and full init  | 20 | 10013          | 500.65            |

Loader object sizes in bytes:
```
213436  obj/loader-glad-dav1dde.o
 41913  obj/loader-glad-tycho.o
200853  obj/loader-volk.o
```

Test program sizes in bytes:
```
123904  bin/test-glad-dav1dde.exe
 54784  bin/test-glad-tycho.exe
 94720  bin/test-volk.exe
```

Section sizes:
```
 text  data   bss  dec     hex    filename
78470  42609  0    121079  1d8f7  bin/test-glad-dav1dde.exe
12566  39377  0    51943   cae7   bin/test-glad-tycho.exe
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
| Volk           | `1.4.321.0-23-g1e0ec16`         |
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
| Load instance functions | 200 | 96220          | 481.1            |
| Load device functions   | 200 | 240935          | 1204.67            |
| Teardown and full init  | 20 | 42008          | 2100.4            |

[GLAD (tycho)](https://github.com/tycho/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 22059          | 110.295            |
| Load device functions   | 200 | 98238          | 491.19            |
| Teardown and full init  | 20 | 13394          | 669.7            |

[Volk](https://github.com/zeux/volk)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 14092          | 70.46            |
| Load device functions   | 200 | 67532          | 337.66            |
| Teardown and full init  | 20 | 8544          | 427.2            |

Loader object sizes in bytes:
```
304096  obj/loader-glad-dav1dde.o
 62136  obj/loader-glad-tycho.o
225864  obj/loader-volk.o
```

Test program sizes in bytes:
```
180224  bin/test-glad-dav1dde
 69728  bin/test-glad-tycho
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
| Load instance functions | 200 | 293463          | 1467.32            |
| Load device functions   | 200 | 480274          | 2401.37            |
| Teardown and full init  | 20 | 108638          | 5431.9            |

[GLAD (tycho)](https://github.com/tycho/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 4774          | 23.87            |
| Load device functions   | 200 | 18405          | 92.025            |
| Teardown and full init  | 20 | 2379          | 118.95            |

[Volk](https://github.com/zeux/volk)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 3014          | 15.07            |
| Load device functions   | 200 | 12050          | 60.25            |
| Teardown and full init  | 20 | 1518          | 75.9            |

Note in particular how much faster volk is with that patch (compared to
[above](#linux)). We cut around 90% of our `Teardown and full init` test time
by using a patched libvulkan.

The difference is even more stark for Windows. Here's MinGW with a patched
libvulkan DLL added to the `bin` directory:

[GLAD (dav1dde)](https://github.com/Dav1dde/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 837792          | 4188.96            |
| Load device functions   | 200 | 1018829          | 5094.15            |
| Teardown and full init  | 20 | 271948          | 13597.4            |

[GLAD (tycho)](https://github.com/tycho/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 577          | 2.885            |
| Load device functions   | 200 | 2759          | 13.795            |
| Teardown and full init  | 20 | 388          | 19.4            |

[Volk](https://github.com/zeux/volk)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 425          | 2.125            |
| Load device functions   | 200 | 2040          | 10.2            |
| Teardown and full init  | 20 | 259          | 12.95            |

So on MinGW, it's more than 98% reduction for `Teardown and full init` on both
my GLAD fork and volk!

On macOS we see similar performance gains compared to the Windows build, when
using a patched libvulkan:

[GLAD (dav1dde)](https://github.com/Dav1dde/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 77575          | 387.875            |
| Load device functions   | 200 | 163820          | 819.1            |
| Teardown and full init  | 20 | 33076          | 1653.8            |

[GLAD (tycho)](https://github.com/tycho/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 696          | 3.48            |
| Load device functions   | 200 | 2995          | 14.975            |
| Teardown and full init  | 20 | 968          | 48.4            |

[Volk](https://github.com/zeux/volk)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 462          | 2.31            |
| Load device functions   | 200 | 1893          | 9.465            |
| Teardown and full init  | 20 | 242          | 12.1            |

volk vs GLAD performance
------------------------

There are two main reasons GLAD ends up slower than volk. The first one I'll
talk about is already mitigated in the above tests to make this as close to
a fair comparison as possible, but it's still worth mentioning.

GLAD has a feature that volk does not: extension detection. With GLAD you can
do things after the API loader has initialized, like this:

```
if (GLAD_VK_EXT_graphics_pipeline_library)
    ...
```

This feature is also available in the GLAD OpenGL API loader.

Extension detection is implemented on the GLAD Vulkan API loader by using these
functions:

```
vkEnumerateInstanceExtensionProperties
vkEnumerateDeviceExtensionProperties
```

These APIs are unfortunately **very** expensive to call, because they end up
loading and unloading ICDs each time they are called.

The second reason GLAD ends up slower is because Volk separates out loading of
instance and device functions, so it doesn't need to do a run-time selection
deciding which API to use for loading the functions.
