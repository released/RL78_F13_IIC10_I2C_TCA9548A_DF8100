/*_____ I N C L U D E S ____________________________________________________*/
#include <stdio.h>
#include "r_cg_userdefine.h"

#include "misc_config.h"
#include "custom_func.h"

#include "simple_i2c_driver.h"
#include "IICA0_slave_driver.h"
#include "TCA9548A_driver.h"
#include "button_sw_debounce.h"
#include "simple_DF8100_driver.h"

/*_____ D E C L A R A T I O N S ____________________________________________*/

struct flag_8bit flag_PROJ_CTL;
#define FLAG_PROJ_TIMER_PERIOD_1000MS                 	(flag_PROJ_CTL.bit0)
#define FLAG_PROJ_TIMER_PERIOD_FORCE_SENSOR             (flag_PROJ_CTL.bit1)
#define FLAG_PROJ_REVERSE2                 	            (flag_PROJ_CTL.bit2)
#define FLAG_PROJ_REVERSE3                              (flag_PROJ_CTL.bit3)
#define FLAG_PROJ_REVERSE4                              (flag_PROJ_CTL.bit4)
#define FLAG_PROJ_REVERSE5                              (flag_PROJ_CTL.bit5)
#define FLAG_PROJ_REVERSE6                              (flag_PROJ_CTL.bit6)
#define FLAG_PROJ_REVERSE7                              (flag_PROJ_CTL.bit7)

/*_____ D E F I N I T I O N S ______________________________________________*/

volatile unsigned long counter_tick = 0;
volatile unsigned long btn_counter_tick = 0;
volatile unsigned long counter_tau1_ch0_tick = 0;

// unsigned char key_buffer = 0;
unsigned char flag_play = 0;

#define BTN_PRESSED_LONG                                (2500)

#define BUILD_TIME()                                    {printf("[build time]%s,%s\r\n", __DATE__ ,__TIME__ );}

/*_____ M A C R O S ________________________________________________________*/

/*_____ F U N C T I O N S __________________________________________________*/

unsigned long btn_get_tick(void)
{
	return (btn_counter_tick);
}

void btn_set_tick(unsigned long t)
{
	btn_counter_tick = t;
}

void btn_tick_counter(void)
{
	btn_counter_tick++;
    if (btn_get_tick() >= 60000)
    {
        btn_set_tick(0);
    }
}

unsigned long get_tick(void)
{
	return (counter_tick);
}

void set_tick(unsigned long t)
{
	counter_tick = t;
}

void tick_counter(void)
{
	counter_tick++;
    if (get_tick() >= 60000)
    {
        set_tick(0);
    }
}


void Timer_TAU1_Channel0_1ms_IRQ(void)
{
    counter_tau1_ch0_tick++;
}


void delay_ms(unsigned long ms)
{
	#if 1

    R_TAU1_Channel0_Start();
    counter_tau1_ch0_tick = 0;

    while(counter_tau1_ch0_tick < ms);

    R_TAU1_Channel0_Stop();

    #else
    unsigned long tickstart = get_tick();
    unsigned long wait = ms;
	unsigned long tmp = 0;
	
    while (1)
    {
		if (get_tick() > tickstart)	// tickstart = 59000 , tick_counter = 60000
		{
			tmp = get_tick() - tickstart;
		}
		else // tickstart = 59000 , tick_counter = 2048
		{
			tmp = 60000 -  tickstart + get_tick();
		}		
		
		if (tmp > wait)
			break;
    }
	
	#endif
}

/*
    SCL	    23 : P62/SCL0
    SDA	    24 : P63/SDA0
    GPIO	40 : P32/INTP7

    SCL	    54 : P10/SCL10
    SDA	    53 : P11/SDA10
    GPIO	29 : P00/INTP9
*/


void Timer_1ms_IRQ(void)
{
    tick_counter();

    if ((get_tick() % 1000) == 0)
    {
        FLAG_PROJ_TIMER_PERIOD_1000MS = 1;
    }

    if ((get_tick() % 2000) == 0)
    {

    }	

    if ((get_tick() % 60) == 0)
    {
        FLAG_PROJ_TIMER_PERIOD_FORCE_SENSOR = 1;
    }	

    if ((get_tick() % 50) == 0)
    {

    }	

    btn_timer_irq();

}


/*
    F13 target board
    LED1 connected to P66, LED2 connected to P67
*/
// void LED_Toggle(void)
// {
//     // PIN_WRITE(6,6) = ~PIN_READ(6,6);
//     // PIN_WRITE(6,7) = ~PIN_READ(6,7);
//     P6_bit.no6 = ~P6_bit.no6;
//     P6_bit.no7 = ~P6_bit.no7;
// }

void loop(void)
{
	static unsigned long LOG1 = 0;
	static unsigned long specific_cnt = 0;

    if (FLAG_PROJ_TIMER_PERIOD_1000MS)
    {
        FLAG_PROJ_TIMER_PERIOD_1000MS = 0;

        if (++specific_cnt == 60)
        {
            printf("log(timer):%4d\r\n",LOG1++);
            specific_cnt = 0;
        }          
    }

    btn_task();
    
    force_sensor_task();
    #if 1
    if (FLAG_PROJ_TIMER_PERIOD_FORCE_SENSOR && Get_DF8100_DisplayLog())
    {
        FLAG_PROJ_TIMER_PERIOD_FORCE_SENSOR = 0;
        force_sensor_print_data();
    }
    #endif

}

// F24 EVB , P137/INTP0 , set both edge 
// void Button_Process_long_counter(void)
// {
//     if (FLAG_PROJ_TRIG_BTN2)
//     {
//         btn_tick_counter();
//     }
//     else
//     {
//         btn_set_tick(0);
//     }
// }

// void Button_Process_in_polling(void)
// {
//     static unsigned char cnt = 0;

//     if (FLAG_PROJ_TRIG_BTN1)
//     {
//         FLAG_PROJ_TRIG_BTN1 = 0;
//         printf("BTN pressed(%d)\r\n",cnt);

//         if (cnt == 0)
//         {
//             FLAG_PROJ_TRIG_BTN2 = 1;
//         }
//         else if (cnt == 1)
//         {
//             FLAG_PROJ_TRIG_BTN2 = 0;
//         }

//         cnt = (cnt >= 1) ? (0) : (cnt+1) ;
//     }

//     if ((FLAG_PROJ_TRIG_BTN2 == 1) && 
//         (btn_get_tick() > BTN_PRESSED_LONG))
//     {         
//         btn_set_tick(0);
//     }
// }

// F13 EVB , P137/INTP0
// void Button_Process_in_IRQ(void)    
// {
//     FLAG_PROJ_TRIG_BTN1 = 1;
// }

void UARTx_Process(unsigned char rxbuf)
{    
    unsigned char tmp = 0;

    if (rxbuf > 0x7F)
    {
        printf("invalid command\r\n");
    }
    else
    {
        printf("press:%c(0x%02X)\r\n" , rxbuf,rxbuf);   // %c :  C99 libraries.
        switch(rxbuf)
        {
            case 'd':
            case 'D':
                tmp = Get_DF8100_DisplayLog();
                tmp = ~tmp;
                Set_DF8100_DisplayLog(tmp);
                break;

            case 'X':
            case 'x':
                RL78_soft_reset(7);
                break;
            case 'Z':
            case 'z':
                RL78_soft_reset(1);
                break;
        }
    }
}


/*
    7:Internal reset by execution of illegal instruction
    1:Internal reset by illegal-memory access
*/
//perform sofware reset
void _reset_by_illegal_instruction(void)
{
    static const unsigned char illegal_Instruction = 0xFF;
    void (*dummy) (void) = (void (*)(void))&illegal_Instruction;
    dummy();
}
void _reset_by_illegal_memory_access(void)
{
    #if 1
    const unsigned char ILLEGAL_ACCESS_ON = 0x80;
    IAWCTL |= ILLEGAL_ACCESS_ON;            // switch IAWEN on (default off)
    *(__far volatile char *)0x00000 = 0x00; //write illegal address 0x00000(RESET VECTOR)
    #else
    signed char __far* a;                   // Create a far-Pointer
    IAWCTL |= _80_CGC_ILLEGAL_ACCESS_ON;    // switch IAWEN on (default off)
    a = (signed char __far*) 0x0000;        // Point to 0x000000 (FLASH-ROM area)
    *a = 0;
    #endif
}

void RL78_soft_reset(unsigned char flag)
{
    switch(flag)
    {
        case 7: // do not use under debug mode
            _reset_by_illegal_instruction();        
            break;
        case 1:
            _reset_by_illegal_memory_access();
            break;
    }
}


// retarget printf
int __far putchar(int c)
{
    // F13 , UART0
    STMK0 = 1U;    /* disable INTST0 interrupt */
    SDR00L = (unsigned char)c;
    while(STIF0 == 0)
    {

    }
    STIF0 = 0U;    /* clear INTST0 interrupt flag */
    return c;
}

void hardware_init(void)
{
    // const unsigned char indicator[] = "hardware_init";
    EI();   //BSP_EI();
    R_UART0_Start();            // P15:TXD0, P16:RXD0
    R_TMR_RJ0_Start();  

    /*
        The system master can reset the TCA9548A in the
        event of a time-out or other improper operation by
        asserting a low in the RESET input      
    */
   
    delay_ms(50);   // prevent 12V to 5V Pre-charge timing

    TCA9548A_2_reset(0);
    TCA9548A_2_reset(1);

    btn_init();

    /*
        I2C master (TCA9548A + force sensor) - IIC10
        54 : P10/SCL10
        53 : P11/SDA10        
    */

    Init_all_DF8100_cfg();
    
    BUILD_TIME();
    printf("\r\n%s finish\r\n",__func__);
}
