# Drone Debug — Daily Log

Bản nháp để review. Các trạng thái dưới đây phản ánh ghi nhận trong Word, không phải nghiệm thu mới. `UNVERIFIED` chỉ phần thiếu căn cứ; Xem [các điểm cần xác nhận](DAILY_LOG_REVIEW.md).

## 2026-10-22 — Tạm dừng B4, chuyển sang Host-based Unit Testing

- **Phạm vi:** B4 — ISR–Main Concurrency; hướng kiểm thử nhóm C (iBUS), D (Cascade PID).
- **Đã thực hiện:** Chốt tạm dừng B4 vì chưa có STM32 để kiểm chứng Mailbox/Snapshot + PRIMASK; mở nhánh Offline Test Harness với CppUTest / Mocks.
- **Kết luận:** Rà soát lý thuyết và build thành công chưa đủ nghiệm thu B4 trên target.
- **Trạng thái:** B4 `UNRESOLVED`; runtime PRIMASK chưa kiểm thử. Chưa có kết quả `Host Unit Test PASS` trong Word.
- **Tiếp theo:** Xây dựng test harness; kiểm thử logic iBUS và Cascade PID; bổ sung target runtime khi có phần cứng.

## 2026-10-19 — Hoàn tất build Mailbox/Snapshot + PRIMASK

- **Phạm vi:** B4.3/B4-3 iBUS.
- **Đã thực hiện:** Hoàn tất tái cấu trúc `iBus_Rx_Buff → iBus_Mail_Box → iBus_Snap_Shot → Checksum_Decode`.
- **Kết quả:** Critical Section lưu PRIMASK, `CPSID i`, `memcpy` Mailbox sang Snapshot rồi khôi phục PRIMASK bằng `MSR`; biên dịch thành công.
- **Trạng thái:** `Build PASS`; `UNRESOLVED` cho target runtime do chưa có phần cứng nạp thử. Không có log compiler hay cấu hình build kèm theo: UNVERIFIED về chi tiết build.
- **Tiếp theo:** Kiểm chứng pipeline và thời gian khóa ngắt trên STM32.

## 2026-10-06 — Kiểm thử pid_flag, thiết kế iBUS và SAFETY-01

- **Phạm vi:** B4.3A; B4.3/B4-3 iBUS và SAFETY-01; B4.5.
- **Kết quả B4.3A:** Chuỗi startup/chuyển tiếp: `308.292` TIM10 EVENT, `304.419` lần điều khiển, `race_count = 0`, `pbprobe_unstable_cnt = 0`. Chuỗi steady-state: `257.517` EVENT và `257.517` lần chạy, tỷ lệ `1:1` tại `500 Hz`.
- **Kết luận có giới hạn:** Source/Disassembly còn khe hở race; nguồn chốt B4.3A `Target Runtime PASS` có giới hạn dựa trên điều kiện đo. Không xóa chênh lệch startup và không suy rộng thành mọi pha đều 1:1.
- **Thiết kế iBUS:** Lấy khung mới nhất qua Mailbox/Snapshot; bảo vệ thao tác sao chép bằng PRIMASK. Giai đoạn triển khai kéo dài đến 2026-10-19, khi ghi nhận `Build PASS`.
- **Thay đổi iBUS:** Từ thiết kế 3 consumer, nguồn ghi RUNNING chỉ cần 2; đã loại consumer dư và thêm cờ dữ liệu sạch. `ibus_min_throttle` chỉ kiểm tra ga đáy. Chưa có kết quả retest riêng.
- **Chính sách SAFETY-01:** Hành vi cũ cho Arm, ép trần `13000 PWM` ở dải chờ. Chính sách mới: mất iBUS → giảm ga có kiểm soát; Low Battery → RTL; mất MPU6050 → MOTOR LOCK, bỏ duy trì ga tối thiểu. Đây là quyết định thiết kế trong nguồn, chưa có bằng chứng thực thi RTL/giảm ga trên target.
- **Trạng thái SAFETY-01:** Nguồn ghi “PASS THIẾT KẾ”; chưa đủ căn cứ đổi thành `Source Review PASS`, `Host Unit Test PASS` hoặc `Target Runtime PASS`.
- **B4.5:** `raw_adc_val` giữ nguyên thiết kế ADC-DMA circular; nguồn không ghi nghiệm thu đạt hay test riêng.
- **Tiếp theo:** Hoàn thiện và build Mailbox/Snapshot + PRIMASK; bổ sung bằng chứng runtime SAFETY-01; kiểm chứng B4 trước khi đóng mục.

## 2026-10-05 — Rà soát dữ liệu dùng chung giữa ISR và main

- **Phạm vi:** B4 Trace 1; B4.1.
- **Đã thực hiện:** Rà soát `pid_flag`, `ibus_cplt_flag`, `raw_adc_val` và kiến trúc UART single-buffer.
- **Vấn đề:** Nguồn xác nhận ba biến ban đầu thiếu `volatile`; UART có thể ghi đè buffer khi main đang tính Checksum hoặc giải mã. Đây là rủi ro từ source review, chưa phải bằng chứng lỗi runtime ngày này.
- **Kết luận:** Các pattern cờ nhị phân, iBUS single-buffer và ADC-DMA thiếu đồng bộ được ghi nhận kế thừa từ M-HIVE.
- **Trạng thái:** B4.1 “ĐÃ NHẬN DIỆN”; chưa có căn cứ `Source Review PASS` hay xác nhận đã sửa `volatile` trong ngày.
- **Tiếp theo:** Kiểm chứng `pid_flag`; bảo vệ dữ liệu iBUS; làm rõ cách xử lý `raw_adc_val`.

## 2026-10-03 — Clock, NVIC và chu kỳ TIM10

- **Phạm vi:** B1, B2, B3.
- **Đã thực hiện:** Đối chiếu firmware với CubeMX `.ioc`; rà NVIC Priority Group 4; đo TIM10 bằng Saleae Logic Analyzer.
- **Kết quả B1:** `HSE = 25 MHz`, PLL `M=12, N=96, P=2`, `SYSCLK = 100 MHz`; source và `.ioc` nhất quán. Chưa đo MCO bằng oscilloscope.
- **Kết quả B2:** Priority 0: TIM10/I2C1_EV/I2C1_ER; 1: EXTI1/USART2; 2: TIM11/DMA2; 15: SysTick. Nguy cơ preempt khi EXTI1 gọi `HAL_I2C_Mem_Read_IT()` mới là phân tích lý thuyết; chưa ghi nhận trễ deadline.
- **Kết quả B3:** 9249 chu kỳ; kỳ vọng `2000 μs`; trung bình `≈2.0000 ms`, Min `1.996 ms`, Max `2.00275 ms`. Nguồn ghi jitter `6.75 μs`, sai số `<0.34%`.
- **Trạng thái:** B1 `Source Review PASS` ở mức danh định; B2 `Source Review PASS` có giới hạn; B3 `Target Runtime PASS` theo Analyzer. Chưa có đo MCO để xác nhận tần số thực.
- **Tiếp theo:** Đối soát timing ở B7 và nguy cơ nghẽn ngắt ở B8.

## 2026-10-02 — Kiểm chứng cảm biến trên khung QAV250

- **Phạm vi:** A7.1–A7.4; kết luận tổng hợp nhóm A.
- **Đã thực hiện:** Test tĩnh không chạy motor, vận động khung bằng tay và kiểm chứng watchdog mất MPU.
- **Kết quả:** Roll/Pitch không drift nghiêm trọng hoặc runaway; không có spike liên tục. Roll `-45° → +47°`, Pitch `-53° → +49°`, `ωmax ≈2.70 rad/s (≈155°/s)`; chưa bão hòa trong dải `±500°/s`, `±16g`.
- **Kết quả watchdog:** TIM11 theo dõi raw accelerometer Z; không đổi quá `100 ms` thì đặt `mpu_loss_flag = 1`, gọi `Motor_Safety()` cắt ga. Nguồn chấp nhận giới hạn thiết kế.
- **Vấn đề:** A7.4 chưa test rung động có tải do thiếu LiPo 4S. Kết luận “A1 → A6 PASS” mâu thuẫn với A5 `UNRESOLVED` ngày 2026-09-29; không có retest A5 kèm theo.
- **Trạng thái:** A7.1–A7.3 `Target Runtime PASS` trong phạm vi nguồn nêu; A7.3 có giới hạn. A7.4 `UNRESOLVED` / chưa kiểm thử. Chuyển trạng thái A5: UNVERIFIED.
- **Tiếp theo:** Test A7.4 khi có pin; xác nhận căn cứ đóng A5.

## 2026-10-01 — Nghiệm thu bộ lọc bù và góc Euler

- **Phạm vi:** A6.1–A6.5, `CONVERT_TO_ORIENT` sau vá A1–01.
- **Đã thực hiện:** Rà công thức và đơn vị; test tĩnh, Pitch `±30°`, Roll hai chiều và hành vi Yaw.
- **Kết quả:** Góc Accel/Gyro cùng đơn vị radian; `MPU_DT = 0.002 s`, `α = 0.98`, trọng số Accel `0.02`, `τ ≈0.098 s ≈0.1 s`.
- **Kết quả runtime:** A6.5–T1 ổn định; T2 đúng dấu và biên độ Pitch; T3 Roll đối xứng; T4 Yaw có hành vi đúng dự kiến nhưng vẫn drift vì chỉ tích phân Gyro Z, theo giải thích trong nguồn.
- **Kết luận:** Nguồn chốt A6 đạt với giới hạn đã biết; chưa chứng minh `α = 0.98` là tối ưu.
- **Trạng thái:** A6 `Source Review PASS` về thuật toán/đơn vị và `Target Runtime PASS` cho các phép thử nêu trên, kèm giới hạn Yaw.
- **Tiếp theo:** Kiểm tra timing chi tiết ở B7.

## 2026-09-29 — Quy đổi dữ liệu đạt; spike Gyro chưa giải quyết

- **Phạm vi:** A4.1–A4.3; A5.
- **Đã thực hiện:** Rà Burst 14-byte, thứ tự byte, ép `int16_t`, hệ số quy đổi; quan sát tĩnh sau `MPU_CALIB_GYRO()`.
- **Kết quả A4:** Accel ở byte 0–5, bỏ temp 6–7, Gyro 8–13; ghép MSB/LSB Big-Endian rồi ép `int16_t`. `AFS_SEL = 3`, `mpu6050_accel_lsbs = 2048.0f (LSB/g)`, dải `±16g`; nghiêng `±30°` đo `≈±0.5g`.
- **Kết quả A4.3:** Dải Gyro `±500°/s`, độ nhạy `65.5 LSB/°/s`; nguồn dùng ví dụ `Raw=6550 → 100°/s ≈1.745 rad/s` để kiểm tra quy đổi, không ghi đây là phép đo runtime riêng.
- **Vấn đề A5:** Trung bình sau trừ bias quanh `0` nhưng có spike khi đứng yên: `Gyro.x ≈3.x rad/s (≈170°/s–200°/s)`, `Gyro.y ≈1.1 rad/s (≈63°/s)`, `Gyro.z ≈0.x rad/s`.
- **Giả thuyết:** Spike rất ngắn chỉ là khả năng được nêu; chưa có capture xác định nguyên nhân hoặc thời lượng.
- **Trạng thái:** A4 `Source Review PASS` về layout/quy đổi; A4.2 có `Target Runtime PASS` từ phép nghiêng tay. A5 `UNRESOLVED`; trung bình gần 0 không đủ đóng lỗi spike.
- **Tiếp theo:** Thiết lập Peak Detector; chụp Snapshot khi vượt ngưỡng; điều tra spike.

## 2026-09-28 — Vá ngắt lỗi, cô lập startup và đóng A3 có giới hạn

- **Phạm vi:** A3-S4.2, A3-S5–A3-S9.
- **Đã thực hiện:** Bật `I2C1_ER_IRQn`; thêm `I2C1_ER_IRQHandler()` gọi `HAL_I2C_ER_IRQHandler(&hi2c1)` trong `stm32f4xx_it.c`; bổ sung `HAL_I2C_ErrorCallback(&hi2c1)` ghi `hi2c1.ErrorCode`, phân loại BERR/ARLO/AF/OVR.
- **Sửa lỗi startup:** Tách bật DATA_RDY và LATCH_INT_RD_CLEAR khỏi `MPU_INIT()`; chỉ bật sau `Pre_Flight_Check()`, tránh IT chen vào `MPU_CALIB_GYRO()`.
- **Retest:** `BUSY=0`, `Completion_while_flag_set=0`, `Start_while_processing=0`, `Error_callback=0`; nguồn chốt A3-S8 đạt. Không ghi thời lượng retest.
- **Thay đổi recovery:** Bỏ gọi `unstuck_i2c1()` ngay khi `HAL_BUSY`; dùng Drop Request → Retry → Recovery khi lỗi kéo dài → MCU FAILSAFE nếu recovery thất bại. Chưa có kết quả fault injection từng nhánh.
- **Giới hạn chấp nhận:** Giữ cờ nhị phân `mpu_data_ready_flag` và `raw_buffer` đơn. Nguồn dẫn lại test 30 s trước đó (`ΔForeground=15435`, `Δcompletion_while_set_flag=0`), không phải phép đo mới ngày này; chưa quan sát Torn Read.
- **Trạng thái:** Nguồn đóng A3 có giới hạn: A3-S5 đạt có điều kiện; A3-S6 cố tình chấp nhận; A3-S7 đạt ở mức vá code; A3-S8 `Target Runtime PASS`; A3-S9 đạt ở mức tái cấu trúc. `Build PASS` và runtime fault injection A3-S7/S9: UNVERIFIED.
- **Kết luận:** Chấp nhận Burst 14-byte, 500 Hz không đồng nghĩa đã loại bỏ mọi rủi ro ownership/event folding; đo START/ACK/9 xung vẫn nằm ngoài A3-LITE.

## 2026-09-25 — Phân tích tranh chấp I2C và Torn Read

- **Phạm vi:** A3-S2.4–A3-S4.2.
- **Đã thực hiện:** Phân tích `hi2c1.State`, `HAL_BUSY`, `unstuck_i2c1()`, `raw_buffer` và việc lấy 500 mẫu trong `MPU_CALIB_GYRO(500)`.
- **Vấn đề:** Blocking và IT cùng dùng `hi2c1`; gọi unstuck chỉ vì `HAL_BUSY` có thể phá transaction Calib. Hàm Calib đếm 500 lượt nhưng không kiểm tra `HAL_StatusTypeDef`, nên không bảo đảm 500 lần đọc thành công.
- **Phân tích, chưa phải lỗi tái hiện:** Accel mẫu N và Gyro mẫu N+1 có thể lệch `2000 μs`. Ví dụ Torn Read: `511 (0x01FF) → 512 (0x0200)`, ghép high byte cũ với low byte mới thành `0x0100 = 256`, dù mẫu thật chỉ đổi `1 LSB`.
- **Kết luận:** Cờ `mpu_data_ready = 1` không bảo đảm ownership; `HAL_OK` hoặc `INT_RD_CLEAR = 1` không tự chứng minh mẫu mới hợp lệ. Tác động lên PID được nguồn suy luận, chưa có test gây lỗi bay.
- **Trạng thái:** Các rủi ro được nhận diện qua phân tích source; chưa có `Target Runtime PASS` cho biện pháp khắc phục ngày này.
- **Tiếp theo:** Cô lập Calib/EXTI; bỏ recovery chỉ dựa `HAL_BUSY`; cân nhắc Double Buffering hoặc Critical Section / PRIMASK cho `raw_buffer`.

## 2026-09-24 — Phân rã startup và quyền sở hữu I2C

- **Phạm vi:** A3-S1, A3-S2; phân tích Startup Life-Cycle.
- **Đã thực hiện:** Rà trình tự `MPU_INIT()`, `MPU_CALIB_GYRO()`, trạng thái `hi2c1.State` và các vector ngắt.
- **Vấn đề:** Nguồn mô tả I2C1_EV và EXTI bật quá sớm; Data Ready 500 Hz chạy song song với Calib đọc blocking 500 mẫu trên cùng `hi2c1`/`raw_buffer`. `I2C1_ER_IRQn` chưa bật.
- **Kết luận:** Cần một nguồn khởi tạo transaction tại mỗi thời điểm; phần mô tả chi tiết nằm trong hồ sơ gộp 24/09–28/09, không có log runtime riêng ngày 24/09.
- **Trạng thái:** Đã phân tích; chưa có căn cứ đóng A3-S1/S2 hoặc xác nhận bản vá trong ngày. Chi tiết ngày hoàn tất từng phát hiện: UNVERIFIED.
- **Tiếp theo:** Cô lập đọc blocking và IT; rà chính sách `HAL_BUSY`; bổ sung xử lý ngắt lỗi.

## 2026-09-23 — Sửa Debug Launch, nghiệm thu A3-LITE steady-state

- **Phạm vi:** A3-0, Memory Watchpoint Investigation, A3-LITE Runtime.
- **Đã thực hiện:** Watchpoint tại `0x200002D7`, đối chiếu Disassembly/ELF/map; dùng nguồn `3.3V` ST-Link / Discovery, chưa cắm LiPo 4S.
- **Lỗi và sửa:** CPU dừng ở `PC=0x08001D70`, lệnh `STRB R2, [R3]`, `R3=0x200002D7`, `R2=1`; ELF mới tại cùng địa chỉ là `CMP`. Nguồn xác định `Download = false` khiến chip chạy firmware cũ; chuyển `Download = true`, Clean → Rebuild → Flash.
- **Retest boot:** Byte D7 hết ghi sai; bốn counter chuỗi thu nhận cùng bằng `1`, bốn counter bất thường bằng `0`.
- **Kết quả steady-state 30 s:** Kỳ vọng `15000 samples ±3%`; `Δmpu_start_i2c_ok_count = Δmpu_i2c_rx_complete_count = Δmpu_foreground_processed_count = 15435` (1:1:1), `Δmpu_exti_count ≈15435`. `Δmpu_i2c_start_busy_count`, `Δmpu_i2c_start_error_count`, `Δmpu_completion_while_flag_set_count`, `Δmpu_i2c_start_while_processing_count` đều `0`.
- **Vấn đề startup:** Vẫn có `mpu_completion_while_flag_set_count = 79`, `mpu_i2c_start_busy_count = 10`. Nguồn ghi EXTI/IT tranh chấp với blocking trong `MPU_INIT / MPU_CALIB_GYRO`; nguy cơ làm sai bias chưa được đo định lượng.
- **Trạng thái:** `Target Runtime PASS` cho clean boot và steady-state A3-LITE 30 s; cảnh báo startup còn mở, không gộp vào kết quả đạt.
- **Tiếp theo:** Dời bật ngắt sau Calib; kiểm tra return status các lần đọc I2C; retest startup.

## 2026-09-22 — Instrumentation A3-LITE và bất thường counter RAM

- **Phạm vi:** A3-LITE Instrumentation & Memory Audit; A3-0.
- **Đã thực hiện:** Thiết kế chuỗi counter `mpu_exti_count`, `mpu_start_i2c_ok_count`, `mpu_i2c_rx_complete_count`, `mpu_foreground_processed_count`; bẫy `mpu_i2c_start_busy_count`, `mpu_i2c_start_error_count`, `mpu_completion_while_flag_set_count`, `mpu_i2c_start_while_processing_count`; giám sát `mpu_processing_active`.
- **Quan sát RAM:** `mpu_exti_count` tại `0x200002D0` hiển thị `10 00 00 00` (nguồn diễn giải là `1`); `mpu_i2c_start_ok_count` tại `0x200002D4` hiển thị `00 00 00 C1` (`0xC1000000`). Phần ghi tiếp lại mô tả D7=`0x01`, counter=`16777216 (0x01000000)`; hai mô tả chưa khớp.
- **Kết quả rà soát:** ELF/map chưa thấy symbol ghi đè. Kết luận ban đầu nghi task/con trỏ khác ghi RAM chưa được chứng minh trong ngày.
- **Trạng thái:** Nguồn ghi `FAIL / TOOLCHAIN BUG` cho sự cố ngày 22/09 trong bảng tổng hợp; nguyên nhân chỉ được xác định ngày 23/09. Tên `mpu_start_i2c_ok_count` và `mpu_i2c_start_ok_count` khác nhau trong nguồn: UNVERIFIED.
- **Tiếp theo:** Watchpoint byte `0x200002D7`; đối chiếu lệnh thực thi với ELF; xác minh dữ liệu Memory View.

## 2026-09-21 — Sửa dấu Pitch, đóng A1 và kiểm tra cấu hình MPU6050

- **Phạm vi:** A1-01/A1–01; A1-T1–A1–T7; A2–T1; mở A3-LITE (nguồn ghi chung 21/09 & 22/09).
- **Test tĩnh/Roll:** Sau `1 phút`, Accel `X≈0, Y≈0, Z≈+1g`, Gyro `≈0°/s`, Roll/Pitch ổn định, Yaw drift `≈-6,3°`. Roll Left: `gyro.x > 0`, Roll `-33°–-36°`; Roll Right: `gyro.x < 0`, Roll `≈+45°`.
- **Lỗi ban đầu:** A1-01 `FAIL / INCONSISTENT`: `CONVERT_TO_ORIENT` trong `mpu6050.c` dùng `pitch_prev - mpu_mea.ptr`, trái dấu Inner PID `+gyro.y`. Nhánh `-accel.x` đúng theo test; IMU đặt chân cắm hướng Front, lỗ ốc hướng Rear.
- **Bằng chứng Pitch:** Nose Down: `gyro.y < 0`, Pitch `≈-32°`, `accel.x ≈+0.51–+0.55 g`; Nose Up: `gyro.y > 0`, Pitch `≈+25°`, `accel.x ≈-0.52–-0.54 g` (giữ khoảng `±30°`).
- **Sửa và retest:** Đổi thành `pitch_prev + mpu_mea.ptr` (phần nghiệm thu ghi `pitch_prev + gyro.y * dt`); Pitch sau sửa `≈-30° / +33°`, về ngang đúng, không nhiễu chéo Roll. A1–T7: Yaw trái `gyro.z > 0`, góc tăng; phải `gyro.z < 0`, góc giảm.
- **Vấn đề còn theo dõi:** A1–T6-2 có một spike `gyro.y = +18`, đơn vị UNVERIFIED; không tái xuất hiện khi retest. Nguyên nhân cơ học/I2C mới là giả thuyết.
- **Kết quả A2:** Gyro `±500°/s`, `65.5 LSB/°/s`; Accel `±16g`, `2048 LSB/g`; `DLPF_CFG = 2` (Gyro `98 Hz`, Accel `94 Hz`, nội `1 kHz`); `SMPLRT_DIV = 1`, Data Ready `500 Hz / 2000 μs`. Analyzer trước đó ghi `≈2 ms`; ngày đo Analyzer riêng UNVERIFIED.
- **Trạng thái:** Nguồn đóng A1 sau retest: `Target Runtime PASS` trong phạm vi quy ước trục, không phải kiểm chứng bay. A2–T1 đạt theo cấu hình và Analyzer được dẫn lại; chưa read-back thanh ghi. Chưa có kết quả build riêng.
- **A3-LITE / tiếp theo:** Hồ sơ chung 21–22/09 nhận diện nguy cơ `HAL_BUSY` gọi nhầm `unstuck_i2c1`, event folding và race `raw_buffer`; ngày riêng từng phát hiện UNVERIFIED. Hoãn đo SDA/SCL (START, ACK, 9 xung) sang G4; theo dõi spike và kiểm chứng acquisition.
