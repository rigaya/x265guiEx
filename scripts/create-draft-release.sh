#!/usr/bin/env bash
set -euo pipefail

assets_dir="${1:-}"

if [[ -z "${GH_TOKEN:-}" ]]; then
    echo "ERROR: RELEASE_PAT is not configured. Add it as a repository Actions secret." >&2
    exit 1
fi

for name in GITHUB_REPOSITORY TAG RELEASE_SHA; do
    if [[ -z "${!name:-}" ]]; then
        echo "ERROR: required environment variable $name is not set." >&2
        exit 1
    fi
done

if [[ -z "$assets_dir" || ! -d "$assets_dir" ]]; then
    echo "ERROR: release assets directory not found: ${assets_dir:-<empty>}" >&2
    exit 1
fi

mapfile -d '' assets < <(find "$assets_dir" -maxdepth 1 -type f -print0)
if (( ${#assets[@]} == 0 )); then
    echo "ERROR: no release assets found in $assets_dir." >&2
    exit 1
fi

error_file="$(mktemp)"
trap 'rm -f "$error_file"' EXIT
api_response=""

# Returns 0 when the resource exists, 1 for HTTP 404, and 2 for any other error.
api_get() {
    local endpoint="$1"
    local jq_filter="$2"

    : > "$error_file"
    if api_response="$(gh api "$endpoint" --jq "$jq_filter" 2>"$error_file")"; then
        return 0
    fi
    if grep -q 'HTTP 404' "$error_file"; then
        return 1
    fi

    cat "$error_file" >&2
    return 2
}

resolve_tag_commit() {
    local object_type="$1"
    local object_sha="$2"
    local tag_object

    while [[ "$object_type" == "tag" ]]; do
        tag_object="$(gh api \
            "repos/$GITHUB_REPOSITORY/git/tags/$object_sha" \
            --jq '.object.type + " " + .object.sha')"
        read -r object_type object_sha <<< "$tag_object"
    done

    if [[ "$object_type" != "commit" ]]; then
        echo "ERROR: tag $TAG resolves to unsupported object type '$object_type'." >&2
        exit 1
    fi

    printf '%s\n' "$object_sha"
}

tag_info=""
tag_state=0
if api_get \
    "repos/$GITHUB_REPOSITORY/git/ref/tags/$TAG" \
    '.object.type + " " + .object.sha'; then
    tag_info="$api_response"
    tag_state=1
else
    status=$?
    if (( status != 1 )); then
        exit "$status"
    fi
fi

if (( tag_state == 0 )); then
    echo "Creating tag $TAG at $RELEASE_SHA"
    if ! gh api \
        --method POST \
        "repos/$GITHUB_REPOSITORY/git/refs" \
        -f "ref=refs/tags/$TAG" \
        -f "sha=$RELEASE_SHA" >/dev/null; then
        echo "ERROR: failed to create tag $TAG." >&2
        exit 1
    fi
    tag_info="commit $RELEASE_SHA"
else
    echo "Reusing existing tag $TAG after verifying its target."
fi

read -r tag_type tag_object_sha <<< "$tag_info"
tag_commit="$(resolve_tag_commit "$tag_type" "$tag_object_sha")"
if [[ "$tag_commit" != "$RELEASE_SHA" ]]; then
    echo "ERROR: tag $TAG points to $tag_commit, expected $RELEASE_SHA." >&2
    echo "The workflow will not move or delete an existing tag." >&2
    exit 1
fi

release_url=""
release_state=0
if api_get \
    "repos/$GITHUB_REPOSITORY/releases/tags/$TAG" \
    '[.draft, .html_url] | @tsv'; then
    release_state=1
else
    status=$?
    if (( status != 1 )); then
        exit "$status"
    fi
fi

if (( release_state == 1 )); then
    IFS=$'\t' read -r is_draft release_url <<< "$api_response"
    if [[ "$is_draft" != "true" ]]; then
        echo "ERROR: release $TAG is already published. It will not be modified." >&2
        exit 1
    fi
    echo "Reusing existing draft release $TAG."
else
    echo "Creating draft release $TAG"
    gh release create "$TAG" \
        --repo "$GITHUB_REPOSITORY" \
        --target "$RELEASE_SHA" \
        --draft \
        --title "$TAG" \
        --notes ""
    release_url="$(gh release view "$TAG" --repo "$GITHUB_REPOSITORY" --json url --jq '.url')"
fi

gh release upload "$TAG" "${assets[@]}" \
    --repo "$GITHUB_REPOSITORY" \
    --clobber

echo "Draft release: $release_url"
if [[ -n "${GITHUB_OUTPUT:-}" ]]; then
    echo "release_url=$release_url" >> "$GITHUB_OUTPUT"
fi
