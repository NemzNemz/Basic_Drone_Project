# Điều phối công việc hằng ngày

Cập nhật: 26/10/2026. Nguồn task và chính sách duy nhất: [PROJECT_CLOSURE_2026.md](PROJECT_CLOSURE_2026.md). Trang này trả lời bốn câu hỏi cho phiên tiếp theo; không tạo backlog mới hoặc thay thế hồ sơ audit/testing.

## A. Hôm nay làm gì?

- Milestone: **G0 — Chuẩn bị và chốt phạm vi**, 24/10–31/10/2026.
- **CL-01: DONE — Source/Design Review only**, chủ dự án chốt ngày 26/10. Không phải firmware safety PASS, hardware verification PASS hoặc quyết định GO; không mở lại thiết kế ARM/DISARM, failsafe hay ESC calibration trong CL-01.
- Nếu FC **chưa được xác nhận sẵn sàng**, nhiệm vụ chính tiếp theo là **CL-03 — PID**, thuộc CL-03–06: Host-Based Firmware Verification bằng CppUTest. Tiếp tục từ harness và UT-PID-001 đã có; không khảo sát lại toàn repo hoặc làm lại CL-01.
- CL-04 iBUS, CL-05 Motor và CL-06 runner/logic tích hợp được triển khai theo giá trị kỹ thuật và dependency; nếu nhóm hiện tại bị chặn, chọn task READY phù hợp từ backlog chính, không tạo task audit mới vì code smell.
- Khi hardware được xác nhận đủ điều kiện, ưu tiên CL-02/CL-07 bring-up và kiểm chứng thực tế theo POLICY-05. Chốt CL-01 không tự mở khóa CL-07 hoặc cho phép chạy motor.
- Ngày đích bay thử: trước 30/11, có điều kiện safety gates. Hạn cuối kết thúc: 31/12.
- Đây là điều phối việc tiếp theo, chưa có test mới hoặc bằng chứng G0 hoàn thành từ lượt cập nhật này. [Roadmap và gates](PROJECT_CLOSURE_2026.md#3-milestone).

## B. Codex phải thực hiện việc gì?

1. Kiểm tra branch/HEAD, working tree, harness và kết quả gần nhất của nhóm được giao; giữ nguyên công việc và cấu hình cá nhân không liên quan.
2. Từ thiết kế hiện hành, xác định chức năng, input/state/preconditions và expected độc lập bằng yêu cầu/công thức/vector. Chưa xác định được expected thì **NEEDS REVIEW**; không lấy output production làm oracle.
3. Chủ động xây dựng harness/stub/mock cần thiết, compile và chạy production code thật; test module và phối hợp module khi hợp lý, không sửa production để thuận tiện test khi chưa được phép.
4. Chạy nhóm test/regression, phân tích FAIL và báo cáo chức năng, input, expected, actual, kết quả và giới hạn; lưu revision/diff, toolchain, lệnh, exit code và bằng chứng vào hồ sơ testing hiện có.

CppUTest chỉ kiểm chứng về mặt lý thuyết production firmware đúng thiết kế hiện hành. Không xây dựng, phán xét, thay đổi hoặc quyết định safety policy; không tự đặt expected theo policy mới. Host/mock PASS không xác nhận hardware, timing, preemption, phản ứng motor hoặc safety gate.

Đây là phạm vi các phiên triển khai được giao tiếp theo; lượt cập nhật tài liệu ngày 26/10 không chạy thêm test, sửa firmware hoặc chạy hardware. Không tự commit/push nếu nhiệm vụ triển khai đó chưa cho phép.

## C. Tôi cần review điều gì?

Review theo nhóm, không cần tự viết test thường quy hoặc học sâu API/harness:

| Nội dung | Câu hỏi review |
| --- | --- |
| Chức năng | Test đang xác nhận hàm/module hoặc phối hợp nào của thiết kế hiện hành? |
| Input | Input, state và preconditions có đúng phạm vi cần kiểm chứng không? |
| Expected | Kết quả kỳ vọng, căn cứ và tolerance có độc lập, rõ ràng và không thêm policy mới không? |
| Actual / kết quả | Đã thực chạy những gì, actual hoặc assertion nào đạt/sai, exit code là gì? |
| Giới hạn | PASS/FAIL/NEEDS REVIEW có ý nghĩa gì; phần nào vẫn cần hardware hoặc chưa được test? |

Chỉ xác nhận kết quả trong phạm vi test; không đóng A5, B4 target verification, F01–F03 hoặc safety gates từ Host Unit Test PASS.

## D. Kết thúc phiên khi nào?

- Có một nhóm kết quả kèm bằng chứng đủ để người phát triển review, kể cả kết quả FAIL hoặc NEEDS REVIEW; không cần ép mọi ca PASS để kết thúc phiên.
- Hoặc hết thời lượng phiên đã chọn, mặc định **3 giờ 30 phút**; ghi checkpoint và bước tiếp theo, không kéo dài để ép PASS.
- Dừng sớm nếu thiếu oracle theo thiết kế hiện hành, cần sửa production, cần hardware chưa có, hoặc chi phí mock tăng mà không tạo bằng chứng có ích. Ghi blocker, không mở vòng thiết kế safety mới.
- Cuối phiên chỉ cập nhật kết quả thực, ref và bước tiếp theo trong hồ sơ hiện có. Không tạo báo cáo dài, file theo ngày hoặc backlog song song.

## E. Chốt phiên ngày 26/10/2026

| Trường | Kết quả đã chốt |
| --- | --- |
| Ngày / thời lượng | 26/10/2026; không ghi nhận thời lượng thực. Ngày 25/10 nghỉ cá nhân, không có kết quả CL-01 mới. |
| Task | **CL-01: DONE — Source/Design Review only**, theo quyết định của chủ dự án. Không phải firmware safety PASS hoặc hardware verification PASS. |
| Evidence / ref | Bảng source/design đã review ngày 26/10; tóm tắt hành vi, giới hạn và NO-GO tại [mục 2.1](PROJECT_CLOSURE_2026.md#21-chốt-cl-01-ngày-26102026). Source `develop@eff243d`, cây `Core/` không đổi so với `6562a4e`. Không có phép đo hardware mới. |
| Giữ nguyên | A5 UNKNOWN / NEEDS REVIEW; B4 target verification UNRESOLVED; F01–F03 OPEN / UNRESOLVED; safety gates chưa kiểm chứng. Không tự mở khóa bring-up/motor/flight. |
| CppUTest | UT-PID-001 PASS lịch sử ngày 23/10: 1 test, 4 assertions, exit code 0. Chưa có kết quả mới trong lượt chốt này. |
| Tiếp theo | Nếu FC chưa sẵn sàng: CL-03–06, bắt đầu CL-03. Nếu hardware đủ điều kiện: ưu tiên bring-up và kiểm chứng thực tế trong backlog hiện có. |

## F. Điểm bắt đầu phiên tiếp theo

**Bước đầu tiên:** xác nhận có thông tin hardware mới hay không, kiểm tra branch/HEAD và mở checkpoint testing gần nhất. Chưa có xác nhận FC sẵn sàng thì tiếp tục CL-03, không làm lại CL-01 hoặc đọc lại toàn bộ 46 mục.

Codex thực hiện theo phần B; người phát triển review theo phần C; kết thúc theo phần D. Nguồn nhiệm vụ duy nhất vẫn là [Master Backlog](PROJECT_CLOSURE_2026.md#5-master-backlog); không thêm nhiệm vụ audit nếu thiếu tác động kỹ thuật thực tế.

Một ngày một nhiệm vụ chính, tối đa hai nhiệm vụ phụ; không đổi workstream liên tục. Task BLOCKED phải có điều kiện mở khóa rồi mới chuyển task READY.

## G. Nhắc ghi sổ cá nhân

Quy ước dưới đây chỉ hỗ trợ Daily Checklist cá nhân, không phải yêu cầu kỹ thuật hoặc tiêu chí nghiệm thu firmware.

- **Sổ 1:** 2–5 dòng về mục tiêu, kết quả, blocker và bước tiếp theo.
- **Sổ 2:** chỉ ghi kết luận debug/audit thực sự liên quan, kèm điều kiện và giới hạn; không chép toàn báo cáo.
- **Sổ 3:** chỉ ghi vấn đề CppUTest cần hiểu: hàm, input, expected/căn cứ, actual, kết quả và giới hạn; không bắt buộc chép API/harness.

Daily Engineering Brief 09:00 chỉ có thể dùng các file này khi công cụ thật sự truy cập được GitHub. Chưa xác minh đồng bộ tự động; không tạo hoặc sửa automation trong lần thiết lập.
