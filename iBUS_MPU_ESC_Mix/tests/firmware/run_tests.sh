#!/usr/bin/env bash
set -euo pipefail
export PATH="/ucrt64/bin:/usr/bin:/bin:$PATH"
fw_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
fw_root="$(cd -- "$fw_dir/../.." && pwd)"
fw_build="$fw_dir/build"
fw_run="$fw_build/runs/$(date -u +%Y%m%dT%H%M%SZ)-$$"
fw_opt="${FW_OPT:--O0}"
[[ "$fw_opt" == -O0 || "$fw_opt" == -O2 ]] || { printf 'FW_OPT chi nhan -O0 hoac -O2\n' >&2; exit 2; }
mkdir -p "$fw_run"
exec > >(tee "$fw_run/run.log") 2>&1
trap 'fw_code=$?; printf "RUN_EXIT_CODE=%s\n" "$fw_code"; printf "%s\n" "$fw_code" > "$fw_run/exit_code.txt"' EXIT
printf 'RUN_DIR=%s\nUTC=%s\n' "$fw_run" "$(date -u +%FT%TZ)"
printf 'SOURCE_BASELINE=94f1a8028dd9a58ce51ee851348bf735bf80ba6f\n'
printf 'HOST_OPTIMIZATION=%s\n' "$fw_opt"
# Git MSYS2 va Git Windows co the doc global config khac nhau.
# Checkout Windows dung CRLF; chi thong nhat cach Git so sanh, khong sua file.
git -c core.autocrlf=true -C "$fw_root" rev-parse HEAD
git -c core.autocrlf=true -C "$fw_root" status --short
git -c core.autocrlf=true -C "$fw_root" diff --quiet 94f1a8028dd9a58ce51ee851348bf735bf80ba6f -- Core
/ucrt64/bin/gcc --version | head -n 1
/ucrt64/bin/g++ --version | head -n 1
/ucrt64/bin/gcc -dumpmachine
cat /ucrt64/lib/pkgconfig/cpputest.pc
fw_flags=("$fw_opt" -g -Wall -Wextra -ffunction-sections -fdata-sections -DFIRMWARE_HOST_TEST -DPID_HOST_TEST -include "$fw_dir/host_compat.h" -I"$fw_root/Core/Inc" -I"$fw_dir")
printf 'COMMON_FLAGS='; printf '%q ' "${fw_flags[@]}"; printf '\n'
fw_objects=()
for fw_module in pid iBUS mpu6050 motor battery unstuck_i2c main; do
    fw_extra=(); [[ "$fw_module" != main ]] || fw_extra=(-Dmain=firmware_main)
    printf 'COMPILE_PRODUCTION=%s\n' "$fw_module"
    timeout 60s /ucrt64/bin/gcc -std=gnu11 "${fw_flags[@]}" "${fw_extra[@]}" -c "$fw_root/Core/Src/$fw_module.c" -o "$fw_build/$fw_module.o"
    fw_objects+=("$fw_build/$fw_module.o")
done
timeout 60s /ucrt64/bin/gcc -std=gnu11 "${fw_flags[@]}" -c "$fw_dir/host_control.c" -o "$fw_build/host_control.o"
fw_objects+=("$fw_build/host_control.o")
for fw_source in "$fw_root/tests/pid/test_main.cpp" "$fw_root/tests/pid/test_pid.cpp" "$fw_dir/fake_hal.cpp" "$fw_dir"/test_*.cpp; do
    fw_name="$(basename "$fw_source" .cpp)"
    printf 'COMPILE_TEST=%s\n' "$fw_source"
    timeout 60s /ucrt64/bin/g++ -std=c++11 "${fw_flags[@]}" -I/ucrt64/include -c "$fw_source" -o "$fw_build/$fw_name.o"
    fw_objects+=("$fw_build/$fw_name.o")
done
printf 'LINK: g++ objects -lCppUTest -lm -Wl,--gc-sections\n'
timeout 60s /ucrt64/bin/g++ "${fw_objects[@]}" -L/ucrt64/lib -lCppUTest -lm -Wl,--gc-sections -o "$fw_build/test_firmware.exe"
sha256sum "$fw_root"/Core/Src/{pid,iBUS,mpu6050,motor,battery,unstuck_i2c,main}.c "$fw_root"/Core/Inc/*.h "$fw_dir"/*.{h,c,cpp,sh} "$fw_root/tests/pid/test_pid.cpp" "$fw_root/tests/pid/test_main.cpp" "$fw_build/test_firmware.exe" | tee "$fw_run/SHA256SUMS.txt"
printf 'RUN: timeout 120s test_firmware.exe -v %s\n' "$*"
set +e
timeout 120s "$fw_build/test_firmware.exe" -v "$@" | tee "$fw_run/test-output.log"
fw_test_exit=${PIPESTATUS[0]}
set -e
if [[ "$fw_test_exit" -ne 0 ]]; then exit "$fw_test_exit"; fi
# Loc sai ten group khong duoc bao PASS khi khong co test nao thuc chay.
if ! grep -Eq 'OK \([0-9]+ tests, [1-9][0-9]* ran,' "$fw_run/test-output.log"; then
    printf 'NOT RUN: khong co bang chung test thuc chay thanh cong.\n' >&2
    exit 2
fi
