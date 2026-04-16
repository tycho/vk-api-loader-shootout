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

**Do not compare numbers across platforms.** Hardware, drivers, OS, and
compiler flags all differ between the Linux, MinGW, and macOS test machines.
The meaningful comparisons are *within* a single platform: loader vs. loader,
and patched vs. unpatched on the same hardware.

### Linux

Tested on AMD Ryzen AI MAX+ PRO 395 (x86\_64, RADV/Mesa).
**[Full results: Linux report](reports/linux.md)**

### MinGW

Tested on AMD Ryzen AI MAX+ PRO 395 (x86\_64, AMD proprietary driver, Windows).
**[Full results: MinGW report](reports/mingw.md)**

### macOS

Tested on Apple M5 (arm64, MoltenVK + KosmicKrisp).
**[Full results: macOS report](reports/macos.md)**

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
unpatched): GLAD (tycho) at ~793µs and Gloam discover at ~801µs, versus
~38µs for Gloam enabled-list which skips enumeration entirely. The
device-load numbers are similarly inflated because
`vkEnumerateDeviceExtensionProperties` runs during that phase: Gloam discover
at ~280µs versus ~34µs for Gloam enabled-list.

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
| Load instance functions (avg)    | 239.85µs       | 3.17µs             | ~76×    |
| Load device functions (avg)      | 280.25µs       | 34.07µs            | ~8×     |
| Init + load all functions (avg)  | 800.73µs       | 38.33µs            | ~21×    |

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
| Load device (avg, unpatched)        | 347.14µs | 34.07µs       | ~10×    |
| Load device (avg, patched)          | 45.47µs  | 5.85µs        | ~8×     |
| Init + load all (avg, unpatched)    | 390.50µs | 38.33µs       | ~10×    |
| Init + load all (avg, patched)      | 54.63µs  | 7.83µs        | ~7×     |

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
| GLAD (dav1dde)        | Load device functions (avg)       | 1427.07µs    | 640.17µs    | ~2×     |
| GLAD (dav1dde)        | Init + load all functions (avg)   | 2219.37µs    | 784.20µs    | ~3×     |
| GLAD (tycho)          | Load device functions (avg)       | 253.82µs     | 39.85µs     | ~6×     |
| GLAD (tycho)          | Init + load all functions (avg)   | 792.57µs     | 76.13µs     | ~10×    |
| Gloam (discover)      | Load device functions (avg)       | 280.25µs     | 49.87µs     | ~6×     |
| Gloam (discover)      | Init + load all functions (avg)   | 800.73µs     | 76.20µs     | ~11×    |
| Gloam (enabled-list)  | Load device functions (avg)       | 34.07µs      | 5.85µs      | ~6×     |
| Gloam (enabled-list)  | Init + load all functions (avg)   | 38.33µs      | 7.83µs      | ~5×     |
| Volk                  | Load device functions (avg)       | 347.14µs     | 45.47µs     | ~8×     |
| Volk                  | Init + load all functions (avg)   | 390.50µs     | 54.63µs     | ~7×     |

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
| Full VK context, persistent (avg)    | 8759.50µs    | 5724.77µs   |
| Full VK context, transient (avg)     | 8349.53µs    | 8238.67µs   |
| Persistent vs transient gap          | ~410µs       | ~2514µs     |

The unpatched gap of ~410µs is noise — the scan happens either way. The patched
gap of ~2514µs is the cost of rebuilding the manifest cache from scratch versus
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
