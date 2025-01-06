#include <iostream>
#include <chrono>
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
#endif

#ifdef USE_VOLK
#include <volk.h>
#elif defined(USE_GLAD)
#include <glad/vulkan.h>
#else
#error "Must define either USE_VOLK or USE_GLAD for this to compile"
#endif

#define ARRAYSIZE(x) (sizeof(x) / sizeof(x[0]))

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
constexpr int OUTER_COUNT = 5;
constexpr int INNER_COUNT = 200;
constexpr int INNER_COUNT_REINIT = INNER_COUNT / 10;

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
    volkInitializeCustom(pfnVkGetInstanceProcAddr);
#elif defined(USE_GLAD)
    int vk_version = gladLoaderLoadVulkan(NULL, NULL, NULL);
    if (!vk_version) {
        std::cerr << "Failed to initialize GLAD!" << std::endl;
        return false;
    }
#endif
    return true;
}

XOM_NOINLINE
static bool loader_load_instance(VkInstance instance)
{
#if defined(USE_VOLK)
    volkLoadInstanceOnly(instance);
#elif defined(USE_GLAD)
    int vk_version = gladLoaderLoadVulkan(instance, NULL, NULL);
    if (!vk_version) {
        std::cerr << "GLAD failed to load VK instance functions!" << std::endl;
        return false;
    }
#endif
    return true;
}

XOM_NOINLINE
static bool loader_load_device(VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice device)
{
#if defined(USE_VOLK)
    volkLoadDevice(device);
#elif defined(USE_GLAD)
    int vk_version = gladLoaderLoadVulkan(instance, physicalDevice, device);
    if (!vk_version) {
        std::cerr << "GLAD failed to load VK device functions!" << std::endl;
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
#endif
    return true;
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    if (!open_libvulkan()) {
        std::cerr << "Failed to open libvulkan" << std::endl;
        return -1;
    }

    VkInstance instance;
    VkDevice device;

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
#ifdef __APPLE__
    const char *enabledExtensions[] = {
        VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
    };
    instanceCreateInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    instanceCreateInfo.ppEnabledExtensionNames = enabledExtensions;
    instanceCreateInfo.enabledExtensionCount = ARRAYSIZE(enabledExtensions);
#endif

    if (!loader_init())
        return -1;

    if (vkCreateInstance(&instanceCreateInfo, nullptr, &instance) != VK_SUCCESS) {
        std::cerr << "Failed to create Vulkan instance!" << std::endl;
        return -1;
    }

    // Load instance functions
    if (!loader_load_instance(instance))
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

    if (vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device) != VK_SUCCESS) {
        std::cerr << "Failed to create logical device!" << std::endl;
        vkDestroyInstance(instance, nullptr);
        return -1;
    }

    // Load device functions
    if (!loader_load_device(instance, physicalDevice, device))
        return -1;

    // Benchmark loading instance functions
    std::chrono::microseconds bestInstanceDuration = std::chrono::microseconds::max();
    for (int i = 0; i < OUTER_COUNT; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < INNER_COUNT; ++i) {
            loader_load_instance(instance);
        }
        auto end = std::chrono::high_resolution_clock::now();
        bestInstanceDuration = std::min(bestInstanceDuration, std::chrono::duration_cast<std::chrono::microseconds>(end - start));
    }

    // Benchmark loading device functions
    std::chrono::microseconds bestDeviceDuration = std::chrono::microseconds::max();
    for (int i = 0; i < OUTER_COUNT; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < INNER_COUNT; ++i) {
            loader_load_device(instance, physicalDevice, device);
        }
        auto end = std::chrono::high_resolution_clock::now();
        bestDeviceDuration = std::min(bestDeviceDuration, std::chrono::duration_cast<std::chrono::microseconds>(end - start));
    }

    // Benchmark full teardown and initialization
    std::chrono::microseconds bestFullDuration = std::chrono::microseconds::max();
    for (int i = 0; i < OUTER_COUNT; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < INNER_COUNT_REINIT; ++i) {
            loader_destroy();
            loader_init();
            loader_load_instance(instance);
            loader_load_device(instance, physicalDevice, device);
        }
        auto end = std::chrono::high_resolution_clock::now();
        bestFullDuration = std::min(bestFullDuration, std::chrono::duration_cast<std::chrono::microseconds>(end - start));
    }

    // Cleanup
    vkDestroyDevice(device, nullptr);
    vkDestroyInstance(instance, nullptr);
    loader_destroy();

    // Output results as a Markdown table
    std::cout << "| Task                 | Iterations | Total Time (µs) | Average Time (µs) |\n";
    std::cout << "|----------------------|------------|-----------------|-------------------|\n";
    std::cout << "| Load instance functions | " << INNER_COUNT << " | " << bestInstanceDuration.count() << "          | "
        << (bestInstanceDuration.count() / static_cast<double>(INNER_COUNT)) << "            |\n";
    std::cout << "| Load device functions   | " << INNER_COUNT << " | " << bestDeviceDuration.count() << "          | "
        << (bestDeviceDuration.count() / static_cast<double>(INNER_COUNT)) << "            |\n";
    std::cout << "| Teardown and full init  | " << INNER_COUNT_REINIT << " | " << bestFullDuration.count() << "          | "
        << (bestFullDuration.count() / static_cast<double>(INNER_COUNT_REINIT)) << "            |\n";

    close_libvulkan();

    return 0;
}

/* vim: set ts=4 sts=4 sw=4 et: */
