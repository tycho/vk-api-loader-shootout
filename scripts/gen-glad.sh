#!/bin/bash

set -ex

SRCROOT="$1"
VENV="$2"
OUTDIR="$3"

# Clean up old virtualenv and outputs
rm -rf "$OUTDIR"

if [[ ! -d "$VENV" ]]; then
	virtualenv "$VENV"
fi
(
	source "$VENV/bin/activate"
	pushd "$SRCROOT"
		git submodule update --init
		#env TARGET=glad/files utility/download.sh
		pip install -r requirements.txt
		pip install .
	popd

	# Upstream GLAD does not support --mx-global anymore, try once with and
	# once without that argument
	glad --reproducible --api=vulkan --out-path="${OUTDIR}" --merge c --alias --loader --mx --mx-global || \
		glad --reproducible --api=vulkan --out-path="${OUTDIR}" --merge c --alias --loader

)

exit 0
