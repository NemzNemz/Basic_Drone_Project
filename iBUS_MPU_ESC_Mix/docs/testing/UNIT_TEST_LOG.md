# Nhật ký kiểm thử đơn vị firmware STM32F411

Nhật ký ghi các phiên theo tiến độ trong cùng tệp, không tạo một file Markdown mới cho mỗi ngày. Giữ lịch sử theo nguyên tắc append-only: nếu kết quả hoặc thông tin cần đính chính, bổ sung bản ghi liên kết thay vì xóa FAIL hoặc thay bằng PASS mới.

## UT-SESSION-2026-10-23-001

### Thông tin phiên

| Thuộc tính | Giá trị |
| --- | --- |
| Ngày | `23/10/2026`, múi giờ `Asia/Bangkok` (UTC+07:00); không ghi lại giờ chạy chính xác |
| Repository / dự án | `Basic_Drone/iBUS_MPU_ESC_Mix` |
| Branch | `test/cpputest` |
| HEAD tại phiên | `e7cde55f3c69f2181939d06febcfaf4865ec2960` |
| Baseline mã PID production | `9280bae`; `pid.c` và `pid.h` không thay đổi trong nhánh kiểm thử hoặc working tree |
| Module / test ID | PID / `UT-PID-001` |
| Ca CppUTest | `PID_PitchRoll.POnly_Cascade` |
| Kết quả lượt chạy hoàn tất | `PASS`: 1 test, 1 test thực chạy, 4 assertions, exit code `0` |
| Phiên bản test/harness | Các file mới trong `tests/pid/` chưa commit; SHA-256 được ghi bên dưới để bổ sung cho HEAD |
| Nguồn bằng chứng | Output của các lệnh build/run đã quan sát trong phiên trao đổi; trích lại bên dưới, chưa có raw log riêng được lưu thành artifact |

### Công việc và môi trường

- Rà soát `host_compat.h`, `test_main.cpp`, `test_pid.cpp` đã có; giữ nguyên expected và tolerance của `UT-PID-001`.
- Tạo `tests/pid/run_tests.sh` để compile production C bằng GCC, compile test C++ bằng G++, link `libCppUTest` và chạy executable.
- Thêm `tests/pid/.gitignore` để bỏ qua `/build/`; thêm `.gitattributes` cục bộ để giữ `run_tests.sh` ở LF.
- Không sửa firmware production, không flash MCU và không sử dụng phần cứng drone trong lượt host-test.
- Windows sử dụng MSYS2 Bash tại `C:/msys64/usr/bin/bash.exe`; script gọi rõ `/ucrt64/bin/gcc` và `/ucrt64/bin/g++`, không sử dụng GCC tại `C:/MinGW/bin`.
- Tra cứu khi lập hồ sơ kết thúc phiên: GCC và G++ UCRT64 đều báo `16.2.0`, target GCC là `x86_64-w64-mingw32`; metadata cài đặt `cpputest.pc` ghi CppUTest `4.0`. Đây là kiểm tra môi trường lúc chốt hồ sơ, không phải output version kèm trong lượt build/run ban đầu.
- Compile flags chung: `-Wall -Wextra -O0 -g -DPID_HOST_TEST`, force-include `tests/pid/host_compat.h` và include `Core/Inc`; C dùng `-std=c11`, C++ dùng `-std=c++11`.
- `host_compat.h` cung cấp `<stdint.h>` và định nghĩa trước guard `__MAIN_H` để không kéo STM32 HAL vào host-build. Phía test dùng `extern "C"` khi include production `pid.h`.
- Link bằng G++, các object đứng trước `-L/ucrt64/lib -lCppUTest`. Tất cả object và executable nằm trong `tests/pid/build/`.

### Lịch sử thực thi trong phiên

| Lượt | Kết quả quan sát | Phân loại và xử lý |
| --- | --- | --- |
| Khởi chạy MSYS2 trong sandbox | Bash báo `NtCreateDirectoryObject` với `0xC0000022`, exit code `-1073741502` | `BLOCKED` do môi trường; chưa compile hoặc chạy test; chạy ngoài sandbox sau khi được phép |
| Khởi chạy script trước khi sửa thứ tự PATH | `dirname: command not found`, `cd: null directory`, exit code `1` | Lỗi khởi tạo harness, chưa chạy test; chuyển thiết lập PATH MSYS2 lên trước khi dùng `dirname` |
| Chạy script sau khi sửa PATH | Compile/link thành công; `1 tests, 1 ran, 4 checks`; exit code `0` | `UT-PID-001 PASS`; không có warning/error compiler hoặc linker trong output lượt này |

Các lần khởi chạy không thành công ở trên không bị ghi thành FAIL của thuật toán PID. Kết quả PASS chỉ áp dụng cho lượt test thực chạy sau đó. Lượt chốt hồ sơ không chạy lại build hoặc test; chỉ kiểm tra source, hash, metadata môi trường và trạng thái Git.

### Lệnh và output tái hiện

Từ thư mục `iBUS_MPU_ESC_Mix` trong MSYS2:

```bash
bash tests/pid/run_tests.sh
```

Lượt xác minh thực tế được gọi từ PowerShell tại thư mục dự án bằng `C:/msys64/usr/bin/bash.exe tests/pid/run_tests.sh`, sau khi được phép chạy ngoài sandbox.

Output lượt chạy thành công đã quan sát:

```text
[1/5] Biên dịch production pid.c bằng GCC UCRT64
[2/5] Biên dịch test_main.cpp bằng G++ UCRT64
[3/5] Biên dịch test_pid.cpp bằng G++ UCRT64
[4/5] Liên kết với libCppUTest
[5/5] Chạy bộ kiểm thử PID
.
OK (1 tests, 1 ran, 4 checks, 0 ignored, 0 filtered out, 0 ms)
```

Exit code của lệnh: `0`. Giá trị `0 ms` là output của framework trên host, không phải phép đo WCET hoặc timing STM32.

### Đối chiếu expected và kết quả

Input: RC `1600`, gyro `0 rad/s`, Euler `2°`; hai `PID_t` độc lập khởi tạo bằng 0; Outer `KP=2`, Inner `KP=1`, cả hai `KI=KD=0`.

| Assertion | Expected | Sai số tuyệt đối | Kết quả |
| --- | --- | --- | --- |
| `outer.PID_OUT` | `6.0` | `0.00001` | PASS |
| `inner.PID_OUT` | `6.0` | `0.00001` | PASS |
| `outer.error_sum` | `0.006` | `0.00001` | PASS |
| `inner.error_sum` | `0.012` | `0.00001` | PASS |

Phép tính expected giữ nguyên trong [UNIT_TEST_CASES.md](UNIT_TEST_CASES.md). Output thành công không in giá trị actual riêng; bằng chứng hiện có là 4 assertions đạt tolerance, không phải bản dump xác nhận các float bằng chính xác các số thập phân trên.

### Định danh source và harness đã đối chiếu

Các hash được kiểm tra sau lượt chạy thành công và đối chiếu lại khi chốt phiên; không thay đổi các file này trong lượt lập hồ sơ.

| Tệp, tính từ thư mục dự án | SHA-256 |
| --- | --- |
| `Core/Src/pid.c` | `7255FA93B3185C99FC339753D2CD4F16463D47F01EAF03B3DE92BD41943AD5C7` |
| `Core/Inc/pid.h` | `52AB413A9DA682D240D28003F7A0B2200EF30929A247E6E9093E4B12B558BDDB` |
| `tests/pid/host_compat.h` | `A4B2AD2D167183FEF58FAD8EE869D237373822A819EDF26CA34F277CE1C79DE4` |
| `tests/pid/test_main.cpp` | `5E39418377941676654115A0E9278622C9A021855F8235A8D3DD44B914DBF327` |
| `tests/pid/test_pid.cpp` | `E2425EDB6428D75861BAB64CD166DC628C356F7CD13208B5014B3BE6BF9A4800` |
| `tests/pid/run_tests.sh` | `ED27781A2DD911238865832DBBDDA8100C46E67200A41638DEBA0436B07DCE45` |

### Rà soát Git và IntelliSense

- Cần đưa vào Git khi được phê duyệt: `UNIT_TEST_CASES.md` cập nhật, `UNIT_TEST_LOG.md` và sáu file harness/cấu hình trong `tests/pid/`: `host_compat.h`, `test_main.cpp`, `test_pid.cpp`, `run_tests.sh`, `.gitignore`, `.gitattributes`.
- `tests/pid/build/` không có file đang được Git tracking; `/build/` trong `.gitignore` bỏ qua object/executable. Không force-add artifact build.
- `.vscode/c_cpp_properties.json` là cấu hình IDE cá nhân, đề xuất không đưa vào commit hồ sơ/harness này. File có đường dẫn tuyệt đối `C:/msys64/ucrt64`, không tham gia lệnh build trong script.
- IntelliSense hiện định nghĩa `PID_HOST_TEST` và `__MAIN_H` cho cấu hình workspace, cùng force-include compatibility header. Cấu hình này dành cho host-test, không thể dùng làm bằng chứng cấu hình firmware đúng hoặc coi mọi header STM32 đã được kiểm tra.
- Đường dẫn `${workspaceFolder}/tests/pid/host_compat.h` giả định mở workspace tại `iBUS_MPU_ESC_Mix`; nếu mở tại repository `Basic_Drone` thì cần đối chiếu lại. Chưa xác minh workspace đang mở trong IDE; không sửa cấu hình ở lượt này.
- Chưa cần tạo `UNIT_TEST_SUMMARY.md` cho một ca đã có kết quả đầy đủ trong CASES/LOG. `UNIT_TEST_PLAN.md` giữ nguyên vì phạm vi và phương pháp không thay đổi.

### Giới hạn và công việc tiếp theo

- Chỉ có một ca cascade P-only; chưa kiểm tra `KI/KD` khác 0, biên tích phân, nhiều lần gọi, reset, yaw hoặc các module khác.
- Chưa thử fault injection để quan sát exit code khi compile/link/test FAIL, hoặc thiếu dependency; hành vi dừng sớm hiện được rà soát từ `set -euo pipefail` và `exec` trong script.
- Chưa xác minh nhiều lượt chạy thành công liên tiếp, máy khác, đường dẫn khác hoặc toolchain khác.
- Không có raw log riêng, timestamp chính xác hoặc toàn bộ metadata công cụ thu đồng thời trong lượt build/run. Bản ghi này giữ rõ giới hạn nguồn bằng chứng.
- Không xác nhận timing MCU, interrupt concurrency, hardware MPU/ESC hoặc phản ứng motor; không dùng PASS này để đóng finding pre-flight ngoài phạm vi logic đã test.
- Công việc tiếp theo được đề xuất: review hồ sơ và harness, quyết định phạm vi commit không gồm IDE cá nhân/build artifacts; sau đó mới giao nhiệm vụ bổ sung contract/test hoặc bằng chứng thực thi. Không tự thực hiện bước tiếp theo trong lượt chốt phiên.
