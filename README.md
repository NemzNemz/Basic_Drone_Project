---
config:
  layout: elk
  theme: mc
---
flowchart TD
    Start([Bắt đầu]) --> Measure[MPU6050 hoàn thành phép đo]
    Measure --> Signal[Phát tín hiệu Data Ready]
    Signal --> ExtiCallback[EXTI Callback]
    ExtiCallback --> CheckPA2{Đúng chân PA2?}
    CheckPA2 -->|Không| EndCallback1[Kết thúc callback]
    CheckPA2 -->|Có| ReadI2C[Đọc 14 byte qua I2C Interrupt]
    ReadI2C --> CheckBusy{I2C BUSY?}
    CheckBusy -->|Có| Unstuck[Unstuck I2C]
    CheckBusy -->|Không| WaitComplete[Chờ hoàn thành]
    Unstuck --> I2CCallback[I2C MemRx Complete Callback]
    WaitComplete --> I2CCallback
    I2CCallback --> CheckI2C1{Đúng I2C1?}
    CheckI2C1 -->|Không| EndCallback2[Kết thúc callback]
    CheckI2C1 -->|Có| SetFlag[mpu_data_ready = 1]
    SetFlag --> MainLoop[Main Loop]
    MainLoop --> CheckFlag{mpu_data_ready == 1?}
    CheckFlag -->|Không| MainLoop
    CheckFlag -->|Có| ResetFlag[Reset cờ]
    ResetFlag --> Decode[Giải mã dữ liệu MPU]
    Decode --> CalcEuler[Tính góc Euler]
    CalcEuler --> UpdateStruct[Cập nhật struct]
    UpdateStruct --> MainLoop
    
    classDef startEnd fill:#f0fdf4,stroke:#4ade80
    classDef process fill:#f0f9ff,stroke:#38bdf8
    classDef decision fill:#fefce8,stroke:#facc15
    classDef error fill:#fef2f2,stroke:#f87171
    
    class Start,StartEnd startEnd
    class Measure,Signal,ExtiCallback,ReadI2C,WaitComplete,I2CCallback,SetFlag,MainLoop,ResetFlag,Decode,CalcEuler,UpdateStruct process
    class CheckPA2,CheckBusy,CheckI2C1,CheckFlag decision
    class EndCallback1,EndCallback2,Unstuck error
