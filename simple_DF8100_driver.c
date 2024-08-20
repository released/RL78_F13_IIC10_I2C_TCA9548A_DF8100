/*_____ I N C L U D E S ____________________________________________________*/
#include <stdio.h>
#include "r_cg_userdefine.h"

#include "misc_config.h"
#include "custom_func.h"

#include "simple_i2c_driver.h"
#include "simple_DF8100_driver.h"
#include "TCA9548A_driver.h"


/*_____ D E C L A R A T I O N S ____________________________________________*/

struct flag_8bit flag_DF8100_CTL;
#define FLAG_DF8100_DISPLAY_LOG               	    (flag_DF8100_CTL.bit0)
#define FLAG_FORCE_INIT_DF8100_RDY                 	(flag_DF8100_CTL.bit1)
#define FLAG_DF8100_REVERSE2                 	    (flag_DF8100_CTL.bit2)
#define FLAG_DF8100_REVERSE3                 	    (flag_DF8100_CTL.bit3)
#define FLAG_DF8100_REVERSE4                 	    (flag_DF8100_CTL.bit4)
#define FLAG_DF8100_REVERSE5                 	    (flag_DF8100_CTL.bit5)
#define FLAG_DF8100_REVERSE6                 	    (flag_DF8100_CTL.bit6)
#define FLAG_DF8100_REVERSE7                 	    (flag_DF8100_CTL.bit7)


/*_____ D E F I N I T I O N S ______________________________________________*/
const unsigned char DF8100_addr_tbl[4] = 
{
	0x48,	//THR_ADR = 10 kΩ Resistor to GND 
	0x49,	//THR_ADR = 100 kΩ Resistor to VDD 
	0x4A,	//THR_ADR = GND 
	0x4B,	//THR_ADR = VDD 
};

enum
{
	GROUP_1 = 0,
	GROUP_2 ,
};

typedef struct ForceSensorEvent_t
{
    unsigned char Group;
    unsigned char SwitchChannel;
    unsigned char DetectValid;
    unsigned char DevAddr;
    // signed short AdcValue;
} ForceSensorEvent_t;

ForceSensorEvent_t event_force_sensor_1[4] = {0};
ForceSensorEvent_t event_force_sensor_2[4] = {0};

signed short force_sensor_data[8] = {0};
unsigned char force_sensor_cnt = 0;

/*_____ M A C R O S ________________________________________________________*/

/*_____ F U N C T I O N S __________________________________________________*/

unsigned char Get_DF8100_DisplayLog(void)
{
	return FLAG_DF8100_DISPLAY_LOG;
}	

void Set_DF8100_DisplayLog(unsigned char flag)
{
	FLAG_DF8100_DISPLAY_LOG = flag;
}

signed short convertToDecimal_12bit(signed short value) 
{
    const unsigned char bits = 12; // 12 bits for the 2's complement number
    int mask = 1 << (bits - 1); // mask for the sign bit

    if (value & mask) 	// negative
	{
		return  -(~value + 1) ;
    }
	else	// positive
	{
    	return value;
	}
}

signed short convertToDecimal(signed short value) 
{
    int bits = 12; // 12 bits for the 2's complement number
    int mask = 1 << (bits - 1); // mask for the sign bit

    // If the number is negative (sign bit is 1)
    if (value & mask) 
	{
        // Compute the 2's complement to get the negative value
        value = value - (1 << bits);
    }

    return value;
}


// TEST I2C communication
MD_STATUS DF8100_read_WAIT_REG(unsigned char device_addr) 	// default : 0xB5
{
	MD_STATUS ret = MD_OK;
    unsigned char tmp = 0;
    unsigned char device_addr_8bit = device_addr << 1;
    unsigned char reg_addr = WAIT_REG;

    ret = IIC10_I2C_read(device_addr_8bit,reg_addr, (unsigned char*) &tmp,1);
    if (ret != MD_OK)
    {
		#if defined (ENABLE_DF8100_ERR_LOG)
        printf("[error]%s:0x%02X\r\n\r\n",__func__,ret);
		#endif
        return ret;
    }

	#if defined (ENABLE_DF8100_DBG_LOG)
    printf("%s-addr:0x%02X,WAIT_REG:0x%02X\r\n",__func__,device_addr,tmp);
	#endif

	return ret;	
}

/*
	read Device and Revision ID (0x80) , default : 0x20
*/
MD_STATUS DF8100_read_DEVID_REVID(unsigned char device_addr,unsigned char *ni_ID) 	// default : 0x20
{
	MD_STATUS ret = MD_OK;
    unsigned char tmp = 0;
    unsigned char device_addr_8bit = device_addr << 1;
    unsigned char reg_addr = DEVID_REG;
	#if defined (ENABLE_DF8100_DBG_LOG)
	unsigned char DEVID = 0;
	unsigned char REVID = 0;
	#endif

    ret = IIC10_I2C_read(device_addr_8bit,reg_addr, (unsigned char*) &tmp,1);
    if (ret != MD_OK)
    {
		#if defined (ENABLE_DF8100_ERR_LOG)
        printf("[error]%s:0x%02X\r\n\r\n",__func__,ret);
		#endif
        return ret;
    }

	*ni_ID = tmp;

	#if defined (ENABLE_DF8100_DBG_LOG)
	DEVID = (tmp & DEVID_MSK) >> DEVID_POS;	// 00100
	REVID = (tmp & REVID_MSK) >> REVID_POS;	// 000
    printf("%s-addr:0x%02X,ID:0x%02X,DEVID:0x%02X,REVID:0x%02X\r\n",__func__,device_addr,*ni_ID,DEVID,REVID);
	#endif

	return ret;	
}


/*
	read  , 	
	0x03 : ADCOUT[15:8] 
	0x04 : ADCOUT[7:4] 

	[HIGHORDER]
	0*  ULP mode (and averaging Disabled) 
	1  HP mode (and averaging Enabled) 

	[ENFILTER]
	0*  ULP mode (and averaging Disabled)  
	1  HP mode (and averaging Enabled) 

	In ULP mode, the data rate is determined by the WAIT register. Data is updated after every “WAIT + 1ms” and the 
	ADCOUT is 12-bits 2’s complement which can be read from ADCOUT[15:4]

	ENFILTER = 0, use ADCOUT[15:4], -2048 to +2047 
	ENFILTER = 1 & HIGHORDER = 0, use ADCOUT[15:0] -32,768 to +32,767 
	ENFILTER = 1 & HIGHORDER = 1, use ADCOUT[15:0] 
*/

MD_STATUS DF8100_read_adc(unsigned char device_addr,signed short *ni_adc) 
{
	MD_STATUS ret = MD_OK;
    unsigned char tmp[2] = {0};
    unsigned char device_addr_8bit = device_addr << 1;
    unsigned char reg_addr = ADCOUT_REG;

    ret = IIC10_I2C_read(device_addr_8bit,reg_addr, (unsigned char*) &tmp,2);
    if (ret != MD_OK)
    {
		#if defined (ENABLE_DF8100_ERR_LOG)
        printf("[error]%s:0x%02X\r\n\r\n",__func__,ret);
		#endif
        return ret;
    }

	#if 1	// if ENFILTER = 0
	*ni_adc = (unsigned short)((tmp[0] << ADCOUT_SHIFT12) | (tmp[1] >> ADCOUT_SHIFT12));
	if (*ni_adc > 2047) 
	{
		*ni_adc |= 0xF000;
	}
	#else	// if ENFILTER = 1
	*ni_adc = (unsigned short)((tmp[0] << ADCOUT_SHIFT16) | (tmp[1]));
	#endif
	
	#if defined (ENABLE_DF8100_DBG_LOG)
    printf("%s-addr:0x%02X,adc:0x%04X\r\n",__func__,device_addr,*ni_adc);
	#endif

	return ret;
	
}

void force_sensor_print_data(void) 
{
	unsigned char i = 0;

	// display
	if (FLAG_DF8100_DISPLAY_LOG)
	{
		for( i = 0 ; i < force_sensor_cnt ; i++)
		{
			// printf("[0x%04X]",force_sensor_data[i]);	// hex
			// printf("[0x%04X,%5d]",force_sensor_data[i],force_sensor_data[i]);	// 12-bits 2’s complement
			// printf("[0x%04X,%5d]",force_sensor_data[i],convertToDecimal(force_sensor_data[i]));	// conver to decimal
			printf("[0x%04X,%5d]",force_sensor_data[i],convertToDecimal_12bit(force_sensor_data[i]));	// conver to decimal

			// printf("[0x%04X,%5d,%5d,%5d]",
			// force_sensor_data[i],
			// force_sensor_data[i],
			// convertToDecimal(force_sensor_data[i]),
			// convertToDecimal_12bit(force_sensor_data[i]));	// conver to decimal
		}
		
		printf("\r\n");
	}
}

MD_STATUS force_sensor_task(void) 
{
	static unsigned char flag = 0;
	unsigned char cnt = 0;
	MD_STATUS ret = MD_BUSY2;	

	#if 0
	signed short tmp = 0;
	
	TCA9548A_2_SetChannel(3);
	DF8100_read_adc(0x48 ,&tmp);
	ary[i++] = tmp;
	DF8100_read_adc(0x49 ,&tmp);
	ary[i++] = tmp;
	DF8100_read_adc(0x4B ,&tmp);
	ary[i++] = tmp;

	
	TCA9548A_2_SetChannel(8);
	DF8100_read_adc(0x4A ,&tmp);
	ary[i++] = tmp;
	DF8100_read_adc(0x4B ,&tmp);
	ary[i++] = tmp;

	i = 0;
	for ( i = 0; i < 5 ; i++)
	{
		printf("ary[%d]:0x%04X,",i,ary[i]);
	}
	printf("\r\n");

	#else
	unsigned char i = 0;
	signed short tmp = 0;
	unsigned char sensor_group_size = 0;

	if (!DF8100_is_Init_Ready())
    {
        // init fail
		if (!flag)
		{
			flag = 1;
			printf("force init fail\r\n");
		}
        return ret;
    }

	cnt = 0;

	// reset data array
	for( i = 0 ; i < sizeof(force_sensor_data)/(sizeof(force_sensor_data[0])) ; i++)
	{
		force_sensor_data[i] = 0;
	}

	// get first group 
	ret = TCA9548A_2_SetChannel(3);
	sensor_group_size = sizeof(event_force_sensor_1)/(sizeof(event_force_sensor_1[0]));
	for( i = 0 ; i < sensor_group_size ; i ++)
	{
		if ((event_force_sensor_1[i].DetectValid) && 
			(event_force_sensor_1[i].DevAddr != 0x00))	// store into array if addr valid
		{
			ret = DF8100_read_adc(event_force_sensor_1[i].DevAddr ,&tmp);
			// event_force_sensor_1[i].AdcValue = tmp;
			force_sensor_data[cnt++] = tmp;
		}
	}

	// get second group
	ret = TCA9548A_2_SetChannel(8);
	sensor_group_size = sizeof(event_force_sensor_2)/(sizeof(event_force_sensor_2[0]));
	for( i = 0 ; i < sensor_group_size ; i ++)
	{
		if ((event_force_sensor_2[i].DetectValid) && 
			(event_force_sensor_2[i].DevAddr != 0x00))	// store into array if addr valid
		{
			ret = DF8100_read_adc(event_force_sensor_2[i].DevAddr ,&tmp);
			// event_force_sensor_2[i].AdcValue = tmp;
			force_sensor_data[cnt++] = tmp;
		}
	}

	#endif

	return ret;
}


void DF8100_set_Init_Flag(bool flag)
{
    FLAG_FORCE_INIT_DF8100_RDY = flag;
}

bool DF8100_is_Init_Ready(void)
{
    return FLAG_FORCE_INIT_DF8100_RDY;
}

MD_STATUS DF8100_Get_ID(void)
{
	unsigned char i = 0;
	unsigned char ch = 0;
	unsigned char cnt = 0;
	unsigned char tmp = 0;
	unsigned char sensor_group_size = 0;
	MD_STATUS ret = MD_OK;	

	ch = 3;
	ret = TCA9548A_2_SetChannel(ch);
	sensor_group_size = sizeof(event_force_sensor_1)/(sizeof(event_force_sensor_1[0]));
	for( i = 0 ; i < sensor_group_size ; i ++)
	{
		if ((event_force_sensor_1[i].DetectValid) && 
			(event_force_sensor_1[i].DevAddr != 0x00))
		{
			ret = DF8100_read_DEVID_REVID(event_force_sensor_1[i].DevAddr ,&tmp);
			cnt++;

			printf("G1,addr:0x%2X,ID:0x%02X\r\n",
			event_force_sensor_1[i].DevAddr,
			tmp);
		}
	}

	ch = 8;
	ret = TCA9548A_2_SetChannel(ch);
	sensor_group_size = sizeof(event_force_sensor_2)/(sizeof(event_force_sensor_2[0]));
	for( i = 0 ; i < sensor_group_size ; i ++)
	{
		if ((event_force_sensor_2[i].DetectValid) && 
			(event_force_sensor_2[i].DevAddr != 0x00))
		{
			ret = DF8100_read_DEVID_REVID(event_force_sensor_2[i].DevAddr ,&tmp);
			cnt++;
			
			printf("G2,addr:0x%2X,ID:0x%02X\r\n",
			event_force_sensor_2[i].DevAddr,
			tmp);
		}
	}

	force_sensor_cnt = cnt;
	printf("%s-available sensor:%d\r\n\r\n",__func__ ,cnt);	

    return ret;	
}

MD_STATUS DF8100_Do_Scan(void)
{
	unsigned char i = 0;
	unsigned char ch = 0;
	unsigned char cnt = 0;
	unsigned char addr_ary_size = sizeof(DF8100_addr_tbl)/(sizeof(DF8100_addr_tbl[0]));
	MD_STATUS ret = MD_OK;	

	// test I2C communication
	#if 1
	ch = 3;
	ret = TCA9548A_2_SetChannel(ch);
	for ( i = 0; i < addr_ary_size ; i++)	// check each add if available
	{			
		event_force_sensor_1[i].Group = GROUP_1;
		event_force_sensor_1[i].SwitchChannel = ch;

		if (DF8100_read_WAIT_REG(DF8100_addr_tbl[i]) == MD_OK)
		{
			event_force_sensor_1[i].DetectValid = 1;
			event_force_sensor_1[i].DevAddr = DF8100_addr_tbl[i];

			printf("[G1:%d][0x%02X][OK]\r\n",cnt,event_force_sensor_1[i].DevAddr);
			cnt++;
		}
		else
		{
			event_force_sensor_1[i].DetectValid = 0;
			event_force_sensor_1[i].DevAddr = 0x00;

			printf("[G1:%d][0x%02X][NG]\r\n",cnt,DF8100_addr_tbl[i]);
		}
	}
	#endif

	#if 1
	ch = 8;
	ret = TCA9548A_2_SetChannel(ch);
	for ( i = 0; i < addr_ary_size ; i++)	// check each add if available
	{
		event_force_sensor_2[i].Group = GROUP_2;
		event_force_sensor_2[i].SwitchChannel = ch;

		if (DF8100_read_WAIT_REG(DF8100_addr_tbl[i]) == MD_OK)
		{			
			event_force_sensor_2[i].DetectValid = 1;
			event_force_sensor_2[i].DevAddr = DF8100_addr_tbl[i];

			printf("[G2:%d][0x%02X][OK]\r\n",cnt,event_force_sensor_2[i].DevAddr);
			cnt++;
		}
		else
		{
			event_force_sensor_2[i].DetectValid = 0;
			event_force_sensor_2[i].DevAddr = 0x00;
			printf("[G2:%d][0x%02X][NG]\r\n",cnt,DF8100_addr_tbl[i]);
		}
	}
	#endif

	force_sensor_cnt = cnt;
	printf("%s-available sensor:%d\r\n\r\n",__func__ ,cnt);

    return ret;	
}

void DF8100_init(void) // use IC default ?
{

	Set_DF8100_DisplayLog(0);	// default display
}

void Init_all_DF8100_cfg(void)
{ 	
	MD_STATUS ret = MD_OK;	

    DF8100_init();  

	ret = DF8100_Do_Scan();	
    if (ret != MD_OK)
    {
        DF8100_set_Init_Flag(0);
    }
    else
    {
        DF8100_set_Init_Flag(1);
    }

	ret = DF8100_Get_ID();
    if (ret != MD_OK)
    {
        DF8100_set_Init_Flag(0);
    }
    else
    {
        DF8100_set_Init_Flag(1);
    }

}



