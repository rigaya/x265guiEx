#!/usr/bin/env bash
set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
version_header="$repo_root/x265guiEx/auo_version.h"

if [[ ! -f "$version_header" ]]; then
    echo "ERROR: version header not found: $version_header" >&2
    exit 1
fi

version="$(
    awk '
        $1 == "#define" && $2 == "AUO_VERSION_STR" {
            value = $3
            sub(/\r$/, "", value)
            gsub(/^"|"$/, "", value)
            print value
            found++
        }
        END {
            if (found != 1) {
                exit 1
            }
        }
    ' "$version_header"
)" || {
    echo "ERROR: AUO_VERSION_STR must be defined exactly once in $version_header" >&2
    exit 1
}

if [[ ! "$version" =~ ^[0-9]+\.[0-9]+$ ]]; then
    echo "ERROR: invalid AUO_VERSION_STR '$version' (expected MAJOR.MINOR)" >&2
    exit 1
fi

printf '%s\n' "$version"
