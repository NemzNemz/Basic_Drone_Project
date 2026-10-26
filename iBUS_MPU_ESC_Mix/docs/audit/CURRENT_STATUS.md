# Trạng thái audit hiện tại

> **Cập nhật bằng chứng 26/10/2026:** Tệp này giữ nguyên bản chốt lịch sử 05/10 bên dưới. Trạng thái mới hơn và ánh xạ bằng chứng xem [HOST_VERIFICATION_2026-10-26.md](HOST_VERIFICATION_2026-10-26.md). Báo cáo host-test nằm trên branch `test/cpputest-firmware` (chưa merge `develop`). Không đánh dấu toàn bộ firmware an toàn/bay được.

## Bổ sung trạng thái đến 26/10/2026 (không ghi đè lịch sử)

- **Host verification:** `77/77 tests PASS`, `501 assertions`, `0 FAIL`, `0 ignored`, `exit code 0`; 7 production C files được host-build từ source gốc. `-r2` và `-O2` PASS theo [báo cáo](https://github.com/NemzNemz/Basic_Drone_Project/blob/test/cpputest-firmware/iBUS_MPU_ESC_Mix/docs/testing/UNIT_TEST_SUMMARY.md).
- **Ma trận 8/46:** 32 chỉ mục có evidence host mới, 14 chỉ mục không có evidence host trực tiếp mới. Có 6 ứng viên phê duyệt **logic-only** (C2, D1, D3, D4, D5, E2); **0 chỉ mục được tự động nâng PASS toàn phần**. Có 30 chỉ mục được đánh dấu có gate phần cứng trong tương lai (không đồng nghĩa 30 chỉ mục hiện UNRESOLVED). Xem ma trận liên kết từng test ID, kết quả và giới hạn.
- **B4 sau ngày 19/10:** theo bản bàn giao, main RUNNING đã bảo vệ mailbox→snapshot bằng PRIMASK, source review và firmware build PASS. Hai helper tiền kiểm tra giữ nguyên theo đánh giá rủi ro. **B4 UNRESOLVED** đến khi có runtime/IRQ/preemption thật. Host-test IT-007 chỉ kiểm tra bàn giao event tuần tự.
- **A5:** test bias/mean PASS trên host; phát hiện spike gyro thực tế ngày 29/09 chưa được retest thỏa đáng. Bản chốt A1–A6 PASS và nhật ký A5 UNRESOLVED còn **mâu thuẫn cần xác nhận**, không tự đóng A5.
- **A7.4:** vẫn UNRESOLVED; **F01–F03:** vẫn OPEN. Các điều kiện ARM/failsafe/calibration chỉ được characterization theo safety policy hiện hành; không xác nhận an toàn vật lý.
- **Thời điểm kiểm tra lại phần cứng:** FC không khả dụng trong tuần 26/10–01/11; không đặt công việc phụ thuộc FC trong khoảng này. Sau khi phần cứng trở lại, ưu tiên an toàn nguồn/sensor, B4/RC, B5–B7 timing, motor/ESC và final pre-flight theo ma trận.

---

Cập nhật theo bản chốt cuối ca **05/10/2026**, do người dùng chuyển từ “Debug drone setup 1”. Đây là kết luận kế thừa, chưa phải kiểm chứng lại source hoặc runtime trong chat tổng hợp.

## Nguồn và thứ tự thời gian

- `HANDOFF_2026-10-05.md`: hồ sơ nền, mốc kỹ thuật 03/10/2026; lập trước ca kỹ thuật 05/10.
- `SESSION_2026-10-05.md`: bản chốt nguyên văn cuối ca 05/10; thay thế mốc “B4 chưa bắt đầu” của hồ sơ nền.
- Giữ nguyên hồ sơ nền; trạng thái mới nhất lấy từ bản chốt cuối ca.

## Checklist

- A1–A6: PASS theo hồ sơ nền, giữ các giới hạn đã chấp nhận.
- A7.1–A7.3: đã xử lý theo hồ sơ nền; A7.4 **UNRESOLVED / HOÃN**, bắt buộc trước flight khi có nguồn phù hợp.
- B1–B3: PASS theo hồ sơ nền; giữ DWT B3 cho B5/B7.
- B4: **UNRESOLVED**, đang sửa quyền sở hữu buffer iBUS; chưa đóng.
- B5–B8 và C–H: chưa có kết quả audit được bàn giao trong hồ sơ hiện có.

## Thay đổi và bằng chứng ca 05/10

- B4.3A `pid_flag`: race đọc–xóa tồn tại từ source; chấp nhận giới hạn hiện tại. Probe mở rộng cửa sổ ghi 308292 TIM10 events, 304489 control runs, race_count=0, probe_unstable_count=0. Không quan sát race không chứng minh race bất khả thi; chênh lệch bộ đếm chưa đủ để quy nguyên nhân. Instrumentation cần dọn trước khi đóng B4.
- ADC DMA: `raw_adc_val` chuyển sang `volatile`; `BAT_INIT()` điều chỉnh qualifier tại biên HAL. Bản chốt yêu cầu xác nhận lại build sạch cảnh báo nếu chưa thực hiện; chưa có log xác nhận trong bản gửi.
- iBUS RUNNING: main là consumer duy nhất; thêm `ibus_fresh_data` để điều kiện ARM dùng frame mới decode và checksum hợp lệ. Helpers nhận/throttle chỉ còn ở pre-flight, theo bản chốt.
- Safety: nhánh MPU ưu tiên khóa motor, CCR1–CCR4=12500; mất iBUS khi ARMED dùng minimum, CCR1–CCR4=13000. Đây là quan sát được báo cáo; tháo DATA_RDY không chứng minh toàn bộ cơ chế phát hiện MPU bị tháo vật lý.
- Shared-buffer iBUS: nguy cơ overwrite giữa ISR và main đã xác nhận từ source; chưa có bằng chứng runtime liên kết với F01. Quyết định sửa trước khi đóng B4.
- Đã thêm `ibus_mailbox[32]`, publish sau byte cuối rồi set completion flag; đã khai báo `ibus_snapshot[32]` riêng main. Main vẫn checksum/decode trên `ibus_rx_buf`; sửa ownership **chưa hoàn tất**.
- Chưa có commit/hash, log full build hoặc ảnh/log gốc của ca này trong tệp bàn giao. Không tự gán phiên bản firmware.

## Điểm tiếp tục

**B4 / iBUS ownership / trước triển khai Bước 4 PRIMASK.** Kết quả Codex về CMSIS/PRIMASK đã có bên chat debug nhưng chưa được phân tích, chưa được gửi trong hồ sơ này.

1. Đọc và đánh giá kết quả Codex về PRIMASK, save/restore, compiler/memory barrier và chi phí critical section; chưa coi phương án là đã triển khai.
2. Chốt rồi triển khai bảo vệ check flag → copy mailbox sang snapshot → clear flag, khôi phục trạng thái interrupt trước đó.
3. Chuyển checksum/decode và các helper có xử lý frame sang snapshot, theo phạm vi đã chốt; kiểm tra mọi call site.
4. Trace lại repo và thử đường iBUS/safety; lưu điều kiện, phiên bản, số đo và kết quả build.
5. Dọn instrumentation B4.3A, giữ DWT B3; đánh giá đóng B4 trước khi chuyển B5/B6.

## Known Issue Register

- F01: **OPEN / UNRESOLVED**, throttle từng tăng đột ngột; chưa liên kết được với race iBUS.
- F02: **OPEN / UNRESOLVED**, control full frame khác PCB-only; chưa có root cause.
- F03: **OPEN / UNRESOLVED**, một motor từng hụt tới khoảng 50% throttle; chưa có root cause.
- Không issue nào được đóng trong ca 05/10.

Không thực hiện audit hoặc sửa firmware chỉ vì hồ sơ này mô tả các bước tiếp theo.
