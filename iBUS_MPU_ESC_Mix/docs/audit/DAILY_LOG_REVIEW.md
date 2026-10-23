# Daily Log — Điểm cần xác nhận

Bản nháp chuyển từ `R:\GIT_HUB_REPO\HỒ SƠ KIỂM TOÁN HỆ THỐNG.docx`. Dùng nội dung Word và xác nhận ngày 2026-10-06 của chủ dự án; không đối chiếu firmware hoặc dùng kiến thức bên ngoài để sửa nội dung. Chưa commit.

## Đối chiếu ngày và phạm vi

- Đã đọc toàn bộ 739 đoạn không rỗng trong thân tài liệu, gồm bảng và 207 đối tượng công thức OMML; không có ảnh nhúng, comment hoặc footnote/endnote trong gói DOCX.
- Đủ 14 mốc ngày cụ thể xuất hiện trong nội dung: 2026-09-21, 22, 23, 24, 25, 28, 29; 2026-10-01, 02, 03, 05, 06, 19, 22. Không tạo ngày trung gian từ các khoảng thời gian.
- Chủ dự án xác nhận phần trước đây chưa rõ ngày thuộc 2026-10-06. A3 ngày 21–22/09 và phần phân tích 24–28/09 vẫn có nội dung gộp; nhật ký nêu rõ giới hạn gán ngày.
- B4.3A, việc giảm consumer, SAFETY-01 và quyết định B4.5 đã gộp vào 2026-10-06 theo xác nhận của chủ dự án; không còn mục chưa rõ ngày.
- Không có mã Known Issue F01–F03 hoặc commit hash trong Word; không tự gán cho các lỗi được mô tả. Không có kết quả kiểm thử cho toàn bộ 8 nhóm / 46 mục; tài liệu tập trung A1–A7 và B1–B4, các mục khác chủ yếu là tham chiếu/hướng tiếp theo.
- Metadata Word ghi 27 trang, chưa xác minh theo bố cục hiển thị. Các dòng dẫn nguồn đã được xóa khỏi nhật ký theo yêu cầu chủ dự án; không coi tên `image_*.png` là số trang Word. Các ảnh chỉ được nhắc tên, không có tệp ảnh trong DOCX để đối chiếu bằng chứng gốc.

## Các điểm cần chủ dự án xác nhận

1. **A5 chưa có bằng chứng đóng lỗi:** Ngày 29/09 ghi `UNRESOLVED` với spike `3.x / 1.1 / 0.x rad/s`; ngày 02/10 lại tổng kết A1–A6 đạt. Cần retest hoặc xác nhận lại tổng kết; bản nháp không chuyển A5 thành đạt.
2. **RAM ngày 22/09 không khớp:** Một phần ghi `00 00 00 C1 / 0xC1000000`, phần tiếp ghi `0x01 / 0x01000000 / 16777216` tại byte cao D7. Chuỗi `10 00 00 00` được nguồn diễn giải là `1` cũng cần đối chiếu ảnh/cách hiển thị. Giữ cả hai mô tả; không tự sửa. Kết luận ngày 23/09 về Debug Launch được giữ đúng ngày.
3. **Tên counter không thống nhất:** Word dùng cả `mpu_start_i2c_ok_count` và `mpu_i2c_start_ok_count`; bản nháp giữ theo từng đoạn nguồn. Cần xác nhận tên chuẩn trước khi chuẩn hóa thêm.
4. **Ngày đã xác nhận:** Hai chuỗi đo B4.3A, thay đổi consumer, SAFETY-01 và B4.5 thuộc 2026-10-06 theo xác nhận của chủ dự án. Điểm này đã giải quyết.
5. **Giới hạn bằng chứng đạt:** A3-S7 ghi đã vá code, A3-S9 ghi tái cấu trúc, SAFETY-01 ghi đạt thiết kế; không có test fault injection/retest đủ chi tiết để nâng thành target runtime. B4 iBUS chỉ có Build PASS ngày 19/10; ngày 22/10 mới quyết định mở Host-based Unit Testing, chưa có Host Unit Test PASS.
6. **Đơn vị/đo lường chưa đủ:** Spike `gyro.y = +18` không có đơn vị; phép đo Analyzer MPU INT được gọi là đo “trước đó” không có ngày riêng; retest startup A3-S8 thiếu thời lượng. B3 dùng từ jitter cho hiệu Max–Min `6.75 μs`; bản nháp giữ cách ghi và Min/Max, không tự đổi định nghĩa phép đo.
7. **Ngắt EXTI khác tên:** Phần đề xuất ngày 23/09 ghi `HAL_NVIC_DisableIRQ(EXTI2_IRQn)`; phần startup ghi `EXTI1_IRQn (hoặc EXTI2)`, B2 ghi EXTI1. Đây là nội dung đề xuất/cấu hình trong nguồn, chưa đủ để khẳng định đã áp dụng đúng IRQ nào. Bản nháp chỉ ghi cô lập EXTI, không biến đề xuất thành thay đổi đã thực hiện.
8. **Bằng chứng gốc còn thiếu:** Cần ảnh sổ, capture Analyzer, log build/test để đối chiếu chi tiết khi cần; các dòng dẫn nguồn đã được bỏ khỏi nhật ký theo yêu cầu. Không suy đoán trang thiếu hoặc các ngày không được ghi.

## Kiểm tra biên tập

- Giữ lỗi Pitch ban đầu và bản vá/retest cùng ngày; giữ FAIL RAM, warning startup 79/10, spike +18, A5 UNRESOLVED, A7.4 hoãn và B4 UNRESOLVED.
- Giữ số liệu 30 s ở ngày 23/09; ngày 28/09 ghi rõ chỉ dẫn lại, không tính là test mới. Không gán kết quả pid_flag cho 05/10 hoặc 19/10.
- Tách giả thuyết Torn Read/phase skew khỏi lỗi runtime đã quan sát; chính sách SAFETY-01 khỏi hành vi đã kiểm thử. Ví dụ quy đổi Raw=6550 được giữ là ví dụ, không đổi thành phép đo.
- Không sửa firmware hoặc `CURRENT_STATUS.md`; không commit, push hoặc merge. Hai file Markdown là bản nháp để review.
