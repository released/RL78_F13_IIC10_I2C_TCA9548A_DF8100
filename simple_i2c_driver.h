/*_____ I N C L U D E S ____________________________________________________*/
#include <stdio.h>

/*_____ D E C L A R A T I O N S ____________________________________________*/

/*_____ D E F I N I T I O N S ______________________________________________*/
#define IIC1x_TIMEOUT_LIMIT                             (0xFFFF)


/*_____ M A C R O S ________________________________________________________*/

/*_____ F U N C T I O N S __________________________________________________*/


MD_STATUS drv_IIC10_I2C_read(unsigned char adr, unsigned char * const rx_buf, unsigned short rx_num);
MD_STATUS drv_IIC10_I2C_write(unsigned char adr, unsigned char * const tx_buf, unsigned short tx_num);
MD_STATUS IIC10_I2C_read(unsigned char device_addr,unsigned char reg_addr,unsigned char* rx_xfer_data,unsigned short rx_num);
MD_STATUS IIC10_I2C_write(unsigned char device_addr,unsigned char reg_addr,unsigned char* tx_xfer_data,unsigned short tx_num);

bool drv_Is_IIC10_bus_busy(void);
void drv_IIC10_callback_error(unsigned char err);
void drv_set_IIC10_receive_flag(bool flag);
bool drv_get_IIC10_receive_flag(void);
void drv_set_IIC10_send_flag(bool flag);
bool drv_get_IIC10_send_flag(void);

MD_STATUS drv_IIC11_I2C_read(unsigned char adr, unsigned char * const rx_buf, unsigned short rx_num);
MD_STATUS drv_IIC11_I2C_write(unsigned char adr, unsigned char * const tx_buf, unsigned short tx_num);
MD_STATUS IIC11_I2C_read(unsigned char device_addr,unsigned char reg_addr,unsigned char* rx_xfer_data,unsigned short rx_num);
MD_STATUS IIC11_I2C_write(unsigned char device_addr,unsigned char reg_addr,unsigned char* tx_xfer_data,unsigned short tx_num);

bool drv_Is_IIC11_bus_busy(void);
void drv_IIC11_callback_error(unsigned char err);
void drv_set_IIC11_receive_flag(bool flag);
bool drv_get_IIC11_receive_flag(void);
void drv_set_IIC11_send_flag(bool flag);
bool drv_get_IIC11_send_flag(void);

