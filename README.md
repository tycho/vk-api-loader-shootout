# Overview

## Building

To build and run this:

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

### Linux

| Project        | Version (git describe)      |
|----------------|-----------------------------|
| GLAD (dav1dde) | `2.0.8-8-ga4ca574` |
| GLAD (tycho)   | `2.0.8-91-g8092eae`   |
| gloam          | `0.2.0-6-gf044460`        |
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
| Load instance functions | 200 | 940596          | 4702.98            |
| Load device functions   | 200 | 1409517          | 7047.59            |
| Teardown and full init  | 20 | 328258          | 16412.9            |

[GLAD (tycho)](https://github.com/tycho/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 12934          | 64.67            |
| Load device functions   | 200 | 148625          | 743.125            |
| Teardown and full init  | 20 | 197237          | 9861.85            |

[Gloam](https://github.com/tycho/gloam)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 14175          | 70.875            |
| Load device functions   | 200 | 151916          | 759.58            |
| Teardown and full init  | 20 | 198197          | 9909.85            |

[Volk](https://github.com/zeux/volk)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 17624          | 88.12            |
| Load device functions   | 200 | 152463          | 762.315            |
| Teardown and full init  | 20 | 17094          | 854.7            |

Loader object sizes in bytes:
```
316288  obj/loader-glad-dav1dde.o
 67192  obj/loader-glad-tycho.o
 43272  obj/loader-gloam.o
312912  obj/loader-volk.o
```

Test program sizes in bytes:
```
104800  bin/test-glad-dav1dde
 80288  bin/test-glad-tycho
 51528  bin/test-gloam
 96568  bin/test-volk
```

Section sizes:
```
 text  data  bss   dec     hex    filename
99060  832   6568  106460  19fdc  bin/test-glad-dav1dde
62043  7360  7040  76443   12a9b  bin/test-glad-tycho
44782  808   7032  52622   cd8e   bin/test-gloam
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

### MinGW

| Project        | Version (git describe)      |
|----------------|-----------------------------|
| GLAD (dav1dde) | `2.0.8-8-ga4ca574` |
| GLAD (tycho)   | `2.0.8-91-g8092eae`   |
| gloam          | `0.2.0-6-gf044460`        |
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
| Load instance functions | 200 | 505213          | 2526.07            |
| Load device functions   | 200 | 939048          | 4695.24            |
| Teardown and full init  | 20 | 185287          | 9264.35            |

[GLAD (tycho)](https://github.com/tycho/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 8706          | 43.53            |
| Load device functions   | 200 | 102093          | 510.465            |
| Teardown and full init  | 20 | 98871          | 4943.55            |

[Gloam](https://github.com/tycho/gloam)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 9690          | 48.45            |
| Load device functions   | 200 | 104974          | 524.87            |
| Teardown and full init  | 20 | 98477          | 4923.85            |

[Volk](https://github.com/zeux/volk)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 12664          | 63.32            |
| Load device functions   | 200 | 94842          | 474.21            |
| Teardown and full init  | 20 | 11973          | 598.65            |

Loader object sizes in bytes:
```
192545  obj/loader-glad-dav1dde.o
 52988  obj/loader-glad-tycho.o
 40785  obj/loader-gloam.o
221552  obj/loader-volk.o
```

Test program sizes in bytes:
```
112128  bin/test-glad-dav1dde.exe
 64000  bin/test-glad-tycho.exe
 57856  bin/test-gloam.exe
101888  bin/test-volk.exe
```

Section sizes:
```
 text  data   bss  dec     hex    filename
70630  39320  0    109950  1ad7e  bin/test-glad-dav1dde.exe
14870  46172  0    61042   ee72   bin/test-glad-tycho.exe
14662  40800  0    55462   d8a6   bin/test-gloam.exe
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

### macOS

| Project        | Version (git describe)      |
|----------------|-----------------------------|
| GLAD (dav1dde) | `2.0.8-8-ga4ca574` |
| GLAD (tycho)   | `2.0.8-91-g8092eae`   |
| gloam          | `0.2.0-6-gf044460`        |
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
| Load instance functions | 200 | 109325          | 546.625            |
| Load device functions   | 200 | 232744          | 1163.72            |
| Teardown and full init  | 20 | 44794          | 2239.7            |

[GLAD (tycho)](https://github.com/tycho/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 4325          | 21.625            |
| Load device functions   | 200 | 28082          | 140.41            |
| Teardown and full init  | 20 | 15922          | 796.1            |

[Gloam](https://github.com/tycho/gloam)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 4812          | 24.06            |
| Load device functions   | 200 | 27604          | 138.02            |
| Teardown and full init  | 20 | 15903          | 795.15            |

[Volk](https://github.com/zeux/volk)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 11201          | 56.005            |
| Load device functions   | 200 | 65151          | 325.755            |
| Teardown and full init  | 20 | 7402          | 370.1            |

Loader object sizes in bytes:
```
275560  obj/loader-glad-dav1dde.o
 75640  obj/loader-glad-tycho.o
 41432  obj/loader-gloam.o
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

# Commentary

## libvulkan is slow

The majority of the time in Gloam is taken up by `strcmp()` calls inside
libvulkan. This, frustratingly, is how libvulkan decides which function pointer
is being requested by the application. I wrote a patch for libvulkan which uses
the same kind of strategy I used in Gloam and my GLAD fork: precomputed XXH3
64-bit values and runtime comparison against the precomputed values. This ends
up being dramatically faster than repeated string comparisons. The libvulkan
patch is available
[here](https://github.com/tycho/vk-api-loader-shootout/blob/master/experiments/0001-use-xxhash-for-function-name-lookups.patch).

### Linux

Here's the Linux build from the previous section, run with a patched libvulkan
dropped into the `bin/` directory (`make run` adds the folder to
`LD_LIBRARY_PATH`):

[GLAD (dav1dde)](https://github.com/Dav1dde/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 128432          | 642.16            |
| Load device functions   | 200 | 358393          | 1791.96            |
| Teardown and full init  | 20 | 61970          | 3098.5            |

[GLAD (tycho)](https://github.com/tycho/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 2225          | 11.125            |
| Load device functions   | 200 | 15258          | 76.29            |
| Teardown and full init  | 20 | 22553          | 1127.65            |

[Gloam](https://github.com/tycho/gloam)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 2467          | 12.335            |
| Load device functions   | 200 | 16088          | 80.44            |
| Teardown and full init  | 20 | 22800          | 1140            |

[Volk](https://github.com/zeux/volk)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 2304          | 11.52            |
| Load device functions   | 200 | 15483          | 77.415            |
| Teardown and full init  | 20 | 1751          | 87.55            |

Note in particular how much faster volk is with that patch (compared to
[above](#linux)). We cut around 90% of our `Teardown and full init` test time
by using a patched libvulkan.

### MinGW

This approach helps on Windows too. Here's MinGW with a patched libvulkan DLL
added to the `bin` directory:

[GLAD (dav1dde)](https://github.com/Dav1dde/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 464205          | 2321.03            |
| Load device functions   | 200 | 628309          | 3141.55            |
| Teardown and full init  | 20 | 150591          | 7529.55            |

[GLAD (tycho)](https://github.com/tycho/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 2933          | 14.665            |
| Load device functions   | 200 | 15660          | 78.3            |
| Teardown and full init  | 20 | 84272          | 4213.6            |

[Gloam](https://github.com/tycho/gloam)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 2981          | 14.905            |
| Load device functions   | 200 | 16317          | 81.585            |
| Teardown and full init  | 20 | 80000          | 4000            |

[Volk](https://github.com/zeux/volk)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 3269          | 16.345            |
| Load device functions   | 200 | 13644          | 68.22            |
| Teardown and full init  | 20 | 1709          | 85.45            |

### macOS

And on macOS with a patched libvulkan:

[GLAD (dav1dde)](https://github.com/Dav1dde/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 54283          | 271.415            |
| Load device functions   | 200 | 130537          | 652.685            |
| Teardown and full init  | 20 | 24580          | 1229            |

[GLAD (tycho)](https://github.com/tycho/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 428          | 2.14            |
| Load device functions   | 200 | 1387          | 6.935            |
| Teardown and full init  | 20 | 4097          | 204.85            |

[Gloam](https://github.com/tycho/gloam)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 467          | 2.335            |
| Load device functions   | 200 | 1403          | 7.015            |
| Teardown and full init  | 20 | 4083          | 204.15            |

[Volk](https://github.com/zeux/volk)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 524          | 2.62            |
| Load device functions   | 200 | 2252          | 11.26            |
| Teardown and full init  | 20 | 289          | 14.45            |

## Why is Volk so fast?

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
