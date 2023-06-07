#!/bin/bash
#
bits=${1:-2048}
f="${HOME}/.config/eagine/defaults.yaml"
mkdir -p "$(dirname ${f})" &&
grep -qxF "main_key: " "${f}" ||\
echo "main_key: $(dd if=/dev/random status=none bs=2048 count=1 | base64 -w 0)" >> "${f}"

