# Ánh xạ Host-Based Firmware Verification vào Audit 8/46 — 2026-10-26

**Loại:** Bổ sung evidence, không phải nghiệm thu bay. **Trạng thái:** REVIEWABLE / không tự đổi PASS–FAIL–UNRESOLVED của checklist cũ. **Nguồn:** baseline `94f1a8028dd9a58ce51ee851348bf735bf80ba6f`, harness `7268b82ce5e4735617f79f02fe654410c3fb0554`, report `8fc4039`; branch `test/cpputest-firmware` chưa merge `develop`.

**Bằng chứng gốc:**
- [Firmware Verification Matrix](https://github.com/NemzNemz/Basic_Drone_Project/blob/test/cpputest-firmware/iBUS_MPU_ESC_Mix/docs/testing/UNIT_TEST_SUMMARY.md)
- [77 input / expected / actual cases](https://github.com/NemzNemz/Basic_Drone_Project/blob/test/cpputest-firmware/iBUS_MPU_ESC_Mix/docs/testing/FIRMWARE_TEST_CASES.md)
- [Final run (77 tests, 501 assertions, exit 0)](https://github.com/NemzNemz/Basic_Drone_Project/blob/test/cpputest-firmware/iBUS_MPU_ESC_Mix/docs/testing/evidence/2026-10-26/firmware-host/20261026T115934Z-1427/run.log)
- [Raw evidence folder](https://github.com/NemzNemz/Basic_Drone_Project/tree/test/cpputest-firmware/iBUS_MPU_ESC_Mix/docs/testing/evidence/2026-10-26/firmware-host)

## Kết quả định lượng, cách đếm

- **77/77 distinct test PASS; 501 assertions; 0 FAIL; 0 ignored; exit code 0.** `-r2` và host `-O2` cũng PASS, không làm tăng số ID duy nhất.
- **32/46 chỉ mục được ánh xạ tới bằng chứng host mới** (bao gồm các bằng chứng phụ trợ như F1/H3; `mapped` không đồng nghĩa `PASS`).
- **0/46 chỉ mục được tự động đóng PASS toàn phần.** Không có thay đổi trạng thái audit chính thức từ chiến dịch này.
- **6 chỉ mục là ứng viên sign-off phần logic thuần host:** C2, D1, D3, D4, D5, E2. Cần reviewer xác nhận tiêu chí từng mục; không tự động chuyển trạng thái.
- **30/46 chỉ mục có điều kiện cần FC/PCB/ESC/sensor hoặc timing thực tế trong kế hoạch nghiệm thu còn lại**, theo danh sách gate trong bảng. Đây là phân loại nhu cầu xác minh phần cứng tương lai, **không có nghĩa 30 mục cùng ở trạng thái UNRESOLVED**, và không mở lại những PASS target đã có trước đây.
- **14/46 chỉ mục không có evidence host trực tiếp mới:** A7, B1, B2, B3, B5, B6, D7, E1, E6, F2, F3, F4, F5, H1. Điều này không phủ nhận bằng chứng audit/target cũ.

## Ma trận bằng chứng

`CÓ` ở cột hardware chỉ nghĩa là một gate trên phần cứng vẫn có thể cần trong quá trình nghiệm thu; không ra lệnh thử phần cứng trong tuần 26/10–01/11.

| ID | Test / evidence mới | Điều được xác nhận, giới hạn | Hardware gate tương lai | Tác động trạng thái |
|---|---|---|---|---|
| A1 | UT-MPU-007; IT-001 | Quy ước dấu trong tính toán; kết quả target trước đây giữ nguyên | KHÔNG xác định gate mới | Không nâng trạng thái |
| A2 | UT-MPU-001–002 | Cấu hình register được gọi tới fake; không xác minh MPU thật | KHÔNG xác định gate mới | Không nâng trạng thái |
| A3 | UT-MPU-010–012 | Request/callback và retry BERR giả lập; không I2C vật lý | KHÔNG xác định gate mới | Không nâng trạng thái |
| A4 | UT-MPU-003–004 | Byte order, signed conversion, sensitivity; không sensor accuracy | KHÔNG xác định gate mới | Không nâng trạng thái |
| A5 | UT-MPU-005–006,013 | Bias/mean toán học; spike gyro thực tế chưa được giải quyết | CÓ | Không nâng trạng thái |
| A6 | UT-MPU-007–009; IT-001 | Complementary filter/state/sign trên input giả; không vibration | KHÔNG xác định gate mới | Không nâng trạng thái |
| A7 | — | Không có bằng chứng host trực tiếp từ chiến dịch 77 ca | CÓ | Không nâng trạng thái |
| B1 | — | Không có bằng chứng host trực tiếp từ chiến dịch 77 ca | KHÔNG xác định gate mới | Không nâng trạng thái |
| B2 | — | Không có bằng chứng host trực tiếp từ chiến dịch 77 ca | KHÔNG xác định gate mới | Không nâng trạng thái |
| B3 | — | Không có bằng chứng host trực tiếp từ chiến dịch 77 ca | KHÔNG xác định gate mới | Không nâng trạng thái |
| B4 | UT-IBUS-004,006; IT-007 | Mailbox/snapshot theo lịch host tuần tự; không PRIMASK/preemption thật | CÓ | Không nâng trạng thái |
| B5 | — | Không có bằng chứng host trực tiếp từ chiến dịch 77 ca | CÓ | Không nâng trạng thái |
| B6 | — | Không có bằng chứng host trực tiếp từ chiến dịch 77 ca | CÓ | Không nâng trạng thái |
| B7 | UT-APP-010–011; IT-011–012,014–015 | Freshness flag/counter logic; không đo latency hoặc sensor age | CÓ | Không nâng trạng thái |
| B8 | UT-APP-013; IT-010 | HAL_Delay/CCR request được quan sát bằng fake; không đo blocking thời gian thật | CÓ | Không nâng trạng thái |
| C1 | UT-IBUS-010; IT-002,011 | USART callback routing/rearm giả lập; không UART line/IRQ thật | CÓ | Không nâng trạng thái |
| C2 | UT-IBUS-001–002,004–006,011; IT-011 | Checksum, corruption, parser/resync, publish; frame contract32 | KHÔNG xác định gate mới | HOST VERIFIED (logic), chờ phê duyệt |
| C3 | UT-IBUS-003,007–008 | Decode kênh/flags theo vector; không TX/RX channel mapping vật lý | CÓ | Không nâng trạng thái |
| C4 | UT-PID-003,009; IT-003–005 | Scale RC sang angle/rate/PID; chưa đủ mọi deadband boundary | KHÔNG xác định gate mới | Không nâng trạng thái |
| C5 | UT-IBUS-003; UT-APP-006–007; IT-015 | Flags và loss response theo policy; không mất RF thật | CÓ | Không nâng trạng thái |
| D1 | UT-PID-001–003,014; IT-001 | Cascade outer/inner với source PID nguyên bản | KHÔNG xác định gate mới | HOST VERIFIED (logic), chờ phê duyệt |
| D2 | UT-MPU-007; IT-003–005 | Dấu toán học tại filter/PID/mixer; không xác minh hướng lực vật lý | CÓ | Không nâng trạng thái |
| D3 | UT-PID-001–003,009 | P-only, trung hòa, endpoints | KHÔNG xác định gate mới | HOST VERIFIED (logic), chờ phê duyệt |
| D4 | UT-PID-004–006,008,010 | I, clamp±500, reset; không suy ra anti-windup actuator | KHÔNG xác định gate mới | HOST VERIFIED (logic), chờ phê duyệt |
| D5 | UT-PID-007,011,013 | Derivative measurement, IIR, yaw PD | KHÔNG xác định gate mới | HOST VERIFIED (logic), chờ phê duyệt |
| D6 | UT-MOTOR-001–002; IT-006,016 | Đa trục và clamp số học/CCR; không motor saturation vật lý | CÓ | Không nâng trạng thái |
| D7 | — | Không có bằng chứng host trực tiếp từ chiến dịch 77 ca | CÓ | Không nâng trạng thái |
| E1 | — | Không có bằng chứng host trực tiếp từ chiến dịch 77 ca | CÓ | Không nâng trạng thái |
| E2 | IT-002–006,016 | Mixer trung hòa/từng trục/tổ hợp với oracle độc lập | KHÔNG xác định gate mới | HOST VERIFIED (logic), chờ phê duyệt |
| E3 | UT-MOTOR-003; IT-004 | Mapping field→CCR và swap logic; không mapping chân trên PCB | CÓ | Không nâng trạng thái |
| E4 | UT-MOTOR-001–005; IT-016 | CCR numeric bounds/lock/minimum; không waveform OneShot125 | CÓ | Không nâng trạng thái |
| E5 | UT-APP-013; IT-010 | Calibration request 25000/12500 và delay giả; không phản ứng ESC | CÓ | Không nâng trạng thái |
| E6 | — | Không có bằng chứng host trực tiếp từ chiến dịch 77 ca | CÓ | Không nâng trạng thái |
| F1 | UT-BAT-001–004; IT-008 | Bằng chứng phụ trợ: ADC→voltage, warning; không power tree/PDB | CÓ | Không nâng trạng thái |
| F2 | — | Không có bằng chứng host trực tiếp từ chiến dịch 77 ca | CÓ | Không nâng trạng thái |
| F3 | — | Không có bằng chứng host trực tiếp từ chiến dịch 77 ca | CÓ | Không nâng trạng thái |
| F4 | — | Không có bằng chứng host trực tiếp từ chiến dịch 77 ca | CÓ | Không nâng trạng thái |
| F5 | — | Không có bằng chứng host trực tiếp từ chiến dịch 77 ca | CÓ | Không nâng trạng thái |
| G1 | UT-IBUS-007,009; IT-009–010 | Pre-flight path với frame scripted; không bảo đảm gate an toàn vật lý | CÓ | Không nâng trạng thái |
| G2 | UT-APP-001–005,009; IT-002 | ARM/DISARM/fresh/lock theo policy hiện hành; không an toàn bay | CÓ | Không nâng trạng thái |
| G3 | UT-APP-006–007; IT-015 | RC loss→CCR minimum giữ unlocked; không xác nhận failsafe an toàn | CÓ | Không nâng trạng thái |
| G4 | UT-APP-008,011–012; IT-012–014 | MPU fault→motor lock giả lập; không bus loss vật lý | CÓ | Không nâng trạng thái |
| G5 | UT-APP-008–009; IT-014–015 | Một số fault/state tổ hợp; không exhaustive và không physical | CÓ | Không nâng trạng thái |
| H1 | — | Không có bằng chứng host trực tiếp từ chiến dịch 77 ca | CÓ | Không nâng trạng thái |
| H2 | UNIT_TEST_SUMMARY.md (inventory) | Đã phân loại 18 tệp C và build 7 tệp production; chưa full source audit | KHÔNG xác định gate mới | Không nâng trạng thái |
| H3 | UNIT_TEST_SUMMARY.md; FIRMWARE_TEST_CASES.md | Ma trận kiểm thử host mới; không phải final pre-flight acceptance | CÓ | Không nâng trạng thái |

## Quy tắc giữ nguyên trạng thái

- **A5:** có bằng chứng bias và calibration mean đúng input giả, **không xóa phát hiện spike gyro** ngày 29/09. Nhật ký Word/review có mâu thuẫn A5 UNRESOLVED so với câu tổng kết A1–A6 PASS ngày 02/10. Giữ `STATUS CONFLICT / RETEST PENDING`, không suy diễn lỗi đã hết.
- **A7.4:** chưa vibration/motor-load test; **UNRESOLVED**. Các PASS lịch sử A1–A6 (trừ điểm cần đối chiếu A5) không bị mở lại tự động.
- **B4:** snapshot/flag/handover hoạt động khi event host được cấp tuần tự; **PRIMASK, IRQ preemption, DMA tearing và latency vẫn UNRESOLVED**. Không suy diễn host `irq_restored=0` chứng minh PRIMASK thật.
- **F01–F03:** cả ba **OPEN / UNRESOLVED**. Suite không liên kết root cause, không chứng minh lỗi đã khắc phục.
- **G1–G5:** host characterization xác minh **hành vi safety policy hiện hành** đối với input đã chọn, không phê duyệt policy hay xác nhận hệ thống đủ an toàn để bay.
- **B5/B6/B7/E4:** thời gian trên host không phải DWT/jitter/deadline/waveform thực của STM32. **E3:** mapping CCR software không phải physical motor mapping.
- BERR branch được kiểm thử theo fake; **non-BERR recovery bằng 9 xung SCL chưa kiểm chứng**; input ngoài contract, null, NaN/Inf, multi-turn yaw và calibration read failure không nằm trong kết luận.
- Không đổi mã firmware, không tự tạo issue mới hay chiến dịch test diện rộng, không thêm acceptance policy.

## Hardware verification — ưu tiên khi FC trở lại (không triển khai trong 26/10–01/11)

1. **Safety phần cứng sau ngập nước:** kiểm tra/đo nguồn, GND, MPU, RX, ESC, tình trạng ăn mòn; không lắp cánh khi thử trên bàn và không dùng LiPo hỏng.
2. **B4 và RC:** xác nhận UART/iBUS IRQ, PRIMASK/mask duration, frame completeness, RC loss/freshness và cơ chế ARM/DISARM trên target.
3. **B5–B7:** đo DWT thời gian vòng điều khiển, deadline 2ms, jitter và sample age; sử dụng các debug GPIO/Logic Analyzer theo PCB thực tế.
4. **A7.4, E1–E6, D2/D7:** xác nhận trục sensor–mixer–motor, OneShot125/CCR physical và motor response an toàn không cánh trước các thử nghiệm động.
5. **G/F/H:** fault-injection có kiểm soát, nguồn dưới tải, failsafe và final pre-flight matrix sau khi các bước trước đạt.

**Không có công việc hardware phụ thuộc FC được đặt lịch trong tuần 2026-10-26–2026-11-01.** Chiến dịch host 77 ca hoàn tất, không yêu cầu mở rộng suite khi chưa có thiếu hụt bằng chứng cần thiết.
