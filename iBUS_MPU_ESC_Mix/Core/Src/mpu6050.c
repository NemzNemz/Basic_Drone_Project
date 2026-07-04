/*
 * mpu6050.c
 *
 *  Created on: Jun 1, 2026
 *      Author: ADMIN
 */

#include "mpu6050.h"
#include "math.h"
#include "unstuck_i2c.h"
/*Thư viện để dùng hàm assert
Hàm assert giống như if-else nhưng dành cho debug
 */
#include "assert.h"

//Lọc nhiễu tĩnh con quay hồi chuyển
static float gbias_x = 0.0f;
static float gbias_y = 0.0f;
static float gbias_z = 0.0f;

//Độ nhạy MPU
static float mpu6050_accel_lsbs;
static float mpu6050_gyro_lsbs;

/*Con trỏ kiểu I2C_Handle tạm chưa trỏ vào hi2cx, sẽ được trỏ vào hi2c1 trong file.c sau.
Con trỏ này chỉ làm việc với thực thể hi2cx duy nhất ở main.c, đảm bảo tính toàn vẹn trạng thái.
Không sử dụng kiểu khai báo phẳng hi2cx vì như thế sẽ tạo bản sao tĩnh.
*/
static I2C_HandleTypeDef *mpu6050_hi2c;
/*
Static để các hàm này chỉ có phạm vi tối đa trong file.c
ko bị lỗi redefine nếu có trùng tên bên file khác
*/
static int is_valid_mpu6050(uint8_t reg);
static void mpu6050_write_reg(uint8_t reg, uint8_t val);
static uint8_t mpu6050_read_reg(uint8_t reg);
static void mpu6050_test_whoami(void);
static void mpu6050_disable_sleep(void);
static void mpu6050_reset_device(void);
static void mpu6050_reset_sensor_paths(void);
static void mpu6050_set_smplrt_div(uint8_t smplrt_div);
static void mpu6050_clksel_xgryo(void);
static void mpu6050_set_fsr_gyro(void);
static void mpu6050_set_fsr_accel(void);
static void mpu6050_set_dplf(uint8_t dplf_sel);
static void mpu6050_data_ready_interrupt_enable(void);
static void mpu6050_enable_latch_int_rd_clear(void);

void CONVERT_TO_ORIENT(MPU_MEASUREMENT* mpu_mea_ptr, EULER_MEASUREMENT* eul_mea_ptr) {
    const float MPU_DT = 0.002f;
    const float ALPHA   = 0.98f;

    // Goc Pitch va Roll lay tu Accel
    float roll_accel = atan2f(-mpu_mea_ptr->accel.x,
                              sqrtf(mpu_mea_ptr->accel.y*mpu_mea_ptr->accel.y +
                                    mpu_mea_ptr->accel.z*mpu_mea_ptr->accel.z));

    float pitch_accel = atan2f(mpu_mea_ptr->accel.y,
                               sqrtf(mpu_mea_ptr->accel.x*mpu_mea_ptr->accel.x +
                                     mpu_mea_ptr->accel.z*mpu_mea_ptr->accel.z));

    // Chuuyển sang rad/s cho giống bên MPU_RAW_MEA
    float roll_prev  = eul_mea_ptr->roll  * DEG_TO_RAD;
    float pitch_prev = eul_mea_ptr->pitch * DEG_TO_RAD;

    //Goc Pitch va Roll lay tu Gyro
    float roll_gyro  = roll_prev  + mpu_mea_ptr->gyro.x * MPU_DT;
    float pitch_gyro = pitch_prev + mpu_mea_ptr->gyro.y * MPU_DT;

    // Dung hợp, by MarkSherstan/CompFilter/Fusion
    float roll_fused  = ALPHA * roll_gyro  + (1.0f - ALPHA) * roll_accel;
    float pitch_fused = ALPHA * pitch_gyro + (1.0f - ALPHA) * pitch_accel;

    // Chuyển về các góc EULER
    eul_mea_ptr->roll  = roll_fused  * RAD_TO_DEG;
    eul_mea_ptr->pitch = pitch_fused * RAD_TO_DEG;
    eul_mea_ptr->yaw += mpu_mea_ptr->gyro.z * RAD_TO_DEG * MPU_DT;

    // Giới hạn góc
    if (eul_mea_ptr->roll > 180.0f)   eul_mea_ptr->roll -= 360.0f;
    if (eul_mea_ptr->roll < -180.0f)  eul_mea_ptr->roll += 360.0f;
    if (eul_mea_ptr->pitch > 180.0f)  eul_mea_ptr->pitch -= 360.0f;
    if (eul_mea_ptr->pitch < -180.0f) eul_mea_ptr->pitch += 360.0f;
}

void MPU_SET_GYRO_BIAS(float x, float y, float z){
	gbias_x = x;
	gbias_y = y;
	gbias_z = z;
}

void MPU_RAW_MEASUREMENT(MPU_MEASUREMENT* mpu_mea_ptr){
	//Tính toán giá trị thô chưa chuẩn hoá theo RAD TO DEG
	mpu_mea_ptr->accel.x = (int16_t)((mpu_mea_ptr->raw_buffer[0] << 8) | mpu_mea_ptr->raw_buffer[1]) / mpu6050_accel_lsbs;
	mpu_mea_ptr->accel.y = (int16_t)((mpu_mea_ptr->raw_buffer[2] << 8) | mpu_mea_ptr->raw_buffer[3]) / mpu6050_accel_lsbs;
	mpu_mea_ptr->accel.z = (int16_t)((mpu_mea_ptr->raw_buffer[4] << 8) | mpu_mea_ptr->raw_buffer[5]) / mpu6050_accel_lsbs;

	mpu_mea_ptr->gyro.x = (int16_t)((mpu_mea_ptr->raw_buffer[8] << 8) | mpu_mea_ptr->raw_buffer[9]) / mpu6050_gyro_lsbs;
	mpu_mea_ptr->gyro.y =(int16_t)((mpu_mea_ptr->raw_buffer[10] << 8) | mpu_mea_ptr->raw_buffer[11]) / mpu6050_gyro_lsbs;
	mpu_mea_ptr->gyro.z =(int16_t)((mpu_mea_ptr->raw_buffer[12] << 8) | mpu_mea_ptr->raw_buffer[13]) / mpu6050_gyro_lsbs;

	//Biến đổi từ độ/s sang rad/s
	mpu_mea_ptr->gyro.x = mpu_mea_ptr->gyro.x * M_PI/180.0f;
	mpu_mea_ptr->gyro.y = mpu_mea_ptr->gyro.y * M_PI/180.0f;
	mpu_mea_ptr->gyro.z = mpu_mea_ptr->gyro.z * M_PI/180.0f;

	//Bù trừ sai số tĩnh gyro
	mpu_mea_ptr->gyro.x += gbias_x;
	mpu_mea_ptr->gyro.y += gbias_y;
	mpu_mea_ptr->gyro.z += gbias_z;
}

void MPU_INIT(I2C_HandleTypeDef *hi2c_hw){
	//Gán hi2c1 vào
	mpu6050_hi2c = hi2c_hw;
	//Đảm bảo hi2c ko rỗng
	assert(mpu6050_hi2c != NULL);
	//Test địa chỉ cái đã
	mpu6050_test_whoami();
	//Reset thiết bị
	mpu6050_reset_device();
	//Reset các khối cảm biến (Gyro, Accel, Temp)
	mpu6050_reset_sensor_paths();
	//Tắt chế độ ngủ
	mpu6050_disable_sleep();
	//Đổi nguồn xung clock cho MPU
	mpu6050_clksel_xgryo();
	//Tần số gyro 1000Hz, trong SMPRT_DIV
	//Tần số lấy mẫu tự tính là 1000Hz/1+SMPRT_DIV = 500Hz
	mpu6050_set_smplrt_div(0x01);
	//Set lọc thông thấp 0x02
	mpu6050_set_dplf(0x02);
	//Set tầm đo gyro
	mpu6050_set_fsr_gyro();
	//Set tầm đo accel
	mpu6050_set_fsr_accel();
	//Bật ngắt khi data sẵn sàng
	mpu6050_data_ready_interrupt_enable();
	//Tự động xoá cờ ngắt dưới nền bus I2C
	mpu6050_enable_latch_int_rd_clear();
}

HAL_StatusTypeDef MPU_TRIGGER_READ_IT
(I2C_HandleTypeDef *hi2c_hw, MPU_MEASUREMENT* mpu_mea_ptr){
	//Kiểm tra tính hợp lệ của dãy địa chỉ đang đọc
	//XOUT_H là địa chỉ khởi đầu của chuỗi đọc 14bytes
	assert(is_valid_mpu6050(MPU6050_ACCEL_XOUT_H));
	//0x3B + 13 = 0x48 là địa chỉ kết thúc chuỗi 14bytes.
	assert(is_valid_mpu6050(MPU6050_ACCEL_XOUT_H + MPU6050_RAW_DATA_SIZE - 1));

	HAL_StatusTypeDef TRI = HAL_I2C_Mem_Read_IT(
			hi2c_hw,
			MPU6050_I2C_ADDR,
			//Địa chỉ thanh ghi khởi đầu của chuỗi đọc 14bytes
		    MPU6050_ACCEL_XOUT_H,
			//Độ rộng không gian địa chỉ trải gọn trong 256 giá trị
			I2C_MEMADD_SIZE_8BIT,
			//Địa chỉ đích của mảng đệm trích xuất dữ liệu thô 14bytes
			mpu_mea_ptr->raw_buffer,
			//Tổng số lượng byte liên tiếp bắt buộc phải kéo về
			MPU6050_RAW_DATA_SIZE);
	return TRI;
}

static int is_valid_mpu6050(uint8_t reg){
	//Lọt dô biên đia chỉ từ 0x19 tới 0x75 là okla, không là sẽ kẹt ở các hàm có assert
	if(reg >= MPU6050_LREG && reg <= MPU6050_HREG){
		return 1;
	}
	else return 0;
}

//Tham số là địa chỉ thanh ghi, giá trị truyền vào thanh ghi đó
static void mpu6050_write_reg(uint8_t reg, uint8_t val){
	//assert check xem phân vùng địa chỉ có hợp lệ không
	assert(is_valid_mpu6050(reg));
	//Trạng thái I2C_Mem_Write sẽ gán vào cho rep, có 4 trạng thái đã mô tả bên .h
	HAL_StatusTypeDef rep = HAL_I2C_Mem_Write(
			mpu6050_hi2c,
			MPU6050_I2C_ADDR,
			reg,
			//Độ rộng không gian địa chỉ trải gọn trong 256 giá trị
			I2C_MEMADD_SIZE_8BIT,
			&val,
			1,
			100);
	//Nếu status trả về HAL_OK thì thao tác ghi thành công
	assert(rep == HAL_OK);
}

static uint8_t mpu6050_read_reg(uint8_t reg){
	//assert check xem phân vùng địa chỉ có hợp lệ không
	assert(is_valid_mpu6050(reg));
	uint8_t reg_value;
	//Trạng thái I2C_Mem_Read sẽ gán vào cho rep, có 4 trạng thái đã mô tả bên .h
	HAL_StatusTypeDef rep = HAL_I2C_Mem_Read(
			mpu6050_hi2c,
			MPU6050_I2C_ADDR,
			reg,
			//Độ rộng không gian địa chỉ trải gọn trong 256 giá trị
			I2C_MEMADD_SIZE_8BIT,
			&reg_value,
			1,
			100);
	if(rep != HAL_OK){
		//Giải vây bus I2C
		unstuck_i2c1();
		__HAL_I2C_RESET_HANDLE_STATE(mpu6050_hi2c); // Ép máy trạng thái phần mềm HAL về READY
		    HAL_I2C_Init(mpu6050_hi2c);                 // Re-config lại các thanh ghi Alternate Function của chân

		    // Phát động lệnh đọc lại lần thứ hai và gán đè vào biến rep
		    rep = HAL_I2C_Mem_Read(
		            mpu6050_hi2c,
		            MPU6050_I2C_ADDR,
		            reg,
		            I2C_MEMADD_SIZE_8BIT,
		            &reg_value,
		            1,
		            100);
	}
	//Nếu status trả về HAL_OK thì thao tác ghi thành công
	assert(rep == HAL_OK);
	return reg_value;
}

static void mpu6050_test_whoami(void){
	//Móc lấy giá trị 0x68 tại vùng địa chỉ 0x75 của WHO_AM_I_REG
	uint8_t who_am_i_val = mpu6050_read_reg(MPU6050_WHO_AM_I_REG);
	//Nếu đúng là địa chỉ 0x68 thì okla
	assert(who_am_i_val == MPU6050_WHO_AM_I_VAL);
}

//Nằm trong thanh ghi PWR_MGMT1 0x6B
static void mpu6050_disable_sleep(void){
	//Móc lấy giá trị tại vùng địa chỉ 0x6B
	uint8_t reg_val = mpu6050_read_reg(MPU6050_PWR_MGMT1);
	//Tắt bit 6 SLEEP về 0
	reg_val &= ~(1U << 6);
	//Gán giá trị mới này vào thanh ghi PWR_MGMT1
	mpu6050_write_reg(MPU6050_PWR_MGMT1, reg_val);
}

static void mpu6050_reset_device(void){
	//Bật bit 7 lên trong PWR_MGMT1 để có thể reset toàn bộ cấu hình
	uint8_t reg_val = 1U<<7;
	mpu6050_write_reg(MPU6050_PWR_MGMT1, reg_val);
	//Datasheet kêu chờ 100ms. 200ms cho dư dả
	HAL_Delay(200);
	/*Một khi bit 7 bị xoá (tự động khi xong), thao tác xoá đã thành công
	Mục 3 Register Map ghi rõ giá trị mặc định của thanh ghi số 107 này là 0x40*/
	while(reg_val != 0x40){
		reg_val = mpu6050_read_reg(MPU6050_PWR_MGMT1);
	}
}

static void mpu6050_reset_sensor_paths(void){
	//Bit 2, 1, 0 đều được bật để reset full GYRO, ACCEL, TEMP
	uint8_t reg_val = 0x07;
	//Ghi nó vào thanh ghi SIGNAL_PATH
	mpu6050_write_reg(MPU6050_SIGNAL_PATH_RESET, reg_val);
}

static void mpu6050_clksel_xgryo(void){
	//Đọc giá trị hiện tại của MPU6050_PWR_MGMT1
	uint8_t reg_val = mpu6050_read_reg(MPU6050_PWR_MGMT1);
	//Set CLK_SEL lên 1 (0x01) vì chọn PLL with X axis gyroscope reference
	reg_val |= (1U<<0);
	//Ghi nó vào thanh ghi PWR_MGMT1
	mpu6050_write_reg(MPU6050_PWR_MGMT1, reg_val);
}

static void mpu6050_set_smplrt_div(uint8_t smplrt_div){
	//Ghi tần số 500Hz dô thanh ghi này
	mpu6050_write_reg(MPU6050_SMPLRT_DIV, smplrt_div);
}

static void mpu6050_set_fsr_gyro(void){
	//Đọc giá trị hiện tại của GYRO_CONFIG
	uint8_t reg_val = mpu6050_read_reg(MPU6050_GYRO_CONFIG);
	/*Set bit field:
	 - bit 7 - 5 Ko xài self-test
	 - bit 2 - 0 Reversed
     - bit 4 - 3 FS_SEL = 0x01 do chọn tầm đo +-500độ/s
	 */
	reg_val &= ~(0x03 << 3); // Xoá FS_SEL
	reg_val |= ((0x01 & 0x03)<< 3); // FS_SEL = 0x01
	mpu6050_gyro_lsbs = 65.5f;
	//Ghi nó vào thanh ghi GYRO_CONFIG
	mpu6050_write_reg(MPU6050_GYRO_CONFIG, reg_val);
}

static void mpu6050_set_fsr_accel(void){
	//Đọc giá trị hiện tại của ACCEL_CONFIG
	uint8_t reg_val = mpu6050_read_reg(MPU6050_ACCEL_CONFIG);
	/*Set bit field:
	 - bit 7 - 5 Ko xài self-test
	 - bit 2 - 0 Reversed
     - bit 4 - 3 AFS_SEL = 0x11 do chọn tầm đo +-16g/s
	 */
	reg_val &= ~(0x03 << 3); // Xoá AFS_SEL
	reg_val |= ((0x03 & 0x03)<< 3); // AFS_SEL = 0x11
	mpu6050_accel_lsbs = 2048.0f;
	//Ghi nó vào thanh ghi GYRO_CONFIG
	mpu6050_write_reg(MPU6050_ACCEL_CONFIG, reg_val);
}

static void mpu6050_set_dplf(uint8_t dplf_sel){
	//Đọc giá trị hiện tại của CONFIG
	uint8_t reg_val = mpu6050_read_reg(MPU6050_CONFIG);
	/*Set bit field:
	 - Bit 5, 4, 3 EXIT ko xài
	 - Bit 2, 1, 0 DLPF_CFG set 0x02 do dùng lọc thông thấp 98hz
	*/
	reg_val &= ~(0x07 << 0);
	reg_val |= ((dplf_sel & 0x07)<< 0);
	//Ghi nó vào thanh ghi CONFIG
	mpu6050_write_reg(MPU6050_CONFIG, reg_val);
}

static void mpu6050_data_ready_interrupt_enable(void){
	//Đọc giá trị hiện tại của INT_ENABLE 0x38
	uint8_t reg_val = mpu6050_read_reg(MPU6050_INT_ENABLE);
	//Set bit 0 DATA_RDY_EN lên 1 để unlock INT khi data ready
	reg_val |= (1U<<0);
	//Ghi nó vào thanh ghi INT_ENABLE
	mpu6050_write_reg(MPU6050_INT_ENABLE, reg_val);
}

static void mpu6050_enable_latch_int_rd_clear(void){
	//Đọc giá trị hiện tại của INT_PIN_CFG 0x37
	uint8_t reg_val = mpu6050_read_reg(MPU6050_INT_PIN_CFG);
	//Set bit 5 LATCH_INT_EN lên 1
	reg_val |= (1U<<5);
	//Set bit 4 INT_RD_CLEAR lên 1, clear cờ ngắt ở mọi giao dịch I2C
	reg_val |= (1U<<4);
	//Ghi nó vào thanh ghi INT_PIN_CFG
	mpu6050_write_reg(MPU6050_INT_PIN_CFG, reg_val);
}
