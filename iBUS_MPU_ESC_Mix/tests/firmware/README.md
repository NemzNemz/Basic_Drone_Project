# Host verification firmware STM32F411

Harness này compile **production C nguyên bản**: `pid.c`, `iBUS.c`, `mpu6050.c`, `motor.c`, `battery.c`, `unstuck_i2c.c`, `main.c`. Test `UT-PID-001` và runner main CppUTest cũ được compile lại trực tiếp, không sửa lịch sử hoặc expected.

## Chạy lại

Từ thư mục `iBUS_MPU_ESC_Mix` trong MSYS2 UCRT64:

```bash
bash tests/firmware/run_tests.sh
bash tests/firmware/run_tests.sh -r2
FW_OPT=-O2 bash tests/firmware/run_tests.sh
```

Yêu cầu GCC/G++ UCRT64 và CppUTest tại `/ucrt64`; script chọn compiler tường minh. Có thể gọi Bash MSYS2 từ PowerShell. `build/` tự tạo và bị ignore. Mỗi run giữ `run.log`, `test-output.log`, `SHA256SUMS.txt`, `exit_code.txt` trong `build/runs/<UTC>-<PID>/`. Compile/link timeout 60 giây/lệnh, test executable 120 giây; runner dừng nếu build/test lỗi và trả nguyên exit code test/timeout. Filter không chạy test nào trả nonzero: môi trường đã kiểm tra trả 1 từ CppUTest; guard bổ sung trả 2 nếu executable trả 0 nhưng không có summary test thực chạy. Không coi run rỗng là PASS.

Production baseline: `94f1a8028dd9a58ce51ee851348bf735bf80ba6f`. Runner kiểm tra `Core/` không khác baseline trước compile; dùng `core.autocrlf=true` riêng cho lệnh Git để tương thích checkout CRLF của Git Windows. Không đổi cấu hình Git hoặc sửa line ending production.

## Cơ chế cô lập

- `host_compat.h` force-include `<stdint.h>`, guard `__MAIN_H` và kiểu/prototype HAL tối thiểu. Không include hoặc giả lập toàn bộ STM32 HAL/CMSIS.
- `fake_hal.cpp` chỉ cấp input/raw register, trả HAL status, ghi lại lời gọi/CCR và cung cấp handle giả. Conversion, filter, parser, PID, quyết định safety và mixer đều chạy trong production.
- `main.c` được compile với `-Dmain=firmware_main`; không đổi source. `host_control.c` dùng `setjmp/longjmp` dừng sau số vòng foreground hữu hạn tại stub `__get_PRIMASK()`. Không nhảy qua đối tượng C++ có destructor trong call chain.
- Luồng main được cấp frame UART, raw MPU và event timer tuần tự. Init peripheral generated là stub no-op; MPU reset register được mô hình hóa về `PWR_MGMT_1=0x40`. HAL delay không ngủ: chỉ lưu thời lượng yêu cầu và CCR tại điểm gọi.
- Test snapshot công bố frame mới ngay sau restore PRIMASK giả rồi kiểm tra foreground vẫn decode snapshot cũ. Đây là kiểm tra logic bàn giao, **không chứng minh instruction PRIMASK hoặc IRQ ownership trên Cortex-M4**.
- Fixture reset global public; bias qua API; battery hysteresis qua input bình thường; parser qua 32 byte zero rồi xóa buffer/flag. Không bỏ `static`, không include bản sao thuật toán, không sửa production.
- `OBS|ID|field|expected=...|actual=...|tolerance=...` ghi số đo host trước assertion. Bảng báo cáo dùng output thật; các phép so byte/pointer có assertion riêng.

## Oracle và giới hạn

Expected dựa trên thiết kế hiện hành, hằng số cấu hình và số học độc lập: RC→setpoint, tích phân/IIR, frame 32 byte/checksum, LSB→g/rad/s, complementary filter, hysteresis, ngưỡng counter, CCR lock/minimum/clamp và mixer đang dùng. Phần protocol/RX chỉ kiểm chứng contract firmware hiện có, không xác nhận mọi loại receiver.

Không kiểm thử policy mới hoặc đánh giá an toàn bay. Không có oracle rõ cho null pointer, frame length khác 32, non-finite sensor, calibration read thất bại hoặc miền góc ngoài contract thì **NEEDS REVIEW**, không tạo expected theo suy đoán.

Không kiểm chứng bus vật lý, NVIC/preemption, atomicity/tearing, timeout theo thời gian thật, sample freshness, GPIO waveform, recovery 9 SCL, ESC/motor, reset/brownout hoặc flight. Non-BERR recovery có vòng poll `TIM2->CNT`, giữ BLOCKED/HARDWARE REQUIRED thay vì dựng timer simulator. Hai warning unused parameter trong `motor.c` được giữ nguyên và ghi trong log; không sửa production.

Matrix, case detail và bằng chứng cuối xem [UNIT_TEST_SUMMARY.md](../../docs/testing/UNIT_TEST_SUMMARY.md).
