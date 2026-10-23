# Hồ sơ các ca kiểm thử đơn vị

## Phạm vi và trạng thái hồ sơ

- Dự án: `iBUS_MPU_ESC_Mix` trong repository `Basic_Drone`.
- Nhánh đối chiếu: `test/cpputest`; commit nguồn: `9280bae`.
- Kế hoạch chung: [UNIT_TEST_PLAN.md](UNIT_TEST_PLAN.md).
- Hồ sơ gồm thiết kế và kết quả kiểm thử được cập nhật theo tiến độ. Phiên ngày `23/10/2026` sử dụng HEAD `e7cde55` cùng các file harness chưa commit; mã PID production không thay đổi so với baseline `9280bae`.

## UT-PID-001 — Kiểm thử cascade P-only

### Hàm và mục tiêu kiểm thử

- Hàm kiểm thử: `pid_pitch_roll()`.
- Mục tiêu: Kiểm tra kết quả hai vòng Outer/Inner và trạng thái tích phân khi `KI=KD=0`.
- Trạng thái: `EXECUTED / PASS` — ngày `23/10/2026`, 1 test, 4 assertions, exit code `0`.

### Bằng chứng từ mã nguồn

- `Core/Inc/pid.h`, dòng 13: `DELTA_T = 0.002f` giây.
- `Core/Inc/pid.h`, dòng 15–24: các trường của `PID_t`.
- `Core/Src/pid.c`, dòng 15–41: chuyển đơn vị gyro, ánh xạ RC, tính và cập nhật vòng Outer.
- `Core/Src/pid.c`, dòng 49–71: lấy đầu ra Outer làm rate setpoint, tính và cập nhật vòng Inner.
- Mã nguồn vẫn cập nhật `error_sum` của cả hai vòng khi `KI=0`; chỉ thành phần đầu ra tích phân bằng 0.
- Các giá trị kỳ vọng dưới đây phù hợp với mã nguồn đã đối chiếu và đã được kiểm tra bằng 4 assertions trong phiên `23/10/2026`; không thay đổi expected sau khi chạy.

### Điều kiện ban đầu

- Tạo hai instance `PID_t` độc lập, gọi là `outer` và `inner`; không dùng chung đối tượng.
- Tất cả trường của mỗi instance được khởi tạo bằng 0 trước khi thiết lập gain: `KP`, `KI`, `KD`, `PID_OUT`, `error_sum`, `prev_val`, `IIR_derivative`.
- Sau đó thiết lập gain theo bảng đầu vào.
- Chỉ gọi `pid_pitch_roll()` một lần với trạng thái ban đầu này.

### Đầu vào

| Tham số | Giá trị |
| --- | --- |
| RC, truyền qua `rc_raw_axis` | `1600` |
| Gyro, truyền qua `gyro_val` | `0 rad/s` |
| Euler, truyền qua `euler_val` | `2°` |
| Outer | `KP=2`, `KI=0`, `KD=0` |
| Inner | `KP=1`, `KI=0`, `KD=0` |
| `DELTA_T` | `0.002` giây |

### Phép tính xác định kết quả kỳ vọng

Các kết quả được tính độc lập bằng số học từ đầu vào, không lấy đầu ra của hàm làm giá trị tham chiếu.

1. Chuyển gyro sang độ/giây:
   `gyro_deg = 0 × 57.295 = 0`.
2. Tính vòng Outer:
   `setpoint = (1600 − 1500) × 0.05 = 5°`;
   `error = 5 − 2 = 3°`;
   `outer.error_sum = 0 + 3 × 0.002 = 0.006`;
   giá trị này nằm trong giới hạn `[-500, 500]`, nên không bị chặn;
   `P_outer = 3 × 2 = 6`;
   `I_outer = 0.006 × 0 = 0`;
   trạng thái lọc D bằng `0 × 0.3 + 0 × 0.7 = 0`, nên `D_outer = 0`;
   `outer.PID_OUT = 6 + 0 + 0 = 6.0`.
3. Tính vòng Inner:
   `rate_setpoint = outer.PID_OUT = 6`;
   `rate_error = 6 − 0 = 6`;
   `inner.error_sum = 0 + 6 × 0.002 = 0.012`;
   giá trị này nằm trong giới hạn `[-500, 500]`, nên không bị chặn;
   `P_inner = 6 × 1 = 6`;
   `I_inner = 0.012 × 0 = 0`;
   `rate_derivative = (0 − 0) / 0.002 = 0`;
   trạng thái lọc D bằng `0 × 0.5 + 0 × 0.5 = 0`, nên `D_inner = 0`;
   `inner.PID_OUT = 6 + 0 + 0 = 6.0`.

### Kết quả kỳ vọng và tiêu chí đánh giá

Sau một lần gọi, kiểm tra đủ bốn giá trị:

| Giá trị quan sát | Kết quả kỳ vọng | Sai số tuyệt đối cho phép |
| --- | --- | --- |
| `outer.PID_OUT` | `6.0` | `0.00001` |
| `inner.PID_OUT` | `6.0` | `0.00001` |
| `outer.error_sum` | `0.006` | `0.00001` |
| `inner.error_sum` | `0.012` | `0.00001` |

Một giá trị đạt khi hữu hạn và có độ lệch tuyệt đối so với giá trị kỳ vọng không vượt quá `0.00001`. Ca kiểm thử chỉ đạt khi cả bốn giá trị đều đạt. Kết quả phiên `23/10/2026`: cả 4 assertions đều PASS.

### Kết quả thực thi ngày 23/10/2026

- Ca CppUTest tương ứng: `PID_PitchRoll.POnly_Cascade` trong `tests/pid/test_pid.cpp`.
- Đã compile chính `Core/Src/pid.c` bằng GCC UCRT64, compile hai file test bằng G++ UCRT64 và liên kết với `libCppUTest`.
- Kết quả: `OK (1 tests, 1 ran, 4 checks, 0 ignored, 0 filtered out, 0 ms)`; exit code `0`.
- Không có warning/error compiler hoặc linker trong lượt chạy thành công được ghi nhận.
- Lệnh tái thực thi từ thư mục dự án trong MSYS2: `bash tests/pid/run_tests.sh`.
- Bằng chứng, định danh harness, các lần khởi chạy bị chặn và giới hạn: [UNIT_TEST_LOG.md — phiên UT-SESSION-2026-10-23-001](UNIT_TEST_LOG.md#ut-session-2026-10-23-001).
- Đây là kết quả của lượt build/run đã quan sát trong phiên làm việc; lượt cập nhật hồ sơ kết thúc phiên không chạy lại test.

### Giới hạn

- Chưa kiểm tra tác động của `KI` hoặc `KD` khác 0.
- Chưa kiểm tra hành vi tại hoặc vượt giới hạn tích phân.
- Chưa kiểm tra nhiều lần gọi liên tiếp hoặc các trạng thái ban đầu khác.
- Chưa kiểm tra timing MCU hoặc phản ứng motor.
- Build/link đã xác nhận cho harness PID và môi trường của phiên này; chưa xác nhận các module khác hoặc môi trường khác.
