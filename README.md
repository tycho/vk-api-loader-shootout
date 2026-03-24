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

[GLAD (dav1dde)](https://github.com/Dav1dde/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 757151          | 3785.76            |
| Load device functions   | 200 | 1208994          | 6044.97            |
| Teardown and full init  | 20 | 271865          | 13593.2            |

[GLAD (tycho)](https://github.com/tycho/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 12566          | 62.83            |
| Load device functions   | 200 | 142006          | 710.03            |
| Teardown and full init  | 20 | 159309          | 7965.45            |

[Gloam](https://github.com/tycho/gloam)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 14631          | 73.155            |
| Load device functions   | 200 | 14456          | 72.28            |
| Teardown and full init  | 20 | 2936          | 146.8            |

[Volk](https://github.com/zeux/volk)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 17295          | 86.475            |
| Load device functions   | 200 | 150089          | 750.445            |
| Teardown and full init  | 20 | 17051          | 852.55            |

Loader object sizes in bytes:
```
316288  obj/loader-glad-dav1dde.o
 67192  obj/loader-glad-tycho.o
 48104  obj/loader-gloam.o
312912  obj/loader-volk.o
```

Test program sizes in bytes:
```
104800  bin/test-glad-dav1dde
 80288  bin/test-glad-tycho
 55712  bin/test-gloam
 96568  bin/test-volk
```

Section sizes:
```
 text  data  bss   dec     hex    filename
99092  832   6568  106492  19ffc  bin/test-glad-dav1dde
62043  7360  7040  76443   12a9b  bin/test-glad-tycho
48092  840   7040  55972   daa4   bin/test-gloam
88993  792   6040  95825   17651  bin/test-volk
```

vulkaninfo
```
'DISPLAY' environment variable not set... skipping surface info
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

[GLAD (dav1dde)](https://github.com/Dav1dde/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 534584          | 2672.92            |
| Load device functions   | 200 | 983645          | 4918.23            |
| Teardown and full init  | 20 | 201104          | 10055.2            |

[GLAD (tycho)](https://github.com/tycho/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 8953          | 44.765            |
| Load device functions   | 200 | 109913          | 549.565            |
| Teardown and full init  | 20 | 106057          | 5302.85            |

[Gloam](https://github.com/tycho/gloam)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 9643          | 48.215            |
| Load device functions   | 200 | 7247          | 36.235            |
| Teardown and full init  | 20 | 1816          | 90.8            |

[Volk](https://github.com/zeux/volk)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 13003          | 65.015            |
| Load device functions   | 200 | 96282          | 481.41            |
| Teardown and full init  | 20 | 12190          | 609.5            |

Loader object sizes in bytes:
```
192545  obj/loader-glad-dav1dde.o
 52988  obj/loader-glad-tycho.o
 44580  obj/loader-gloam.o
221552  obj/loader-volk.o
```

Test program sizes in bytes:
```
112128  bin/test-glad-dav1dde.exe
 64000  bin/test-glad-tycho.exe
 60928  bin/test-gloam.exe
101888  bin/test-volk.exe
```

Section sizes:
```
 text  data   bss  dec     hex    filename
70646  39320  0    109966  1ad8e  bin/test-glad-dav1dde.exe
14886  46172  0    61058   ee82   bin/test-glad-tycho.exe
17270  41136  0    58406   e426   bin/test-gloam.exe
70326  29244  0    99570   184f2  bin/test-volk.exe
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

[GLAD (dav1dde)](https://github.com/Dav1dde/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 107995          | 539.975            |
| Load device functions   | 200 | 227384          | 1136.92            |
| Teardown and full init  | 20 | 44170          | 2208.5            |

[GLAD (tycho)](https://github.com/tycho/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 4354          | 21.77            |
| Load device functions   | 200 | 26572          | 132.86            |
| Teardown and full init  | 20 | 15461          | 773.05            |

[Gloam](https://github.com/tycho/gloam)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 8737          | 43.685            |
| Load device functions   | 200 | 5004          | 25.02            |
| Teardown and full init  | 20 | 1402          | 70.1            |

[Volk](https://github.com/zeux/volk)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 10792          | 53.96            |
| Load device functions   | 200 | 63357          | 316.785            |
| Teardown and full init  | 20 | 7351          | 367.55            |

Loader object sizes in bytes:
```
275560  obj/loader-glad-dav1dde.o
 75640  obj/loader-glad-tycho.o
 44880  obj/loader-gloam.o
250024  obj/loader-volk.o
```

Test program sizes in bytes:
```
161344  bin/test-glad-dav1dde
 70032  bin/test-glad-tycho
 69992  bin/test-gloam
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
        deviceUUID         = fb030000-0100-0000-0000-000000000000
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
| Load instance functions | 200 | 122923          | 614.615            |
| Load device functions   | 200 | 347979          | 1739.89            |
| Teardown and full init  | 20 | 59780          | 2989            |

[GLAD (tycho)](https://github.com/tycho/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 2143          | 10.715            |
| Load device functions   | 200 | 20933          | 104.665            |
| Teardown and full init  | 20 | 22696          | 1134.8            |

[Gloam](https://github.com/tycho/gloam)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 2493          | 12.465            |
| Load device functions   | 200 | 1497          | 7.485            |
| Teardown and full init  | 20 | 417          | 20.85            |

[Volk](https://github.com/zeux/volk)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 2268          | 11.34            |
| Load device functions   | 200 | 21713          | 108.565            |
| Teardown and full init  | 20 | 2414          | 120.7            |

Note in particular how much faster volk is with that patch (compared to
[above](#linux)). We cut around 90% of our `Teardown and full init` test time
by using a patched libvulkan.

### MinGW

This approach helps on Windows too. Here's MinGW with a patched libvulkan DLL
added to the `bin` directory:

[GLAD (dav1dde)](https://github.com/Dav1dde/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 516683          | 2583.41            |
| Load device functions   | 200 | 671456          | 3357.28            |
| Teardown and full init  | 20 | 163671          | 8183.55            |

[GLAD (tycho)](https://github.com/tycho/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 2905          | 14.525            |
| Load device functions   | 200 | 15760          | 78.8            |
| Teardown and full init  | 20 | 90383          | 4519.15            |

[Gloam](https://github.com/tycho/gloam)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 4025          | 20.125            |
| Load device functions   | 200 | 2330          | 11.65            |
| Teardown and full init  | 20 | 648          | 32.4            |

[Volk](https://github.com/zeux/volk)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 3338          | 16.69            |
| Load device functions   | 200 | 13677          | 68.385            |
| Teardown and full init  | 20 | 1700          | 85            |

### macOS

And on macOS with a patched libvulkan:

[GLAD (dav1dde)](https://github.com/Dav1dde/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 51812          | 259.06            |
| Load device functions   | 200 | 124249          | 621.245            |
| Teardown and full init  | 20 | 23489          | 1174.45            |

[GLAD (tycho)](https://github.com/tycho/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 424          | 2.12            |
| Load device functions   | 200 | 1362          | 6.81            |
| Teardown and full init  | 20 | 3763          | 188.15            |

[Gloam](https://github.com/tycho/gloam)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 962          | 4.81            |
| Load device functions   | 200 | 329          | 1.645            |
| Teardown and full init  | 20 | 132          | 6.6            |

[Volk](https://github.com/zeux/volk)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 507          | 2.535            |
| Load device functions   | 200 | 2163          | 10.815            |
| Teardown and full init  | 20 | 277          | 13.85            |

## Why is Volk so fast compared to GLAD?

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

## Gloam: discovery mode vs. enabled-list loading

Gloam offers two Vulkan loading workflows. The **discovery mode**
(`gloamLoaderLoadVulkan`) works like GLAD: it calls
`vkEnumerateInstanceExtensionProperties` and
`vkEnumerateDeviceExtensionProperties` to find out what the driver supports,
then sets the extension presence flags and loads the corresponding PFNs. The
**enabled-list mode** (`gloamVulkanInitialize` / `gloamVulkanLoadInstance`
/ `gloamVulkanLoadDevice`) skips enumeration entirely — the caller tells gloam
which API version and extensions they enabled, and gloam loads only those PFNs.

The benchmark numbers in the results tables above use the enabled-list mode.
Here's what Gloam looks like in discovery mode on the same Linux machine, for
comparison.

### Linux (discovery mode, unpatched libvulkan)

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

[Gloam](https://github.com/tycho/gloam) (discovery mode)
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

### Linux (discovery mode, patched libvulkan)

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

[Gloam](https://github.com/tycho/gloam) (discovery mode)
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

### Analysis

In discovery mode, Gloam's device-load and teardown numbers closely match GLAD
(tycho) — both are around 760µs per device load and ~9900µs per teardown cycle
with the unpatched libvulkan. The cost is dominated by
`vkEnumerateDeviceExtensionProperties`, which the Vulkan loader implements by
loading and querying every ICD and scanning implicit layers. Both GLAD and
Gloam pay this cost when they discover extensions from the driver.

Volk avoids this cost entirely by not doing any extension detection, which is
why its teardown-and-reinit is so much faster (855µs vs ~9900µs). But this
comes with a correctness trade-off: Volk blindly calls `vkGetInstanceProcAddr`
and `vkGetDeviceProcAddr` for every function in the spec, including extensions
that were never enabled. The Vulkan spec allows `vkGetInstanceProcAddr` to
return non-NULL for any known command name regardless of whether the extension
was enabled, so Volk's context can end up populated with loader trampoline
pointers that are not safe to call. The user has no way to distinguish these
from legitimately loaded functions without tracking their enabled extensions
separately — which defeats the purpose of having a loader manage the dispatch
table. Volk also does not perform alias resolution, so promoted functions (like
`vkCmdDrawIndirectCount` in VK 1.2, aliased from `vkCmdDrawIndirectCountKHR`)
may have one slot populated and the other NULL depending on driver behavior,
forcing the user to manually copy pointers between slots.

Gloam's enabled-list mode avoids all of these issues. Slots for functions you
didn't enable stay NULL, which is a clean and unambiguous signal not to call
them. Alias resolution runs automatically over the enabled set, so both the
core and extension names work regardless of which name the driver resolves.

Switching Gloam to the enabled-list mode eliminates both penalties. Compare the
Gloam rows between the discovery and enabled-list results:

| Metric (unpatched libvulkan)  | Discovery mode | Enabled-list mode | Speedup |
|-------------------------------|---------------|-------------------|---------|
| Load device functions (avg)   | 759.58µs      | 72.28µs           | ~10.5×  |
| Teardown and full init (avg)  | 9909.85µs     | 146.8µs           | ~67×    |

The device-load improvement comes from loading only the PFNs for enabled
extensions rather than querying the driver for every function in the spec. The
teardown improvement comes from skipping
`vkEnumerateInstanceExtensionProperties` and
`vkEnumerateDeviceExtensionProperties` entirely — `gloamVulkanFinalize` is just
a conditional `dlclose` and a `memset`, and `gloamVulkanInitialize` only loads
the handful of global-scope bootstrap functions needed to create an instance.

Instance loading barely changes (~71µs to ~73µs) because the discovery path was
already cheap at the instance level — `vkEnumerateInstanceExtensionProperties`
is much less expensive than the device variant, and the number of
instance-scope PFNs for VK 1.3 core is similar regardless of mode.

## Why is Gloam so fast compared to Volk?

Even comparing the enabled-list numbers against Volk, Gloam is substantially
faster in every category.

**Volk loads everything.** When you call `volkLoadInstanceOnly(instance)`, Volk
calls `vkGetInstanceProcAddr` for every instance-scope function in the entire
Vulkan spec — hundreds of entry points spanning all versions and all
extensions, regardless of whether they're relevant to your application.
`volkLoadDevice(device)` does the same for every device-scope function via
`vkGetDeviceProcAddr`.

**Gloam loads only what you enabled.** Because the caller tells gloam the API
version and the list of enabled extensions, gloam knows exactly which PFN slots
need to be populated. A `gloamVulkanLoadDevice` call for a VK 1.3 device with
no device extensions only loads the device-scope functions from VK 1.0 through
1.3 core — a fraction of the full spec.

This explains the device-loading numbers in particular. On Linux with a stock
libvulkan, Gloam loads device functions in ~72µs while Volk takes ~750µs
— a 10× difference. With the patched xxhash-based libvulkan that makes each
individual proc-addr lookup cheaper, the gap persists: Gloam at ~7.5µs vs Volk
at ~109µs, roughly 14×. The per-lookup cost dropped for both, but Volk still
has to do it for hundreds of functions the application never asked for.

The teardown-and-reinitialize benchmark tells the same story. Gloam's
`gloamVulkanFinalize` is a conditional `dlclose` plus a `memset`, and the
subsequent `gloamVulkanInitialize` only loads the handful of global-scope
bootstrap functions. Volk's `volkFinalize` zeros every slot by calling a dummy
proc-addr that always returns NULL, and reinitialization reloads the full set.
On Linux: Gloam at ~147µs vs Volk at ~853µs for the full cycle.

The instance-loading numbers are closer (Gloam ~73µs vs Volk ~86µs) because
most Vulkan extensions add device-scope functions, not instance-scope ones. The
"load everything" penalty is smaller at the instance level since the VK 1.3
core instance functions are already a large proportion of the total
instance-scope spec.

There's also a secondary effect: gloam's enabled-path dispatch doesn't use
a callback. It calls `vkGetInstanceProcAddr` and `vkGetDeviceProcAddr` directly
from the context struct, avoiding the function-pointer indirection that
callback-based loaders pay on every lookup.

