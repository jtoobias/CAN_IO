This repo holds the firmware for a custom CAN input and output circuit board centered around the STM32 microcontroller.

Completed:
- Polling of IMU data is working, on new hardware (v1.0.0)
- CAN bus is working on new hardware (v1.0.0)
- Use DMA to get IMU data
- RSDOs working

ToDo:
- Set CAN bus interface with computer for GUI instead of USB for now
- NMT commands to go from idle -> pre-op -> operation , error -> idle (need to test)
- Handle PDOs - Periodic send, send on change, send on sync, Store PDO mapping in EEprom 
- Need to send Unique ID
- USB still not working due to hardware
test
https://webhelp.kollmorgen.com/AKD2G/English/Content/AKD2G%20CANopen/CANopen_06_04_05%20Service%20Data%20Objects%20SDO.htm?TocPath=AKD2G%20CANopen%7CCommunication%20Profile%20%E2%80%93%20DS301%7C_____11