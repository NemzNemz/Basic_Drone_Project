# Chi tiết ca Host-Based Firmware Verification

Ngày thực thi: **26/10/2026**. Production baseline `94f1a8028dd9a58ce51ee851348bf735bf80ba6f`; harness revision `7268b82ce5e4735617f79f02fe654410c3fb0554`. Final regression: **77 tests, 501 checks, exit code 0**. [Matrix và giới hạn](UNIT_TEST_SUMMARY.md).

Actual dưới đây được rút gọn từ các dòng `OBS` của [final run](evidence/2026-10-26/firmware-host/20261026T115934Z-1427/run.log); số đầy đủ và tolerance nằm trong [observations.tsv](evidence/2026-10-26/firmware-host/observations.tsv). Các so sánh byte/pointer và CHECK bổ sung xem test source/log. Trạng thái PASS chỉ áp dụng các assertion thực chạy; không xác nhận toàn module hoặc hardware.

## Căn cứ expected

- Số học độc lập từ input, công thức và hằng số đã chốt trong production: dt=.002, RC scale .05/.36, IIR .3/.7 hoặc .5/.5, integral clamp ±500; raw sensitivities 2048/65.5, π/180, DEG_TO_RAD=.017453, RAD_TO_DEG=57.295, filter α=.98.
- Frame contract hiện hành: 32 byte, header20/40, checksum tổng bù65535, channel little-endian/12 bit, flag high nibble byte13. Đây không phải xác minh receiver/protocol specification ngoài firmware.
- Contract hành vi hiện hành đã review CL-01: ARM/fresh, DISARM, minimum khi RC loss, lock khi MPU loss, battery warning và calibration có chủ đích. Không đặt expected theo policy mới.
- Mixer oracle tại gyro/Euler0: P=2×(RCpitch−1500)×.05, R=2×(RCroll−1500)×.05, Y=(RCyaw−1500)×.36; base=12500+(throttle−1000)×12.5. Logical M1=base+P+R−Y; M2=base−P+R+Y; M3=base−P−R−Y; M4=base+P−R+Y. Physical state nhận M4/M2/M3/M1 rồi clamp13000..25000. Giá trị oracle được tính trước thực thi, không lấy actual làm expected.
- Float tolerance được chốt trước chạy theo đơn vị/độ lớn và tích lũy; không đổi để ép PASS. Toàn bộ ca so sánh hữu hạn trước DOUBLES_EQUAL. Test invalid ngoài contract được tách NEEDS REVIEW trong summary.

## PID

| Test ID | Function | Purpose | Input / Preconditions | Expected / Basis | Actual | Result | Meaning / Limitations |
| --- | --- | --- | --- | --- | --- | --- | --- |
| UT-PID-001 | pid_pitch_roll | Cascade P-only lịch sử | RC1600; gyro0; Euler2; KP outer2/inner1, KI=KD=0 | Outer=6; inner=6; sums=.006/.012; tol1e-5 | 4 assertions PASS; test cũ không in actual riêng | PASS | Giữ nguyên ca và expected từ 23/10; không phải toàn PID. |
| UT-PID-002 | pid_pitch_roll | Điểm trung hòa | RC1500, Euler0, gyro0; KP=1 | Hai output=0; tol1e-6 | outer=0; inner=0 | PASS | Một lần gọi, state zero. |
| UT-PID-003 | pid_pitch_roll | Hai đầu RC | RC1000/2000; Euler=gyro0; KP2/1; reset giữa input | setpoint ±25° → output ±50; outer sum -.05, inner sum +.1; tol1e-5/1e-6 | low_inner=-50; low_sum=-0.0500000007; high_inner=50; high_sum=0.100000001 | PASS | Giới hạn RC thiết kế, không xác minh TX. |
| UT-PID-004 | pid_pitch_roll | I-only qua hai lần gọi | RC1600/Euler2/gyro0; outer KI2, inner KI3; P=D=0 | Outer .012→.024; inner .000072→.000216; dt=.002; tol1e-7/1e-8 | outer_first=0.0120000001; inner_first=0.0000720000025; outer_second=0.0240000002; inner_second=0.000216000015 | PASS | Oracle từ tích phân từng bước. |
| UT-PID-005 | pid_pitch_roll | Clamp tích phân dương | Sums499.999; RC2000; KI=1 | Hai sums=500 sau bước vượt ngưỡng; tol1e-6 | outer_sum=500; inner_sum=500 | PASS | Không suy ra anti-windup actuator. |
| UT-PID-006 | pid_pitch_roll | Clamp tích phân âm | Sums-499.999; RC1000; KI=1 | Hai sums=-500; tol1e-6 | outer_sum=-500; inner_sum=-500 | PASS | Biên âm. |
| UT-PID-007 | pid_pitch_roll | D step và IIR decay | Gyro=10/57.295 rad/s; outer KD2; inner KP1/KD.1; hai gọi | IIR outer=7; inner=-274; lần2 outer=-18.2, inner=-153.2; tol≤2e-4 | outer_IIR=7; inner_first=-274; outer_second=-18.2000008; inner_second=-153.199997 | PASS | D trên measurement, không đánh giá tuning. |
| UT-PID-008 | reset_error | Phạm vi reset hiện hành | sum9, prev3, IIR4, out5, KP2 | Chỉ sum→0; các field khác giữ nguyên; tol0 | sum=0; prev=3; IIR=4; out=5; gain=2 | PASS | Không yêu cầu reset toàn state mới. |
| UT-PID-009 | pid_yaw_rate | Hai đầu yaw rate | RC1000/2000, gyro0, KP1.5 | Rate ±180 → output ±270; tol1e-4 | low=-270; high=270 | PASS | Scale .36 theo cấu hình. |
| UT-PID-010 | pid_yaw_rate | Feedback và clamp I | RCcenter/gyro90°/s; rồi sums±499.9, RC endpoints | Output -90; sums chặn ±500; tol1e-4/1e-6 | feedback=-90; positive_clamp=500; negative_clamp=-500 | PASS | Không xác minh rate sensor thật. |
| UT-PID-011 | pid_yaw_rate | D step rồi hằng | Gyro10°/s, RC1500, KP1/KD.1; hai gọi | Output -260→-135; tol2e-4 | step=-260; constant=-135 | PASS | IIR .5 và dt .002. |
| UT-PID-012 | pid_yaw_angle | Wrap qua ±180 | Target/Euler -179/179 và 179/-179; KP2 | Error ±2 → output ±4; tol1e-6 | wrap_positive=4; wrap_negative=-4 | PASS | Không mở rộng contract sang góc nhiều vòng. |
| UT-PID-013 | pid_yaw_angle | Biên ±180 và không dùng KI | KP1, target±180; rồi target10, gyro10°/s, KD2, KI99,sum7 | Output ±180; PD=0; sum vẫn7; tol≤1e-5 | plus180=180; minus180=-180; heading_PD=0; sum_unchanged=7 | PASS | Đúng thiết kế yaw angle không tích phân. |
| UT-PID-014 | pid_pitch_roll | Lặp state P-only | 1000 lần RC1600/Euler2, KP2/1 | Output6/6; sums6/12; tol1e-5 output, .001 tích lũy | outer=6; inner=6; outer_sum=6.00003386; inner_sum=12.0000677 | PASS | Stress số học hữu hạn, không timing. |

## iBUS

| Test ID | Function | Purpose | Input / Preconditions | Expected / Basis | Actual | Result | Meaning / Limitations |
| --- | --- | --- | --- | --- | --- | --- | --- |
| UT-IBUS-001 | ibus_check_sum | Vector checksum cố định | 32 byte: 20 40, payload0, checksum9F FF | Checksum PASS; 65535-96=65439 | literal_frame=1 | PASS | Contract frame32 của firmware, không chứng nhận protocol/RF. |
| UT-IBUS-002 | ibus_check_sum | Lỗi từng byte | XOR1 từng byte0..31 của frame hợp lệ | 32 lần đều reject | corrupted_byte=0 (32 lần) | PASS | Không fuzz mọi tổ hợp lỗi. |
| UT-IBUS-003 | ia6b_decode_data/is_failsafe | Channel/mask/flag | Direct vector1000/1500/2000/1200/2000/1000; high nibble A | Giữ giá trị 12 bit; flags10→failsafe1, flags0→0 | roll=1000; pitch=1500; throttle=2000; yaw=1200; SwA=2000; SwB_masked=1000; flags=10; failsafe=1; no_failsafe=0 | PASS | Direct decoder, không coi vector đã sửa flags là checksum PASS. |
| UT-IBUS-004 | iBUS_Parse_Byte | Ranh giới publish | Feed31 byte rồi byte32, mailbox ban đầu55 | Trước cuối: flag0/mailbox55; sau: flag1/frame đủ32 byte | pending_before_last=0; mailbox_before_last=85; published=1 | PASS | Chỉ thứ tự tuần tự trên host. |
| UT-IBUS-005 | iBUS_Parse_Byte | Noise và header sai | 99 40 20 41 00 40 rồi frame hợp lệ | Không publish noise; resync/publish frame sau | no_false_publish=0; resynced=1 | PASS | Không đo mất byte UART. |
| UT-IBUS-006 | iBUS_Parse_Byte | Latest-frame-wins | Hai frame hoàn chỉnh rồi15 byte frame tiếp | Mailbox giữ frame thứ2 khi frame3 chưa hoàn chỉnh | notification_still_pending=1 | PASS | Không chứng minh ISR preemption. |
| UT-IBUS-007 | is_iBUS_Received | Consume đúng một lần | Frame hợp lệ pending | Return1/decode/clear; gọi tiếp return0 | received=1; flag_clear=0; decoded=2000; no_second_consume=0 | PASS | Helper startup, không RUNNING consumer. |
| UT-IBUS-008 | is_iBUS_Received | Checksum fail không decode | State throttle777, frame bị lật byte8 | Return0, throttle vẫn777, flag clear | reject=0; old_state=777; consumed=0 | PASS | Không đọc frame đồng thời ISR. |
| UT-IBUS-009 | is_iBUS_Throttle_Min | Biên ga pre-flight | Frame throttle1009 rồi1010 | 1009→1; 1010→0; phép so <1010 | 1009=1; 1010=0 | PASS | Không thay ngưỡng ARM hiện có. |
| UT-IBUS-010 | HAL_UART_RxCpltCallback/parser | Chuỗi callback thật | 32 byte qua buffer RX và USART2 callback; thêm UART khác | 32 rearm, mailbox đúng; UART khác không rearm | rearm_per_byte=32; frame_ready=1; ignore_other_uart=33 | PASS | Callback gọi bằng host, không IRQ thật. |
| UT-IBUS-011 | parser→helper | Stream dài xác định | 1000 frame tuần tự, throttle1000..1999 | 1000 consume thành công, cuối1999/flag0 | last_channel=1999; last_consumed=0 | PASS | Không phải fuzz hoặc baud stress. |

## MPU6050 và recovery có thể cô lập

| Test ID | Function | Purpose | Input / Preconditions | Expected / Basis | Actual | Result | Meaning / Limitations |
| --- | --- | --- | --- | --- | --- | --- | --- |
| UT-MPU-001 | MPU_INIT | Cấu hình register | Fake WHO_AM_I68, reset PWR40, register khác0 | PWR1; divider1; CONFIG2; GYRO8; ACCEL24; DRDY chưa bật | PWR=1; divider=1; DLPF=2; FS_GYRO=8; FS_ACCEL=24; DRDY_not_enabled=0 | PASS | Thiết bị giả cấp register, không read-back MPU thật. |
| UT-MPU-002 | MPU_DATA_READY_START | Bật DRDY và giữ bit khác | INT_PIN_CFG80, INT_ENABLE10 | B0 và11 theo mask30/01 | INT_PIN_CFG=176; INT_ENABLE=17 | PASS | Không chứng minh chân INT. |
| UT-MPU-003 | MPU_RAW_MEASUREMENT | Endian/scaling | Accel2048/-1024/-2048; gyro6550/-6550/131 | g=1/-.5/-1; rad/s=±1.745329252/.034906585; tol1e-6/1e-7 | ax_g=1; ay_g=-0.5; az_g=-1; gx_rad=1.74532926; gy_rad=-1.74532926; gz_rad=0.0349065848 | PASS | Raw→deg/s→rad/s, không sensor accuracy. |
| UT-MPU-004 | MPU_RAW_MEASUREMENT | Signed extrema/temp | Raw -32768/+32767; thay temp6..7 | Accel -16/15.99951171875; gyro theo raw/65.5×π/180; temp không đổi gyro; tol2e-6 | min_accel=-16; max_accel=15.9995117; min_gyro=-8.73144245; max_gyro=8.73117638; temp_ignored=-8.73144245 | PASS | API raw signed16; không bão hòa phần cứng. |
| UT-MPU-005 | MPU_SET_GYRO_BIAS→RAW | Bias đơn vị cuối | Bias inputs .1/-.2/.3; raw0 | Gyro -.1/.2/-.3; tol1e-7 | gx=-0.100000001; gy=0.200000003; gz=-0.300000012 | PASS | Xác minh dấu bias, không chất lượng calibration. |
| UT-MPU-006 | MPU_CALIB_GYRO→RAW | Mean hằng và samples0 | 4 fake reads gyro131/-262/393; rồi samples0 | 4 read/4 delay; bias -2/+4/-6×.017453; samples0 thành1 read | reads=4; delays=4; bias_x=-0.0349059999; bias_y=0.0698119998; bias_z=-0.104718; zero_becomes_one=1 | PASS | Chỉ HAL_OK; không freshness. |
| UT-MPU-007 | CONVERT_TO_ORIENT | Dấu gyro ba trục | Level z1, gyro1/1/1, Euler0 | Roll=-.1122982; pitch=+.1122982; yaw=.11459; tol1e-6 | roll=-0.112298198; pitch=0.112298198; yaw=0.114590004 | PASS | Toán theo convention đã chốt, không đo trục vật lý. |
| UT-MPU-008 | CONVERT_TO_ORIENT | Accel tilt30° từng trục | (-.5,0,√.75) rồi (0,-.5,√.75); gyro0 | Axis nghiêng .02×π/6×57.295≈.599991837; axis kia0; tol2e-6 | pitch=0.599991262; roll_zero=0; roll=0.599991262 | PASS | Một bước filter, chưa hội tụ. |
| UT-MPU-009 | CONVERT_TO_ORIENT | State và fixed dt | Euler10/-20/30 level; rồi1000 bước yaw gyro1 | Roll/pitch .98×prev×.017453×57.295; yaw30; tích lũy114.59 tol.002 | roll_state=9.79970264; pitch_state=-19.5994053; yaw_state=30; yaw_1000=114.5914 | PASS | Không đo khoảng cách giữa lần gọi. |
| UT-MPU-010 | MPU_TRIGGER_READ_IT | Arguments/status passthrough | Fake lần lượt OK/BUSY/ERROR/TIMEOUT | Forward status; addrD0/reg3B/size14/buffer đúng | status_passthrough=0→2→1→3; addr=208; start_reg=59; size=14 | PASS | Không khẳng định HAL thật có mọi đường return giả được cấp. |
| UT-MPU-011 | EXTI/MemRx callbacks | Instance và BUSY behavior | Pin khác/đúng PB1; I2C khác/I2C1; fake HAL_BUSY | Đúng pin mới read; đúng I2C mới set flag; BUSY không RCC/GPIO recovery | other_pin=0; correct_pin=1; other_i2c=0; completion=1; busy_no_reset=0; busy_no_gpio=0 | PASS | Không HAL transaction hoặc IRQ completion thật. |
| UT-MPU-012 | mpu6050_read_reg/unstuck_i2c1 | Retry khi lỗi BERR | Read đầu HAL_ERROR, ErrorCodeBERR; retry OK | HAL reinit1; CPU ghi SR1=FEFF; không GPIO recovery | reinit=1; BERR_clear_write=65279; no_gpio_recovery=0 | PASS | Chỉ nhánh BERR; không chứng minh register rc_w0/bus. |
| UT-MPU-013 | MPU_CALIB_GYRO | Mean nhiều mẫu khác dấu | X131/262/-131/0; Y0/131/262/393; Z-131/-131/131/131 | Bias -1/-3/0×.017453; 4×3=12ms yêu cầu; tol1e-7 | mean_bias_x=-0.0174529999; mean_bias_y=-0.0523589998; mean_bias_z=0; requested_delay_ms=12 | PASS | Delay giả không đo12ms thật. |

## Motor

| Test ID | Function | Purpose | Input / Preconditions | Expected / Basis | Actual | Result | Meaning / Limitations |
| --- | --- | --- | --- | --- | --- | --- | --- |
| UT-MOTOR-001 | Motor_Update_Values | Lower/exact bounds | 0/12999/13000/25000 | 13000/13000/13000/25000 | m1=13000; m2=13000; m3=13000; m4=25000 | PASS | Clamp số học. |
| UT-MOTOR-002 | Motor_Update_Values | Upper/độc lập kênh | 25001/65535/17000/19000 | 25000/25000/17000/19000 | m1=25000; m2=25000; m3=17000; m4=19000 | PASS | Miền uint16 hợp lệ, không float→uint16 ngoài miền. |
| UT-MOTOR-003 | Motor_Init/Set_Speed | Field tới CCR | MOTOR14000/15000/16000/17000 | CCR1..4 cùng thứ tự | CCR1=14000; CCR2=15000; CCR3=16000; CCR4=17000 | PASS | Register RAM giả, không pin. |
| UT-MOTOR-004 | Motor_Lock/Min_Throttle | Hai mức hiện hành | Gọi Lock rồi Minimum | Bốn CCR12500 rồi13000 | CCR1=12500→13000; CCR2=12500→13000; CCR3=12500→13000; CCR4=12500→13000 | PASS | Không suy ra RPM hoặc motor dừng. |
| UT-MOTOR-005 | Update→Set_Speed | Clamp rồi xuất | 1/16000/25001/25000 | CCR13000/16000/25000/25000 | CCR1=13000; CCR2=16000; CCR3=25000; CCR4=25000 | PASS | Chạy hai hàm production, không ESC. |

## Battery

| Test ID | Function | Purpose | Input / Preconditions | Expected / Basis | Actual | Result | Meaning / Limitations |
| --- | --- | --- | --- | --- | --- | --- | --- |
| UT-BAT-001 | BAT_GET_VOL | Conversion miền ADC12 | Raw0/1000/4095 | 0/13.63/55.81485 V theo .01363; tol≤1e-5 | zero=0; 1000=13.6300001; 4095=55.8148499 | PASS | Không ADC calibration thật. |
| UT-BAT-002 | is_bat_low | Hysteresis/boundaries | 13→12.999→13→13.5→13.5001→13.2 | State0→1→1→1→0→0 | normal_at_13=0; below_13=1; hold_at_13=1; hold_at_13_5=1; recover_above=0; hold_normal=0 | PASS | Giữ đúng ngưỡng, không policy mới. |
| UT-BAT-003 | BAT_INIT | Biên DMA API | Địa chỉ volatile uint32 local | HAL nhận đúng địa chỉ, length1, call1 | length=1; calls=1 | PASS | Không DMA data-width/atomicity thật. |
| UT-BAT-004 | Conversion→hysteresis | Chuỗi điện áp | Raw950/970/1000 | 12.9485/13.2211/13.63 V → state1/1/0 | low_12_9485=1; hold_13_2211=1; recover_13_63=0 | PASS | Phối hợp production, không nguồn dưới tải. |

## Application state/watchdog/calibration

| Test ID | Function | Purpose | Input / Preconditions | Expected / Basis | Actual | Result | Meaning / Limitations |
| --- | --- | --- | --- | --- | --- | --- | --- |
| UT-APP-001 | Motor_Safety | Default locked | Lock1, SwA2000, throttle1500 | Lock1; CCR12500×4 | lock=1; CCR1=12500; CCR2=12500; CCR3=12500; CCR4=12500 | PASS | Contract hiện hành, không phán xét policy. |
| UT-APP-002 | Motor_Safety | ARM fresh/ga boundary | SwA2000, throttle1010, fresh1, yaw12 | Unlock0; fresh0; yaw ref12; CCR13000×4 | armed=0; fresh_consumed=0; yaw_ref=12; CCR1=13000; CCR2=13000; CCR3=13000; CCR4=13000 | PASS | Kiểm tra ba điều kiện đã có. |
| UT-APP-003 | Motor_Safety | Reject high/stale | Ga1011/fresh1; rồi1000/fresh0 | Vẫn lock; fresh clear kể cả không ARM | reject_high=1; fresh_consumed=0; reject_stale=1 | PASS | Không thêm điều kiện ARM mới. |
| UT-APP-004 | Motor_Safety | DISARM ưu tiên | Armed, SwA1000, failsafe1 | Lock1/CCR12500 | disarmed=1; CCR1=12500; CCR2=12500; CCR3=12500; CCR4=12500 | PASS | Hành vi state hiện hành. |
| UT-APP-005 | Motor_Safety | Biên xuất motor | Armed; ga1050 rồi1051; target18000/19000/20000/21000 | Minimum rồi target CCR | CCR1=13000→18000; CCR2=13000→19000; CCR3=13000→20000; CCR4=13000→21000 | PASS | Không motor response. |
| UT-APP-006 | Motor_Safety | Mất iBUS và phục hồi | Armed, RC loss1 rồi0 | Minimum13000, lock vẫn0; phục hồi target | no_lock=0; CCR1=13000→18000; CCR2=13000→19000; CCR3=13000→20000; CCR4=13000→21000 | PASS | Characterization thiết kế, không quyết định failsafe. |
| UT-APP-007 | Motor_Safety | Failsafe hiện hành | Armed, failsafe1 | Minimum13000, lock không đổi | lock_unchanged=0; CCR1=13000; CCR2=13000; CCR3=13000; CCR4=13000 | PASS | Không xác nhận RF failsafe. |
| UT-APP-008 | Motor_Safety | Ưu tiên MPU fault | Armed, MPU/RC/failsafe đều1 | Lock1, CCR12500 | mpu_lock=1; CCR1=12500; CCR2=12500; CCR3=12500; CCR4=12500 | PASS | Không thêm fault policy. |
| UT-APP-009 | Motor_Safety | Re-arm theo mức | MPU fault→lock; xóa fault, SwA vẫn2000, ga1000/fresh1 | Re-arm không cần switch edge; fresh clear | rearm_without_switch_edge=0; consumed=0 | PASS | Chỉ kiểm chứng luồng đã review. |
| UT-APP-010 | HAL_TIM_PeriodElapsedCallback | Filter timer/cờ nhị phân | TIM3 rồi TIM10 hai lần | Other0; TIM10 flag1; hai event vẫn1 | other_timer=0; tim10_event=1; binary_event=1 | PASS | Không chứng minh read-clear race. |
| UT-APP-011 | HAL_TIM_PeriodElapsedCallback | Ngưỡng counter | iBUS248/MPU48; hai tick TIM11 | Tick1 chưa lỗi; tick2 flags1 và counters0 | ibus_before=0; mpu_before=0; ibus_after=1; mpu_after=1; ibus_reset=0; mpu_reset=0 | PASS | 250/50 tick, không đo500/100ms. |
| UT-APP-012 | Timer→Motor_Safety | IRQ chỉ đặt flag | CCR target; MPUcnt49; tick rồi Motor_Safety | Sau tick CCR giữ; main call mới CCR12500 | CCR1=18000→12500; CCR2=19000→12500; CCR3=20000→12500; CCR4=21000→12500 | PASS | Không chứng minh CPU-stall protection. |
| UT-APP-013 | ESC_Calib | Sequence có chủ đích | Lock1; gọi calibration | CCR25000 tại delay7000, CCR12500 tại delay8000; lock không đổi | lock_unchanged=1; delay_count=2; high_ms=7000; low_ms=8000; high_CCR=25000 (4 lần); low_CCR=12500 (4 lần); CCR1=12500; CCR2=12500; CCR3=12500; CCR4=12500 | PASS | Không ARM gate là thiết kế; delay không ngủ. |
| UT-APP-014 | parser/helper→Motor_Safety | Luồng liên module | Frame ARM hợp lệ rồi frame flags1 | Decode ARM0, sau failsafe minimum13000 | decoded_arm=0; CCR1=13000; CCR2=13000; CCR3=13000; CCR4=13000 | PASS | Ghép public helper; RUNNING thật được test riêng ở IT. |

## Host integration — production main-loop

| Test ID | Function | Purpose | Input / Preconditions | Expected / Basis | Actual | Result | Meaning / Limitations |
| --- | --- | --- | --- | --- | --- | --- | --- |
| IT-001 | RAW→ORIENT→pid_pitch_roll | Sensor math tới cascade | Raw Z2048/GY655; Euler0; RC1500; KP2/1 | Rate≈9.99986395°/s; pitch≈.0195997333; outer≈-.0391994667; inner≈-10.039063416; tol≤2e-5 | pitch_angle=0.0195997339; outer=-0.0391994677; inner=-10.0390635 | PASS | Sensor fake raw, math/control thật. |
| IT-002 | firmware_main→mixer→CCR | Neutral full logic | Boot frame disarm; loop1 ARM; loop2/3 ga1500, IMUlevel | Armed; CCR18750×4, restore mask0 | bounded_main=1; arm=0; CCR1=18750; CCR2=18750; CCR3=18750; CCR4=18750; last_mixer=18750; irq_restored=0 | PASS | 3 vòng thật, không clock/peripheral init thật. |
| IT-003 | main Pitch→mixer | Pitch và front/rear | Sau ARM: R_UD1400, ga1500; RC kháccenter | Pitch_PID10; CCR18760/18740/18740/18760 | Pitch_PID=10; CCR1=18760; CCR2=18740; CCR3=18740; CCR4=18760 | PASS | Mapping phần mềm, không lực đẩy. |
| IT-004 | main Roll→mixer | Roll và swap M1/M4 | R_RL1600, ga1500, trục kháccenter | Roll_PID10; CCR18740/18760/18740/18760 | Roll_PID=10; CCR1=18740; CCR2=18760; CCR3=18740; CCR4=18760 | PASS | Giữ swap PCB đã có, không coi là bug. |
| IT-005 | main yaw-rate→mixer | Yaw rate mode | L_RL1600, ga1500 | Yaw_PID36; CCR18786/18786/18714/18714 | Yaw_PID=36; CCR1=18786; CCR2=18786; CCR3=18714; CCR4=18714 | PASS | Scale .36; không moment quay thật. |
| IT-006 | main cascade→mixed CCR | Ba trục đồng thời | R_RL1600/R_UD1400/L_RL1600, ga1500 | P10/R10/Y36; CCR18786/18786/18694/18734 | CCR1=18786; CCR2=18786; CCR3=18694; CCR4=18734 | PASS | Oracle tính độc lập từ bốn phương trình mixer. |
| IT-007 | main mailbox→snapshot→decode | Publish N+1 sau copy | FrameA ga1500/roll1600; publishB ga1800 ngay sau restore | Decode A; mailbox B pending; snapshot vẫn A; restore2 | decoded_snapshot_throttle=1500; decoded_snapshot_roll=1600; next_pending=1; restore_calls=2 | PASS | Chèn event tuần tự ở boundary, không PRIMASK/IRQ thật. |
| IT-008 | main ADC→warning/output | Low battery warning only | Raw ADC900; loop1 ARM, loop2 ga1500 | 12.267 V; vẫn armed; buzzer CCR50; motor CCR13000 từ target vòng trước | voltage=12.2670002; still_armed=0; buzzer=50; CCR1=13000; CCR2=13000; CCR3=13000; CCR4=13000 | PASS | Không thêm cut/RTL; output quan sát đúng thứ tự main. |
| IT-009 | Pre_Flight_Check→helpers | Pre-flight thông thường | Frame hợp lệ SwA1000/SwB1000/ga1000 | Qua gate; lock1; không có delay7000 | throttle=1000; disarm_switch=1000; motor_lock=1 | PASS | UART cấp frame mỗi HAL delay giả; không blocking thật. |
| IT-010 | Pre_Flight_Check→ESC_Calib | Chọn calibration bằng SwB | Frame đầuSwB2000, các frame sau1000/ga1000/SwA1000 | Một high7000/25000; một low8000/12500; SwB1000; lock1 | calibration_high=25000; calibration_low=12500; high_once=1; low_once=1; switch_released=1000; lock_unchanged=1 | PASS | Đúng chức năng có chủ đích, không test ESC. |
| IT-011 | main checksum→bookkeeping | Frame sai checksum | Loop1 ARM; loop2 frame hỏng, loss counter12 | Không decode/reset counter; throttle cũ1000; fresh0 | old_throttle=1000; counter_not_reset=12; no_fresh=0 | PASS | Không safety policy mới. |
| IT-012 | main MPU consume→watchdog | Z mới reset loss | MPUcnt49/flag1; sample Z2048 khác old0 | RawZ2048; cnt0/flag0; data-ready clear | raw_z=2048; counter=0; flag=0; consumed=0 | PASS | Logical sample consume, không freshness bus. |
| IT-013 | main MPU consume→watchdog | Z không đổi | MPUcnt49/flag1; sample Z0=old0 | Counter49/flag1; lock1/CCR12500 | counter=49; flag=1; lock=1; CCR1=12500; CCR2=12500; CCR3=12500; CCR4=12500 | PASS | Không xác nhận frozen detection thật. |
| IT-014 | main timeout→output | Không có completion MPU | Không auto completion, MPUcnt49; một event TIM11 | MPU loss1; flag data0; CCR12500 | mpu_loss=1; no_completion=0; CCR1=12500; CCR2=12500; CCR3=12500; CCR4=12500 | PASS | Ngưỡng tick giả, không đo deadline. |
| IT-015 | main timeout→output | Không có frame RC mới | Sau ARM bỏ frame loop2; cnt249 và một tick | RC loss1; lock0; CCR13000 | rc_loss=1; no_motor_lock=0; CCR1=13000; CCR2=13000; CCR3=13000; CCR4=13000 | PASS | Không phải mất RF thật. |
| IT-016 | main mixer→clamp→CCR | Saturation đa trục | Ga2000/R_RL2000/R_UD1000/L_RL2000 | Base25000; P50/R50/Y180; CCR25000/25000/24720/24920 | CCR1=25000; CCR2=25000; CCR3=24720; CCR4=24920 | PASS | Chỉ vector hợp lệ, chưa float cast ngoài miền. |
