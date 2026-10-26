# Kế hoạch phục hồi và kết thúc dự án năm 2026

Cập nhật: 26/10/2026. Chủ dự án đã phê duyệt POLICY-01–06 và chốt CL-01 ở mức Source/Design Review only. Đây là nguồn điều hành cho dự án quadcopter STM32F411 phát triển firmware từ tháng 5/2026; bằng chứng chi tiết tiếp tục nằm trong hồ sơ audit và testing.

Mở [DAILY_WORK.md](DAILY_WORK.md) để bắt đầu phiên tiếp theo. Kế hoạch và ngày dự kiến không phải kết quả nghiệm thu.

## 1. Mục tiêu và chính sách đã phê duyệt

| Chính sách | Quyết định có hiệu lực |
| --- | --- |
| POLICY-01 | Ưu tiên chuyến bay thử đầu tiên có kiểm soát trước 30/11/2026, chỉ khi safety gates đạt. Chưa đủ điều kiện thì NO-GO; không chấp nhận rủi ro chưa kiểm soát để kịp ngày. |
| POLICY-02 | Đóng dự án chậm nhất 31/12/2026. Kết quả hợp lệ: flight-validated prototype, bench-validated prototype hoặc documented prototype với giới hạn/issue rõ. Không kéo dài sang 2027 để tuning, thêm tính năng, coverage hay refactor. |
| POLICY-03 | Giữ 8 nhóm/46 mục để truy vết; ưu tiên rủi ro mất kiểm soát, actuator, failsafe và nguồn. Không yêu cầu 46/46 PASS. MUST cho closure khác MUST cho flight attempt. |
| POLICY-04 | CppUTest được Codex tự động hóa tối đa; người phát triển review contract, expected, actual, kết quả và giới hạn. Không giới hạn cứng ở ba module, không đặt số test/coverage. Dự kiến dừng mở rộng ngày 06/12/2026. |
| POLICY-05 | Đang mua linh kiện; dự kiến đủ chậm nhất cuối tháng 10 là kế hoạch, chưa phải xác nhận. Khi hardware đủ điều kiện, ưu tiên bring-up và kiểm chứng thực tế hơn mở rộng host test ít quan trọng. |
| POLICY-06 | Tháng 12 ưu tiên regression, README, sơ đồ, bằng chứng, debug case studies, Known Issues, portfolio và CV Embedded Firmware. Mô tả đúng đóng góp cá nhân và phần AI/Codex hỗ trợ. |

MUST: cần cho mục tiêu sử dụng hoặc xử lý blocker quan trọng. SHOULD: có giá trị nếu còn thời gian. ACCEPT: chỉ khi có căn cứ và phạm vi chấp nhận cụ thể. DEFER: không làm trước deadline. Các nhãn này độc lập với trạng thái thực thi.

## 2. Baseline và mức bằng chứng

Đối chiếu Git cục bộ và GitHub ngày 24/10/2026:

| Thành phần | Baseline / kết luận được bảo toàn |
| --- | --- |
| Firmware chính | `develop@6562a4e4f02454da08f97ad83a4da8dae3ba262f`. |
| Nhánh Unit Test | Snapshot cục bộ `test/cpputest@94f1a8028dd9a58ce51ee851348bf735bf80ba6f`; cùng cây `Core/` với develop trong khảo sát. |
| PID host test | `UT-PID-001`: PASS lịch sử ngày 23/10, 1 test, 4 assertions, exit code 0. Hồ sơ/harness được lưu ở `79c277266825fe03b985cae66c9bdda8cd6ab150`; ca P-only không xác nhận toàn PID. |
| B4 | UNRESOLVED: RUNNING có Mailbox/Snapshot + PRIMASK; chưa có target verification cho bản sửa. Commit `9280bae` ghi chưa xác nhận build/runtime; claim Build PASS ngày 19/10 chưa gắn chắc với revision đó. |
| A5 | UNKNOWN / NEEDS REVIEW: nhật ký ghi spike chưa giải quyết, hồ sơ bàn giao cũ ghi đo hậu pipeline khoảng 30 phút, peak khoảng 0,388 rad/s và PASS. Cần đối chiếu artifact và firmware thực chạy, không tự chọn PASS. |
| Known Issues | F01–F03 còn OPEN / UNRESOLVED; chưa có bằng chứng đóng. Dẫn register gốc tại mục 10, không lập register cạnh tranh trong tài liệu này. |
| Hardware | Chưa xác nhận linh kiện/PCB V2 đã nhận, đã bring-up hoặc đủ điều kiện cấp nguồn; thiết bị từng vào nước phải được đánh giá lại. |
| Flight readiness | Chưa có quyết định GO hoặc bằng chứng flight-ready. |

SOURCE FACT là nội dung đọc từ code/Git. RUNTIME EVIDENCE là kết quả thực thi có nguồn; phải nêu khi chỉ còn log trích hoặc bản ghi. DOCUMENTED CLAIM là kết luận tài liệu chưa xác minh độc lập. INFERENCE là suy luận/đề xuất. UNKNOWN là thiếu bằng chứng hoặc mâu thuẫn. Không dùng tài liệu kế hoạch để nâng trạng thái kỹ thuật.

[CURRENT_STATUS cũ](audit/CURRENT_STATUS.md) còn chỉ tới bước trước triển khai PRIMASK và trước cleanup instrumentation. Không dùng riêng file này làm trạng thái điều hành hiện tại. DWT/counter B3/B4.3A đã cleanup tại `6562a4e`/`94f1a80`; phép đo mới cần instrumentation được review.

### 2.1. Chốt CL-01 ngày 26/10/2026

**CL-01: DONE — Source/Design Review only.** Chủ dự án đã review và chốt bảng phân tích ngày 26/10. Đây là hoàn thành nhiệm vụ rà soát tài liệu/source theo thiết kế hiện hành, **không phải firmware safety PASS, hardware verification PASS hoặc quyết định GO**. Ngày 25/10 nghỉ vì việc cá nhân, không có kết quả CL-01 mới.

Firmware đối chiếu: `develop@eff243d734474ae0a37ce29ca1a97688822cc33c`, cùng cây `Core/` với baseline `6562a4e`. Các hành vi SOURCE FACT được giữ:

- Boot khởi tạo lock; ARM dùng SwA=2000, ga <=1010 và fresh frame; DISARM SwA=1000 ghi CCR=12500. ESC calibration bằng SwB là chức năng có chủ đích: CCR=25000 trong 7 giây, rồi 12500 trong 8 giây; không qua ARM lock không tự là bug.
- Khi đang unlocked và không bị nhánh ưu tiên khác chặn, mất iBUS/failsafe ghi minimum=13000, không phải ramp-down; mất MPU theo watchdog raw Z dẫn tới lock=12500 khi main xử lý. Pin yếu chỉ cảnh báo còi, không RTL. Watchdog TIM11 đặt cờ; output bảo vệ phụ thuộc main còn thực thi.
- Tham chiếu: [main.c](../Core/Src/main.c), [motor.c](../Core/Src/motor.c), [battery.c](../Core/Src/battery.c). Hành vi ESC/FC thực, waveform, độ trễ fault/phục hồi và output khi CPU/main dừng còn UNRESOLVED. Giữ A5 UNKNOWN / NEEDS REVIEW, B4 target verification UNRESOLVED, F01–F03 OPEN / UNRESOLVED và trạng thái safety gates tại mục 4.
- NO-GO có motor nếu hardware/nguồn chưa đủ điều kiện, chưa tháo cánh, chưa kiểm soát mapping/output/calibration hoặc chưa có cách ngắt nguồn phù hợp. Không giả định lock/minimum đồng nghĩa motor đã dừng. Flight chưa GO khi thiếu bằng chứng gate liên quan; không tiếp tục hoặc tăng ga khi output bất thường hay mất khả năng dừng phép thử.

Không mở lại thiết kế ARM/DISARM, failsafe hoặc calibration trong CL-01; phần cần hardware được giữ mở cho các nhiệm vụ hiện có, không tạo backlog mới.

## 3. Milestone

| Mốc | Thời gian năm 2026 | Đầu ra dự kiến / checkpoint |
| --- | --- | --- |
| G0 | 24/10–31/10 | Safety contract và NO-GO; inventory linh kiện/PCB/revision; checklist bring-up; tiếp tục host test bằng Codex. |
| G1 | 01/11–07/11 | Kiểm tra nguồn/PCB, boot/clock/timer, MPU/I2C, iBUS/UART, PWM; ghi evidence hoặc blocker. Chưa đủ điều kiện thì không cấp nguồn motor. |
| G2 | 08/11–14/11 | Dấu IMU/PID, timing/sampling/stale, RC/watchdog, ARM/DISARM/failsafe và control output/mixer. |
| G3 | 15/11–21/11 | Mapping, clamp/saturation/lock, motor-on behavior và retest F01–F03 có trọng tâm; tháo cánh, có cách ngắt nguồn phù hợp. |
| G4 | 22/11–30/11 | Review safety gates; người phát triển quyết định GO/NO-GO. Chỉ bay ngắn có kiểm soát nếu đạt điều kiện; ghi kết quả/giới hạn. Mục tiêu bay trước 30/11 không tự được gia hạn hoặc coi là đã đạt. |
| Closure 1 | 01/12–06/12 | Chốt phạm vi firmware/test, regression liên quan; dừng mở rộng CppUTest dự kiến 06/12. |
| Closure 2 | 07/12–13/12 | Bằng chứng và tài liệu hệ thống. |
| Closure 3 | 14/12–20/12 | README, portfolio và CV. |
| Closure 4 | 21/12–27/12 | Review cuối, đánh giá Known Issues và mức kết thúc có căn cứ. |
| Closure 5 | 28/12–31/12 | Dự phòng và đóng dự án, không mở tính năng mới. |

Hardware trễ: ghi blocker và làm task offline READY. Khi hardware đủ điều kiện thì ưu tiên việc flight-critical. Không nén/bỏ safety gates để bù tiến độ. Sau mục tiêu tháng 11, cập nhật quyết định theo bằng chứng; không tự kéo dài dự án. Mốc 22/11 là bắt đầu review flight readiness, thay thế đề xuất cũ tự chuyển sang documented prototype vào ngày này.

## 4. Safety gates và quyết định GO hoặc NO-GO

Trạng thái các gate hiện tại: CHƯA XÁC MINH ĐỦ. Checklist dưới đây là yêu cầu cho phép thử tương ứng, không phải kết quả PASS.

| Gate | Bằng chứng bắt buộc phù hợp với phép thử | Điều kiện NO-GO |
| --- | --- | --- |
| SG-01: hardware/nguồn | Định danh board/MCU/nguồn/linh kiện; kiểm tra thiết bị sau vào nước, rail/ground và dấu hiệu reset dưới điều kiện thử; firmware thực nạp khớp ELF. | Nguồn, dây, PCB, TX/RX/IMU/ESC liên quan chưa đủ điều kiện sử dụng. Khởi động được không chứng minh an toàn. |
| SG-02: sensor/input | Dấu, đơn vị, acquisition, freshness/stale, mapping RC; phản ứng khi mất input hoặc sensor có kết quả target phù hợp. A5 phải được đánh giá đủ để loại trừ rủi ro flight-critical. | Input/sensor sai hoặc stale không được phát hiện/xử lý; spike quan trọng chưa giải thích hay kiểm soát. |
| SG-03: timing/concurrency | B4 handover target; execution time/deadline, latency và sample age dưới tải đại diện, tiêu chí đo được chốt trước. B1–B3 lịch sử không thay thế các phép này. | Miss deadline hoặc sample age/ownership gây rủi ro điều khiển chưa kiểm soát. |
| SG-04: output/control | Mapping logic → CCR/pin → motor, chiều tác động mixer/PID, waveform PWM, clamp/saturation/lock và đáp ứng motor thực. | Mapping/dấu/output bất thường, F01/F03 hoặc vấn đề actuator chưa được giải thích và kiểm soát bằng bằng chứng phù hợp. |
| SG-05: safety/fault | Boot, ARM/DISARM, mất RC/MPU và các fault kết hợp có ý nghĩa; đo/quan sát output bảo vệ và khả năng ngắt nguồn. | Hành vi fault sai/không rõ hoặc không dừng được phép thử. Host/mock PASS không thay thế target. |
| SG-06: tích hợp và điều kiện bay | Motor-on sensor/frame behavior, đánh giá F02, đáp ứng động phù hợp; cấu hình/gain cuối có căn cứ, người phát triển kiểm tra địa điểm và phương án thử có kiểm soát. | Rủi ro mất kiểm soát còn mở, thiếu bằng chứng trọng yếu hoặc điều kiện thử không phù hợp. |
| SG-07: review cuối | Người phát triển review evidence trên đúng revision/cấu hình, ghi quyết định GO/NO-GO và giới hạn phép bay. | Thiếu quyết định hoặc bất kỳ gate flight-critical nào chưa đáp ứng. |

Bench logic/PWM: hardware đủ điều kiện cấp nguồn và nguồn motor được cô lập theo thiết lập. Bench có motor: thêm kiểm tra nguồn/ESC/mapping/output bảo vệ, tháo cánh, phương pháp giữ/đỡ phù hợp và ngắt nguồn. Flight: phải qua các gate liên quan; không yêu cầu mọi mục audit đều PASS nhưng không bỏ gate có rủi ro thực tế.

Source hiện tại dùng minimum khi mất iBUS/failsafe, còi khi pin yếu, lock khi mất MPU. CL-01 đã review các SOURCE FACT này tại mục 2.1; kết quả không chứng minh policy đó đủ an toàn cho bay. Không có căn cứ tuyên bố RTL hoặc hạ ga theo một quá trình được kiểm chứng.

## 5. Master Backlog

Đây là backlog điều hành duy nhất. Không tạo backlog song song hoặc task từ mọi code smell. READY = có thể bắt đầu; IN PROGRESS = thực sự đang làm; BLOCKED = thiếu điều kiện; DEFERRED = chủ động hoãn; UNKNOWN = chưa đủ thông tin; DONE = có bằng chứng hoàn thành. Khi chốt nhóm còn blocker phải ghi blocker, không đổi thành PASS.

| ID / workstream / công việc | Closure / flight attempt | Ưu tiên / trạng thái | Tiên quyết / milestone | Definition of Done và evidence/output | Nguồn kỹ thuật |
| --- | --- | --- | --- | --- | --- |
| CL-01 / AUDIT / Safety contract và phạm vi sử dụng | MUST / MUST | P0 / DONE — Source/Design Review only | Chủ dự án chốt ngày 26/10/2026; G0 | Bảng source/design, giới hạn và NO-GO đã được review; hoàn thành rà soát, không phải firmware safety PASS hoặc hardware verification PASS. Không đóng issue/gate và không thiết kế lại policy. | Mục 2.1, 4 và nguồn audit tại mục 10; `Core/Src/main.c`. |
| CL-02 / HARDWARE / Inventory và phương án kiểm chứng | MUST xác định blocker / MUST trước cấp nguồn | P0 / READY | Thiết bị/thông tin thực khi có; G0–G1 | Inventory từng thiết bị, tình trạng xác nhận/UNKNOWN, PCB/MCU/pinout/nguồn/debug và thứ còn chờ; chọn cách kiểm chứng theo tình trạng thực. | `iBUS_MPU_ESC_Mix.ioc`, hồ sơ audit; không giả định PCB V2 đạt. |
| CL-03 / UNIT_TEST / PID CppUTest | MUST cho phần chọn có giá trị / SHOULD; FAIL liên quan flight phải xử lý | P1 / READY | Harness PID; G0 và thời gian offline tới 06/12 | Contract/oracle độc lập; test P/I/D/state/yaw theo giá trị/chi phí; log/revision/phạm vi có review. Không tuyên bố toàn PID PASS từ một ca. | CASES/LOG/PLAN tại mục 10; `79c2772`, `Core/Src/pid.c`. |
| CL-04 / UNIT_TEST / iBUS CppUTest | MUST cho logic input chọn lọc / SHOULD; target input vẫn MUST | P0 / READY | Contract frame/state/failsafe được làm rõ; G0–G2, offline tới 06/12 | Test production checksum/decode/parser/stream và phần ownership logic cô lập được; giữ input lỗi, log, seed nếu có; đánh dấu NEEDS REVIEW khi thiếu oracle. | PLAN, `Core/Src/iBUS.c`; `9280bae`; không thay B4 target. |
| CL-05 / UNIT_TEST / Motor và logic output | MUST cho phần chọn lọc / SHOULD; output thật MUST | P0 / READY | Timer model/contract; G0–G3, offline tới 06/12 | Clamp/kênh/CCR/lock/minimum; mixer production nếu cô lập hợp lý; fixture/expected/log rõ, giới hạn mock rõ. | PLAN; `Core/Src/motor.c`, `main.c`, `tim.c`. |
| CL-06 / UNIT_TEST / Runner, mở rộng hợp lý và evidence | MUST tái lập kết quả / SHOULD; không thay hardware gates | P1 / READY | Harness hiện có; scope MPU/Battery/Safety theo chi phí; tới 06/12 | Runner/exit code/timeout/log; chạy production thật, regression và báo cáo nhóm; MPU math/Battery/Safety được test hoặc có lý do BLOCKED/DEFERRED/NEEDS REVIEW. Không đặt số test/coverage. | PLAN/LOG/script tại mục 10; `mpu6050.c`, `battery.c`, `main.c`. |
| CL-07 / HARDWARE / Bring-up, bench logic/PWM/safety | MUST xác định kết quả hoặc blocker / MUST thực hiện đủ trước flight | P0 / BLOCKED | Hardware đủ điều kiện, CL-01/02; G1–G2 | Checkpoint nguồn/SWD/boot rồi clock/acquisition/RC/timing/PWM/fault; revision, điều kiện, phép đo và lỗi còn lại có review. B4 target và freshness được đánh giá; không gộp build thành runtime. | Safety gates, audit A/B/C/G, source/cấu hình baseline. |
| CL-08 / BUG / Motor integration và Known Issues | MUST giữ kết luận trung thực / MUST xử lý rủi ro flight-critical | P0 / BLOCKED | CL-07, nguồn/ESC/motor, mapping và stop method; G3–G4 | Motor-on/mapping/clamp/saturation, retest trọng tâm có trace. Bổ sung register gốc khi có evidence; không đóng F01–F03 từ một lượt không tái hiện; đánh giá GO/NO-GO. | Register F01–F03 tại mục 10; audit D/E/F/G. |
| CL-09 / DOCUMENTATION / README kỹ thuật | MUST / SHOULD cho trình bày; thông tin vận hành an toàn vẫn cần | P1 / READY | Có thể khởi thảo từ baseline, chốt sau kết quả; Closure | Kiến trúc/cấu hình/build/test, mức kiểm chứng, giới hạn và issue được link; không claim RTL/flight-ready thiếu evidence. | `README.c`, baseline, hồ sơ audit/testing. |
| CL-10 / DOCUMENTATION / Evidence package và portfolio | MUST / SHOULD; bằng chứng gate vẫn MUST | P1 / READY | Thu thập ngay, chốt sau baseline cuối; Closure | Sơ đồ, log/capture/revision, debug case studies và CV đúng đóng góp/AI hỗ trợ; không nhân bản register hay test cases. | Hồ sơ audit/testing; commit `4d3b6df`, `34ec229`, `9280bae`, `79c2772`. |
| CL-11 / DOCUMENTATION / Final review và closure | MUST / MUST review riêng nếu xin GO | P1 / BLOCKED | Các MUST closure có kết quả hoặc blocker/giới hạn được review; Closure | Chốt revision, mức kết thúc có evidence, Known Issues/giới hạn, README/portfolio và quyết định của chủ dự án trước 31/12; không yêu cầu 46/46 PASS. | Toàn bộ nguồn dẫn tại mục 10 và kết quả phiên thực tế. |

Cập nhật ngày 26/10: READY 7; IN PROGRESS 0; BLOCKED 3; DEFERRED 0; DONE 1; UNKNOWN 0 ở cấp CL. CL-01 DONE chỉ cho Source/Design Review; không đóng G0, CL-11, Known Issues hoặc safety gates. Khi FC chưa sẵn sàng, ưu tiên CL-03–06; khi hardware đủ điều kiện, ưu tiên bring-up và kiểm chứng thực tế theo POLICY-05.

## 6. Chính sách CppUTest tự động hóa

CL-03–06 là workstream Host-Based Firmware Verification bằng CppUTest, kiểm chứng về mặt lý thuyết rằng production firmware hoạt động đúng thiết kế hiện hành: từng module và sự phối hợp giữa các module khi có thể cô lập hợp lý. CL-01 là Source/Design Review trong systematic pre-flight audit; CppUTest không xây dựng, phán xét, thay đổi hoặc quyết định chính sách an toàn. Không tự đặt expected theo policy mới. Nếu chưa xác định được expected từ thiết kế hiện hành, ghi **NEEDS REVIEW**, không tự thiết kế lại safety để tạo oracle.

Ưu tiên theo rủi ro/giá trị: PID, iBUS, motor logic; tiếp theo MPU math/conversion, Battery và Safety/Watchdog/control state có thể cô lập hợp lý. Không giới hạn số ca; không test lại toàn HAL/CMSIS. Nếu cần sửa production để cô lập, dừng tại đề xuất và xin phê duyệt thay đổi đó.

| Codex chịu trách nhiệm chính | Người phát triển review |
| --- | --- |
| Chủ động khảo sát có mục tiêu, lập contract/case và expected độc lập theo thiết kế hiện hành; xây dựng harness/stub/mock; compile/link và chạy production code thật; regression, phân loại FAIL, lưu log/revision và báo cáo PASS/FAIL theo từng nhóm. Khi thiếu oracle thì NEEDS REVIEW; cần sửa production thì dừng xin phê duyệt. | Chức năng, input/preconditions, expected và căn cứ, actual, kết quả và giới hạn. Không bắt buộc tự viết test thường quy hoặc học sâu API/harness. |

Quy trình: contract/oracle → harness → build/run → phân tích sai khác → regression → review nhóm. Không dùng output production làm expected, không thay expected/production để ép PASS, không gọi test mới thiết kế là PASS. FAIL giữ log gốc; sửa yêu cầu/expected phải có căn cứ và review. Host PASS không xác nhận timing, preemption, RF, bus vật lý, motor hoặc readiness bay. Không có oracle đáng tin thì NEEDS REVIEW.

Mỗi báo cáo nhóm gồm metadata (source/test/harness/diff, toolchain, lệnh, thời điểm, exit code, log) và bảng:

| Test ID / hàm | Mục đích | Input / preconditions | Expected / căn cứ / tolerance | Actual | Kết quả | Ý nghĩa / giới hạn |
| --- | --- | --- | --- | --- | --- | --- |
| Chưa có ca mới trong lần thiết lập | Chỉ là mẫu báo cáo | Chưa thực thi | Chưa xác định | Không có | NOT RUN | Không tạo bằng chứng runtime mới |

Ngân sách khởi đầu từ kế hoạch: Codex 22–39 giờ; người phát triển review khoảng 3–6,5 giờ; cộng 25–30% dự phòng, khoảng 31–59 giờ khối lượng tổng. Đây là ước lượng chưa đo, không phải cam kết automation miễn phí. Theo dõi riêng giờ xử lý Codex, giờ review và thời gian lịch/blocker. Hardware flight-critical được ưu tiên khi sẵn sàng.

Automation ban đầu: một worker; regression nhỏ timeout 60–120 giây; log tối đa 100 MiB/tác vụ. Fuzz/replay/coverage chỉ mở khi oracle/fixture ổn định và lợi ích rõ: fuzz tối đa 5 phút, replay tối đa 120 giây/1 triệu mẫu/100 MiB, static analysis có chọn lọc tối đa 10 phút; đều giữ input/seed, timeout và bước review. Không lập lịch hoặc bật automation chỉ từ tài liệu này.

Dừng mở rộng khi chi phí mock tăng nhưng chủ yếu test mô hình, thiếu oracle, cần thay production ngoài phạm vi hoặc lấn thời gian portfolio. Nếu tốn thêm một phiên mà chưa tiến triển, review chi phí trước khi tiếp tục. Đóng workstream khi phần chọn có căn cứ, kết quả tái lập, FAIL được phân tích, phần bỏ có lý do; không yêu cầu toàn module PASS. Sau 06/12 chỉ regression/sửa có quyết định rõ, không kéo dài sang 2027.

## 7. Chấp nhận giới hạn và phạm vi hoãn

Không tạo chấp nhận rủi ro mới chỉ bằng kế hoạch này. Giới hạn lịch sử A3/A6 như buffer MPU đơn, cờ nhị phân, fixed dt, yaw drift và calibration chưa kiểm tra status từng read được giữ đúng phạm vi từng được chấp nhận; không cấp quyền flight từ các giới hạn đó. Flight-critical finding phải có evidence xử lý/kiểm soát trước GO.

ACCEPT mới phải có phạm vi sử dụng, evidence, lý do, hệ quả, điều kiện xem xét lại và người phát triển phê duyệt. Thiếu căn cứ thì UNKNOWN/UNRESOLVED hoặc BLOCKED. Không ACCEPT throttle/motor/failsafe/nguồn chưa kiểm soát để đạt ngày bay.

DEFER: tối ưu HAL/NVIC không có số đo tác động, redesign kiến trúc/RTOS/estimator mới, tính năng mới, coverage mục tiêu, framework test tổng quát và recovery sâu có chi phí cao nhưng giá trị thấp. Fuzz/coverage không bị cấm nhưng không bắt buộc. Không biến tất cả module ngoài PID/iBUS/Motor thành DEFER cố định.

Không đạt gate bay thì NO-GO; vẫn có thể đóng documented hoặc bench-validated prototype đúng phạm vi đã chứng minh.

## 8. Phụ thuộc hardware và thông tin còn thiếu

- Linh kiện dự kiến cuối tháng 10: chưa xác nhận đã có.
- PCB V2: thiếu schematic/BOM/pinout/revision trong repo; chưa biết đã nhận hoặc bring-up.
- Source/cấu hình dùng STM32F411CEUx/CEU6; ngữ cảnh cũ có VET6: phải xác nhận part thực.
- PB3 hiện cấu hình SWO; yêu cầu GPIO debug cho V2 chưa phải thay đổi đã áp dụng.
- Nguồn 4S cũ đã bị loại theo hồ sơ; nguồn thay thế chưa xác nhận.
- TX/RX/antenna, MPU INT, board/ESC từng liên quan vào nước cần đánh giá thực tế, không suy từ lên nguồn.
- A5, phương pháp đo B3 (DWT/Analyzer), số đếm B4 khác nguồn, build revision và phạm vi error callback/recovery còn cần đối chiếu có mục tiêu.

CL-02 ghi inventory thực; CL-07/08 chỉ mở khi điều kiện tương ứng đạt. Không mua/đổi linh kiện tự động từ tài liệu này.

## 9. Điều kiện kết thúc dự án

Chủ dự án review và chốt trước 31/12/2026: revision/cấu hình cuối, mức kiểm chứng có bằng chứng, Known Issues/giới hạn/blocker, cách tái lập build/test đã xác minh và hồ sơ portfolio trung thực. Flight-validated chỉ khi có phép bay thực được ghi nhận; một chuyến bay ngắn không chứng minh mọi điều kiện vận hành. Bench-validated phải nêu rõ đã thử logic/PWM hay motor; documented prototype không tuyên bố hardware đã đạt.

Không đóng issue chỉ vì hết hạn. Giữ lịch sử FAIL; không bắt buộc toàn bộ task kỹ thuật hay audit phải DONE để đóng dự án với phạm vi hạn chế được review.

## 10. Liên kết bằng chứng

Nhánh tài liệu xuất phát từ develop, hiện chỉ có CURRENT_STATUS trong docs/audit. Hồ sơ mới trên nhánh test được dẫn bằng commit cố định để không tạo link tương đối hỏng hoặc merge test/cpputest.

- [CURRENT_STATUS cũ](audit/CURRENT_STATUS.md): nguồn kế thừa, không phải trạng thái điều hành mới.
- [Daily audit log](https://github.com/NemzNemz/Basic_Drone_Project/blob/94f1a8028dd9a58ce51ee851348bf735bf80ba6f/iBUS_MPU_ESC_Mix/docs/audit/DAILY_LOG.md) và [điểm cần review](https://github.com/NemzNemz/Basic_Drone_Project/blob/94f1a8028dd9a58ce51ee851348bf735bf80ba6f/iBUS_MPU_ESC_Mix/docs/audit/DAILY_LOG_REVIEW.md).
- [Known Issue Register gốc](https://github.com/NemzNemz/Basic_Drone_Project/blob/008467b31d2c6c60bc575a1b2c5f345dcfa6fa5b/iBUS_MPU_ESC_Mix/docs/audit/HANDOFF_2026-10-05.md#4-known-issue-register): F01–F03; đối chiếu bổ sung [CURRENT_STATUS](audit/CURRENT_STATUS.md#known-issue-register). Các snapshot có thể cũ; kết quả mới phải dẫn record thực, không âm thầm viết lại lịch sử.
- [UNIT_TEST_PLAN](https://github.com/NemzNemz/Basic_Drone_Project/blob/94f1a8028dd9a58ce51ee851348bf735bf80ba6f/iBUS_MPU_ESC_Mix/docs/testing/UNIT_TEST_PLAN.md), [UNIT_TEST_CASES](https://github.com/NemzNemz/Basic_Drone_Project/blob/94f1a8028dd9a58ce51ee851348bf735bf80ba6f/iBUS_MPU_ESC_Mix/docs/testing/UNIT_TEST_CASES.md), [UNIT_TEST_LOG](https://github.com/NemzNemz/Basic_Drone_Project/blob/94f1a8028dd9a58ce51ee851348bf735bf80ba6f/iBUS_MPU_ESC_Mix/docs/testing/UNIT_TEST_LOG.md).
- [Harness/script PID](https://github.com/NemzNemz/Basic_Drone_Project/blob/94f1a8028dd9a58ce51ee851348bf735bf80ba6f/iBUS_MPU_ESC_Mix/tests/pid/run_tests.sh); [commit mốc 79c2772](https://github.com/NemzNemz/Basic_Drone_Project/commit/79c277266825fe03b985cae66c9bdda8cd6ab150).
- [Nguồn firmware baseline](../Core/Src/main.c), [cấu hình](../iBUS_MPU_ESC_Mix.ioc), [README nền](../README.c).

Khi hồ sơ được tích hợp hợp lệ vào develop ở nhiệm vụ riêng, có thể đổi link phù hợp; lần thiết lập này không merge và không sửa hồ sơ gốc.

## 11. Quyết định và bảo trì

| Ngày | Quyết định / thay đổi |
| --- | --- |
| 24/10/2026 | Chủ dự án phê duyệt POLICY-01–06: mục tiêu flight có điều kiện trước 30/11, hard closure 31/12, risk-based audit, automation-first rộng có giá trị, hardware-first và ưu tiên portfolio tháng 12. |
| 24/10/2026 | Thiết lập backlog CL-01–11 và Daily Work. Thay thế đề xuất cũ coi flight chỉ là phần ngoài kế hoạch; không xác nhận task kỹ thuật/gate đã hoàn thành. |
| 26/10/2026 | Chủ dự án chốt CL-01 DONE — Source/Design Review only; không nâng trạng thái firmware/hardware/gates hoặc đóng issue. Làm rõ CppUTest chỉ kiểm chứng thiết kế hiện hành; ưu tiên CL-03–06 khi FC chưa sẵn sàng, hardware-first khi đủ điều kiện. Không mở thêm vòng thiết kế safety. |

PROJECT_CLOSURE_2026.md là nguồn task/policy/milestone chính, thay khi có tiến độ trạng thái hoặc quyết định liên quan; DAILY_WORK.md điều phối phiên và cập nhật sau tiến độ thực tế, không tạo backlog thứ hai. Audit/testing giữ bằng chứng chi tiết; không audit lại toàn repo mỗi sáng, không tạo file từng ngày và không nhân bản Known Issues.

Daily Engineering Brief 09:00 có thể đọc hai file nếu công cụ có quyền GitHub phù hợp. Chưa xác minh quyền đọc của automation hoặc đồng bộ tự động; lần thiết lập này không tạo/sửa lịch.

Thao tác Git của lần thiết lập chỉ được tạo hai tài liệu trên nhánh riêng, commit/push, không merge/PR. Mọi phiên triển khai sau phải theo phạm vi được giao; chính sách tự động hóa không tự cấp quyền sửa production, commit hoặc chạy hardware.
