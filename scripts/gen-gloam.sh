#!/bin/bash

set -ex

SRCROOT="$1"
OUTDIR="$2"

# Clean up old virtualenv and outputs
rm -rf "$OUTDIR"

(
	export PATH="$SRCROOT/target/release:$PATH"
	pushd "$SRCROOT"
		cargo build --release
	popd

	gloam --api=vulkan --out-path="${OUTDIR}" --merge c --alias --loader

	# We use a common version from extern/xxHash instead
	rm -f "${OUTDIR}/include/xxhash.h"
)

exit 0
