#!/bin/sh

TARGET_FILE="$1"

# XXX command subst vs. splitting?

TARGET_DIR="$(dirname "${TARGET_FILE}")"
cd "${TARGET_DIR}"

TARGET_FILE="$(basename "${TARGET_FILE}")"

# Iterate down the path through a (possible) chain of symlinks
#
# XXX check for symlink loops by watching for too many iterations
#
while [ -L "${TARGET_FILE}" ]; do
	TARGET_FILE="$(readlink "${TARGET_FILE}")"
	cd "$(dirname "${TARGET_FILE}")"
	TARGET_FILE="$(basename "${TARGET_FILE}")"
done

# Compute the canonicalized name by asking the kernel for the physical path of
# the directory we're now in and appending the target file.
#
PHYS_DIR="$(pwd -P)"
RESULT="${PHYS_DIR}/${TARGET_FILE}"

echo "${RESULT}"
