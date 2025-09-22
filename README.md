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
| GLAD (tycho)   | `2.0.8-68-g213a330`   |
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
| Load instance functions | 200 | 871486          | 4357.43            |
| Load device functions   | 200 | 1348372          | 6741.86            |
| Teardown and full init  | 20 | 303631          | 15181.5            |

[GLAD (tycho)](https://github.com/tycho/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 12883          | 64.415            |
| Load device functions   | 200 | 144481          | 722.405            |
| Teardown and full init  | 20 | 182237          | 9111.85            |

[Volk](https://github.com/zeux/volk)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 15797          | 78.985            |
| Load device functions   | 200 | 128289          | 641.445            |
| Teardown and full init  | 20 | 14507          | 725.35            |

Loader object sizes in bytes:
```
349968  obj/loader-glad-dav1dde.o
 66024  obj/loader-glad-tycho.o
283040  obj/loader-volk.o
```

Test program sizes in bytes:
```
121184  bin/test-glad-dav1dde
 76200  bin/test-glad-tycho
 92472  bin/test-volk
```

Section sizes:
```
  text  data  bss   dec     hex    filename
111112  832   7160  119104  1d140  bin/test-glad-dav1dde
 60879  6904  6552  74335   1225f  bin/test-glad-tycho
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
| GLAD (tycho)   | `2.0.8-68-g213a330`   |
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
| `OPTFLAGS` | `-O2 -march=x86-64-v2 -mtune=znver3 -fno-unroll-loops -g0` |
| `CFLAGS`   | `-std=c17` |
| `CXXFLAGS` | `-std=c++20` |

[GLAD (dav1dde)](https://github.com/Dav1dde/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 374355          | 1871.78            |
| Load device functions   | 200 | 841692          | 4208.46            |
| Teardown and full init  | 20 | 156983          | 7849.15            |

[GLAD (tycho)](https://github.com/tycho/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 9562          | 47.81            |
| Load device functions   | 200 | 94848          | 474.24            |
| Teardown and full init  | 20 | 69349          | 3467.45            |

[Volk](https://github.com/zeux/volk)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 12055          | 60.275            |
| Load device functions   | 200 | 81649          | 408.245            |
| Teardown and full init  | 20 | 10719          | 535.95            |

Loader object sizes in bytes:
```
213520  obj/loader-glad-dav1dde.o
 49443  obj/loader-glad-tycho.o
200853  obj/loader-volk.o
```

Test program sizes in bytes:
```
122880  bin/test-glad-dav1dde.exe
 60416  bin/test-glad-tycho.exe
 93696  bin/test-volk.exe
```

Section sizes:
```
 text  data   bss  dec     hex    filename
77510  42585  0    120095  1d51f  bin/test-glad-dav1dde.exe
14198  43357  0    57555   e0d3   bin/test-glad-tycho.exe
63254  27413  0    90667   1622b  bin/test-volk.exe
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
| GLAD (tycho)   | `2.0.8-68-g213a330`   |
| Volk           | `1.4.321.0-23-g1e0ec16`         |
| xxHash         | `0.7.4-976-gc961fbe`       |
| Vulkan-Headers | `1.4.327`   |

| Tool         | Name             | Version             |
|--------------|------------------|---------------------|
| Kernel       | `Darwin`   | `25.0.0`      |
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
| Load instance functions | 200 | 103140          | 515.7            |
| Load device functions   | 200 | 262790          | 1313.95            |
| Teardown and full init  | 20 | 49472          | 2473.6            |

[GLAD (tycho)](https://github.com/tycho/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 6655          | 33.275            |
| Load device functions   | 200 | 37479          | 187.395            |
| Teardown and full init  | 20 | 13238          | 661.9            |

[Volk](https://github.com/zeux/volk)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 15628          | 78.14            |
| Load device functions   | 200 | 74490          | 372.45            |
| Teardown and full init  | 20 | 9833          | 491.65            |

Loader object sizes in bytes:
```
304096  obj/loader-glad-dav1dde.o
 73432  obj/loader-glad-tycho.o
225864  obj/loader-volk.o
```

Test program sizes in bytes:
```
180224  bin/test-glad-dav1dde
 70064  bin/test-glad-tycho
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
        deviceID           = 0x1a000209
        deviceType         = PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU
        deviceName         = Apple M4 Pro
        driverID           = DRIVER_ID_MOLTENVK
        driverName         = MoltenVK
        driverInfo         = 1.3.0
        conformanceVersion = 1.3.8.0
        deviceUUID         = 0000106b-1a00-0209-0000-000000000000
        driverUUID         = 4d564b00-0000-283c-1a00-020900000000
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
| Load instance functions | 200 | 840576          | 4202.88            |
| Load device functions   | 200 | 1092903          | 5464.52            |
| Teardown and full init  | 20 | 283264          | 14163.2            |

[GLAD (tycho)](https://github.com/tycho/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 2141          | 10.705            |
| Load device functions   | 200 | 14896          | 74.48            |
| Teardown and full init  | 20 | 168324          | 8416.2            |

[Volk](https://github.com/zeux/volk)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 2125          | 10.625            |
| Load device functions   | 200 | 13314          | 66.57            |
| Teardown and full init  | 20 | 1548          | 77.4            |

Note in particular how much faster volk is with that patch (compared to
[above](#linux)). We cut around 90% of our `Teardown and full init` test time
by using a patched libvulkan.

This approach helps on Windows too. Here's MinGW with a patched libvulkan DLL
added to the `bin` directory:

[GLAD (dav1dde)](https://github.com/Dav1dde/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 321240          | 1606.2            |
| Load device functions   | 200 | 516164          | 2580.82            |
| Teardown and full init  | 20 | 120276          | 6013.8            |

[GLAD (tycho)](https://github.com/tycho/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 2997          | 14.985            |
| Load device functions   | 200 | 15784          | 78.92            |
| Teardown and full init  | 20 | 56091          | 2804.55            |

[Volk](https://github.com/zeux/volk)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 3328          | 16.64            |
| Load device functions   | 200 | 13088          | 65.44            |
| Teardown and full init  | 20 | 1612          | 80.6            |

And on macOS with a patched libvulkan:

[GLAD (dav1dde)](https://github.com/Dav1dde/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 81071          | 405.355            |
| Load device functions   | 200 | 168657          | 843.285            |
| Teardown and full init  | 20 | 33660          | 1683            |

[GLAD (tycho)](https://github.com/tycho/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 406          | 2.03            |
| Load device functions   | 200 | 1308          | 6.54            |
| Teardown and full init  | 20 | 8074          | 403.7            |

[Volk](https://github.com/zeux/volk)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 461          | 2.305            |
| Load device functions   | 200 | 1878          | 9.39            |
| Teardown and full init  | 20 | 243          | 12.15            |

volk vs GLAD performance
------------------------

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
