# Hồ sơ các ca kiểm thử đơn vị

## Phạm vi và trạng thái hồ sơ

- Dự án: `iBUS_MPU_ESC_Mix` trong repository `Basic_Drone`.
- Nhánh đối chiếu: `test/cpputest`; commit nguồn: `9280bae`.
- Chưa có `UNIT_TEST_PLAN.md` tại thời điểm lập hồ sơ này; không tạo kế hoạch thay thế.
- Hồ sơ chỉ mô tả thiết kế kiểm thử, chưa có mã CppUTest, chưa biên dịch hoặc chạy kiểm thử.

## UT-PID-001 — Kiểm thử cascade P-only

### Hàm và mục tiêu kiểm thử

- Hàm kiểm thử: `pid_pitch_roll()`.
- Mục tiêu: Kiểm tra kết quả hai vòng Outer/Inner và trạng thái tích phân khi `KI=KD=0`.
- Trạng thái: `DESIGNED / NOT RUN`.

### Bằng chứng từ mã nguồn

- `Core/Inc/pid.h`, dòng 13: `DELTA_T = 0.002f` giây.
- `Core/Inc/pid.h`, dòng 15–24: các trường của `PID_t`.
- `Core/Src/pid.c`, dòng 15–41: chuyển đơn vị gyro, ánh xạ RC, tính và cập nhật vòng Outer.
- `Core/Src/pid.c`, dòng 49–71: lấy đầu ra Outer làm rate setpoint, tính và cập nhật vòng Inner.
- Mã nguồn vẫn cập nhật `error_sum` của cả hai vòng khi `KI=0`; chỉ thành phần đầu ra tích phân bằng 0.
- Các giá trị kỳ vọng dưới đây phù hợp với mã nguồn đã đối chiếu; chưa được xác nhận bằng thực thi.

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

Một giá trị đạt khi hữu hạn và có độ lệch tuyệt đối so với giá trị kỳ vọng không vượt quá `0.00001`. Ca kiểm thử chỉ đạt khi cả bốn giá trị đều đạt. Hiện chưa có kết quả thực thi để kết luận đạt hoặc không đạt.

### Giới hạn

- Chưa kiểm tra tác động của `KI` hoặc `KD` khác 0.
- Chưa kiểm tra hành vi tại hoặc vượt giới hạn tích phân.
- Chưa kiểm tra nhiều lần gọi liên tiếp hoặc các trạng thái ban đầu khác.
- Chưa kiểm tra timing MCU hoặc phản ứng motor.
- Chưa xác nhận khả năng biên dịch và liên kết CppUTest trên Windows.
