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
| GLAD (tycho)   | `2.0.8-46-gfd2c902`   |
| Volk           | `1.4.304-1-gf97f25c`         |
| xxHash         | `0.7.4-837-g1ac318b`       |
| Vulkan-Headers | `1.4.304`   |

| Tool         | Name             | Version             |
|--------------|------------------|---------------------|
| Kernel       | `Linux`   | `6.12.7-1-hsw`      |
| Architecture | `x86_64`   | `AMD Ryzen 9 3950X 16-Core Processor`    |
| CC           | `clang`        | `19.1.6`   |
| CXX          | `clang++`       | `19.1.6`  |

| Variable     | Value         |
|--------------|---------------|
| `OPTFLAGS` | `-O2 -march=x86-64-v2 -mtune=znver3 -g0` |
| `CFLAGS`   | `-std=c17` |
| `CXXFLAGS` | `-std=c++20` |

[GLAD (dav1dde)](https://github.com/Dav1dde/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 720747          | 3603.74            |
| Load device functions   | 200 | 1140607          | 5703.03            |
| Teardown and full init  | 20 | 255698          | 12784.9            |

[GLAD (tycho)](https://github.com/tycho/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 150200          | 751            |
| Load device functions   | 200 | 139282          | 696.41            |
| Teardown and full init  | 20 | 152534          | 7626.7            |

[Volk](https://github.com/zeux/volk)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 14186          | 70.93            |
| Load device functions   | 200 | 116361          | 581.805            |
| Teardown and full init  | 20 | 13179          | 658.95            |

Loader object sizes in bytes:
```
316216  obj/loader-glad-dav1dde.o
 87336  obj/loader-glad-tycho.o
260640  obj/loader-volk.o
```

Test program sizes in bytes:
```
108768  bin/test-glad-dav1dde
 80984  bin/test-glad-tycho
 84136  bin/test-volk
```

Section sizes:
```
   text    data     bss     dec     hex filename
  98825     792    6568  106185   19ec9 bin/test-glad-dav1dde
  63281   11432    6184   80897   13c01 bin/test-glad-tycho
  74111     736    5344   80191   1393f bin/test-volk
```


MinGW
-----
| Project        | Version (git describe)      |
|----------------|-----------------------------|
| GLAD (dav1dde) | `2.0.8-1-gf492295` |
| GLAD (tycho)   | `2.0.8-46-gfd2c902`   |
| Volk           | `1.4.304-1-gf97f25c`         |
| xxHash         | `0.7.4-837-g1ac318b`       |
| Vulkan-Headers | `1.4.304`   |

| Tool         | Name             | Version             |
|--------------|------------------|---------------------|
| Kernel       | `MINGW64_NT-10.0-26100`   | `3.5.4-0bc1222b.x86_64`      |
| Architecture | `x86_64`   | `AMD Ryzen 9 5950X 16-Core Processor`    |
| CC           | `clang`        | `19.1.6`   |
| CXX          | `clang++`       | `19.1.6`  |

| Variable     | Value         |
|--------------|---------------|
| `OPTFLAGS` | `-O2 -march=x86-64-v2 -mtune=znver3 -g0` |
| `CFLAGS`   | `-std=c17` |
| `CXXFLAGS` | `-std=c++20` |

[GLAD (dav1dde)](https://github.com/Dav1dde/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 564451          | 2822.26            |
| Load device functions   | 200 | 1116487          | 5582.44            |
| Teardown and full init  | 20 | 219186          | 10959.3            |

[GLAD (tycho)](https://github.com/tycho/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 219945          | 1099.72            |
| Load device functions   | 200 | 155970          | 779.85            |
| Teardown and full init  | 20 | 109250          | 5462.5            |

[Volk](https://github.com/zeux/volk)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 19158          | 95.79            |
| Load device functions   | 200 | 126358          | 631.79            |
| Teardown and full init  | 20 | 15311          | 765.55            |

Loader object sizes in bytes:
```
192281  obj/loader-glad-dav1dde.o
 63727  obj/loader-glad-tycho.o
185595  obj/loader-volk.o
```

Test program sizes in bytes:
```
112128  bin/test-glad-dav1dde.exe
 70656  bin/test-glad-tycho.exe
 88064  bin/test-volk.exe
```

Section sizes:
```
   text    data     bss     dec     hex filename
  70566   39001       0  109567   1abff bin/test-glad-dav1dde.exe
  21622   45861       0   67483   1079b bin/test-glad-tycho.exe
  59030   25937       0   84967   14be7 bin/test-volk.exe
```


macOS
-----
| Project        | Version (git describe)      |
|----------------|-----------------------------|
| GLAD (dav1dde) | `2.0.8-1-gf492295` |
| GLAD (tycho)   | `2.0.8-46-gfd2c902`   |
| Volk           | `1.4.304-1-gf97f25c`         |
| xxHash         | `0.7.4-837-g1ac318b`       |
| Vulkan-Headers | `1.4.304`   |

| Tool         | Name             | Version             |
|--------------|------------------|---------------------|
| Kernel       | `Darwin`   | `24.2.0`      |
| Architecture | `arm64`   | `Apple M1`    |
| CC           | `clang`        | `16.0.0`   |
| CXX          | `clang++`       | `16.0.0`  |

| Variable     | Value         |
|--------------|---------------|
| `OPTFLAGS` | `-O2 -mcpu=native -g0` |
| `CFLAGS`   | `-std=c17` |
| `CXXFLAGS` | `-std=c++20` |

[GLAD (dav1dde)](https://github.com/Dav1dde/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 134515          | 672.575            |
| Load device functions   | 200 | 342931          | 1714.65            |
| Teardown and full init  | 20 | 62021          | 3101.05            |

[GLAD (tycho)](https://github.com/tycho/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 60958          | 304.79            |
| Load device functions   | 200 | 41532          | 207.66            |
| Teardown and full init  | 20 | 20013          | 1000.65            |

[Volk](https://github.com/zeux/volk)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 16525          | 82.625            |
| Load device functions   | 200 | 82648          | 413.24            |
| Teardown and full init  | 20 | 10747          | 537.35            |

Loader object sizes in bytes:
```
264152  obj/loader-glad-dav1dde.o
 85344  obj/loader-glad-tycho.o
214544  obj/loader-volk.o
```

Test program sizes in bytes:
```
161328  bin/test-glad-dav1dde
 86576  bin/test-glad-tycho
102712  bin/test-volk
```

Section sizes:
```
__TEXT	__DATA	__OBJC	others	dec	hex
114688	16384	0	4295016448	4295147520	10002c000	bin/test-glad-dav1dde
49152	16384	0	4295000064	4295065600	100018000	bin/test-glad-tycho
81920	16384	0	4295000064	4295098368	100020000	bin/test-volk
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
| Load instance functions | 200 | 685695          | 3428.47            |
| Load device functions   | 200 | 909244          | 4546.22            |
| Teardown and full init  | 20 | 229969          | 11498.5            |

[GLAD (tycho)](https://github.com/tycho/glad)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 24484          | 122.42            |
| Load device functions   | 200 | 20375          | 101.875            |
| Teardown and full init  | 20 | 138170          | 6908.5            |

[Volk](https://github.com/zeux/volk)
| Task                 | Iterations | Total Time (µs) | Average Time (µs) |
|----------------------|------------|-----------------|-------------------|
| Load instance functions | 200 | 2275          | 11.375            |
| Load device functions   | 200 | 13006          | 65.03            |
| Teardown and full init  | 20 | 1530          | 76.5            |

Note in particular how much faster volk is with that patch. We cut 89% of our
`Teardown and full init` test time by using a patched libvulkan.

volk vs GLAD performance
------------------------

The main reason GLAD ends up slower than volk here is because GLAD has
a feature that volk does not: extension detection. With GLAD you can do things
after the API loader has initialized, like this:

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
