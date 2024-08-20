/*_____ I N C L U D E S ____________________________________________________*/
#include <stdio.h>

/*_____ D E C L A R A T I O N S ____________________________________________*/

/*_____ D E F I N I T I O N S ______________________________________________*/

#define TCA9548A_ADDR_7BIT 								(0x70)
#define TCA9548A_ADDR_8BIT 								(TCA9548A_ADDR_7BIT << 1)	// 0xE0

/*
	BIT 	7	6	5	4	3	2	1	0
	CH		7	6	5	4	3	2	1	0
			8 	4 	2 	1	8	4	2	1

*/

#define TCA9548A_CHANNEL_0 								(0x1)
#define TCA9548A_CHANNEL_1 								(0x2)
#define TCA9548A_CHANNEL_2 								(0x4)
#define TCA9548A_CHANNEL_3 								(0x8)
#define TCA9548A_CHANNEL_4 								(0x10)
#define TCA9548A_CHANNEL_5 								(0x20)
#define TCA9548A_CHANNEL_6 								(0x40)
#define TCA9548A_CHANNEL_7 								(0x80)

#define TCA9548A_CHANNEL_ALL 							(0xFF)

/*_____ M A C R O S ________________________________________________________*/

/*_____ F U N C T I O N S __________________________________________________*/

void TCA9548A_reset(unsigned char rst);
void TCA9548A_2_reset(unsigned char rst);

MD_STATUS TCA9548A_SetChannel(unsigned char channel);
MD_STATUS TCA9548A_2_SetChannel(unsigned char channel);


