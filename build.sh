#!/bin/bash
set -e

echo "Starting cross-compilation sequentially for all platforms..."

docker compose run --rm linux-x64
docker compose run --rm linux-x86
docker compose run --rm linux-arm64
docker compose run --rm linux-armv6
docker compose run --rm windows-x64
docker compose run --rm windows-x86
docker compose run --rm windows-arm64
docker compose run --rm macos-x64
docker compose run --rm macos-arm64

echo "Build complete. Binaries are located in the build/ directory:"
find build -type f -name "WakeOnLAN*"
