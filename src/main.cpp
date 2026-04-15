#include <iostream>
#include <iomanip>
#include <chrono>
#include <cstring>
#include <string>
#include <string_view>
#include <vector>

// Injected by the Makefile as '-DLOADER_ID="<name>"'. Falls back to "unknown"
// when compiled outside the Makefile (e.g. the MSVC solution), which only
// affects --json output - the human-readable default mode doesn't use it.
#ifndef LOADER_ID
#define LOADER_ID "unknown"
#endif
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#else
#include <dlfcn.h>
#include <unistd.h>  // getpid()
#endif

#ifdef USE_VOLK
#include <volk.h>
#elif defined(USE_GLAD)
#include <glad/vulkan.h>
#elif defined(USE_GLOAM_ENABLED) || defined(USE_GLOAM_DISCOVER)
#include <gloam/vk.h>
#endif

#if ((defined(USE_VOLK) ? 1 : 0) + \
     (defined(USE_GLAD) ? 1 : 0) + \
     (defined(USE_GLOAM_ENABLED) ? 1 : 0) + \
     (defined(USE_GLOAM_DISCOVER) ? 1 : 0)) != 1
#error "Must define exactly one of (USE_VOLK, USE_GLOAM_ENABLED, USE_GLOAM_DISCOVER, USE_GLAD) for this to compile"
#endif


// ARGH, Microsoft!
#ifndef __has_attribute
#define __has_attribute(x) 0
#endif

// Disable inlining so we get nice profiling data with complete call stacks.
#if defined(XOM_COMPILER_CLANG) && __has_attribute(noinline)
#define XOM_NOINLINE [[clang::noinline]]
#elif defined(XOM_COMPILER_GCC) && __has_attribute(noinline)
#define XOM_NOINLINE __attribute__((noinline))
#elif defined(XOM_COMPILER_MSVC)
#define XOM_NOINLINE __declspec(noinline)
#else
#define XOM_NOINLINE
#endif


// Number of repetitions for timing
constexpr int OUTER_COUNT = 10;
constexpr int INNER_COUNT = 300;
constexpr int INNER_COUNT_REINIT = INNER_COUNT / 10;

// Number of outer iterations for --profile mode (runs long enough for
// sampling profilers like macOS `sample` or Instruments to collect data)
constexpr int PROFILE_OUTER_COUNT = 5000;

// We keep this around to prevent libvulkan from being unloaded and reloaded
// for each API loader teardown and initialization
static void *libvulkan_handle = nullptr;
PFN_vkGetInstanceProcAddr pfnVkGetInstanceProcAddr = nullptr;

XOM_NOINLINE
static bool open_libvulkan()
{
#if defined(_WIN32)
    HMODULE module = LoadLibraryA("vulkan-1.dll");
    if (!module)
        return false;

    // note: function pointer is cast through void function pointer to silence cast-function-type warning on gcc8
    pfnVkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)(void(*)(void))GetProcAddress(module, "vkGetInstanceProcAddr");
#elif defined(__APPLE__)
    void *module = dlopen("libvulkan.dylib", RTLD_NOW | RTLD_LOCAL);
    if (!module)
        module = dlopen("libvulkan.1.dylib", RTLD_NOW | RTLD_LOCAL);
    if (!module)
        module = dlopen("libMoltenVK.dylib", RTLD_NOW | RTLD_LOCAL);
    // Add support for using Vulkan and MoltenVK in a Framework. App store rules for iOS
    // strictly enforce no .dylib's. If they aren't found it just falls through
    if (!module)
        module = dlopen("vulkan.framework/vulkan", RTLD_NOW | RTLD_LOCAL);
    if (!module)
        module = dlopen("MoltenVK.framework/MoltenVK", RTLD_NOW | RTLD_LOCAL);
    // modern versions of macOS don't search /usr/local/lib automatically contrary to what man dlopen says
    // Vulkan SDK uses this as the system-wide installation location, so we're going to fallback to this if all else fails
    if (!module && getenv("DYLD_FALLBACK_LIBRARY_PATH") == NULL)
        module = dlopen("/usr/local/lib/libvulkan.dylib", RTLD_NOW | RTLD_LOCAL);
    if (!module)
        return false;

    pfnVkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)dlsym(module, "vkGetInstanceProcAddr");
#else
    void *module = dlopen("libvulkan.so.1", RTLD_NOW | RTLD_LOCAL);
    if (!module)
        module = dlopen("libvulkan.so", RTLD_NOW | RTLD_LOCAL);
    if (!module)
        return false;
    pfnVkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)dlsym(module, "vkGetInstanceProcAddr");
#endif

    libvulkan_handle = static_cast<void *>(module);
    return true;
}

XOM_NOINLINE
static bool close_libvulkan()
{
    if (libvulkan_handle) {
#if defined(_WIN32)
        FreeLibrary((HMODULE)libvulkan_handle);
#else
        dlclose(libvulkan_handle);
#endif
        libvulkan_handle = nullptr;
        pfnVkGetInstanceProcAddr = nullptr;
    }
    return true;
}

XOM_NOINLINE
static bool loader_init()
{
#if defined(USE_VOLK)
    if (pfnVkGetInstanceProcAddr)
        volkInitializeCustom(pfnVkGetInstanceProcAddr);
    else
        volkInitialize();
#elif defined(USE_GLAD)
    int vk_version = gladLoaderLoadVulkan(NULL, NULL, NULL);
    if (!vk_version) {
        std::cerr << "Failed to initialize GLAD!" << std::endl;
        return false;
    }
#elif defined(USE_GLOAM_DISCOVER)
    int vk_version = gloamLoaderLoadVulkan(NULL, NULL, NULL);
    if (!vk_version) {
        std::cerr << "Failed to initialize gloam!" << std::endl;
        return false;
    }
#elif defined(USE_GLOAM_ENABLED)
    int vk_version = gloamVulkanInitialize(libvulkan_handle);
    if (!vk_version) {
        std::cerr << "Failed to initialize gloam!" << std::endl;
        return false;
    }
#endif
    return true;
}

XOM_NOINLINE
static bool loader_load_instance(VkInstance instance, uint32_t apiVersion, uint32_t nEnabledExtensions, const char * const *ppEnabledExtensions)
{
#if defined(USE_VOLK)
    volkLoadInstanceOnly(instance);
#elif defined(USE_GLAD)
    int vk_version = gladLoaderLoadVulkan(instance, NULL, NULL);
    if (!vk_version) {
        std::cerr << "GLAD failed to load VK instance functions!" << std::endl;
        return false;
    }
#elif defined(USE_GLOAM_DISCOVER)
    // Gloam has a marker in the discover API to indicate whether it's already
    // loaded extensions for instance/device. We need to reset that marker here
    // since we're trying to measure the cost of that path. In normal usage,
    // you'd only load instance/device functions once, though.
    gloam_vk_context.vk_found_instance_exts = 0;

    int vk_version = gloamLoaderLoadVulkan(instance, NULL, NULL);
    if (!vk_version) {
        std::cerr << "gloam failed to load VK instance functions!" << std::endl;
        return false;
    }
#elif defined(USE_GLOAM_ENABLED)
    int vk_version = gloamVulkanLoadInstance(instance, apiVersion, nEnabledExtensions, ppEnabledExtensions);
    if (!vk_version) {
        std::cerr << "gloam failed to load VK instance functions!" << std::endl;
        return false;
    }
#endif
    return true;
}

XOM_NOINLINE
static bool loader_load_device(VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice device, uint32_t nEnabledExtensions, const char * const *ppEnabledExtensions)
{
#if defined(USE_VOLK)
    volkLoadDevice(device);
#elif defined(USE_GLAD)
    int vk_version = gladLoaderLoadVulkan(instance, physicalDevice, device);
    if (!vk_version) {
        std::cerr << "GLAD failed to load VK device functions!" << std::endl;
        return false;
    }
#elif defined(USE_GLOAM_DISCOVER)
    // Gloam has a marker in the discover API to indicate whether it's already
    // loaded extensions for instance/device. We need to reset that marker here
    // since we're trying to measure the cost of that path. In normal usage,
    // you'd only load instance/device functions once, though.
    gloam_vk_context.vk_found_device_exts = 0;

    int vk_version = gloamLoaderLoadVulkan(instance, physicalDevice, device);
    if (!vk_version) {
        std::cerr << "gloam failed to load VK device functions!" << std::endl;
        return false;
    }
#elif defined(USE_GLOAM_ENABLED)
    int vk_version = gloamVulkanLoadDevice(device, physicalDevice, nEnabledExtensions, ppEnabledExtensions);
    if (!vk_version) {
        std::cerr << "gloam failed to load VK device functions!" << std::endl;
        return false;
    }
#endif
    return true;
}

XOM_NOINLINE
static bool loader_destroy()
{
#if defined(USE_VOLK)
    volkFinalize();
#elif defined(USE_GLAD)
    gladLoaderUnloadVulkan();
#elif defined(USE_GLOAM_DISCOVER)
    gloamLoaderUnloadVulkan();
#elif defined(USE_GLOAM_ENABLED)
    gloamVulkanFinalize();
#endif
    return true;
}

static bool full_vulkan_context_and_teardown()
{
    if (!loader_init())
        return false;

    VkInstance instance;
    VkDevice device;
    std::vector<const char *> enabledInstanceExtensions;
    std::vector<const char *> enabledDeviceExtensions;
    bool supportsPortabilityEnumeration = false;

    // Vulkan instance creation info
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Vulkan API loader benchmark";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo instanceCreateInfo{};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pApplicationInfo = &appInfo;
#if defined(GLAD_VK_KHR_portability_enumeration)
    supportsPortabilityEnumeration = GLAD_VK_KHR_portability_enumeration;
#elif defined(GLOAM_VK_KHR_portability_enumeration) && defined(USE_GLOAM_DISCOVER)
    supportsPortabilityEnumeration = GLOAM_VK_KHR_portability_enumeration;
#else
    {
        uint32_t count = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);
        std::vector<VkExtensionProperties> extensions(count);
        vkEnumerateInstanceExtensionProperties(nullptr, &count, extensions.data());
        for (uint32_t i = 0; i < count; i++) {
            if (strcmp(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME, extensions[i].extensionName) == 0) {
                supportsPortabilityEnumeration = true;
            }
        }
    }
#endif
    if (supportsPortabilityEnumeration) {
        enabledInstanceExtensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
        instanceCreateInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    }

    instanceCreateInfo.ppEnabledExtensionNames = enabledInstanceExtensions.empty() ? nullptr : enabledInstanceExtensions.data();
    instanceCreateInfo.enabledExtensionCount = enabledInstanceExtensions.size();

    if (vkCreateInstance(&instanceCreateInfo, nullptr, &instance) != VK_SUCCESS) {
        std::cerr << "Failed to create Vulkan instance!" << std::endl;
        return false;
    }

    // Load instance functions
    if (!loader_load_instance(instance, appInfo.apiVersion, instanceCreateInfo.enabledExtensionCount, instanceCreateInfo.ppEnabledExtensionNames))
        return false;

    // Select physical device
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (deviceCount == 0) {
        std::cerr << "Failed to find GPUs with Vulkan support!" << std::endl;
        vkDestroyInstance(instance, nullptr);
        return false;
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    VkPhysicalDevice &physicalDevice = devices[0];

    // Device creation info
    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = 0;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkDeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
    deviceCreateInfo.ppEnabledExtensionNames = enabledDeviceExtensions.empty() ? nullptr : enabledDeviceExtensions.data();
    deviceCreateInfo.enabledExtensionCount = enabledDeviceExtensions.size();

    if (vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device) != VK_SUCCESS) {
        std::cerr << "Failed to create logical device!" << std::endl;
        vkDestroyInstance(instance, nullptr);
        return false;
    }

    // Load device functions
    if (!loader_load_device(instance, physicalDevice, device, deviceCreateInfo.enabledExtensionCount, deviceCreateInfo.ppEnabledExtensionNames))
        return false;

    // Cleanup
    vkDestroyDevice(device, nullptr);
    vkDestroyInstance(instance, nullptr);
    loader_destroy();

    return true;
}

// Collected benchmark results. Each entry carries both the machine-readable
// task id (used in --json output) and the human-readable title (used in the
// default Markdown table). main() accumulates into g_results and then emits
// the right format at the end based on g_emit_json.
struct BenchResult {
    std::string_view task_id;
    std::string_view title;
    uint64_t iterations;
    std::chrono::microseconds duration;
};

static std::vector<BenchResult> g_results;
static bool g_emit_json = false;
static const char *g_profile_task = nullptr;  // non-null = --profile mode

static void record_result(std::string_view task_id, std::string_view title, uint64_t iterations, std::chrono::microseconds duration)
{
    g_results.push_back({task_id, title, iterations, duration});
    if (g_emit_json)
        return;
    std::cout << "| " <<
        std::setw(42) << std::left << title << " | " <<
        std::setw(10) << std::right << iterations << " | " <<
        std::setw(15) << std::right << duration.count() << " | " <<
        std::setw(17) << std::right << std::fixed << std::setprecision(2) << (duration.count() / static_cast<double>(iterations)) << " |\n";
}

static void emit_json()
{
    std::cout << "{\n";
    std::cout << "  \"loader\": \"" << LOADER_ID << "\",\n";
    std::cout << "  \"benchmarks\": {\n";
    for (size_t i = 0; i < g_results.size(); ++i) {
        const auto &r = g_results[i];
        double avg = r.duration.count() / static_cast<double>(r.iterations);
        std::cout << "    \"" << r.task_id << "\": { "
                  << "\"iterations\": " << r.iterations << ", "
                  << "\"total_us\": " << r.duration.count() << ", "
                  << "\"avg_us\": " << std::fixed << std::setprecision(2) << avg
                  << " }" << (i + 1 < g_results.size() ? "," : "") << "\n";
    }
    std::cout << "  }\n";
    std::cout << "}\n";
}

int main(int argc, char **argv) {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--json") == 0) {
            g_emit_json = true;
        } else if (std::strcmp(argv[i], "--profile") == 0) {
            if (i + 1 >= argc) {
                std::cerr << "--profile requires a task id\n";
                std::cerr << "valid tasks: full_ctx_persistent, full_ctx_transient\n";
                return 2;
            }
            g_profile_task = argv[++i];
        } else {
            std::cerr << "unknown argument: " << argv[i] << "\n";
            std::cerr << "usage: " << argv[0] << " [--json] [--profile <task_id>]\n";
            return 2;
        }
    }

    // --profile mode: run a single task in a long loop so a sampling profiler
    // (macOS `sample`, Instruments, samply, etc.) can collect meaningful data.
    // The process prints its PID on stderr so you can attach a profiler, then
    // runs PROFILE_OUTER_COUNT * INNER_COUNT_REINIT iterations and exits.
    if (g_profile_task) {
        std::cerr << "profile mode: task=" << g_profile_task
                  << "  pid=" <<
#ifdef _WIN32
                     GetCurrentProcessId()
#else
                     getpid()
#endif
                  << "  iterations=" << PROFILE_OUTER_COUNT * INNER_COUNT_REINIT
                  << std::endl;

        if (std::strcmp(g_profile_task, "full_ctx_transient") == 0) {
            // libvulkan is NOT pre-opened — each iteration does dlopen/dlclose
            for (int i = 0; i < PROFILE_OUTER_COUNT; ++i) {
                for (int j = 0; j < INNER_COUNT_REINIT; ++j) {
                    if (!full_vulkan_context_and_teardown())
                        return 1;
                }
            }
            std::cerr << "profile complete" << std::endl;
            return 0;
        }

        if (std::strcmp(g_profile_task, "full_ctx_persistent") == 0) {
            if (!open_libvulkan()) {
                std::cerr << "Failed to open libvulkan" << std::endl;
                return -1;
            }
            for (int i = 0; i < PROFILE_OUTER_COUNT; ++i) {
                for (int j = 0; j < INNER_COUNT_REINIT; ++j) {
                    if (!full_vulkan_context_and_teardown())
                        return 1;
                }
            }
            close_libvulkan();
            std::cerr << "profile complete" << std::endl;
            return 0;
        }

        std::cerr << "unknown profile task: " << g_profile_task << "\n";
        std::cerr << "valid tasks: full_ctx_persistent, full_ctx_transient\n";
        return 2;
    }

    if (!open_libvulkan()) {
        std::cerr << "Failed to open libvulkan" << std::endl;
        return -1;
    }

    if (!loader_init())
        return -1;

    VkInstance instance;
    VkDevice device;
    std::vector<const char *> enabledInstanceExtensions;
    std::vector<const char *> enabledDeviceExtensions;
    bool supportsPortabilityEnumeration = false;

    // Vulkan instance creation info
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Vulkan API loader benchmark";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo instanceCreateInfo{};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pApplicationInfo = &appInfo;
#if defined(GLAD_VK_KHR_portability_enumeration)
    supportsPortabilityEnumeration = GLAD_VK_KHR_portability_enumeration;
#elif defined(GLOAM_VK_KHR_portability_enumeration) && defined(USE_GLOAM_DISCOVER)
    supportsPortabilityEnumeration = GLOAM_VK_KHR_portability_enumeration;
#else
    {
        uint32_t count = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);
        std::vector<VkExtensionProperties> extensions(count);
        vkEnumerateInstanceExtensionProperties(nullptr, &count, extensions.data());
        for (uint32_t i = 0; i < count; i++) {
            if (strcmp(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME, extensions[i].extensionName) == 0) {
                supportsPortabilityEnumeration = true;
            }
        }
    }
#endif
    if (supportsPortabilityEnumeration) {
        enabledInstanceExtensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
        instanceCreateInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    }

    instanceCreateInfo.ppEnabledExtensionNames = enabledInstanceExtensions.empty() ? nullptr : enabledInstanceExtensions.data();
    instanceCreateInfo.enabledExtensionCount = enabledInstanceExtensions.size();

    if (vkCreateInstance(&instanceCreateInfo, nullptr, &instance) != VK_SUCCESS) {
        std::cerr << "Failed to create Vulkan instance!" << std::endl;
        return -1;
    }

    // Load instance functions
    if (!loader_load_instance(instance, appInfo.apiVersion, instanceCreateInfo.enabledExtensionCount, instanceCreateInfo.ppEnabledExtensionNames))
        return -1;

    // Select physical device
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (deviceCount == 0) {
        std::cerr << "Failed to find GPUs with Vulkan support!" << std::endl;
        vkDestroyInstance(instance, nullptr);
        return -1;
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    VkPhysicalDevice &physicalDevice = devices[0];

    // Device creation info
    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = 0;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkDeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
    deviceCreateInfo.ppEnabledExtensionNames = enabledDeviceExtensions.empty() ? nullptr : enabledDeviceExtensions.data();
    deviceCreateInfo.enabledExtensionCount = enabledDeviceExtensions.size();

    if (vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device) != VK_SUCCESS) {
        std::cerr << "Failed to create logical device!" << std::endl;
        vkDestroyInstance(instance, nullptr);
        return -1;
    }

    // Load device functions
    if (!loader_load_device(instance, physicalDevice, device, deviceCreateInfo.enabledExtensionCount, deviceCreateInfo.ppEnabledExtensionNames))
        return -1;

    if (!g_emit_json) {
        std::cout << "| Task                                       | Iterations | Total Time (µs) | Average Time (µs) |\n";
        std::cout << "|--------------------------------------------|------------|-----------------|-------------------|\n";
    }

    // Benchmark loading instance functions
    std::chrono::microseconds bestInstanceDuration = std::chrono::microseconds::max();
    for (int i = 0; i < OUTER_COUNT; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < INNER_COUNT; ++i) {
            if (!loader_load_instance(instance, appInfo.apiVersion, instanceCreateInfo.enabledExtensionCount, instanceCreateInfo.ppEnabledExtensionNames))
                return 1;
        }
        auto end = std::chrono::high_resolution_clock::now();
        bestInstanceDuration = std::min(bestInstanceDuration, std::chrono::duration_cast<std::chrono::microseconds>(end - start));
    }
    record_result("load_instance", "Load instance functions", INNER_COUNT, bestInstanceDuration);

    // Benchmark loading device functions
    std::chrono::microseconds bestDeviceDuration = std::chrono::microseconds::max();
    for (int i = 0; i < OUTER_COUNT; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < INNER_COUNT; ++i) {
            if (!loader_load_device(instance, physicalDevice, device, deviceCreateInfo.enabledExtensionCount, deviceCreateInfo.ppEnabledExtensionNames))
                return 1;
        }
        auto end = std::chrono::high_resolution_clock::now();
        bestDeviceDuration = std::min(bestDeviceDuration, std::chrono::duration_cast<std::chrono::microseconds>(end - start));
    }
    record_result("load_device", "Load device functions", INNER_COUNT, bestDeviceDuration);

    // Benchmark full teardown and initialization of API loader
    std::chrono::microseconds bestReDetect = std::chrono::microseconds::max();
    for (int i = 0; i < OUTER_COUNT; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < INNER_COUNT_REINIT; ++i) {
            if (!loader_destroy())
                return 1;
            if (!loader_init())
                return 1;
            if (!loader_load_instance(instance, appInfo.apiVersion, instanceCreateInfo.enabledExtensionCount, instanceCreateInfo.ppEnabledExtensionNames))
                return 1;
            if (!loader_load_device(instance, physicalDevice, device, deviceCreateInfo.enabledExtensionCount, deviceCreateInfo.ppEnabledExtensionNames))
                return 1;
        }
        auto end = std::chrono::high_resolution_clock::now();
        bestReDetect = std::min(bestReDetect, std::chrono::duration_cast<std::chrono::microseconds>(end - start));
    }
    record_result("init_load_all", "Init + load all functions", INNER_COUNT_REINIT, bestReDetect);

    // Cleanup
    vkDestroyDevice(device, nullptr);
    vkDestroyInstance(instance, nullptr);
    loader_destroy();

    // Benchmark full teardown and initialization of Vulkan context, with libvulkan staying resident
    std::chrono::microseconds bestContextResidentLibvulkan = std::chrono::microseconds::max();
    for (int i = 0; i < OUTER_COUNT; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < INNER_COUNT_REINIT; ++i) {
            if (!full_vulkan_context_and_teardown())
                return 1;
        }
        auto end = std::chrono::high_resolution_clock::now();
        bestContextResidentLibvulkan = std::min(bestContextResidentLibvulkan, std::chrono::duration_cast<std::chrono::microseconds>(end - start));
    }
    record_result("full_ctx_persistent", "Full VK context (libvulkan persistent)", INNER_COUNT_REINIT, bestContextResidentLibvulkan);

    close_libvulkan();

    // Benchmark full teardown and initialization of Vulkan context, without libvulkan staying resident
    std::chrono::microseconds bestContextNoLibvulkan = std::chrono::microseconds::max();
    for (int i = 0; i < OUTER_COUNT; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < INNER_COUNT_REINIT; ++i) {
            if (!full_vulkan_context_and_teardown())
                return 1;
        }
        auto end = std::chrono::high_resolution_clock::now();
        bestContextNoLibvulkan = std::min(bestContextNoLibvulkan, std::chrono::duration_cast<std::chrono::microseconds>(end - start));
    }
    record_result("full_ctx_transient", "Full VK context (libvulkan transient)", INNER_COUNT_REINIT, bestContextNoLibvulkan);

    if (g_emit_json)
        emit_json();

    return 0;
}

/* vim: set ts=4 sts=4 sw=4 et: */
