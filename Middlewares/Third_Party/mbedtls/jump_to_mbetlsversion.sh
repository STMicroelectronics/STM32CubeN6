#!/bin/bash

set -euo pipefail

if [ "$#" -ne 5 ]
then
  echo "Usage:"
  echo "  $0 <source_repo> <start_tag> <end_tag> <target_repo> <target_branch>"
  echo
  echo "Example:"
  echo "  $0 /repos/projA v1.0.0 v1.1.0 /repos/projB release/integration_branch"
  exit 1
fi

SOURCE_REPO="$1"
START_TAG="$2"
END_TAG="$3"
TARGET_REPO="$4"
TARGET_BRANCH="$5"

mkdir -p /c/Temp/mbed_tls_v366/tmp_dir
PATCH_DIR="/c/Temp/mbed_tls_v366/tmp_dir"

mkdir -p /c/Temp/mbed_tls_v366/abort_dir
DIR_ABORT="/c/Temp/mbed_tls_v366/abort_dir"

cleanup() 
{
    if [[ $? -eq 0 ]]
    then
        echo "All good exiting..."
        rm -rf "$PATCH_DIR"
    else
        echo "Error occured, Directory of patches is: $PATCH_DIR"
    fi
}

trap cleanup EXIT

echo "Source repo     : $SOURCE_REPO"
echo "Tag range       : $START_TAG..$END_TAG"
echo "Target repo     : $TARGET_REPO"
echo "Target branch   : $TARGET_BRANCH"
echo "Temporary patch : $PATCH_DIR"
echo

# Validate source repo
if [ ! -d "$SOURCE_REPO/.git" ]; then
  echo "Error: source repo is not a git repository: $SOURCE_REPO"
  exit 1
fi

# Validate target repo
if [ ! -d "$TARGET_REPO/.git" ]; then
  echo "Error: target repo is not a git repository: $TARGET_REPO"
  exit 1
fi

# Validate tags in source repo
git -C "$SOURCE_REPO" rev-parse "$START_TAG" >/dev/null 2>&1 || 
{
  echo "Error: start tag not found: $START_TAG"
  exit 1
}

git -C "$SOURCE_REPO" rev-parse "$END_TAG" >/dev/null 2>&1 ||
{
  echo "Error: end tag not found: $END_TAG"
  exit 1
}

echo "Generating patches from source repository..."
#git -C "$SOURCE_REPO" format-patch -o "$PATCH_DIR" "$START_TAG..$END_TAG"

PATCH_COUNT="$(find "$PATCH_DIR" -maxdepth 1 -name '*.patch' | wc -l | tr -d ' ')"
if [ "$PATCH_COUNT" = "0" ]; then
  echo "No commits found in range $START_TAG..$END_TAG"
  exit 0
fi

echo "Generated $PATCH_COUNT patch(es)"
echo

echo "Checking out target branch..."
#git -C "$TARGET_REPO" checkout "$TARGET_BRANCH"

echo "Applying patches to target repository..."
for patch in "$PATCH_DIR"/*.patch; do
  echo "Applying: $(basename "$patch")"
  if ! git -C "$TARGET_REPO" am "$patch"; then
    echo
    echo "Conflict or error while applying: $(basename "$patch")"
    echo "Resolve manually in target repo:"
    echo "  cd \"$TARGET_REPO\""
    echo "  git status"
    echo "  git am --continue    # after resolving"
    echo "or abort with:"
    echo "  git am --abort"
    echo "Copy patch to abort dir..."
    cp "$patch" "$DIR_ABORT"
    git am --abort
    #exit 1
  fi
done

echo
echo "All patches applied successfully to $TARGET_REPO on branch $TARGET_BRANCH"
