# Điều phối công việc hằng ngày

Cập nhật thiết lập: 24/10/2026. Nguồn task và chính sách duy nhất: [PROJECT_CLOSURE_2026.md](PROJECT_CLOSURE_2026.md). Trang này chỉ điều phối phiên, không thay thế hồ sơ audit/testing.

## A. Mốc hiện tại

- Milestone: **G0 — Chuẩn bị và chốt phạm vi**, 24/10–31/10/2026.
- Điều kiện hoàn thành G0: safety contract/NO-GO được review; inventory linh kiện/PCB/revision có trạng thái thật; checklist bring-up sẵn sàng; có checkpoint CppUTest hoặc blocker rõ.
- Ngày đích bay thử: trước 30/11, có điều kiện safety gates. Hạn cuối kết thúc: 31/12.
- Đây là kế hoạch; chưa có bằng chứng G0 hoàn thành. [Roadmap và gates](PROJECT_CLOSURE_2026.md#3-milestone).

## B. Nhiệm vụ hiện tại

**Nhiệm vụ chính: CL-01 — Safety contract và phạm vi sử dụng. Trạng thái: READY, chưa hoàn thành.**

Mục tiêu: một bảng ngắn nối sự kiện/input/state tới output hiện tại, hành vi mong muốn, bằng chứng và điều kiện NO-GO. Ưu tiên vì phải biết chính xác hành vi bảo vệ trước khi bring-up/thử motor.

- Thời lượng linh hoạt 2 giờ 30 phút–4 giờ; mặc định **3 giờ 30 phút**.
- Nhiệm vụ phụ duy nhất: CL-02 inventory, chỉ khi CL-01 đạt checkpoint hoặc cần dừng chờ bằng chứng và còn thời gian.
- Không bắt đầu thêm CppUTest hoặc sửa firmware trong phiên thiết lập tài liệu.
- Khi hardware sẵn sàng, đánh giá ưu tiên task bring-up flight-critical theo POLICY-05; không tự bỏ qua CL-01 hoặc safety gates.

## C. Checklist thao tác

Mọi ô dưới đây chưa được đánh dấu hoàn thành. Đây là mission tiếp theo, không phải những việc đã làm trong lần tạo trang.

| Thời gian mặc định | Phân công | Hành động và đầu ra |
| --- | --- | --- |
| 00:00–00:15 | MANUAL | Mở trang này; ghi ngày, task, branch/HEAD và tình trạng hardware mới được xác nhận. Giữ baseline cũ nếu chưa có thông tin mới. |
| 00:15–00:55 | CODEX | Từ kết quả khảo sát, chuẩn bị bảng boot/disarm, ARM, mất iBUS, mất MPU và pin yếu: input/precondition/state/output hiện tại, evidence và điều chưa rõ. Chỉ đọc bổ sung đoạn source cần thiết. |
| 00:55–01:25 | REVIEW | Chủ dự án review bảng: hành vi source khác hành vi mong muốn ở đâu; không coi minimum là ramp-down hay còi là RTL; giữ câu hỏi an toàn chưa rõ. |
| 01:25–01:35 | MANUAL | Nghỉ ngắn, ghi checkpoint để không mất mạch. |
| 01:35–02:15 | CODEX | Đối chiếu có mục tiêu A5, B3 và Build PASS/revision từ nguồn đã có. Thiếu ảnh/log gốc thì ghi UNKNOWN và bước phân biệt, không tái kiểm kê repository. |
| 02:15–02:50 | REVIEW | Chốt phạm vi bằng chứng, blocker và điều kiện NO-GO; giữ F01–F03 mở. Không tự chấp nhận rủi ro để đạt ngày bay. |
| 02:50–03:15 | CODEX | Chuẩn bị kết quả CL-01 để review; nếu đạt checkpoint và còn thời gian, lập inventory CL-02 ngắn: đã có/chưa có/chưa xác nhận và phép kiểm tra tiếp. |
| 03:15–03:30 | MANUAL / REVIEW | Cập nhật phần E/F bằng kết quả thật và ref; ghi sổ ngắn. Chỉ đổi trạng thái task nguồn khi có căn cứ. |

- [ ] Định danh source và điều kiện hardware của phiên.
- [ ] Có bảng safety contract và những quyết định còn mở.
- [ ] Có nguồn hoặc nhãn UNKNOWN cho A5/B3/build revision.
- [ ] Có điều kiện dừng và NO-GO theo phạm vi sử dụng.
- [ ] Có checkpoint, evidence/ref và bước đầu phiên sau.

Definition of Done cho checkpoint CL-01: bảng safety được chủ dự án review; biết hiện tại source làm gì, chưa biết gì và điều kiện nào cấm thử tiếp. Không yêu cầu sửa xong firmware hoặc biến các unknown thành PASS trong một phiên.

Bằng chứng cần lưu trong hồ sơ phù hợp: phiên/ngày, source revision/diff nếu có, reference file/commit, kết luận và người review; liên kết artifact gốc khi có. Mẫu bảng: sự kiện | input/precondition/state | output source | hành vi yêu cầu | evidence | kết luận/giới hạn.

Điều kiện dừng: hết thời lượng; cần phép đo/hardware chưa có; cần sửa production; expected/policy chưa thống nhất; hoặc bất thường an toàn. Ghi blocker và bước tiếp theo, không kéo dài để ép PASS. Không chạy build/test/flash/hardware trong lượt thiết lập này.

## D. Tóm tắt workstream

| Workstream | Tình trạng được biết lúc thiết lập | Bước điều phối |
| --- | --- | --- |
| Hardware | Đang mua linh kiện, dự kiến cuối tháng 10; chưa xác nhận đủ điều kiện cấp nguồn hoặc PCB V2 đã bring-up. | CL-02 inventory; CL-07 BLOCKED đến khi điều kiện thực được xác nhận. [Phụ thuộc](PROJECT_CLOSURE_2026.md#8-phụ-thuộc-hardware-và-thông-tin-còn-thiếu). |
| Audit / Known Issues | B4 chưa target verification, A5 cần review; F01–F03 còn mở. Không có kết quả mới từ việc tạo tài liệu. | CL-01 trước; register gốc và nguồn chi tiết nằm trong [liên kết bằng chứng](PROJECT_CLOSURE_2026.md#10-liên-kết-bằng-chứng). |
| CppUTest | UT-PID-001 PASS lịch sử: 1 test, 4 assertions; không phải toàn PID đạt. Chưa có ca mới trong lần thiết lập. | CL-03–06 theo giá trị/chi phí; Codex triển khai, người phát triển review nhóm. [Chính sách](PROJECT_CLOSURE_2026.md#6-chính-sách-cpputest-tự-động-hóa). |
| Portfolio | Chưa có bằng chứng CL-09/10/11 hoàn thành. Tháng 12 ưu tiên tài liệu/bằng chứng/CV. | Có thể gom bằng chứng sẵn; không mở tính năng mới để làm đẹp portfolio. |

Không sao chép triệu chứng/root cause từng Known Issue vào trang này. Không tạo task cấp cao mới chỉ vì phát hiện code smell.

## E. Chốt phiên

Phần này dành cho kết quả thực tế của mission, hiện chưa có phiên CL-01 hoàn tất. Lần thiết lập chỉ tạo tài liệu điều phối, không nghiệm thu safety contract.

| Trường | Giá trị cần cập nhật sau phiên |
| --- | --- |
| Ngày / thời lượng thực | Chưa thực hiện mission |
| Task và trạng thái sau phiên | CL-01 READY; chỉ đổi theo bằng chứng |
| Đã hoàn thành | Chưa ghi nhận kết quả thực hiện CL-01 |
| Evidence / artifact | Chưa có artifact mới của mission |
| Git commit/ref của phiên | Chưa có commit thực hiện CL-01; commit tạo tài liệu không thay thế kết quả kỹ thuật |
| Blocker | Cập nhật sau review; unknown ban đầu được dẫn trong kế hoạch |
| Phần chưa xong | Safety contract, đối chiếu có mục tiêu và review kết quả |
| Quyết định / next checkpoint | Chưa có quyết định nghiệm thu mới |

Sau phiên, ghi kết quả có ngày và link tới hồ sơ audit/test thích hợp, giữ lịch sử FAIL ở nguồn. Không tạo file Markdown mới mỗi ngày. Không tự commit/push nếu phiên đó chưa được cho phép.

## F. Điểm bắt đầu phiên tiếp theo

**Bước đầu tiên:** thực hiện dòng 00:00–00:15 rồi giao Codex lập bảng safety từ `Core/Src/main.c` theo baseline hiện hành; chưa sửa code.

Nếu phiên CL-01 chưa xong, tiếp tục đúng dòng/checkpoint bị dừng. Nếu đã đủ DoD và được review, chuyển CL-02 để xác định điều kiện hardware; khi hardware chưa sẵn sàng, chọn CL-03/04 phù hợp từ backlog chính. Không đọc lại toàn bộ báo cáo 46 mục.

Một ngày một nhiệm vụ chính, tối đa hai nhiệm vụ phụ; không đổi workstream liên tục. Task BLOCKED phải có điều kiện mở khóa rồi mới chuyển task READY.

## G. Nhắc ghi sổ cá nhân

Quy ước dưới đây chỉ hỗ trợ Daily Checklist cá nhân, không phải yêu cầu kỹ thuật hoặc tiêu chí nghiệm thu firmware.

- **Sổ 1:** 2–5 dòng về mục tiêu, kết quả, blocker và bước tiếp theo.
- **Sổ 2:** chỉ ghi kết luận debug/audit thực sự liên quan, kèm điều kiện và giới hạn; không chép toàn báo cáo.
- **Sổ 3:** chỉ ghi vấn đề CppUTest cần hiểu: hàm, input, expected/căn cứ, actual, kết quả và giới hạn; không bắt buộc chép API/harness.

Daily Engineering Brief 09:00 chỉ có thể dùng các file này khi công cụ thật sự truy cập được GitHub. Chưa xác minh đồng bộ tự động; không tạo hoặc sửa automation trong lần thiết lập.
