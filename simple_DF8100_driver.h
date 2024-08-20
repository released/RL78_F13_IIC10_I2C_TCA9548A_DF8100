/*_____ I N C L U D E S ____________________________________________________*/
#include <stdio.h>

/*_____ D E C L A R A T I O N S ____________________________________________*/

/*_____ D E F I N I T I O N S ______________________________________________*/

// #define DF8100_ADDR_7BIT                                (0x48)      //THR_ADR = 10 kΩ Resistor to GND 
// #define DF8100_ADDR_7BIT                                (0x49)      //THR_ADR = 100 kΩ Resistor to VDD 
// #define DF8100_ADDR_7BIT                                (0x4A)      //THR_ADR = GND 
// #define DF8100_ADDR_7BIT                                (0x4B)      //THR_ADR = VDD 

// #define DF8100_ADDR_8BIT 								(DF8100_ADDR_7BIT << 1)


// from Qorvo-NextInput driver
#define MIN_OFF                     -1000
#define MAX_OFF                     1000

// Registers
#define TOEN_REG                    0
#define TOEN_POS                    7
#define TOEN_MSK                    0x80

#define WAIT_REG                    0
#define WAIT_POS                    4
#define WAIT_MSK                    0x70

#define ADCRAW_REG                  0
#define ADCRAW_POS                  3
#define ADCRAW_MSK                  0x08

#define EN_REG                      0
#define EN_POS                      0
#define EN_MSK                      0x01

#define INAGAIN_REG                 1
#define INAGAIN_POS                 4
#define INAGAIN_MSK                 0x70

#define PRECHARGE_REG               1
#define PRECHARGE_POS               0
#define PRECHARGE_MSK               0x07

#define OTPBUSY_REG                 2
#define OTPBUSY_POS                 7
#define OTPBUSY_MSK                 0x80

#define OVRINTRTH_REG               2
#define OVRINTRTH_POS               3
#define OVRINTRTH_MSK               0x08

#define OVRACALTH_REG               2
#define OVRACALTH_POS               2
#define OVRACALTH_MSK               0x04

#define INTR_REG                    2
#define INTR_POS                    0
#define INTR_MSK                    0x01

#define ADCOUT_REG                  3
#define ADCOUT_SHIFT12				4
#define ADCOUT_SHIFT16              8

#define SCOUNT_REG12                4
#define SCOUNT_POS12                0
#define SCOUNT_MSK12                0x0f

#define SCOUNT_REG16				5
#define SCOUNT_POS16				0				
#define SCOUNT_MSK16				0xff

#define AUTOCAL_REG                 7
#define AUTOCAL_SHIFT12				4
#define AUTOCAL_SHIFT16             8

#define AUTOPRELDADJ_REG            6
#define AUTOPRELDADJ_POS            7
#define AUTOPRELDADJ_MSK            0x80

#define ENFILTER_REG				6
#define ENFILTER_POS				5
#define ENFILTER_MSK				0x20

#define HIGHORDER_REG				6
#define HIGHORDER_POS				4
#define HIGHORDER_MSK				0x10

/*
#define ENNEGZONE_REG               8
#define ENNEGZONE_POS               0
#define ENNEGZONE_MSK               0x01
*/

#define ENCALMODE_REG               9
#define ENCALMODE_POS               7
#define ENCALMODE_MSK               0x80

#define CALRESET_REG                9
#define CALRESET_POS                4
#define CALRESET_MSK                0x70

#define INTRPOL_REG                 9
#define INTRPOL_POS                 3
#define INTRPOL_MSK                 0x08

#define CALPERIOD_REG               9
#define CALPERIOD_POS               0
#define CALPERIOD_MSK               0x07

#define RISEBLWGT_REG               10
#define RISEBLWGT_POS               4
#define RISEBLWGT_MSK               0x70

#define LIFTDELAY_REG               10
#define LIFTDELAY_POS               0
#define LIFTDELAY_MSK               0x07

#define PRELDADJ_REG                11
#define PRELDADJ_POS                3
#define PRELDADJ_MSK                0xf8

#define FALLBLWGT_REG               11
#define FALLBLWGT_POS               0
#define FALLBLWGT_MSK               0x07

#define INTRTHRSLD_REG              12
#define INTRTHRSLD_SHIFT12			4
#define INTRTHRSLD_SHIFT16          8

#define FALLTHRSEL_REG              6
#define FALLTHRSEL_POS              0
#define FALLTHRSEL_MSK              0x07

#define INTREN_REG                  14
#define INTREN_POS                  7
#define INTREN_MSK                  0x80

#define INTRMODE_REG                14
#define INTRMODE_POS                6
#define INTRMODE_MSK                0x40

#define INTRPERSIST_REG             14
#define INTRPERSIST_POS             5
#define INTRPERSIST_MSK             0x20

#define BTNMODE_REG                 14
#define BTNMODE_POS                 4
#define BTNMODE_MSK                 0x10

#define INTRSAMPLES_REG             14
#define INTRSAMPLES_POS             0
#define INTRSAMPLES_MSK             0x07

#define FORCEBL_REG                 15
#define FORCEBL_POS                 0
#define FORCEBL_MSK                 0x01

#define BASELINE_REG                18
#define BASELINE_SHIFT12			4
#define BASELINE_SHIFT16            8

#define FALLTHR_REG                 22
#define FALLTHR_SHIFT12				4
#define FALLTHR_SHIFT16             8

#define DEVID_REG                   0x80
#define DEVID_POS                   3
#define DEVID_MSK                   0xf8

#define REVID_REG                   0x80
#define REVID_POS                   0
#define REVID_MSK                   0x07

// Wait
#define WAIT_0MS                    0
#define WAIT_1MS                    1
#define WAIT_4MS                    2
#define WAIT_8MS                    3
#define WAIT_16MS                   4
#define WAIT_32MS                   5
#define WAIT_64MS                   6
#define WAIT_256MS                  7

// Adcraw
#define ADCRAW_BL                   0
#define ADCRAW_RAW                  1

// Inagain
#define INAGAIN_1X                  0
#define INAGAIN_8X                  1
#define INAGAIN_16X                 2
#define INAGAIN_31X                 3
#define INAGAIN_61X                 4
#define INAGAIN_115X                5
#define INAGAIN_208X                6
#define INAGAIN_416X                7

// Precharge
#define PRECHARGE_50US              0
#define PRECHARGE_100US             1
#define PRECHARGE_200US             2
#define PRECHARGE_400US             3
#define PRECHARGE_800US             4
#define PRECHARGE_1600US            5
#define PRECHARGE_3200US            6
#define PRECHARGE_6400US            7

// Calreset
#define CALRESET_500MS              0
#define CALRESET_1000MS             1
#define CALRESET_2000MS             2
#define CALRESET_4000MS             3
#define CALRESET_8000MS             4
#define CALRESET_16000MS            5
#define CALRESET_32000MS            6
#define CALRESET_DISABLED           7

// Calperiod
#define CALPERIOD_25MS              0
#define CALPERIOD_50MS              1
#define CALPERIOD_100MS             2
#define CALPERIOD_200MS             3
#define CALPERIOD_400MS             4
#define CALPERIOD_800MS             5
#define CALPERIOD_1600MS            6
#define CALPERIOD_DISABLED          7

// Riseblwgt
#define RISEBLWGT_0X                0
#define RISEBLWGT_1X                1
#define RISEBLWGT_3X                2
#define RISEBLWGT_7X                3
#define RISEBLWGT_15X               4
#define RISEBLWGT_31X               5
#define RISEBLWGT_63X               6
#define RISEBLWGT_127X              7

// Liftdelay
#define LIFTDELAY_DISABLED          0
#define LIFTDELAY_20MS              1
#define LIFTDELAY_40MS              2
#define LIFTDELAY_80MS              3
#define LIFTDELAY_160MS             4
#define LIFTDELAY_320MS             5
#define LIFTDELAY_640MS             6
#define LIFTDELAY_460MS             7

// Fallblwgt
#define FALLBLWGT_0X                0
#define FALLBLWGT_1X                1
#define FALLBLWGT_3X                2
#define FALLBLWGT_7X                3
#define FALLBLWGT_15X               4
#define FALLBLWGT_31X               5
#define FALLBLWGT_63X               6
#define FALLBLWGT_127X              7

// Intrmode
#define INTRMODE_THRESH             0
#define INTRMODE_DRDY               1

// Intrpersist
#define INTRPERSIST_PULSE           0
#define INTRPERSIST_INF             1

// Intrsamples
#define INTRSAMPLES_1               1
#define INTRSAMPLES_2               2
#define INTRSAMPLES_3               3
#define INTRSAMPLES_4               4
#define INTRSAMPLES_5               5
#define INTRSAMPLES_6               6
#define INTRSAMPLES_7               7

// Enfilter
#define ENFILTER_DISABLED			0
#define ENFILTER_ENABLED			1

/*_____ M A C R O S ________________________________________________________*/

/*_____ F U N C T I O N S __________________________________________________*/

unsigned char Get_DF8100_DisplayLog(void);
void Set_DF8100_DisplayLog(unsigned char flag);

MD_STATUS DF8100_read_WAIT_REG(unsigned char device_addr);
MD_STATUS DF8100_read_DEVID_REVID(unsigned char device_addr,unsigned char *ni_ID) ;
MD_STATUS DF8100_read_adc(unsigned char device_addr,signed short *ni_adc) ;


void DF8100_set_Init_Flag(bool flag);
bool DF8100_is_Init_Ready(void);

void force_sensor_print_data(void);
MD_STATUS force_sensor_task(void);

void Init_all_DF8100_cfg(void);


