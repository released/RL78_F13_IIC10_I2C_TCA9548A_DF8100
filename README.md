# RL78_F13_IIC10_I2C_TCA9548A_DF8100
 RL78_F13_IIC10_I2C_TCA9548A_DF8100

udpate @ 2024/08/20

1. initial below pin define to function 

- UART : P15:TXD0, P16:RXD0

- IIC10 : P10/SCL10, P11/SDA10 , to drive TCA9548A and DF8100 force sensor

- TCA9548A RESET PIN : P43

- BUTTON : BTN1:P90 , BTN2:P91 , BTN3:P92 , BTN4:P93 , BTN5:P94 , BTN6:P95 , BTN7:P96 , BTN8:P97

2. by using I2C switch (TCA9548A)

send 0x01 , 0x02 , 0x04 , ... ,  to switch to different channel , check TCA9548A_2_SetChannel

3. under terminal , press digit D/d , to enable/disable display DF8100 log

![image](https://github.com/released/RL78_F13_IIC10_I2C_TCA9548A_DF8100/blob/main/log.jpg)

4. use BTN8 to enable/disable display DF8100 log

5. during the initial , use WAIT_REG to detect available sensors address , check DF8100_Do_Scan flow

![image](https://github.com/released/RL78_F13_IIC10_I2C_TCA9548A_DF8100/blob/main/WAIT_REG.jpg)

if sensor address not available 

![image](https://github.com/released/RL78_F13_IIC10_I2C_TCA9548A_DF8100/blob/main/WAIT_REG_no_ACK.jpg)

6. use DEVID_REG to double confirm scanned address , check DF8100_Get_ID flow

![image](https://github.com/released/RL78_F13_IIC10_I2C_TCA9548A_DF8100/blob/main/DEVID_REG.jpg)

7. use ADCOUT_REG to start read sensor data , check DF8100_read_adc flow

![image](https://github.com/released/RL78_F13_IIC10_I2C_TCA9548A_DF8100/blob/main/ADCOUT_REG.jpg)

8. below is LA capture with all available sensor data

![image](https://github.com/released/RL78_F13_IIC10_I2C_TCA9548A_DF8100/blob/main/LA_force_sensor_5.jpg)

