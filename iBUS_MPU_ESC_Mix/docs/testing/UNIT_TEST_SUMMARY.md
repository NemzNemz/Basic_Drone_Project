# Tổng hợp Host-Based Firmware Verification

## Kết quả ngày 26/10/2026

**77 ca khác nhau đã thực chạy, 501 assertions, 77 PASS, 0 FAIL, exit code 0** trong final regression. Không có ca bị ignored hoặc filtered out. Kết quả chỉ xác nhận các input, trạng thái và thứ tự event được liệt kê trong [chi tiết 77 ca](FIRMWARE_TEST_CASES.md), với giới hạn của stub; không xác nhận toàn bộ firmware đúng hoặc đủ điều kiện bay.

- Đã khảo sát 18 tệp C trong `Core/Src/` cùng header và các tệp ứng dụng liên quan. Bảy tệp chứa logic ứng dụng được compile bằng GCC UCRT64 từ **production source nguyên bản**, gồm cả `main.c`.
- Thực thi parser, decoder, conversion/filter, PID, state machine, watchdog bookkeeping, mixer và ghi CCR bằng mã production. Không thay chúng bằng bản sao trong fake.
- Các ca tích hợp chạy số vòng hữu hạn của `main()` thật, được cấp input tại boundary đã mô phỏng; đây không phải mô phỏng STM32 hoặc IRQ thật.
- Giữ nguyên `UT-PID-001`, thư mục `tests/pid/`, kế hoạch kiểm thử và bản ghi ngày 23/10. Không sửa production, safety policy, hồ sơ audit hoặc hai tài liệu kế hoạch CL-01.
- Các finding/giới hạn target trước đây không được đóng bởi kết quả host này. Không thay đổi trạng thái A5, B4 target verification, F01–F03 hoặc safety gates.

## Firmware Verification Matrix

Số ca trong bảng tính theo ID khác nhau. Nhánh recovery BERR dùng chung `UT-MPU-012`, không cộng thêm vào tổng.

| Module | Function/logic tested | Cases | PASS | FAIL | BLOCKED/UNTESTED | Confidence/Limitations |
| --- | --- | --- | --- | --- | --- | --- |
| PID | `pid_pitch_roll`, `pid_yaw_rate`, `pid_yaw_angle`, `reset_error` | UT-PID-001–014: 14 | 14 | 0 | Miền góc nhiều vòng, non-finite, mọi tổ hợp state/gain | Có oracle số học độc lập cho P/I/D, clamp, IIR, wrap và lặp 1000 bước; không tuning/timing MCU |
| iBUS | checksum, decode, failsafe bit, parser, UART callback, init, hai helper pre-flight | UT-IBUS-001–011: 11 | 11 | 0 | Byte loss/baud/IRQ concurrency; length ngoài contract32 | Frame cố định, lật từng byte, resync, latest-frame-wins và 1000 frame tuần tự; không RF hoặc stress thời gian thật |
| MPU6050 | init/register masks, DRDY start, raw/scaling/bias, calibration mean, filter/state/sign, async arguments/status, callbacks | UT-MPU-001–013: 13 | 13 | 0 | Bus thật, freshness, tearing, HAL error state machine, calibration failure oracle | Math và request boundary được chạy; fake cung cấp register/raw/status, không chứng minh hardware |
| Motor | init, clamp, field→CCR, lock/minimum, update→set | UT-MOTOR-001–005: 5 | 5 | 0 | Pin/PWM/ESC/RPM; float→uint16 ngoài miền hợp lệ | CCR là register RAM giả; không kết luận motor vật lý dừng/chạy |
| Battery | conversion, hysteresis, DMA API address/length, conversion→state | UT-BAT-001–004: 4 | 4 | 0 | ADC calibration, DMA transfer width/visibility/atomicity, nguồn dưới tải | Xác nhận phép tính/hysteresis và tham số request, không DMA thật |
| Application | `Motor_Safety`, timer flags/counters, timeout→output, ESC calibration sequence, RC→state | UT-APP-001–014: 14 | 14 | 0 | CPU stall, mọi tổ hợp fault/switch, real delay, physical safety outcome | Characterization đúng thiết kế hiện hành; không đặt hoặc phán xét policy mới |
| Tích hợp | raw→filter→PID; main UART→snapshot→decode→control→mixer→CCR; battery→warning; pre-flight→calibration; watchdog | IT-001–016: 16 | 16 | 0 | NVIC/preemption/PRIMASK thật, deadline/CPU margin, toàn bộ lịch event | Chạy main và các module thật, chèn event tuần tự tại boundary; 15 ca IT-002–016 kiểm tra main/pre-flight, IT-001 math chain |
| Utility recovery | startup read retry và CPU write BERR trong `unstuck_i2c1` | Chung UT-MPU-012 | Chung 1 | 0 | Nhánh non-BERR: 9 SCL, RCC reset, `delay_nanos` | PARTIALLY TESTABLE; vòng poll TIM2 cần timer tiến triển; không dựng timer simulator |
| Generated/config/startup | Đã đọc để phân loại, không tạo test cho wrapper thuần | 0 | — | — | Hardware integration required | MX init được stub; suite không xác minh cấu hình peripheral, clock hoặc vector IRQ |
| **Tổng không trùng** | **Các ca thực thi nêu trên** | **77** | **77** | **0** | **Các giới hạn được giữ riêng** | **Không đo coverage phần trăm hoặc bao phủ hết branch/input** |

## Inventory và truy vết source

Các dòng dưới đây tham chiếu production baseline; source không thay đổi trong chiến dịch.

| Tệp / vị trí chính | API hoặc logic / state | Phân loại và phạm vi đã thực thi |
| --- | --- | --- |
| `Core/Src/pid.c:15,77,107,132`; `Core/Inc/pid.h` | Bốn API PID; `PID_t.error_sum/prev_val/IIR_derivative/PID_OUT`, gain và dt | UNIT TESTABLE; không phụ thuộc HAL trong phép tính sau compatibility include |
| `Core/Src/iBUS.c:16–20,23,36,48,53,91,101,114` | Buffer RX/mailbox, cờ, parser state, checksum/decode/failsafe, init và helpers | UNIT TESTABLE; callback boundary PARTIALLY TESTABLE. Hai helper vẫn truy cập buffer RX trực tiếp; test tuần tự không chứng minh ownership khi ISR chen vào |
| `Core/Src/mpu6050.c:49,87,142,148,169,195,243,392` | `CONVERT_TO_ORIENT`, calibration, set bias, raw parse, init, IT read, read register, DRDY start; private config helpers đi qua public API | Math UNIT TESTABLE; register driver PARTIALLY TESTABLE; I2C/HAL/HW REQUIRED cho transaction thật |
| `Core/Src/motor.c:13,19,26,33,40` | `Motor_Init/Set_Speed/Lock/Min_Throttle/Update_Values`; timer pointer nội bộ, MOTOR fields | Số học UNIT TESTABLE; register boundary PARTIALLY TESTABLE |
| `Core/Src/battery.c:12,20,24` | `BAT_INIT/BAT_GET_VOL/is_bat_low`; hysteresis static | UNIT TESTABLE cho conversion/state; DMA request boundary PARTIALLY TESTABLE |
| `Core/Src/unstuck_i2c.c:18,26,57,65` | `delay_nanos/i2c_absolve/unstuck_i2c1_rcc_reset/unstuck_i2c1`; TIM2, I2C1, GPIO/RCC | BERR CPU branch PARTIALLY TESTABLE; non-BERR BLOCKED trong harness hiện tại, cần timer/bus hardware |
| `Core/Src/main.c:173–346` | Startup/handover, MPU consume/rawZ, snapshot PRIMASK, valid-frame bookkeeping, battery, pid_flag, PID và mixer | HOST INTEGRATION TESTABLE với bounded main; không kiểm chứng atomicity/latency |
| `Core/Src/main.c:401,425,445,456` | `Pre_Flight_Check`, EXTI, I2C completion, UART callbacks | Scripted healthy input/callback filtering thực thi trên host; không phải NVIC/async HAL implementation thật |
| `Core/Src/main.c:466–510,533,547,609` | Buzzer helpers, `ESC_Calib/Motor_Safety/HAL_TIM_PeriodElapsedCallback` | State/tick logic và CCR/delay request được test; không bao phủ độc lập mọi buzzer pattern hoặc mọi early-exit |
| `Core/Src/main.c:359,636,655` | `SystemClock_Config/Error_Handler/assert_failed` | Clock call path dùng stub; không khẳng định cấu hình đúng qua PASS. Vòng lỗi vô hạn/assert failure chưa thực thi để xác nhận liveness |
| `adc.c,dma.c,gpio.c,i2c.c,tim.c,usart.c` | MX peripheral init/config | HARDWARE REQUIRED; không test từng wrapper generated |
| `stm32f4xx_hal_msp.c,stm32f4xx_it.c,system_stm32f4xx.c` | MSP, IRQ wrappers, system/startup glue | HARDWARE REQUIRED; callback được gọi từ harness, không qua vector/IRQ thật |
| `syscalls.c,sysmem.c` | Runtime/syscall/heap glue | Không có business logic cần thêm test trong chiến dịch; không host-verify target runtime |
| `Core/Startup/*.s`, STM32 HAL/CMSIS | Vector, startup, driver MCU | Ngoài unit scope; không compile trong host harness |
| `README.c` | Tài liệu dạng comment | Không phải module logic thực thi. Prototype `Buzzer_Ont` không có body không được tính là chức năng đã test |

Bảy tệp C production được compile; 11 tệp C generated/runtime còn lại được phân loại ở bảng trên. Các state public được reset trong fixture; state private được reset qua API/input bình thường, không bỏ `static`. Lượt `-r2` kiểm tra lại cùng suite trong cùng process; không chứng minh mọi thứ tự test hoặc lịch interrupt đều độc lập.

## Case detail và oracle

[Chi tiết từng ca](FIRMWARE_TEST_CASES.md) gồm input/preconditions, expected/basis, actual, tolerance qua artifact, kết quả và giới hạn. Actual được lấy từ log thực thi, **không dùng làm expected**. Có [365 observation số](evidence/2026-10-26/firmware-host/observations.tsv) của 76 ca mới; `UT-PID-001` giữ test cũ chỉ in PASS của bốn assertions, không thêm output actual giả.

Oracle dựa trên công thức và hằng số hiện hành được tính độc lập, vector frame xác định và state transitions đã review. Test behavior hiện có không đồng nghĩa đánh giá thiết kế là an toàn. Fake chỉ cấp input/register/status và lưu lời gọi; không tính PID, filter, parser, mixer hoặc quyết định motor thay production.

Các tolerance được ghi trong test và observation. Có kiểm tra finite trước phép so float của test mới; không nới tolerance theo actual. Không có production-test FAIL bị đổi expected, bị ignored hoặc bị bỏ khỏi final suite. Lỗi khởi tạo môi trường/harness được giữ riêng ở lịch sử dưới đây.

## Giới hạn còn lại

| Phần | Trạng thái | Lý do / điều chưa chứng minh |
| --- | --- | --- |
| I2C/UART, MPU register thực tế, IRQ/NVIC, PRIMASK | HARDWARE REQUIRED | Stub không mô phỏng peripheral state machine, ACK/NACK, preemption, interrupt masking hay memory barrier instruction |
| DMA, buffer tearing/ownership, sample freshness/coalescing | HARDWARE REQUIRED | Event host được lập lịch tuần tự. Snapshot case kiểm tra consumer dùng đúng copy sau publish mới; không chứng minh mọi interleaving |
| Clock/PWM/timeouts, execution time/WCET/deadline | HARDWARE REQUIRED | Tick/delay là input và request; số ms framework là host, không phải thời gian STM32 |
| Non-BERR recovery và TIM2 polling | BLOCKED / PARTIALLY TESTABLE | Chỉ BERR branch được chạy. Không tạo timer simulator hoặc kết luận recovery 9 SCL thành công |
| Null pointer, buffer/length ngoài contract32 | NEEDS REVIEW | API không có contract lỗi đáng tin để đặt expected hợp lệ; không đưa input gây đọc vượt vùng rồi gọi đó là policy FAIL |
| NaN/Inf hoặc float→uint16 ngoài miền biểu diễn, multi-turn yaw | NEEDS REVIEW | Không xác định expected theo thiết kế hiện hành; không tự đặt sanitization/wrap mới |
| Calibration blocking read thất bại | NEEDS REVIEW | Không tạo expected bias/fail-start mới. Các ca mean hiện chỉ cấp HAL_OK; không xác nhận freshness hoặc error handling |
| Buzzer patterns, lỗi init/assert/Error_Handler, tổ hợp fault chưa liệt kê | UNTESTED / PARTIALLY TESTABLE | Có một số chuỗi delay/CCR được quan sát, chưa bao phủ mọi pattern/branch; target liveness cần bằng chứng khác |
| ARM/failsafe/ESC/độ chính xác sensor và flight | Ngoài kết luận host | Test đúng thiết kế hiện hành; không xây dựng/phán xét safety policy, không thay hardware validation |

Không gán xác suất race, coverage phần trăm hoặc đánh giá mức độ an toàn từ suite này. Việc không có FAIL trong 77 ca không phủ định những fault ngoài input/contract đã chạy.

## Môi trường, revision và tái lập

| Thuộc tính | Giá trị |
| --- | --- |
| Source baseline / nhánh gốc | `test/cpputest@94f1a8028dd9a58ce51ee851348bf735bf80ba6f` |
| Nhánh chiến dịch | `test/cpputest-firmware`, worktree riêng; không merge develop |
| Harness revision đã commit trước final regression | `7268b82ce5e4735617f79f02fe654410c3fb0554` |
| Final run | `20261026T115934Z-1427`, bắt đầu 11:59:34 UTC, tức 18:59:34 Asia/Bangkok ngày 26/10/2026 |
| Toolchain | MSYS2 UCRT64 GCC/G++ `16.2.0 (Rev6)`, target `x86_64-w64-mingw32`; CppUTest `4.0` |
| Compile/link | GNU11 cho production C, C++11 cho test; `-O0 -g -Wall -Wextra -ffunction-sections -fdata-sections`; forced `host_compat.h`; link `-lCppUTest -lm -Wl,--gc-sections` |
| Main adaptation | Chỉ compiler define `main=firmware_main`; C `setjmp/longjmp` dừng tại intrinsic stub sau số vòng hữu hạn, không sửa source |
| Warnings final | Hai warning production `hw_esc_htim` unused tại `motor.c:26,33`; không có warning harness mới |
| Raw evidence | [Thư mục bằng chứng](evidence/2026-10-26/firmware-host/); mỗi run có log, exit; run đã build có checksum và output test |
| Định danh executable/source | `SHA256SUMS.txt` theo từng run; executable/object nằm trong build bị ignore, không commit binary |
| Final-state regression | Harness đã commit và sạch khi chạy final; commit báo cáo sau đó chỉ thêm tài liệu/evidence, không đổi source/test đã chạy |

Từ thư mục `iBUS_MPU_ESC_Mix` trong MSYS2 UCRT64:

```bash
bash tests/firmware/run_tests.sh
bash tests/firmware/run_tests.sh -r2
FW_OPT=-O2 bash tests/firmware/run_tests.sh
```

Runner tự tạo build và thư mục run UTC; có timeout compile/link60s mỗi lệnh, executable120s. Exit compile/link/test nonzero được giữ; không cho run rỗng được ghi PASS. Tham số `-g NO_SUCH_GROUP` được dùng để kiểm chứng negative path của runner, không phải production FAIL.

## Lịch sử thực thi được giữ lại

| Run / thao tác | Kết quả | Diễn giải |
| --- | --- | --- |
| [113410Z-2022](evidence/2026-10-26/firmware-host/20261026T113410Z-2022/run.log) | Exit1, chưa compile | MSYS Git nhìn checkout CRLF khác canonical do khác system config; sửa cách gọi Git trong runner, không sửa source hoặc Git config |
| [114634Z-600](evidence/2026-10-26/firmware-host/20261026T114634Z-600/run.log) | 69 tests/452 checks, exit0 | Lượt mở rộng ban đầu; có warning harness misleading-indentation, đã thêm braces trước final; không đổi expected |
| [115112Z-1361](evidence/2026-10-26/firmware-host/20261026T115112Z-1361/run.log) | 77/501, exit0 | Sau bổ sung boundary/integration cases |
| [115448Z-1049](evidence/2026-10-26/firmware-host/20261026T115448Z-1049/run.log) | `-r2`: 2×77/501, exit0 | 154 lượt thực thi,1002 checks; vẫn77 ID khác nhau |
| Khởi chạy nested Bash bằng PATH chưa có `bash` | Exit1 trước runner | Ghi trong [environment-notes.txt](evidence/2026-10-26/firmware-host/environment-notes.txt); gọi `/usr/bin/bash` tường minh để thử lại |
| [115555Z-1199](evidence/2026-10-26/firmware-host/20261026T115555Z-1199/run.log) | `-O2`:77/501, exit0 | Khác optimization của host; không chứng minh production MCU optimization/concurrency |
| [115756Z-1613](evidence/2026-10-26/firmware-host/20261026T115756Z-1613/run.log) | 0 ran, exit1 | Negative runner probe có chủ đích, kết quả mong muốn là nonzero; không production FAIL |
| [115934Z-1427](evidence/2026-10-26/firmware-host/20261026T115934Z-1427/run.log) | Final77/501, exit0 | HEAD harness7268b82, O0; hai warning production đã nêu |

Không ghi đè hoặc sửa log thô của các lần trước. Không kiểm chứng đủ mọi loại compiler/linker failure; các đường đó được bảo vệ bởi runner nhưng chỉ negative no-tests path có phép thử chủ đích trong phiên. Thời gian vài ms CppUTest in ra chỉ là thời gian chạy host.
