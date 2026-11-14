#!/usr/bin/env bash
# Build one overlay-app blob (.app) for Frequency Manager
set -euo pipefail

APP="$(basename "$PWD")"            # memo
APP_NAME="Memo F4IAE"                # App name in menu
APP_VER="1.0"
APP_API_MIN=3
APP_VMA=${APP_VMA:-0x20000280}      # pinned overlay VMA
OUT="memo"                   # blob basename

CC=/opt/toolchain/bin/arm-none-eabi-gcc
OBJCOPY=/opt/toolchain/bin/arm-none-eabi-objcopy
command -v arm-none-eabi-gcc >/dev/null 2>&1 && { CC=arm-none-eabi-gcc; OBJCOPY=arm-none-eabi-objcopy; }

# GCC 13 supports -Oz: prioritize overlay size over instruction count.
CFLAGS="-mcpu=cortex-m0plus -mthumb -Oz -fno-jump-tables -std=gnu11 -ffreestanding -fno-builtin -fno-common \
  -fomit-frame-pointer -ffunction-sections -fdata-sections -Wall -Wextra"
LDFLAGS="-nostdlib -nostartfiles -T app.ld -Wl,--defsym,APP_VMA=${APP_VMA} \
  -Wl,--gc-sections -Wl,-Map=${APP}.map -Wl,--build-id=none -Wl,--no-warn-rwx-segments"

rm -f ./*.app ./*.elf ./*.bin        # drop stale artifacts

step() { printf '\r  🔨 %-13s [%d/4] %-8s' "$APP_NAME" "$1" "$2"; }
trap 'printf "\r  ❌ %-13s build failed            \n" "$APP_NAME"' ERR

step 1 assets  ; APP_VER="$APP_VER" python3 ./gen_assets.py "${APP}_assets.bin" "${APP}_assets.h"
# A failed link (4 KiB overflow) leaves no ELF: keep an object to disassemble.
step 2 compile ; "$CC" $CFLAGS $LDFLAGS "memo_app.c" -lgcc -o "${APP}.elf" \
                   || { "$CC" $CFLAGS -c "memo_app.c" -o "${APP}.o"; false; }
step 3 objcopy ; "$OBJCOPY" -O binary "${APP}.elf" "${APP}.bin"
step 4 pack    ; python3 ../pack_app.py "${APP}.bin" "${OUT}.app" \
                   --name "$APP_NAME" --ver "$APP_VER" --api-min "$APP_API_MIN" --vma "${APP_VMA}" \
                   --shortcut none --assets "${APP}_assets.bin" >/dev/null
trap - ERR

BYTES=$(wc -c < "${APP}.bin")
if [ "$BYTES" -gt 4096 ]; then
  printf '\r  🚨 %-13s OVERFLOWS 4 KiB (%d B)     \n' "$APP_NAME" "$BYTES"; exit 1
fi
printf '\r  ✅ %-13s %4d B  (%d%% of 4 KiB)  ->  %s.app   \n' \
       "$APP_NAME" "$BYTES" "$(( BYTES * 100 / 4096 ))" "$OUT"
