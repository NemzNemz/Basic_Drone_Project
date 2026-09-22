/**
 ******************************************************************************
 * @file    flight_controller.c
 * @author  Lê Nam
 * @brief   Bộ điều khiển bay Quadcopter STM32F4 (Phiên bản nền tảng v0)
 *
 * @details
 * Mã nguồn này là phiên bản nền tảng được xây dựng thông qua
 * quá trình nghiên cứu, tổng hợp, chọn lọc và tái cấu trúc từ nhiều tài liệu kỹ thuật,
 * Github, các video hướng dẫn cũng như kinh nghiệm từ trước.
 *
 * Hệ thống được triển khai trên vi điều khiển STM32F4 bằng ngôn ngữ C Bare-metal,
 * sử dụng thư viện HAL kết hợp thao tác thanh ghi tại các khối (được đề cập rõ trong datasheet)
 * nhằm cân bằng giữa tính trực quan, khả năng học tập và hiệu năng.
 *
 * Hệ thống vận hành theo chu kỳ ngắt cố định 2.0 ms (500 Hz), được xây dựng như
 * một bộ mã nguồn nền tảng phục vụ học tập, nghiên cứu và làm cơ sở để tiếp tục
 * phát triển, tối ưu và mở rộng trong các phiên bản sau.
 *
 * Thông tin hệ thống:
 * - Vi điều khiển       : STM32F411CEU6 (ARM Cortex-M4)
 * - Ngôn ngữ            : C for Embedded
 * - Chu kỳ điều khiển   : 2.0 ms (Fixed-step)
 * - Tần số điều khiển   : 500 Hz
 * - Kiến trúc phần mềm  : Event-driven / Non-blocking Super-loop
 *
 * * Cấu hình phần cứng tham chiếu :
 * - Khung drone (Frame) : Quadcopter F250 / Mark4 5 inch
 * - Động cơ (Motor)     : Brushless RS2205 2300KV
 * - Cánh quạt (Propeller): Cánh 3 lá 5045 / 2 lá
 * - Mạch điều tốc (ESC) : BLHeliS LittleBee 30A
 * - Cảm biến (IMU)      : MPU6050 (Nên thay thế qua ICM20602)
 * - Nguồn cấp (Battery) : Pin LiPo 4S 1550mAh 120C
 * - Thu phát (RC RX)    : Tay cầm FlySky i6 + RX iA6B
 ******************************************************************************
 */

/**
 ******************************************************************************
 * @warning  CẢNH BÁO VỀ TÍNH CHÍNH DANH HỌC THUẬT
 *
 * Mã nguồn này được chia sẻ nhằm phục vụ học tập, nghiên cứu và tham khảo
 * kiến trúc phần mềm nhúng phi chặn (KHÔNG CÓ DELAY TRONG LOOP CHÍNH).
 *
 * Không khuyến khích việc sao chép nguyên trạng mã nguồn để làm bài tập,
 * đồ án hoặc các sản phẩm học thuật mà không hiểu bản chất kiến trúc phần mềm bên dưới.
 *
 * Mã nguồn và các câu hỏi thẩm định kiến trúc do tác giả biên soạn
 * đã được gửi trực tiếp về thầy ĐQV để phục vụ công tác phản biện.
 *
 * Việc đánh giá nên tập trung vào khả năng giải thích kiến trúc hệ thống,
 * thao tác thanh ghi, xử lý ngắt, giao tiếp ngoại vi, điều khiển thời gian
 * thực và các quyết định thiết kế thay vì chỉ dựa trên kết quả chạy chương trình.
 *
 ******************************************************************************
 */

/**
 ******************************************************************************
 * @brief   Kiến thức nền tảng bắt buộc
 * Để đọc, hiểu và phát triển tiếp codebase này, cần nắm vững các nội dung sau:
 *
 * 1. Lập trình C nhúng & Thao tác thanh ghi
 *    - C for Embedded
 *    - Các thao tác cấp thanh ghi cơ bản.
 *    - Volatile, Pointer, Memory Map, Memory Alignment, Struct Padding.
 *
 * 2. Kiến trúc ARM Cortex-M
 *    - NVIC.
 *    - Interrupt Service Routine (ISR).
 *    - Interrupt Vector Table.
 *    - Preemption Priority và Subpriority.
 *    - Xử lý và xóa cờ ngắt phần cứng.
 *
 * 3. Giao tiếp ngoại vi
 *    - I2C: Timing Diagram, START/STOP/ACK, và I2C Recovery.
 *    - UART.
 *    - iBUS Protocol: Non-blocking UART, Frame Parser và Checksum 16-bit.
 *
 * 4. STM32 Basic
 * 	  - Bắt buộc phải làm quen kiến trúc phần mềm nhúng chuẩn STM trước khi chạm đến mã nguồn này
 *
 * 5. Hệ thống điều khiển thời gian thực
 *    - Event-driven và Non-blocking.
 *    - Super-loop.
 *    - Chu kỳ lấy mẫu Ts = 0.002 s.
 *    - Complementary Filter.
 *    - Cascade PID.
 *    - Motor Mixing.
 ******************************************************************************
 */

/**
 ******************************************************************************
 * @brief   Công cụ chẩn đoán khuyến nghị (Recommended Tools)
 *
 * Hardware:
 * - ST-Link V2 (SWD Debugging).
 * - Saleae Logic Analyzer (8 Channels) để kiểm tra chu kỳ ngắt, jitter và
 *   giao tiếp I2C/iBUS.
 * - Đồng hồ đo điện (VOM).
 *
 * Software:
 * - STM32CubeIDE.
 * - Keil MDK.
 *
 * Khuyến nghị sử dụng:
 * - Live Expression.
 * - Register View.
 * - Breakpoint.
 * - Single Expression
 * - Nếu có ý định xuất UART thì sử dụng thêm Hercules + CH340 UART to TTL
 ******************************************************************************
 */

/**
 ******************************************************************************
/**
 ******************************************************************************
 * @brief   Lộ trình tự học & tiếp cận mã nguồn
 *
 * Nấc 0: Nền tảng STM32 Cơ bản
 * ----------------------------
 * - Làm quen với vi điều khiển STM32 cơ bản (như STM32F103C8T6 / Blue Pill) để
 *   nắm tư duy cấu hình clock, ngắt và ngoại vi cơ bản trước khi chạm vào mã nguồn này.
 *
 * Nấc 1: Hạ tầng & Khởi tạo Thời gian thực
 * ----------------------------------------
 * - Đọc Reference Manual (RM0383): CHỈ TRA CỨU đúng các thanh ghi/bit liên quan
 *   đến module đang làm việc (RCC, TIM, GPIO). Việc chủ động tra đúng tên thanh ghi
 *   và các bit chức năng trong datasheet đã là một thành tựu quan trọng.
 * - Cấu hình Clock 100 MHz và các ngắt. Đo Jitter bằng Saleae Logic.
 *
 * Nấc 2: Tầng Driver Ngoại vi (Non-blocking)
 * -------------------------------------------
 * - TIMx PWM Oneshot125 (2 kHz).
 * - USART1 RX + IDLE Interrupt.
 * - I2C1 Fast-Mode 400kHz MPU6050.
 *
 * Nấc 3: Thuật toán Điều khiển & Xử lý Tín hiệu
 * ---------------------------------------------
 * - Complementary Filter: Tính góc Euler (Roll/Pitch/Yaw).
 * - Cascade PID: Outer (Góc -> Rate), Inner (Rate -> Delta PWM) kèm lọc IIR khâu D.
 * - Motor Mixing: Ánh xá đầu ra PID + Throttle sang 4 kênh PWM động cơ.
 *
 * Nấc 4: Ghép nối Tầng Application & An toàn (Safety)
 * ---------------------------------------------------
 * - Đưa toàn bộ module vào Event-driven Super-loop điều phối bởi cờ ngắt.
 * - Theo dõi góc nghiêng và tham số PID qua các công cụ debug
 * - Kiểm thử kịch bản an toàn (Mất sóng iBUS, MPU6050 đóng băng) trước khi cất cánh.
 ******************************************************************************
 */

/**
 ******************************************************************************
 * @brief   Bối cảnh phát triển (Development Context)
 *
 * - Dự án được thực hiện chủ yếu thông qua quá trình tự nghiên cứu, tự học và
 *   thử nghiệm độc lập, không có mentor hoặc chương trình hướng dẫn chuyên sâu
 *   trong suốt quá trình phát triển.
 *
 * - Mã nguồn được xây dựng trên cơ sở tổng hợp, chọn lọc và tái cấu trúc từ
 *   nhiều tài liệu, sau đó được điều chỉnh để phù hợp với kiến trúc và mục tiêu của hệ thống.
 *
 * - Tại thời điểm thực hiện, chưa có đồ án hoặc bộ mã nguồn Flight Controller
 *   STM32 tương tự từ các khóa trước để kế thừa trực tiếp, do đó không thể tránh khỏi
 *   thiếu sót hoặc các điểm chưa tối ưu.
 *
 * - Quá trình kiểm thử, đo timing và kiểm chẩn ngoại vi chủ yếu sử dụng
 *   ST-Link SWD Debugger và Saleae Logic Analyzer trong điều kiện cá nhân chp phép
 ******************************************************************************
 */

/**
 ******************************************************************************
 * @brief   Giới hạn kỹ thuật của phiên bản nền tảng v0
 *
 * Tầng cảm biến:
 * - MPU6050 sử dụng giao tiếp I2C.
 * - Đã triển khai cơ chế phục hồi Bus I2C (unstuck_i2c).
 * - Độ trễ truyền dữ liệu vẫn cao hơn các cảm biến sử dụng SPI.
 *
 * Tầng điều khiển:
 * - Complementary Filter.
 * - Cascade PID rời rạc.
 * - Chưa tích hợp (KHÔNG ĐỦ TRÌNH ĐỘ TẠI THỜI ĐIỂM PHÁT TRIỂN MÃ NGUỒN) các bộ
 *   ước lượng trạng thái như EKF hoặc các bộ điều khiển nâng cao như LQR và MPC.
 *
 * Kiến trúc phần mềm:
 * - Event-driven Super-loop.
 * - Non-blocking execution.
 * - Chưa sử dụng hệ điều hành thời gian thực (RTOS).
 *
 * @note
 * Các giới hạn trên phản ánh phạm vi kỹ thuật của phiên bản nền tảng v0, đồng thời
 * cũng là các hướng có thể tiếp tục nghiên cứu, tối ưu và mở rộng trong các phiên bản kế nhiệm.
 *
 * @note LƯU Ý VỀ VIỆC THAM KHẢO NGUỒN TÀI LIỆU VÀ CÁC CHI TIẾT THIẾT KẾ
 *
 * Mã nguồn này chỉ đóng vai trò là một bộ khung kiến trúc tham chiếu, KHÔNG PHẢI là
 * tài liệu hướng dẫn từng bước.
 *
 * BẮT BUỘC phải chủ động tự tra cứu và tham khảo thêm nhiều nguồn tài liệu
 * bên ngoài (Datasheet, Reference Manual, Forum, GitHub, các dự án mở khác) để tự
 * giải mã các quyết định thiết kế trong code.
 *
 * Rất nhiều chi tiết kỹ thuật nhỏ lẻ đã được cài đặt trực tiếp trong mã
 * nguồn mà không có comment giải thích chi tiết. Người đọc cần tự phân tích code để
 * hiểu lý do tại sao các khối chức năng này lại được thiết kế như vậy.
 ******************************************************************************
 */

 /**
 ******************************************************************************
 * @warning  CẢNH BÁO VỀ THAM SỐ PID VÀ NGUY CƠ MẤT AN TOÀN KHI BAY THỰC TẾ
 *
 * 1. THAM SỐ PID TRONG MÃ NGUỒN CHỈ MANG TÍNH CHẤT MINH HỌA:
 *    - Các hệ số PID (Kp, Ki, Kd) khởi tạo trong bộ mã nguồn này TUYỆT ĐỐI KHÔNG
 *      phải là bộ tham số tối ưu chuẩn cho khung drone thực tế.
 *
 *    - Tác giả cố tình KHÔNG cung cấp bộ tham số PID đã cân chỉnh hoàn chỉnh.
 *      Người phát triển tiếp theo BẮT BUỘC phải tự thực nghiệm, tính toán quy đổi
 *      hoặc dò tham số (PID Tuning) phù hợp với cơ tính khung drone của riêng mình.
 *
 * 2. NGUY CƠ CHÁY NỔ PHẦN CỨNG VÀ MẤT AN TOÀN VẬT LÝ KHI TUNE PID SAI:
 *    - Khâu D (Derivative) quá lớn hoặc thiếu bộ lọc IIR: Sẽ gây khuếch đại nhiễu
 *      cao tần từ MPU6050, làm ESC / động cơ bị phát nóng cực nhanh và dẫn đến
 *      CHÁY ESC / NỔ ĐỘNG CƠ chỉ sau vài giây cấp nguồn.
 *
 *    - Khâu P (Proportional) quá cao: Gây hiện tượng dao động cưỡng bức
 *      tần số cao, làm mất kiểm soát thái độ và va đập vật lý.
 *
 *    - Khâu I (Integral) bị tích luỹ ngầm: Dẫn đến hiện tượng rồ ga
 *      bất ngờ khi cất cánh hoặc phản hồi trễ gây lật khung ngay trên mặt đất.
 *
 * -> NGUYÊN TẮC: Luôn tháo cánh quạt khi debug code trên bàn làm việc,
 *    và bắt buộc phải tự chịu trách nhiệm hoàn toàn về độ an toàn khi nạp tham số PID,
 *    đồng thời tham khảo khoá "LÝ THUYẾT ĐIỀU KHIỂN TỰ ĐỘNG HUST" để hiểu PID là gì
 *
 ******************************************************************************
 */

