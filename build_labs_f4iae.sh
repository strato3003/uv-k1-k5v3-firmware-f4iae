#!/bin/bash

# Build script for Labs_f4iae firmware
# This firmware includes the frequency manager system

# Set version defines
AUTHOR_STRING_1="Labs" \
AUTHOR_STRING_2="F4IAE" \
VERSION_STRING_1="v6.3.0" \
VERSION_STRING_2="v6.3.0" \
DISPLAY_VERSION_STRING_2="v6.3.0 Labs" \
EDITION_STRING="Labs_f4iae" \
VERSION_STRING="v6.3.0" \

# Build firmware
./compile-firmware.sh \
    -D ENABLE_FEAT_F4HWN=ON \
    -D ENABLE_UART=ON \
    -D AUTHOR_STRING_1="$AUTHOR_STRING_1" \
    -D AUTHOR_STRING_2="$AUTHOR_STRING_2" \
    -D VERSION_STRING_1="$VERSION_STRING_1" \
    -D VERSION_STRING_2="$VERSION_STRING_2" \
    -D DISPLAY_VERSION_STRING_2="$DISPLAY_VERSION_STRING_2" \
    -D EDITION_STRING="$EDITION_STRING" \
    -D VERSION_STRING="$VERSION_STRING" \
    "$@"

echo ""
echo "=========================================="
echo "Labs_f4iae firmware built successfully!"
echo "Version: $EDITION_STRING $VERSION_STRING_2"
echo "=========================================="
