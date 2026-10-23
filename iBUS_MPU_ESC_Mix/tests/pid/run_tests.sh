#!/usr/bin/env bash
set -euo pipefail

# Ưu tiên toolchain, DLL và tiện ích MSYS2 trước khi xử lý đường dẫn.
export PATH="/ucrt64/bin:/usr/bin:/bin:$PATH"

# Xác định đường dẫn độc lập với thư mục đang chạy lệnh.
pid_script_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
pid_project_dir="$(cd -- "$pid_script_dir/../.." && pwd)"
pid_build_dir="$pid_script_dir/build"
pid_cc="/ucrt64/bin/gcc"
pid_cxx="/ucrt64/bin/g++"

for pid_tool in "$pid_cc" "$pid_cxx"; do
    if [[ ! -x "$pid_tool" ]]; then
        printf 'Không tìm thấy compiler UCRT64: %s\n' "$pid_tool" >&2
        exit 1
    fi
done

for pid_input in \
    "$pid_project_dir/Core/Src/pid.c" \
    "$pid_project_dir/Core/Inc/pid.h" \
    "$pid_script_dir/host_compat.h" \
    "$pid_script_dir/test_main.cpp" \
    "$pid_script_dir/test_pid.cpp" \
    "/ucrt64/include/CppUTest/TestHarness.h" \
    "/ucrt64/include/CppUTest/CommandLineTestRunner.h" \
    "/ucrt64/lib/libCppUTest.a"; do
    if [[ ! -f "$pid_input" ]]; then
        printf 'Thiếu tệp cần thiết: %s\n' "$pid_input" >&2
        exit 1
    fi
done

mkdir -p -- "$pid_build_dir"

# Force-include compatibility header cho cả mã C và test C++.
pid_common_flags=(
    -Wall -Wextra -O0 -g
    -DPID_HOST_TEST
    -include "$pid_script_dir/host_compat.h"
    -I"$pid_project_dir/Core/Inc"
)

printf '[1/5] Biên dịch production pid.c bằng GCC UCRT64\n'
"$pid_cc" -std=c11 "${pid_common_flags[@]}" \
    -c "$pid_project_dir/Core/Src/pid.c" \
    -o "$pid_build_dir/pid.o"

printf '[2/5] Biên dịch test_main.cpp bằng G++ UCRT64\n'
"$pid_cxx" -std=c++11 "${pid_common_flags[@]}" -I/ucrt64/include \
    -c "$pid_script_dir/test_main.cpp" \
    -o "$pid_build_dir/test_main.o"

printf '[3/5] Biên dịch test_pid.cpp bằng G++ UCRT64\n'
"$pid_cxx" -std=c++11 "${pid_common_flags[@]}" -I/ucrt64/include \
    -c "$pid_script_dir/test_pid.cpp" \
    -o "$pid_build_dir/test_pid.o"

printf '[4/5] Liên kết với libCppUTest\n'
"$pid_cxx" \
    "$pid_build_dir/pid.o" \
    "$pid_build_dir/test_main.o" \
    "$pid_build_dir/test_pid.o" \
    -L/ucrt64/lib -lCppUTest \
    -o "$pid_build_dir/test_pid.exe"

# Trả nguyên exit code của executable; không chạy bản cũ khi build lỗi.
printf '[5/5] Chạy bộ kiểm thử PID\n'
exec "$pid_build_dir/test_pid.exe" "$@"
