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
ifeq ($(uname_M),arm64)
OPTFLAGS = -O2 -mcpu=native $(DEBUGFLAGS)
else
OPTFLAGS = -O2 -march=x86-64-v2 -mtune=znver3 $(DEBUGFLAGS)
endif

# Enable symbols
ifdef DEBUG
ifeq ($(uname_S),Darwin)
STRIP := dsymutil
else
STRIP := :
endif
DEBUGFLAGS := -g3
endif

# For C language only
CFLAGS   := -std=c17

# For C++ language only
CXXFLAGS := -std=c++20

#ifneq (,$(findstring clang,$(CXX)))
#CXXFLAGS += -stdlib=libc++
#endif

VOLK_VERSION := $(shell cd extern/volk && git describe --tags | sed 's/^v//')
XXHASH_VERSION := $(shell cd extern/glad-tycho/third_party/xxHash && git describe --tags | sed 's/^v//')
VK_HEADERS_VERSION := $(shell cd extern/Vulkan-Headers && git describe --tags | sed 's/^v//')
GLAD_TYCHO_VERSION := $(shell cd extern/glad-tycho && git describe --tags | sed 's/^v//')
GLAD_DAV1DDE_VERSION := $(shell cd extern/glad-dav1dde && git describe --tags | sed 's/^v//')

CC_VERSION := $(shell $(CC) -dumpversion)
CXX_VERSION := $(shell $(CXX) -dumpversion)

all: run

clean:
	rm -f .cflags
	rm -rf bin/test-*
	rm -f obj/loader-*.o
	rm -f obj/main-*.o

distclean: clean
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
	@echo "| \`OPTFLAGS\` | \`$(OPTFLAGS)\` |"
	@echo "| \`CFLAGS\`   | \`$(CFLAGS)\` |"
	@echo "| \`CXXFLAGS\` | \`$(CXXFLAGS)\` |"
	@echo
	@echo "[GLAD (dav1dde)](https://github.com/Dav1dde/glad)"
	@(cd bin; ./test-glad-dav1dde)
	@echo
	@echo "[GLAD (tycho)](https://github.com/tycho/glad)"
	@(cd bin; ./test-glad-tycho)
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
	@size bin/test-*
	@echo "\`\`\`"

build: bin/test-volk bin/test-glad-dav1dde bin/test-glad-tycho

.PHONY: all clean distclean run build


bin/test-volk: src/main.cpp extern/volk/volk.c .cflags
	$(CC) -c -o obj/loader-volk.o $(OPTFLAGS) $(CFLAGS) -Iextern/volk -Iextern/Vulkan-Headers/include extern/volk/volk.c
	$(CXX) -c -o obj/main-volk.o $(OPTFLAGS) $(CXXFLAGS) -DUSE_VOLK -Iextern/volk -Iextern/Vulkan-Headers/include src/main.cpp
	$(LINK) -o $@ $(OPTFLAGS) obj/loader-volk.o obj/main-volk.o
	[[ -f $@.exe ]] && $(STRIP) $@.exe || $(STRIP) $@
bin/test-glad-dav1dde: src/main.cpp generated/glad-dav1dde/src/vulkan.c .cflags
	$(CC) -c -o obj/loader-glad-dav1dde.o $(OPTFLAGS) $(CFLAGS) -Igenerated/glad-dav1dde/include generated/glad-dav1dde/src/vulkan.c
	$(CXX) -c -o obj/main-glad-dav1dde.o $(OPTFLAGS) $(CXXFLAGS) -DUSE_GLAD -Igenerated/glad-dav1dde/include src/main.cpp
	$(LINK) -o $@ $(OPTFLAGS) obj/loader-glad-dav1dde.o obj/main-glad-dav1dde.o
	[[ -f $@.exe ]] && $(STRIP) $@.exe || $(STRIP) $@
bin/test-glad-tycho: src/main.cpp generated/glad-tycho/src/vulkan.c extern/glad-tycho/third_party/xxHash/xxhash.h .cflags
	$(CC) -c -o obj/loader-glad-tycho.o $(OPTFLAGS) $(CFLAGS) -Igenerated/glad-tycho/include -Iextern/glad-tycho/third_party/xxHash generated/glad-tycho/src/vulkan.c
	$(CXX) -c -o obj/main-glad-tycho.o $(OPTFLAGS) $(CXXFLAGS) -DUSE_GLAD -Igenerated/glad-tycho/include -Iextern/glad-tycho/third_party/xxHash src/main.cpp
	$(LINK) -o $@ $(OPTFLAGS) obj/loader-glad-tycho.o obj/main-glad-tycho.o
	[[ -f $@.exe ]] && $(STRIP) $@.exe || $(STRIP) $@

gen-glad-dav1dde:
	scripts/gen-glad.sh extern/glad-dav1dde venv/glad-dav1dde generated/glad-dav1dde
generated/glad-dav1dde/src/vulkan.c:
	scripts/gen-glad.sh extern/glad-dav1dde venv/glad-dav1dde generated/glad-dav1dde
gen-glad-tycho:
	scripts/gen-glad.sh extern/glad-tycho venv/glad-tycho generated/glad-tycho
generated/glad-tycho/src/vulkan.c:
	scripts/gen-glad.sh extern/glad-tycho venv/glad-tycho generated/glad-tycho
.PHONY: gen-glad-dav1dde gen-glad-tycho

ifeq (,$(findstring clean,$(MAKECMDGOALS)))

TRACK_CFLAGS = $(subst ','\'',$(CC) $(CXX) $(OPTFLAGS) $(CFLAGS) $(CXXFLAGS) $(uname_S))

.cflags: .force-cflags
	@FLAGS='$(TRACK_CFLAGS)'; \
	if test x"$$FLAGS" != x"`cat .cflags 2>/dev/null`" ; then \
			echo "    * rebuilding: new build flags"; \
			echo "$$FLAGS" > .cflags; \
	fi

.PHONY: .force-cflags

endif
