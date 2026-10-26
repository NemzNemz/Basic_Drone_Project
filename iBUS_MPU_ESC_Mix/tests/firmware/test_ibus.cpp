#include "test_support.h"
#include <cstring>
TEST_GROUP(IBUS) {
    uint8_t frame[32];
    void setup(){fake_reset();app_reset();make_frame(frame,1000,1500,2000,1200,2000,1000,0);}
};
TEST(IBUS, IBUS001_KnownChecksum)
{uint8_t known[32]={0x20,0x40};known[30]=0x9f;known[31]=0xff;OBS_EQ("UT-IBUS-001","literal_frame",1,ibus_check_sum(known,32));}
TEST(IBUS, IBUS002_EveryByteCorruptionRejected)
{for(unsigned b=0;b<32;b++){frame[b]^=1;OBS_EQ("UT-IBUS-002","corrupted_byte",0,ibus_check_sum(frame,32));frame[b]^=1;}}
TEST(IBUS, IBUS003_DecodeChannelsAndMask)
{
    frame[13]|=0xa0;ia6b_decode_data(frame,&fs_i6);
    OBS_EQ("UT-IBUS-003","roll",1000,fs_i6.R_RL);OBS_EQ("UT-IBUS-003","pitch",1500,fs_i6.R_UD);OBS_EQ("UT-IBUS-003","throttle",2000,fs_i6.L_UD);
    OBS_EQ("UT-IBUS-003","yaw",1200,fs_i6.L_RL);OBS_EQ("UT-IBUS-003","SwA",2000,fs_i6.SwA);OBS_EQ("UT-IBUS-003","SwB_masked",1000,fs_i6.SwB);OBS_EQ("UT-IBUS-003","flags",10,fs_i6.failsafe);OBS_EQ("UT-IBUS-003","failsafe",1,is_failsafe(&fs_i6));
    fs_i6.failsafe=0;OBS_EQ("UT-IBUS-003","no_failsafe",0,is_failsafe(&fs_i6));
}
TEST(IBUS, IBUS004_PublishOnlyByte32)
{
    std::memset(ibus_mailbox,0x55,32);
    for(unsigned n=0;n<31;n++)iBUS_Parse_Byte(&frame[n]);
    OBS_EQ("UT-IBUS-004","pending_before_last",0,ibus_cplt_flag);OBS_EQ("UT-IBUS-004","mailbox_before_last",0x55,ibus_mailbox[0]);
    iBUS_Parse_Byte(&frame[31]);OBS_EQ("UT-IBUS-004","published",1,ibus_cplt_flag);MEMCMP_EQUAL(frame,ibus_mailbox,32);
}
TEST(IBUS, IBUS005_NoiseAndBadSecondHeader)
{uint8_t noise[]={0x99,0x40,0x20,0x41,0,0x40};for(unsigned n=0;n<sizeof noise;n++)iBUS_Parse_Byte(&noise[n]);OBS_EQ("UT-IBUS-005","no_false_publish",0,ibus_cplt_flag);feed_parser(frame);OBS_EQ("UT-IBUS-005","resynced",1,ibus_cplt_flag);MEMCMP_EQUAL(frame,ibus_mailbox,32);}
TEST(IBUS, IBUS006_LatestMailboxAndPartialNext)
{
    feed_parser(frame);uint8_t next[32];make_frame(next,2000,1000,1500,1800,1000,2000,0);feed_parser(next);MEMCMP_EQUAL(next,ibus_mailbox,32);
    for(unsigned n=0;n<15;n++){iBUS_Parse_Byte(&frame[n]);}
    MEMCMP_EQUAL(next,ibus_mailbox,32);OBS_EQ("UT-IBUS-006","notification_still_pending",1,ibus_cplt_flag);
}
TEST(IBUS, IBUS007_ReceivedConsumesValidFrame)
{feed_parser(frame);OBS_EQ("UT-IBUS-007","received",1,is_iBUS_Received());OBS_EQ("UT-IBUS-007","flag_clear",0,ibus_cplt_flag);OBS_EQ("UT-IBUS-007","decoded",2000,fs_i6.L_UD);OBS_EQ("UT-IBUS-007","no_second_consume",0,is_iBUS_Received());}
TEST(IBUS, IBUS008_InvalidFrameNotDecoded)
{fs_i6.L_UD=777;frame[8]^=1;feed_parser(frame);OBS_EQ("UT-IBUS-008","reject",0,is_iBUS_Received());OBS_EQ("UT-IBUS-008","old_state",777,fs_i6.L_UD);OBS_EQ("UT-IBUS-008","consumed",0,ibus_cplt_flag);}
TEST(IBUS, IBUS009_PreflightThrottleBoundary)
{make_frame(frame,1500,1500,1009,1500,1000,1000,0);feed_parser(frame);OBS_EQ("UT-IBUS-009","1009",1,is_iBUS_Throttle_Min());make_frame(frame,1500,1500,1010,1500,1000,1000,0);feed_parser(frame);OBS_EQ("UT-IBUS-009","1010",0,is_iBUS_Throttle_Min());}
TEST(IBUS, IBUS010_UartCallbackToParser)
{fs_i6ab_init(&huart2);const unsigned before=fake.uart_calls;feed_uart(frame);OBS_EQ("UT-IBUS-010","rearm_per_byte",32,fake.uart_calls-before);OBS_EQ("UT-IBUS-010","frame_ready",1,ibus_cplt_flag);MEMCMP_EQUAL(frame,ibus_mailbox,32);UART_HandleTypeDef other={0};HAL_UART_RxCpltCallback(&other);OBS_EQ("UT-IBUS-010","ignore_other_uart",before+32,fake.uart_calls);}
TEST(IBUS, IBUS011_LongDeterministicStream)
{for(unsigned n=0;n<1000;n++){make_frame(frame,1000+n%1001,1500,1000+n%1001,1500,1000,1000,0);feed_parser(frame);if(!is_iBUS_Received())FAIL("Frame hop le bi mat trong stream tuan tu");}OBS_EQ("UT-IBUS-011","last_channel",1999,fs_i6.L_UD);OBS_EQ("UT-IBUS-011","last_consumed",0,ibus_cplt_flag);}
