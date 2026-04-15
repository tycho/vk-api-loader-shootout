MAKEFLAGS += --no-builtin-rules --no-builtin-variables

CC = clang
CXX = clang++
LINK = $(CXX)
STRIP = strip
SHELL := bash

uname_S := $(shell uname -s)
uname_M := $(shell uname -m)
uname_R := $(shell uname -r)
CPU_BRAND := $(shell scripts/get_cpu_brand.sh)

# Common flags
DEBUGFLAGS := -g0
LDFLAGS := -s

# Enable symbols
ifdef DEBUG
ifeq ($(uname_S),Darwin)
STRIP := dsymutil
else
STRIP := :
endif
DEBUGFLAGS := -g3
LDFLAGS :=
endif

ifeq ($(uname_M),arm64)
ARCHFLAGS := -mcpu=native
else
ARCHFLAGS := -march=x86-64-v2 -mtune=znver3
endif

OPTFLAGS := -O2 -fno-unroll-loops $(ARCHFLAGS) $(DEBUGFLAGS)

# For C language only
CFLAGS   := -std=c17

# For C++ language only
CXXFLAGS := -std=c++20

#ifneq (,$(findstring clang,$(CXX)))
#CXXFLAGS += -stdlib=libc++
#endif

DEPFLAGS = -MMD -MP -MF $(@:.o=.d)

VOLK_VERSION := $(shell cd extern/volk && git describe --tags | sed 's/^vulkan-sdk-//g;s/^v//g')
XXHASH_VERSION := $(shell cd extern/xxHash && git describe --tags | sed 's/^v//')
VK_HEADERS_VERSION := $(shell cd extern/Vulkan-Headers && git describe --tags | sed 's/^v//')
GLAD_TYCHO_VERSION := $(shell cd extern/glad-tycho && git describe --tags | sed 's/^v//')
GLOAM_VERSION := $(shell cd extern/gloam && git describe --tags | sed 's/^v//')
GLAD_DAV1DDE_VERSION := $(shell cd extern/glad-dav1dde && git describe --tags | sed 's/^v//')

# Per-platform Vulkan loader library names. On Windows we copy the DLL (MinGW
# symlinks silently degrade to copies anyway); elsewhere we symlink so that an
# updated prefix/ build is picked up automatically.
ifneq (,$(filter MINGW% MSYS% CYGWIN%,$(uname_S)))
VK_HOST_OS      := windows
VK_LOADER_SRC   := prefix/bin/vulkan-1.dll
VK_LOADER_LINKS := bin/vulkan-1.dll bin/vulkan-1-999-0-0-0.dll
VK_TOOLS_BIN    := bin/vulkaninfo.exe
VK_TOOLS_SRC    := prefix/bin/vulkaninfo.exe
else ifeq ($(uname_S),Darwin)
VK_HOST_OS      := macos
VK_LOADER_SRC   := prefix/lib/libvulkan.1.dylib
VK_LOADER_LINKS := bin/libvulkan.dylib bin/libvulkan.1.dylib
VK_TOOLS_BIN    := bin/vulkaninfo
VK_TOOLS_SRC    := prefix/bin/vulkaninfo
else
VK_HOST_OS      := linux
VK_LOADER_SRC   := prefix/lib/libvulkan.so.1
VK_LOADER_LINKS := bin/libvulkan.so bin/libvulkan.so.1
VK_TOOLS_BIN    := bin/vulkaninfo
VK_TOOLS_SRC    := prefix/bin/vulkaninfo
endif

CC_VERSION := $(shell $(CC) -dumpversion)
CXX_VERSION := $(shell $(CXX) -dumpversion)

# Report-generation configuration. HOST names a subdirectory under REPORTS_DIR
# and defaults to a short, OS-derived id matching the display names the
# renderer knows about (linux / macos / mingw). Override on the command line
# when you want to distinguish multiple machines of the same OS, e.g.
# `make report HOST=linux-ryzen-9950x`.
ifeq ($(VK_HOST_OS),windows)
HOST ?= mingw
else
HOST ?= $(VK_HOST_OS)
endif

REPORTS_DIR     := reports
REPORT_HOST_DIR := $(REPORTS_DIR)/$(HOST)

# Python interpreter used by the report scripts. Override with e.g.
# `PYTHON=python3.12` or `PYTHON=python` if python3 isn't on PATH.
PYTHON ?= python3

XXHASH_DEP := extern/xxHash/xxhash.h
XXHASH_INC := -Iextern/xxHash

BINS :=

all: run

clean:
	rm -f .cflags
	rm -rf bin/test-*
	rm -f obj/loader-*.o
	rm -f obj/main-*.o
	rm -f obj/*.d

distclean: clean
	(cd extern/gloam; cargo clean)
	rm -rf generated/*
	rm -rf venv/glad-*

run: export MVK_CONFIG_LOG_LEVEL=1
run: export LD_LIBRARY_PATH=$(shell cd bin && pwd -P)
run: export DYLD_LIBRARY_PATH=$(shell cd bin && pwd -P)
run: build
	@echo
	@echo "| Project        | Version (git describe)      |"
	@echo "|----------------|-----------------------------|"
	@echo "| GLAD (dav1dde) | \`$(GLAD_DAV1DDE_VERSION)\` |"
	@echo "| GLAD (tycho)   | \`$(GLAD_TYCHO_VERSION)\`   |"
	@echo "| gloam          | \`$(GLOAM_VERSION)\`        |"
	@echo "| Volk           | \`$(VOLK_VERSION)\`         |"
	@echo "| xxHash         | \`$(XXHASH_VERSION)\`       |"
	@echo "| Vulkan-Headers | \`$(VK_HEADERS_VERSION)\`   |"
	@echo
	@echo "| Tool         | Name             | Version             |"
	@echo "|--------------|------------------|---------------------|"
	@echo "| Kernel       | \`$(uname_S)\`   | \`$(uname_R)\`      |"
	@echo "| Architecture | \`$(uname_M)\`   | \`$(CPU_BRAND)\`    |"
	@echo "| CC           | \`$(CC)\`        | \`$(CC_VERSION)\`   |"
	@echo "| CXX          | \`$(CXX)\`       | \`$(CXX_VERSION)\`  |"
	@echo
	@echo "| Variable     | Value         |"
	@echo "|--------------|---------------|"
	@echo "| \`OPTFLAGS\`   | \`$(OPTFLAGS)\` |"
	@echo "| \`CFLAGS\`     | \`$(CFLAGS)\` |"
	@echo "| \`CXXFLAGS\`   | \`$(CXXFLAGS)\` |"
	@echo
	@echo "[GLAD (dav1dde)](https://github.com/Dav1dde/glad)"
	@(cd bin; ./test-glad-dav1dde) && sleep 1
	@echo
	@echo "[GLAD (tycho)](https://github.com/tycho/glad)"
	@(cd bin; ./test-glad-tycho) && sleep 1
	@echo
	@echo "[Gloam (discover API)](https://github.com/tycho/gloam)"
	@(cd bin; ./test-gloam-discover) && sleep 1
	@echo
	@echo "[Gloam (enabled-list API)](https://github.com/tycho/gloam)"
	@(cd bin; ./test-gloam-enabled) && sleep 1
	@echo
	@echo "[Volk](https://github.com/zeux/volk)"
	@(cd bin; ./test-volk)
	@echo
	@echo Loader object sizes in bytes:
	@echo "\`\`\`"
	@du -sb obj/loader-*.o | column -t -R 1
	@echo "\`\`\`"
	@echo
	@echo Test program sizes in bytes:
	@echo "\`\`\`"
	@du -sb bin/test-* | column -t -R 1
	@echo "\`\`\`"
	@echo
	@echo Section sizes:
	@echo "\`\`\`"
	@size bin/test-* | column -t -R 1
	@echo "\`\`\`"
	@echo
	@echo vulkaninfo
	@echo "\`\`\`"
	@# Prefer the locally-built vulkaninfo (inside bin/) over whatever is on
	@# PATH. On Windows this matters because the system-global vulkaninfo.exe
	@# lives next to the system vulkan-1.dll in System32 and will pick that
	@# DLL up regardless of PATH/cwd, bypassing our patched loader.
	@(cd bin && \
		if [[ -x ./vulkaninfo.exe ]]; then ./vulkaninfo.exe --summary; \
		elif [[ -x ./vulkaninfo ]]; then ./vulkaninfo --summary; \
		else vulkaninfo --summary; fi) | grep -A9999 '^Devices:$$'
	@echo "\`\`\`"

.PHONY: all clean distclean run build
.PHONY: vulkan-headers libvulkan vulkan-tools link-vulkan unlink-vulkan

# $(1) = target name (e.g. volk, glad-tycho, gloam)
# $(2) = loader source file
# $(3) = loader include flags
# $(4) = C++ define (e.g. USE_VOLK)
# $(5) = main.cpp include flags
# $(6) = extra prerequisites (e.g. extern/xxHash/xxhash.h)
define build_test
BINS += bin/test-$(1)

obj/loader-$(1).o: $(2) $(6) .cflags
	$$(CC) -c -o $$@ $$(OPTFLAGS) $$(CFLAGS) $$(DEPFLAGS) $(3) $$<

obj/main-$(1).o: src/main.cpp $(2) $(6) .cflags
	$$(CXX) -c -o $$@ $$(OPTFLAGS) $$(CXXFLAGS) $$(DEPFLAGS) -D$(4) '-DLOADER_ID="$(1)"' $(5) $$<

bin/test-$(1): obj/loader-$(1).o obj/main-$(1).o
	$$(LINK) -o $$@ $$(OPTFLAGS) $$(LDFLAGS) $$^
	[[ -f $$@.exe ]] && $$(STRIP) $$@.exe || $$(STRIP) $$@
endef

$(eval $(call build_test,volk,\
  extern/volk/volk.c,\
  -Iextern/volk -Iextern/Vulkan-Headers/include,\
  USE_VOLK,\
  -Iextern/volk -Iextern/Vulkan-Headers/include,\
))

$(eval $(call build_test,glad-dav1dde,\
  generated/glad-dav1dde/src/vulkan.c,\
  -Igenerated/glad-dav1dde/include,\
  USE_GLAD,\
  -Igenerated/glad-dav1dde/include,\
))

$(eval $(call build_test,glad-tycho,\
  generated/glad-tycho/src/vulkan.c,\
  $(XXHASH_INC) -Igenerated/glad-tycho/include,\
  USE_GLAD,\
  $(XXHASH_INC) -Igenerated/glad-tycho/include,\
  $(XXHASH_DEP)))

$(eval $(call build_test,gloam-discover,\
  generated/gloam/src/vk.c,\
  $(XXHASH_INC) -Igenerated/gloam/include,\
  USE_GLOAM_DISCOVER,\
  -Igenerated/gloam/include,\
  $(XXHASH_DEP)))

$(eval $(call build_test,gloam-enabled,\
  generated/gloam/src/vk.c,\
  $(XXHASH_INC) -Igenerated/gloam/include,\
  USE_GLOAM_ENABLED,\
  -Igenerated/gloam/include,\
  $(XXHASH_DEP)))

vulkan-headers:
	mkdir -p build
	rm -rf build/vulkan-headers
	cmake -S extern/Vulkan-Headers -B build/vulkan-headers -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_INSTALL_PREFIX=$(PWD)/prefix -G Ninja
	cmake --build build/vulkan-headers
	cmake --install build/vulkan-headers

# Loader depends on the headers, so force the sequence via a prerequisite.
vulkan-loader: vulkan-headers
	mkdir -p build
	rm -rf build/vulkan-loader
	cmake -S extern/Vulkan-Loader -B build/vulkan-loader -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_CXX_FLAGS="-I$(PWD)/prefix/include" -DCMAKE_C_FLAGS="-I$(PWD)/prefix/include" -DVULKAN_HEADERS_INSTALL_DIR=$(PWD)/prefix -DCMAKE_INSTALL_PREFIX=$(PWD)/prefix -G Ninja
	cmake --build build/vulkan-loader
	cmake --install build/vulkan-loader

# Vulkan-Tools supplies vulkaninfo. We build only vulkaninfo (no cube / mock
# ICD / tests) and link it against the loader we just installed into prefix/.
#
# Patch: upstream gates the Win32 user32.dll handle loader on _MSC_VER, which
# leaves user32_handles as nullptr under MinGW/clang64 and segfaults inside
# AppCreateWin32Window. patches/vulkan-tools-mingw-user32.patch flips that
# guard to _WIN32. Applied idempotently: we use `apply --reverse --check` to
# detect an already-patched tree and skip re-applying.
vulkan-tools: vulkan-loader
	mkdir -p build
	rm -rf build/vulkan-tools
	@if git -C extern/Vulkan-Tools apply --reverse --check $(PWD)/patches/vulkan-tools-mingw-user32.patch 2>/dev/null; then \
		echo "  patch already applied: vulkan-tools-mingw-user32.patch"; \
	else \
		echo "  applying patch: vulkan-tools-mingw-user32.patch"; \
		git -C extern/Vulkan-Tools apply $(PWD)/patches/vulkan-tools-mingw-user32.patch; \
	fi
	cmake -S extern/Vulkan-Tools -B build/vulkan-tools \
		-DCMAKE_BUILD_TYPE=RelWithDebInfo \
		-DCMAKE_C_FLAGS="-I$(PWD)/prefix/include" \
		-DCMAKE_CXX_FLAGS="-I$(PWD)/prefix/include" \
		-DVULKAN_HEADERS_INSTALL_DIR=$(PWD)/prefix \
		-DVULKAN_LOADER_INSTALL_DIR=$(PWD)/prefix \
		-DCMAKE_PREFIX_PATH=$(PWD)/prefix \
		-DCMAKE_INSTALL_PREFIX=$(PWD)/prefix \
		-DBUILD_CUBE=OFF -DBUILD_ICD=OFF -DBUILD_TESTS=OFF \
		-G Ninja
	cmake --build build/vulkan-tools
	cmake --install build/vulkan-tools

# Copy/symlink the loader (and vulkaninfo, if built) from prefix/ into bin/
# so the test binaries resolve the patched loader rather than the system one.
# On Windows this means vulkan-1.dll sits next to the test executables (and a
# versioned alias is provided to match the canonical system name layout); on
# Linux/macOS the dynamic loader finds libvulkan via LD_LIBRARY_PATH=bin (see
# the `run` target exports).
link-vulkan:
	@mkdir -p bin
	@if [[ ! -f "$(VK_LOADER_SRC)" ]]; then \
		echo "error: $(VK_LOADER_SRC) not found - run 'make libvulkan' first" >&2; \
		exit 1; \
	fi
ifeq ($(VK_HOST_OS),windows)
	cp -f "$(VK_LOADER_SRC)" bin/vulkan-1.dll
	cp -f "$(VK_LOADER_SRC)" bin/vulkan-1-999-0-0-0.dll
else
	ln -sf "$(PWD)/$(VK_LOADER_SRC)" $(word 2,$(VK_LOADER_LINKS))
	ln -sf "$(notdir $(word 2,$(VK_LOADER_LINKS)))" $(word 1,$(VK_LOADER_LINKS))
endif
	@if [[ -f "$(VK_TOOLS_SRC)" ]]; then \
		echo "  installing $(VK_TOOLS_BIN)"; \
		cp -f "$(VK_TOOLS_SRC)" "$(VK_TOOLS_BIN)"; \
	fi

unlink-vulkan:
	rm -f $(VK_LOADER_LINKS) $(VK_TOOLS_BIN)

# ---------------------------------------------------------------------------
# Report generation
#
# `make report` produces a machine-readable snapshot of the benchmark results
# plus host metadata under $(REPORT_HOST_DIR)/. The snapshot is three JSON
# files: metadata.json (host/toolchain/versions/vulkaninfo), unpatched.json
# (benchmarks + sizes with the stock Vulkan loader), and patched.json (same
# with our patched loader). `make render` turns those JSON files into
# Markdown fragments + SVG bar charts under the same directory plus an
# amalgamated reports/$(HOST).md for quick viewing.
#
# Report generation is deliberately NOT a prerequisite of `make run` - the
# existing run target is still the quick "does this work on my box?" path.
# Report targets are invoked manually on each machine, the per-host subdir
# is copied into the shared reports/ tree on a workstation, and render/
# updates the rendered artifacts.

# Turn 'bin/test-foo bin/test-bar' into '--loader=foo --loader=bar' for
# collect-variant.py. Uses $(BINS), which build_test already populates.
LOADER_FLAGS := $(addprefix --loader=,$(patsubst bin/test-%,%,$(BINS)))

# Note on sequencing: report-unpatched must run before report-patched because
# they toggle the loader link state (unlink-vulkan vs link-vulkan) and we
# don't want them to race under parallel make. The top-level report: target
# recursively invokes sub-makes to force serial execution regardless of -j.
report:
	@$(MAKE) report-unpatched
	@$(MAKE) report-patched
	@$(MAKE) report-metadata
	@echo
	@echo "report written to $(REPORT_HOST_DIR)/"
	@echo "  run 'make render' to regenerate the markdown and charts"

report-metadata: | $(REPORT_HOST_DIR)
	$(PYTHON) scripts/collect-metadata.py \
		--host-id='$(HOST)' \
		--output='$(REPORT_HOST_DIR)/metadata.json' \
		--os='$(uname_S)' \
		--arch='$(uname_M)' \
		--kernel-version='$(uname_R)' \
		--cpu-brand='$(CPU_BRAND)' \
		--cc='$(CC)' \
		--cc-version='$(CC_VERSION)' \
		--cxx='$(CXX)' \
		--cxx-version='$(CXX_VERSION)' \
		--optflags='$(OPTFLAGS)' \
		--cflags='$(CFLAGS)' \
		--cxxflags='$(CXXFLAGS)' \
		--version='glad_dav1dde=$(GLAD_DAV1DDE_VERSION)' \
		--version='glad_tycho=$(GLAD_TYCHO_VERSION)' \
		--version='gloam=$(GLOAM_VERSION)' \
		--version='volk=$(VOLK_VERSION)' \
		--version='xxhash=$(XXHASH_VERSION)' \
		--version='vulkan_headers=$(VK_HEADERS_VERSION)' \
		--vulkaninfo='$(VK_TOOLS_BIN)'

# Unpatched variant: unlink the patched loader out of bin/ so the test
# binaries resolve against the system Vulkan loader, build everything if
# needed, then run each test binary in --json mode.
report-unpatched: unlink-vulkan build | $(REPORT_HOST_DIR)
	$(PYTHON) scripts/collect-variant.py \
		--variant=unpatched \
		--bin-dir=bin \
		--obj-dir=obj \
		$(LOADER_FLAGS) \
		--output='$(REPORT_HOST_DIR)/unpatched.json'

# Patched variant: ensure the patched loader and vulkaninfo are built (via
# vulkan-tools, which transitively depends on vulkan-loader and vkheaders),
# link them into bin/, build the test binaries, then run each in --json mode.
report-patched: export MVK_CONFIG_LOG_LEVEL=1
report-patched: export LD_LIBRARY_PATH=$(shell cd bin && pwd -P)
report-patched: export DYLD_LIBRARY_PATH=$(shell cd bin && pwd -P)
report-patched: vulkan-tools link-vulkan build | $(REPORT_HOST_DIR)
	$(PYTHON) scripts/collect-variant.py \
		--variant=patched \
		--bin-dir=bin \
		--obj-dir=obj \
		$(LOADER_FLAGS) \
		--output='$(REPORT_HOST_DIR)/patched.json'

render:
	$(PYTHON) scripts/render-report.py --reports-dir='$(REPORTS_DIR)'

reports-clean:
	rm -rf '$(REPORT_HOST_DIR)'
	rm -f '$(REPORTS_DIR)/$(HOST).md'

$(REPORT_HOST_DIR):
	mkdir -p '$@'

.PHONY: report report-metadata report-unpatched report-patched render reports-clean

build: $(BINS)

gen-glad-dav1dde:
	# This is disabled because upstream Glad chokes on current Vulkan XML files
	#cp extern/glad-tycho/glad/files/* extern/glad-dav1dde/glad/files/
	scripts/gen-glad.sh extern/glad-dav1dde venv/glad-dav1dde generated/glad-dav1dde
generated/glad-dav1dde/src/vulkan.c: scripts/gen-glad.sh
	# This is disabled because upstream Glad chokes on current Vulkan XML files
	#cp extern/glad-tycho/glad/files/* extern/glad-dav1dde/glad/files/
	scripts/gen-glad.sh extern/glad-dav1dde venv/glad-dav1dde generated/glad-dav1dde
gen-glad-tycho:
	scripts/gen-glad.sh extern/glad-tycho venv/glad-tycho generated/glad-tycho
generated/glad-tycho/src/vulkan.c: scripts/gen-glad.sh
	scripts/gen-glad.sh extern/glad-tycho venv/glad-tycho generated/glad-tycho
gen-gloam:
	scripts/gen-gloam.sh extern/gloam generated/gloam
generated/gloam/src/vk.c: scripts/gen-gloam.sh
	scripts/gen-gloam.sh extern/gloam generated/gloam
.PHONY: gen-glad-dav1dde gen-glad-tycho gen-gloam

ifeq (,$(findstring clean,$(MAKECMDGOALS)))

TRACK_CFLAGS = $(subst ','\'',$(CC) $(CXX) $(OPTFLAGS) $(CFLAGS) $(CXXFLAGS) $(LDFLAGS) $(uname_S))

.cflags: .force-cflags
	@FLAGS='$(TRACK_CFLAGS)'; \
	if test x"$$FLAGS" != x"`cat .cflags 2>/dev/null`" ; then \
			echo "    * rebuilding: new build flags"; \
			echo "$$FLAGS" > .cflags; \
	fi

.PHONY: .force-cflags

-include $(wildcard obj/*.d)

endif
