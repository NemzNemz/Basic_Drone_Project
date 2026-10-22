# Kế hoạch kiểm thử đơn vị firmware STM32F411

## 1. Thông tin quản lý tài liệu

| Thuộc tính | Giá trị |
| --- | --- |
| Mã tài liệu | `UT-PLAN-001` |
| Phiên bản | `0.2` |
| Ngày lập | `22/10/2026` |
| Trạng thái | `DRAFT` |
| Repository | `R:/GIT_HUB_REPO/Basic_Drone` |
| Dự án | `iBUS_MPU_ESC_Mix` |
| Branch | `test/cpputest` |
| Baseline source | `9280baea43388dbb60920486ca539a855fc804df` — `fix(ibus): bảo vệ snapshot frame RUNNING bằng PRIMASK` |
| Phạm vi | Các module nghiệp vụ tùy biến của toàn bộ firmware STM32F411, không chỉ PID |
| Trách nhiệm kỹ thuật | Thiết kế kiểm thử; triển khai/thực thi; rà soát/phê duyệt |
| Nguyên tắc cập nhật | Kế hoạch chung, chỉ cập nhật khi phạm vi hoặc phương pháp kiểm thử thay đổi; không dùng làm nhật ký tiến độ |

### 1.1. Mục tiêu

- Xây dựng quy trình kiểm thử offline bằng CppUTest trên Windows, có thể lặp lại và mở rộng theo từng module.
- Kiểm tra mã C production thật, với expected được xác định độc lập và có thể giải thích.
- Ưu tiên logic liên quan đến an toàn, điều khiển và tính ổn định của drone; cân đối với chi phí cô lập dependency.
- Phân biệt kiểm thử logic trên host với hardware integration test và systematic pre-flight audit.
- Tạo hồ sơ phục vụ thực hành, học tập và truy vết kỹ thuật lâu dài; không tuyên bố chất lượng sản phẩm thương mại hoặc trạng thái sẵn sàng bay từ kết quả host-test.

### 1.2. Nguồn thông tin và mức bằng chứng

- **XÁC NHẬN TỪ SOURCE:** khai báo, công thức, dependency và control flow đọc từ baseline nêu trên.
- **ĐÁNH GIÁ / ĐỀ XUẤT:** mức ưu tiên, khả năng cô lập và chiến lược kiểm thử; chưa phải kết quả thực thi.
- **THÔNG TIN NGƯỜI PHÁT TRIỂN CUNG CẤP:** kết quả môi trường CppUTest cơ bản 3/3 test ví dụ PASS. Chưa đối chiếu log gốc trong repository khi lập kế hoạch.
- **BẰNG CHỨNG THỰC THI:** chỉ hình thành sau khi lưu lệnh, môi trường, phiên bản mã, expected/actual và kết quả chạy. Hiện chưa có kết quả Unit Test chính thức của firmware drone.

Các đường dẫn source bên dưới tính từ thư mục dự án `iBUS_MPU_ESC_Mix`; số dòng thuộc baseline, cần đối chiếu lại sau khi mã thay đổi.

## 2. Phạm vi và phân loại kiểm thử

### 2.1. Phân loại A/B/C và P0/P1/P2

| Nhóm | Ý nghĩa |
| --- | --- |
| A | Logic hoặc tính toán tương đối dễ cô lập; vẫn có thể cần shim header và xử lý dependency khi liên kết nguyên file |
| B | Cần Stub/Mock, thanh ghi giả hoặc test harness đáng kể để kiểm thử hành vi phần mềm |
| C | Bằng chứng chính phải đến từ hardware integration test; host chỉ hỗ trợ kiểm tra mô hình hoặc thứ tự thao tác |

| Ưu tiên | Căn cứ |
| --- | --- |
| P0 | Có đường ảnh hưởng trực tiếp đến lệnh điều khiển, ARM/DISARM, motor output hoặc dữ liệu đầu vào của controller |
| P1 | Ảnh hưởng acquisition, initialization, phản ứng lỗi hoặc điều phối hệ thống; cần kiểm thử sau hoặc song song với lõi P0 |
| P2 | Cảnh báo, chẩn đoán, chức năng phụ hoặc phần có giá trị kiểm chứng host thấp hơn |

Một module có thể chứa nhiều loại A/B/C và nhiều mức ưu tiên. P0 không đồng nghĩa phải triển khai trước mọi việc khác nếu chưa có harness phù hợp.

### 2.2. Ngoài phạm vi Unit Test nghiệp vụ trực tiếp

- Không kiểm thử lại implementation STM32 HAL/CMSIS như mã do dự án tự phát triển.
- Không đưa toàn bộ mã CubeMX, startup, `system_stm32f4xx.c`, `syscalls.c`, `sysmem.c` vào danh mục thuật toán nghiệp vụ.
- Cấu hình peripheral, `.ioc` và IRQ wrapper vẫn là dependency hoặc đối tượng integration/configuration review khi cần; không bị coi là không quan trọng.
- `README.c` là tài liệu, không phải module nghiệp vụ trong danh sách source build hiện tại.
- Không tự thay đổi firmware để thuận tiện kiểm thử; thay đổi production phải có nhiệm vụ và review riêng.

## 3. Danh mục module nghiệp vụ

### 3.1. Các module C riêng

| Thành phần và truy vết source | Chức năng, hàm chính | Nhóm / ưu tiên | Có thể kiểm thử offline | Bắt buộc kiểm chứng trên hardware | Dependency, trạng thái và trở ngại | Tiến độ |
| --- | --- | --- | --- | --- | --- | --- |
| PID — `Core/Src/pid.c:15,77,107,132`; `Core/Inc/pid.h:11–30` | `pid_pitch_roll()`, `pid_yaw_rate()`, `pid_yaw_angle()`, `reset_error()`; cascade, tích phân, IIR và wrap góc | A / P0 | Công thức, state qua các lần gọi, biên tích phân, reset và chuyển đơn vị | Chu kỳ gọi thật, tuning, ổn định drone và phản ứng motor | Không gọi HAL; state nằm trong `PID_t`; `DELTA_T=0.002f`; header kéo theo `main.h` | `UT-PID-001`: `DESIGNED / NOT RUN`; chưa host-build |
| iBUS — `Core/Src/iBUS.c:23,36,48,53,91,101,114` | `ibus_check_sum()`, `ia6b_decode_data()`, `is_failsafe()`, `iBUS_Parse_Byte()`, `fs_i6ab_init()`, `is_iBUS_Throttle_Min()`, `is_iBUS_Received()` | A cho checksum/decode/parser tuần tự; B cho init/helper; C cho timing/concurrency thật / P0 | Replay byte stream, frame hoàn chỉnh, decode, failsafe, checksum, state parser và policy mailbox | UART thật, mất byte do overrun, ngắt chen vào, mapping RC vật lý | Global `fs_i6`, buffer 32 byte, cờ và byte UART; static `ibus_huart`, `index`; init gọi `HAL_UART_Receive_IT()` | Đã inventory; chưa triển khai test, `NOT RUN` |
| MPU6050 — `Core/Src/mpu6050.c:49,87,142,148,169,195,392` | `CONVERT_TO_ORIENT()`, `MPU_CALIB_GYRO()`, `MPU_SET_GYRO_BIAS()`, `MPU_RAW_MEASUREMENT()`, `MPU_INIT()`, `MPU_TRIGGER_READ_IT()`, `MPU_DATA_READY_START()` | A cho toán học; B cho thiết lập state/driver/calibration; C cho cảm biến và bus / P0 cho conversion/filter, P1 cho driver | Vector raw, scaling/bias, chuỗi Euler, mock register và return status HAL, phép tính calibration | Trục/dấu vật lý, nhiễu/rung, freshness mẫu, I2C và độ chính xác bias thực | `atan2f()`, `sqrtf()`, `M_PI`; I2C HAL, delay, assert, recovery; static bias, scale và I2C handle; scale được thiết lập trong init | Đã inventory; chưa triển khai test, `NOT RUN` |
| Motor — `Core/Src/motor.c:13,19,26,33,40`; `Core/Inc/motor.h:9–25` | `Motor_Init()`, `Motor_Set_Speed()`, `Motor_Lock()`, `Motor_Min_Throttle()`, `Motor_Update_Values()` | A cho clamp; B cho register interface; C cho PWM/ESC / P0 | Clamp từng field `MOTOR`, mapping field tới CCR giả, giá trị lock/minimum | TIM1/pin thật, OneShot125, đấu nối và phản ứng ESC/motor | Static `esc_htim`; ghi trực tiếp `Instance->CCR1..4`; clamp `13000..25000`, lock `12500` | Đã inventory; chưa triển khai test, `NOT RUN` |
| Battery — `Core/Src/battery.c:12,20,24`; `Core/Inc/battery.h:13` | `BAT_INIT()`, `BAT_GET_VOL()`, `is_bat_low()`; conversion và hysteresis | A cho conversion/hysteresis; B cho ADC DMA init; C cho đo điện áp / P2 | Factor `0.01363f`, chuyển trạng thái dưới 13.0 V/trên 13.5 V và vùng giữ state | ADC DMA thật, mạch chia áp, calibration điện áp và nhiễu nguồn | Static `hw_adc`, static local `bat_low`; `HAL_ADC_Start_DMA()`; đường hiện tại dùng cho cảnh báo pin | Đã inventory; chưa triển khai test, `NOT RUN` |
| Unstuck I2C — `Core/Src/unstuck_i2c.c:18,26,57,65` | API `unstuck_i2c1()`; `delay_nanos()` có external linkage nhưng không nằm trong header; helper private `i2c_absolve()`, `unstuck_i2c1_rcc_reset()` | B cho nhánh/thứ tự thao tác; chủ yếu C cho hiệu quả recovery / P2 cho host | Điều kiện BERR, chuỗi GPIO/reset/init trong mô hình giả | Hiệu quả giải phóng bus, mức SDA/SCL, số xung và timing thật | `hi2c1`, I2C1, GPIOB/PB6, RCC, TIM2 và HAL; poll `TIM2->CNT` cần mô hình tiến thời gian để tránh treo test | Đã inventory; chưa triển khai test, `NOT RUN` |

### 3.2. Các nhóm nghiệp vụ đang nằm trong `main.c`

Các nhóm dưới đây chưa phải module độc lập. Không sao chép lại công thức hoặc state machine sang test rồi coi đó là kiểm thử production. Khả năng cô lập phải được chứng minh ở Bước 04; chưa đề xuất refactor trong kế hoạch này.

| Nhóm và truy vết source | Chức năng, logic chính | Nhóm / ưu tiên | Có thể kiểm thử offline | Bắt buộc kiểm chứng trên hardware | Dependency, trạng thái và trở ngại | Tiến độ |
| --- | --- | --- | --- | --- | --- | --- |
| Motor Safety — `Core/Src/main.c:584–644` | `Motor_Safety()`; ARM/DISARM, lỗi MPU, failsafe và lựa chọn output | B / P0 | Điều kiện, chuyển trạng thái, ưu tiên nhánh và lời gọi motor/buzzer qua harness | Output và phản ứng motor thật, thời gian xử lý lỗi | `fs_i6`, `ibus_fresh_data`, `motor_lock_flag`, loss/failsafe flags, heading, `BAT_vol`, timer và motor state | Đã inventory; chưa có test thực thi |
| Pre-flight — `Core/Src/main.c:438–460` | `Pre_Flight_Check()`; chờ RX, chọn calibration và kiểm tra throttle/SwA | B / P0 | Control flow với chuỗi helper giả và input có kiểm soát | RX thật, điều kiện nguồn/ESC và quy trình thao tác an toàn | Helper iBUS, `fs_i6`, vòng chờ, beep/delay; input bất biến có thể làm test không kết thúc | Đã inventory; chưa có test thực thi |
| ESC Calibration — `Core/Src/main.c:570–582` | `ESC_Calib()`; chuỗi CCR `25000` rồi `12500` với delay | B cho sequence; C cho tác động / P0 | Giá trị ghi và thứ tự delay/register trong harness | ESC có nguồn, trạng thái boot, calibration thực và motor | TIM1 trực tiếp, `HAL_Delay()`; đường này không qua điều kiện ARM của `Motor_Safety()` | Đã inventory; chưa có test thực thi |
| Control/Mixer — `Core/Src/main.c:306–383` | Reset/call PID, chọn yaw mode, RC mapping, bốn phương trình mixer và hoán đổi output M1/M4 | B hiện tại; phép tính có tiềm năng A khi cô lập được / P0 | Giá trị số, dấu biểu thức, biên chuyển mode, trình tự gọi và mapping trong phần mềm | Corrective action vật lý, orientation, thứ tự motor, ESC và ổn định điều khiển | Sáu `PID_t`, RC, MPU/Euler, `Yaw_HEADING_REF`, các output PID, `motor_speed`; chưa có hàm mixer riêng | Đã inventory; chưa có test thực thi |
| iBUS handover — `Core/Src/main.c:262–295` | Save PRIMASK, check/copy/clear flag, restore, checksum/decode snapshot và cập nhật freshness | B cho logic; C cho bảo đảm target / P0 | Mô hình cờ/snapshot, nhánh có/không có frame, bookkeeping; không coi mock là chứng minh masking thật | ARM instruction, interrupt latency, UART overrun và lịch ISR/main thật | PRIMASK/CMSIS, shared mailbox, `ibus_snapshot`, `fs_i6`, cờ fresh/failsafe/loss | Đã inventory; chưa có test thực thi |
| Watchdog/Scheduler — `Core/Src/main.c:246–259,306–323,646–694` | Consume MPU, so raw-Z, đếm loss, đặt `pid_flag` và đo DWT | B cho state transition; C cho định thời / P0 cho safety watchdog, P1 cho scheduler/chẩn đoán | Ngưỡng bộ đếm, reset, callback sequence và trạng thái lỗi trong mô hình | Khoảng thời gian thật, jitter, missed event, DMA/ISR concurrency và DWT target | `old_raw_z`, `curren_raw_z`, loss counters/flags, timer handles, `pid_flag`, DWT và counter đo B3/B4 | Đã inventory; chưa có test thực thi |
| Interrupt callbacks — `Core/Src/main.c:462,482,493` và nhánh timer tại 646 | `HAL_GPIO_EXTI_Callback()`, `HAL_I2C_MemRxCpltCallback()`, `HAL_UART_RxCpltCallback()`, `HAL_TIM_PeriodElapsedCallback()` | B cho adapter; C cho IRQ thật / P1 | Instance/pin filtering, forward handle/buffer, flag và yêu cầu nhận tiếp | Vector/NVIC, preemption, completion/error thực và bus vật lý | HAL handles, `mpu_mea`, byte UART, parser, timer và flags; callback được host gọi trực tiếp không phải ngắt thật | Đã inventory; chưa có test thực thi |
| Buzzer — `Core/Src/main.c:503,510,515,525,536` | `Buzzer_On()`, `Buzzer_Off()`, `Buzzer_Error_Beep()`, `Buzzer_Status_Beep()`, `Buzzer_Success()` | B cho sequence; C cho âm thanh/timing / P2 | Giá trị register, lời gọi PWM/delay và thứ tự thao tác | Âm thanh, duty/frequency thực và độ trễ trong hệ thống | TIM3, HAL PWM, delay; không có persistent state riêng trong nhóm hàm | Đã inventory; chưa có test thực thi |

### 3.3. Lưu ý khi cô lập mã hiện tại

- Các header tùy biến kéo theo `main.h` hoặc kiểu STM32; cần xem xét include chain và C/C++ linkage trước host-build.
- Nhiều hàm logic A nằm cùng translation unit với hàm HAL. Test hàm A không nhất thiết loại bỏ mọi unresolved symbol khi liên kết nguyên file.
- `pid.c` không có global state riêng; fixture tạo `PID_t` độc lập cho từng test. State ẩn của parser, hysteresis và MPU phải được khởi tạo có kiểm soát, không mặc định tự reset giữa test.
- `MPU_RAW_MEASUREMENT()` phụ thuộc scale static được init thiết lập; không gọi trong fixture với scale chưa sẵn sàng.
- `reset_error()` chỉ xóa `error_sum`; đây là hành vi source, không được tự thay thành reset toàn bộ state trong expected.
- Hai helper tiền kiểm tra vẫn checksum/decode `ibus_rx_buf`; consumer RUNNING dùng snapshot. Host-test tuần tự không tự chứng minh hoặc bác bỏ race đã audit.
- `iBUS_Take_Frame()` đang comment; `Buzzer_Ont()` chỉ có declaration. Không tính hai tên này là chức năng đang hoạt động.
- Startup helper MPU vẫn gọi `unstuck_i2c1()` khi read register lỗi; không coi recovery là dead code. Lời gọi recovery trong nhánh runtime `HAL_BUSY` đang comment.
- `assert` và `assert_param` là các cơ chế khác nhau; cần ghi cấu hình áp dụng cho host/target, không âm thầm tắt assert để làm test chạy qua.
- Điểm cần khảo sát qua test gồm contract độ dài checksum iBUS, cast trước clamp trong mixer, state sau reset và phản ứng calibration khi read lỗi. Đây là ứng viên kiểm thử, không phải kết luận lỗi mới hoặc chỉ thị sửa firmware.

## 4. Quy trình kiểm thử 8 bước

Quy trình áp dụng lặp lại theo module hoặc nhóm test. Mỗi bước có thể quay về bước trước khi phát hiện thiếu đặc tả, dependency hoặc bằng chứng. Tạo được tài liệu không đồng nghĩa bước đã PASS; trạng thái tiến độ và kết quả thực thi phải tách riêng.

### Bước 01 — Test Scope & Inventory

- **Mục đích:** xác định chính xác phần mã được kiểm tra và ranh giới host/hardware.
- **Công việc:** liệt kê module/hàm; dependency và state; phân loại A/B/C; xếp P0/P1/P2; ghi phần loại trừ và giới hạn bằng chứng.
- **Trách nhiệm:** thiết kế kiểm thử xác định danh mục và phạm vi; triển khai/thực thi hỗ trợ truy vết; rà soát/phê duyệt kiểm tra phạm vi trước khi chốt.
- **Đầu ra:** danh mục có source reference trong kế hoạch này và các điểm chưa xác minh.
- **Điều kiện chuyển bước:** người phát triển review phạm vi của thành phần tiếp theo; không bỏ sót side effect quan trọng hoặc ghi hardware behavior thành logic thuần.

### Bước 02 — Requirements / Function Contract

- **Mục đích:** có đặc tả kiểm thử được, không chỉ mô tả tên hàm.
- **Công việc:** xác định input/output, kiểu và đơn vị, state trước/sau, công thức, miền hợp lệ, side effect, giả định initialization và xử lý input bất thường. Tách hành vi source hiện tại khỏi hành vi mong muốn; ghi yêu cầu chưa thống nhất.
- **Trách nhiệm:** thiết kế kiểm thử xác định contract; triển khai/thực thi đối chiếu source; rà soát/phê duyệt kiểm tra contract và các giả định ảnh hưởng an toàn hoặc expected.
- **Đầu ra:** contract gắn với test ID trong `UNIT_TEST_CASES.md` hoặc phần đặc tả liên quan của hồ sơ Git.
- **Điều kiện chuyển bước:** input, expected behavior và giả định đủ rõ để tính expected độc lập; mâu thuẫn source/yêu cầu đã được ghi nhận và có hướng xử lý, không âm thầm chọn một phía.

### Bước 03 — Test Case Design

- **Mục đích:** biến contract thành ca kiểm thử có thể tái thực thi.
- **Công việc:** ghi test ID, mục tiêu, precondition, input, expected, tolerance, điều kiện đạt và giới hạn. Xem xét trường hợp thông thường, biên, state và bất thường khi phù hợp; tính expected độc lập với production function.
- **Trách nhiệm:** thiết kế kiểm thử xác định expected và đặc tả; triển khai/thực thi đánh giá tính khả thi; rà soát/phê duyệt kiểm tra đặc tả trước khi viết test.
- **Đầu ra:** các ca đã review trong `UNIT_TEST_CASES.md`, trạng thái `DESIGNED / NOT RUN` nếu chưa chạy.
- **Điều kiện chuyển bước:** expected có phép tính hoặc nguồn tham chiếu; tolerance có căn cứ; fixture đủ rõ; không sao chép thuật toán production để làm oracle duy nhất.

### Bước 04 — Test Harness Setup

- **Mục đích:** chạy mã production thật trên host, cô lập phần cứng mà không làm sai đối tượng kiểm thử.
- **Công việc:** thiết lập CppUTest, build configuration, shim HAL/CMSIS, Stub/Mock hoặc register model khi cần. Xác định cách reset fixture/state; ngăn vòng chờ/delay làm treo host. Đảm bảo mock chỉ thay dependency, không thay chính logic cần kiểm thử.
- **Trách nhiệm:** triển khai/thực thi thiết lập harness; thiết kế kiểm thử xác nhận dependency và phạm vi cô lập; rà soát/phê duyệt kiểm tra harness và mọi đề nghị tác động firmware.
- **Đầu ra:** cấu hình và harness host-test tái sử dụng được, có hướng dẫn môi trường và dependency.
- **Điều kiện chuyển bước:** xác định đúng file production được compile, C/C++ linkage, state isolation và các khác biệt với target; không yêu cầu sửa production ngoài một nhiệm vụ đã được duyệt riêng.

### Bước 05 — Host Build

- **Mục đích:** tạo executable kiểm thử native Windows một cách có thể tái lập.
- **Công việc:** compile mã C production bằng GCC; compile test C++ và liên kết CppUTest; kiểm tra warnings, dependencies, assert, C/C++ linkage và thư viện toán. Ghi toolchain/flags, lệnh và exit code; không nhầm `arm-none-eabi-gcc` với compiler native.
- **Trách nhiệm:** triển khai/thực thi thực hiện build và lưu log; rà soát/phê duyệt kiểm tra warning/error và xác nhận đúng executable.
- **Đầu ra:** executable build thành công cùng log và thông tin môi trường.
- **Điều kiện chuyển bước:** không còn build error; warning đã được review và xử lý hoặc chấp nhận có lý do; không đổi expected để che lỗi build.

### Bước 06 — Test Execution

- **Mục đích:** tạo bằng chứng thực thi cho từng test ID.
- **Công việc:** chạy executable đúng phiên bản; thu expected/actual, tolerance, PASS/FAIL/BLOCKED/NOT RUN; lưu lệnh, môi trường, commit, thay đổi chưa commit nếu có và log. Lưu seed/input cho test sinh dữ liệu.
- **Trách nhiệm:** triển khai/thực thi chạy test và thu bằng chứng; rà soát/phê duyệt kiểm tra kết quả có ý nghĩa an toàn, bất thường hoặc FAIL.
- **Đầu ra:** bằng chứng có thể tái thực thi và mục phiên trong `UNIT_TEST_LOG.md` khi hồ sơ này được tạo ở nhiệm vụ sau.
- **Điều kiện chuyển bước:** kết quả gắn đúng source/test/harness; phân biệt test đã chạy với test bị skip hoặc chưa chạy; không suy ra toàn module PASS từ một test đơn lẻ.

### Bước 07 — Failure Analysis & Regression

- **Mục đích:** xác định nguyên nhân FAIL và bảo vệ hành vi đã kiểm chứng.
- **Công việc:** tái hiện; phân biệt lỗi test, môi trường, yêu cầu và production; ghi mức bằng chứng. Chỉ sửa production sau phân tích và review; chạy lại ca lỗi cùng regression liên quan sau thay đổi.
- **Trách nhiệm:** triển khai/thực thi tái hiện và phân tích; thiết kế kiểm thử đối chiếu contract/expected; rà soát/phê duyệt quyết định hướng xử lý và kiểm tra thay đổi trước khi chốt.
- **Đầu ra:** vấn đề có kết luận, liên kết phiên FAIL ban đầu với phiên regression và commit sửa nếu được phép thực hiện.
- **Điều kiện chuyển bước:** FAIL quan trọng có kết luận hoặc được giữ mở rõ ràng; regression có bằng chứng; không xóa FAIL cũ khi lần sau PASS.

### Bước 08 — Review & Reporting

- **Mục đích:** tổng hợp đúng phạm vi, kết quả và giới hạn.
- **Công việc:** review suite, contract, log và coverage nếu có; cập nhật phần đã/chưa test, rủi ro, hardware validation còn thiếu và trạng thái hiện hành.
- **Trách nhiệm:** triển khai/thực thi tổng hợp bằng chứng; thiết kế kiểm thử đánh giá phạm vi đã kiểm tra; rà soát/phê duyệt kết luận trước khi dùng kết quả cho audit hoặc quyết định thay đổi firmware.
- **Đầu ra:** `UNIT_TEST_SUMMARY.md` khi được tạo ở nhiệm vụ sau, liên kết test case/log và kết luận có bằng chứng.
- **Điều kiện hoàn thành vòng:** không tuyên bố vượt quá bằng chứng; mọi giới hạn được lưu; xác định công việc tiếp theo. Không tự đóng vấn đề pre-flight chỉ vì host suite PASS.

## 5. Trách nhiệm kỹ thuật

### 5.1. Thiết kế kiểm thử

- Xác định chức năng, input/output, đơn vị, trạng thái, công thức và contract cần kiểm thử.
- Thiết kế test case, expected độc lập, tolerance và tiêu chí đánh giá; không chỉ chấp nhận vì công cụ cho kết quả giống source.
- Xác định phạm vi, giả định, giới hạn bằng chứng và ảnh hưởng của thay đổi yêu cầu tới test.

### 5.2. Triển khai và thực thi

- Truy vết source/dependency, triển khai test và harness, cấu hình môi trường, build và chạy theo phạm vi đã được duyệt.
- Kiểm soát fixture/state, định danh executable, lưu log và dữ liệu tái hiện; phân biệt đề xuất với kết quả đã chạy.
- Tái hiện FAIL, hỗ trợ phân tích và thực hiện regression sau các thay đổi đã được phê duyệt.

### 5.3. Rà soát và phê duyệt

- Đối chiếu source và contract; kiểm tra expected, tolerance và các giả định quan trọng.
- Review harness, phân tích FAIL đáng chú ý, quyết định hướng xử lý và phê duyệt thay đổi có tác động đến firmware.
- Kết luận phạm vi của PASS/FAIL, giới hạn còn lại và việc sử dụng kết quả cho audit; ghi rõ phần đã duyệt và phần còn mở.
- Cập nhật expected phải có lý do, nguồn và review độc lập với việc làm cho test xanh.

### 5.4. Sử dụng công cụ hỗ trợ

- Có thể dùng công cụ tự động để hỗ trợ inventory, tạo tài liệu, thiết lập harness, build, chạy test và tổng hợp kết quả trong phạm vi được giao.
- Người thực hiện có thể đảm nhận nhiều vai trò, nhưng phải tách rõ việc thiết kế, thực thi và phê duyệt; công cụ không thay thế trách nhiệm kỹ thuật hoặc review.
- Không để một quy trình tự động tự sửa production, tự đổi expected rồi tự tuyên bố PASS mà không có rà soát/phê duyệt.
- Kế hoạch này không cấp quyền tự động sửa firmware, build, chạy test, commit, push hoặc merge.

## 6. Quy tắc duy trì kế hoạch và hồ sơ tiến độ

- `UNIT_TEST_PLAN.md` là kế hoạch chung về phạm vi và phương pháp kiểm thử. Chỉ cập nhật khi phạm vi hoặc phương pháp thay đổi, không cập nhật theo từng buổi làm việc hay từng lần chạy test.
- `UNIT_TEST_CASES.md` được duy trì theo tiến độ thiết kế và review ca kiểm thử; thay contract, expected hoặc tolerance phải có lý do và truy vết.
- `UNIT_TEST_LOG.md` được bổ sung theo từng phiên thiết kế, triển khai hoặc thực thi; phiên chưa chạy test không được ghi thành kết quả thực thi. Lịch sử kết quả được giữ theo nguyên tắc append-only tại mục 7.2.
- `UNIT_TEST_SUMMARY.md` được cập nhật để phản ánh trạng thái tổng hợp mới nhất, phần còn mở và giới hạn bằng chứng; không thay thế lịch sử log.
- Duy trì các tệp hồ sơ cố định nêu trên, không tạo một file Markdown mới cho mỗi ngày hoặc mỗi phiên. Ngày và mã phiên được ghi thành mục trong log; dữ liệu tái hiện và artifact có thể tổ chức theo thư mục bằng chứng tại mục 7.3.
- Các bảng tiến độ tại mục 3 và mục 10 là ảnh chụp trạng thái khi lập baseline kế hoạch, không phải bảng theo dõi hằng ngày. Tiến độ mới nhất được duy trì trong `UNIT_TEST_CASES.md`, `UNIT_TEST_LOG.md` và `UNIT_TEST_SUMMARY.md`.

## 7. Quy định hồ sơ Git và truy vết

### 7.1. Hệ thống hồ sơ dự kiến

| Tệp trong `docs/testing/` | Vai trò | Trạng thái tại thời điểm lập kế hoạch |
| --- | --- | --- |
| `UNIT_TEST_PLAN.md` | Kế hoạch chung; chỉ cập nhật khi phạm vi hoặc phương pháp thay đổi | `DRAFT` |
| `UNIT_TEST_CASES.md` | Hồ sơ contract và ca kiểm thử, cập nhật theo tiến độ thiết kế/review | Đã có `UT-PID-001`; giữ nguyên nội dung hiện hữu |
| `UNIT_TEST_LOG.md` | Hồ sơ lịch sử phiên, bổ sung theo tiến độ và giữ kết quả cũ | Chưa tạo; chỉ tạo khi có nhiệm vụ tiếp theo |
| `UNIT_TEST_SUMMARY.md` | Hồ sơ tổng hợp trạng thái hiện hành, cập nhật theo tiến độ | Chưa tạo; chỉ tạo khi có nhiệm vụ tiếp theo |

Lượt lập kế hoạch chỉ tạo `UNIT_TEST_PLAN.md`; không sửa `UNIT_TEST_CASES.md`, không tạo log/summary hoặc tệp bằng chứng. Câu ghi chưa có kế hoạch trong hồ sơ ca kiểm thử mô tả thời điểm lập hồ sơ đó, không phải chỉ thị xóa nội dung lịch sử.

### 7.2. Nhật ký append-only về mặt lịch sử

- Mỗi phiên có ngày/giờ, module, test ID, commit source/test/harness, công việc, kết quả, đường dẫn bằng chứng và bước tiếp theo.
- Khi có working-tree diff, ghi hash hoặc nội dung patch kèm bằng chứng; chỉ commit ID là chưa đủ để định danh mã đã chạy.
- Không xóa hoặc thay FAIL cũ bằng PASS mới. Phiên mới liên kết phiên cũ; sửa thông tin lịch sử bằng ghi chú đính chính có lý do.
- Phân biệt kết quả mới nhất trong summary với lịch sử đầy đủ trong log.
- Chỉ ghi kết quả đã quan sát. Kế hoạch chạy thử, dự đoán và nhận định chưa xác minh phải được đánh dấu riêng.
- Khi thay contract, expected hoặc tolerance, ghi lý do, phiên bản, người review và các test bị ảnh hưởng.

### 7.3. Nội dung tối thiểu của bằng chứng thực thi

- Ngày/giờ và mã phiên; Windows/MSYS2 shell; đường dẫn và phiên bản GCC/G++, CppUTest, công cụ bổ trợ.
- Source baseline, phiên bản test/harness, trạng thái working tree, cấu hình compile/link và assert.
- Lệnh chạy, exit code, số test thực chạy, kết quả assertion, expected/actual khi FAIL và output lỗi.
- Với replay/fuzzing: input/corpus, seed, giới hạn chạy, timeout và dữ liệu tái hiện.
- Đường dẫn đề xuất: `docs/testing/evidence/<ngày-YYYY-MM-DD>/<mã-phiên>/<tác-vụ>/`. Đây là quy ước dự kiến, chưa tạo thư mục hoặc file; không đưa mật khẩu/token hoặc dữ liệu không liên quan vào log.
- Artifact lớn không tự động commit vào Git. Cách lưu, giới hạn dung lượng và chính sách tracking bằng chứng sẽ được review khi thiết lập harness; không tạo lại hồ sơ HANDOFF/SESSION đã được yêu cầu bỏ tracking.

## 8. Automation và chiến lược kiểm thử offline

### 8.1. Điều kiện chung trước khi chạy không giám sát

- Có nhiệm vụ cho phép chạy; harness đã review, có smoke test thành công và executable được định danh.
- Contract, oracle/tolerance hoặc tiêu chí phát hiện bất thường đã chốt cho tác vụ.
- Fixture/state isolation được kiểm tra; không truy cập địa chỉ peripheral thật, không flash hoặc điều khiển drone.
- Ghi môi trường, source/test/harness, seed/input và budget trước khi bắt đầu; mọi vòng lặp hoặc poll đều có timeout ở mức tác vụ/harness.
- Chỉ chạy một worker trong giai đoạn đầu; không dùng đa luồng để giả định đã mô phỏng đúng ISR Cortex-M4.
- Các budget dưới đây là đề xuất ban đầu cho phiên tự động, không phải số đo hoặc deadline firmware. Người phát triển review và điều chỉnh trước khi bật automation.

### 8.2. Danh mục tác vụ dự kiến

Mỗi tác vụ lưu log riêng và lệnh trong `<thư-mục-phiên>/<tác-vụ>/`; dữ liệu tái hiện lưu cùng thư mục. Giới hạn log thô đề xuất tối đa 100 MiB/tác vụ, không ghi đè lỗi đầu tiên khi cắt log; khi đạt giới hạn phải dừng có ghi chú.

| Tác vụ | Mục tiêu | Điều kiện riêng | Giới hạn đề xuất | Log và dữ liệu tái hiện | Tiêu chí kết quả | Review sau chạy |
| --- | --- | --- | --- | --- | --- | --- |
| Regression số học PID | Phát hiện thay đổi output và state so với contract | Test đã review, fixture độc lập, tolerance và miền input rõ | 60 giây/phiên, một worker; log theo giới hạn chung | `pid-regression/`: lệnh, kết quả assertion, input gây FAIL | PASS khi tất cả test được chọn thực chạy và đạt; timeout/crash/FAIL được ghi riêng | Expected, tolerance, state ban đầu và mọi khác biệt số thực |
| iBUS stream replay | Kiểm tra parser/checksum/decode với stream tái lập | Corpus có nguồn, buffer đúng contract, parser state được kiểm soát | 120 giây/phiên, tối đa 100 MiB input | `ibus-replay/`: stream, thứ tự feed byte, output/flags | Assertion theo contract; dữ liệu decode sai hoặc mất đồng bộ ngoài yêu cầu là bất thường cần phân tích | Tính đại diện corpus, contract resync, frame bị bỏ theo policy |
| iBUS fuzzing | Tìm crash, truy cập sai hoặc hành vi vi phạm invariant | Harness/corpus cơ sở đã ổn định; seed và invariant được review; input độ dài ngoài contract tách riêng | 5 phút/phiên, một worker, tối đa 100 MiB corpus lưu | `ibus-fuzz/`: seed, input lỗi tối thiểu nếu có, diagnostic và lệnh tái hiện | Phát hiện crash, timeout hoặc invariant FAIL; không phát hiện bất thường không đồng nghĩa parser không còn lỗi | Tái hiện, giảm input, phân biệt vi phạm contract với lỗi production |
| Motor clamp/boundary testing | Kiểm tra miền giá trị và sự độc lập các kênh | Contract clamp/mapping, struct/register giả đã review | 60 giây/phiên, miền quét được định danh | `motor-boundary/`: miền quét, vector lỗi, assertion | Kết quả khớp contract cho các giá trị đã chạy | Giá trị trước/sau ép kiểu, biên clamp và mapping phần mềm |
| MPU sensor-data replay | Kiểm tra conversion, bias và state filter trên chuỗi | Scale/bias/state đầu được xác định; đơn vị, trace và tolerance rõ | 120 giây/phiên, tối đa 1 triệu mẫu và 100 MiB input; dừng khi chạm giới hạn đầu tiên | `mpu-replay/`: trace hoặc generator/seed, state đầu, diagnostic | Assertion/invariant đã chốt; NaN/Inf chỉ là FAIL khi trái contract | Sai số tích lũy, nguồn trace, khởi tạo và giới hạn của mô hình |
| Battery hysteresis sequences | Kiểm tra chuyển trạng thái và vùng giữ | Static state được thiết lập qua quy trình có kiểm soát | 60 giây/phiên, một worker | `battery-sequence/`: chuỗi điện áp/raw, state và output | Chuyển trạng thái theo contract, không phụ thuộc thứ tự test ngoài thiết kế | Ngưỡng so sánh, calibration factor và tính độc lập fixture |
| Static analysis | Phát hiện diagnostic đáng xem xét trên mã trong scope | Tool/phiên bản và compile configuration phù hợp đã được lựa chọn | 10 phút/phiên, một worker; không tự sửa mã | `static-analysis/`: cấu hình, diagnostic đầy đủ và fingerprint | Chưa có diagnostic mới chưa review trong phạm vi đã chốt; lỗi công cụ là BLOCKED, không gán lỗi firmware | False positive, macro/kiểu target, mức độ và khả năng tái hiện |
| Coverage và kiểm tra lặp lại | Xác định phần chưa được suite thực thi và phát hiện state leak | Suite đã chạy ổn định; compiler/tool hỗ trợ, cấu hình coverage đã review | 5 phút/phiên; lặp suite tối đa 100 lần khi được bật; log theo giới hạn chung | `coverage-repeat/`: báo cáo, số vòng thật, thứ tự test và kết quả | Regression vẫn đạt; phần chưa cover được báo cáo, không tự đặt ngưỡng 100% | Nhánh chưa cover, code bị loại khi link, state ẩn và ý nghĩa thực tế coverage |

### 8.3. Quy tắc kết luận và quyền hạn automation

- Chưa tác vụ nào trong bảng được thực hiện chỉ vì được liệt kê trong kế hoạch.
- Test có expected rõ mới dùng PASS/FAIL theo assertion. Fuzzing không thấy lỗi chỉ được kết luận không phát hiện bất thường trong budget/corpus đã chạy.
- Lỗi công cụ, thiếu dependency hoặc timeout chưa rõ nguyên nhân không tự trở thành production bug; giữ BLOCKED hoặc kết quả cần phân tích phù hợp.
- Không dùng thời gian chạy trên laptop làm WCET, jitter hoặc CPU margin của STM32.
- Automation không tự sửa firmware, thay expected, commit, push hoặc merge; chỉ thu bằng chứng trong phạm vi được cho phép.

## 9. Ranh giới bằng chứng

- Unit Test trên Windows xác nhận logic của mã đã compile trong điều kiện input/state/harness xác định.
- Không thay thế kiểm thử clock/timing STM32, interrupt concurrency thực, I2C vật lý, MPU6050, ADC DMA thật, PWM/ESC, motor hoặc flight test.
- Mock HAL, PRIMASK hoặc thanh ghi giả không chứng minh peripheral thật hoạt động đúng; thứ tự callback nhân tạo không phải phép đo preemption.
- Coverage cao không chứng minh không còn lỗi, không chứng minh mọi miền input hoặc mọi lịch ngắt đã được kiểm tra.
- Test đã chạy chỉ chứng minh các assertion đã nêu; thay source, compiler, flags, contract hoặc harness có thể cần chạy lại.
- Kết quả host-test chỉ được dùng làm bằng chứng cho audit khi câu hỏi audit nằm trong phạm vi đã kiểm chứng. Không tự mở lại mục audit đã chốt nếu không có bằng chứng mới mâu thuẫn; cũng không tự đóng mục còn cần hardware.
- Khi source hiện tại khác hành vi mong muốn, phải review contract thay vì mặc định source đúng hoặc sửa expected cho khớp output.

## 10. Tiến độ hiện tại và thứ tự triển khai

### 10.1. Trạng thái tại ngày lập kế hoạch

| Nội dung | Trạng thái thực tế | Căn cứ và giới hạn |
| --- | --- | --- |
| MSYS2 UCRT64, GCC/G++ và CppUTest cơ bản | Người phát triển cung cấp kết quả 3/3 test ví dụ PASS | Chưa đối chiếu log gốc, phiên bản và đường dẫn công cụ trong repository; không phải test firmware drone |
| Source inventory toàn firmware | Đã khảo sát source | Tổng hợp tại mục 3; kế hoạch còn DRAFT, cần review phạm vi trước từng nhóm |
| `UT-PID-001` | `DESIGNED / NOT RUN`; đặc tả đã review | Có điều kiện đầu, input, expected và tolerance trong `UNIT_TEST_CASES.md`; chưa có bằng chứng thực thi |
| Host-build `pid.c` | Chưa thực hiện | Chưa xác nhận compile/link production module trong harness CppUTest |
| Unit Test chính thức trên firmware drone | Chưa chạy | Không có kết quả PASS/FAIL firmware để báo cáo |
| Harness, automation, fuzzing, stress và coverage firmware | Chưa triển khai/chưa chạy trong workflow này | Các mục 4 và 8 là kế hoạch, không phải bằng chứng hoàn thành |
| Sử dụng Unit Test để đóng vấn đề audit | Chưa có kết quả đủ điều kiện | Không dùng 3 test ví dụ hoặc việc tạo tài liệu để đóng finding firmware |

Riêng `UT-PID-001` kiểm tra một lần gọi cascade P-only; expected là `outer.PID_OUT=6.0`, `inner.PID_OUT=6.0`, `outer.error_sum=0.006`, `inner.error_sum=0.012`, sai số tuyệt đối `0.00001`. Nội dung đầy đủ và phép tính nằm trong hồ sơ ca kiểm thử; không nhân bản thành một đặc tả cạnh tranh trong kế hoạch.

### 10.2. Thứ tự triển khai sơ bộ

1. PID: hoàn thiện contract/harness và dùng `UT-PID-001` làm workflow mẫu.
2. iBUS: checksum, decode, parser và các helper theo khả năng cô lập.
3. Motor: clamp và interface register giả; giữ xác nhận PWM/ESC cho hardware.
4. MPU math/filter: conversion, bias và replay trước khi mở rộng driver/mock.
5. Safety, Mixer, Watchdog và các nhóm khác theo khả năng cô lập, mức rủi ro và bằng chứng mới; Battery có thể làm sớm như module logic nhỏ nếu phù hợp tiến độ.

Thứ tự có thể điều chỉnh sau review. Tài liệu này không cho phép tự chuyển bước, thiết lập môi trường hoặc chạy công cụ ngay khi được tạo.

### 10.3. Thông tin còn phải xác minh

- Log, phiên bản GCC/G++, CppUTest, kiến trúc native và lệnh của môi trường ví dụ 3/3 PASS.
- Cách cô lập header STM32, cấu hình C/C++ linkage, assert, thư viện toán và `M_PI` trên host mà không thay logic production.
- Khả năng init/reset state static, liên kết nguyên translation unit và test nhóm nằm trong `main.c`.
- Contract cho input ngoài miền, buffer length, giá trị không hữu hạn, reset, wrap và các side effect quan trọng; chưa tự coi mọi khả năng đó là yêu cầu đã được duyệt.
- Chọn công cụ static analysis/coverage/fuzzing, budget và chính sách lưu artifact khi bắt đầu các bước tương ứng.
- Kết quả build/run thực tế của từng test firmware; hardware validation vẫn là công việc riêng.

## 11. Tiêu chí hoàn thành dự án kiểm thử

- Các ứng viên offline đã được khảo sát; phần loại trừ hoặc không khả thi có lý do.
- Các test có giá trị và khả thi trong phạm vi đã thống nhất được triển khai trên mã production thật, có contract và expected được review.
- Phân biệt rõ phần đã test, chưa test, BLOCKED và cần hardware validation.
- FAIL quan trọng có phân tích và kết luận; vấn đề chưa giải quyết được giữ mở với ảnh hưởng và hướng kiểm tra tiếp theo.
- Kết quả có thể tái thực thi từ mã, môi trường, lệnh và dữ liệu đã lưu; summary không thay thế lịch sử log.
- Giới hạn của mock, coverage, replay và host timing được ghi rõ.
- Người phát triển review kết quả cuối; không đặt mục tiêu giả tạo rằng 100% firmware hoặc drone đã được kiểm chứng chỉ vì toàn bộ test trên laptop PASS.

## 12. Kiểm soát thay đổi kế hoạch

- Khi phạm vi hoặc phương pháp thay đổi, cập nhật kế hoạch và ghi phiên bản, ngày, nội dung, lý do cùng mức review. Thay đổi ca kiểm thử, expected, kết quả và tiến độ được ghi trong hồ sơ tương ứng tại mục 6 và mục 7, không yêu cầu sửa kế hoạch cho mỗi phiên.
- Không ghi đè hoặc xóa lịch sử FAIL để làm đẹp tiến độ. Thay đổi quyết định được ghi thành cập nhật có căn cứ.
- Mọi sửa production, tạo test, build/run, commit/push hoặc PR là nhiệm vụ riêng cần quyền thực hiện rõ ràng.
- Sau lượt lập tài liệu này: dừng ở báo cáo file, cấu trúc, thông tin cần xác minh và Git status; không tự tiếp tục Bước 04 hoặc bước khác.

| Phiên bản | Ngày | Nội dung | Review |
| --- | --- | --- | --- |
| `0.1` | `22/10/2026` | Tạo kế hoạch toàn firmware từ source inventory, quy trình 8 bước và trạng thái hiện có; giữ nguyên `UT-PID-001` | `DRAFT` — chờ người phát triển review kế hoạch |
| `0.2` | `22/10/2026` | Chuẩn hóa trách nhiệm kỹ thuật và quy tắc duy trì hồ sơ; giữ nguyên phạm vi kỹ thuật, inventory và quy trình 8 bước | `DRAFT` — chờ review nội dung cập nhật |
