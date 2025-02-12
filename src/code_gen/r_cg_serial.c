/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
* No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY
* LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT,
* INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR
* ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability 
* of this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2012, 2021 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name    : r_cg_serial.c
* Version      : CodeGenerator for RL78/F13 V2.03.07.02 [08 Nov 2021]
* Device(s)    : R5F10BMG
* Tool-Chain   : CCRL
* Description  : This file implements device driver for Serial module.
* Creation Date: 2024/8/20
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_serial.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
volatile uint8_t * gp_uart0_tx_address;        /* uart0 transmit buffer address */
volatile uint16_t  g_uart0_tx_count;           /* uart0 transmit data number */
volatile uint8_t * gp_uart0_rx_address;        /* uart0 receive buffer address */
volatile uint16_t  g_uart0_rx_count;           /* uart0 receive data number */
volatile uint16_t  g_uart0_rx_length;          /* uart0 receive data length */
volatile uint8_t   g_iic10_master_status_flag; /* iic10 start flag for send address check by master mode */
volatile uint8_t * gp_iic10_tx_address;        /* iic10 send data pointer by master mode */
volatile uint16_t  g_iic10_tx_count;           /* iic10 send data size by master mode */
volatile uint8_t * gp_iic10_rx_address;        /* iic10 receive data pointer by master mode */
volatile uint16_t  g_iic10_rx_count;           /* iic10 receive data size by master mode */
volatile uint16_t  g_iic10_rx_length;          /* iic10 receive data length by master mode */
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_SAU0_Create
* Description  : This function initializes the SAU0 module.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_SAU0_Create(void)
{
    SAU0EN = 1U;    /* supply SAU0 clock */
    NOP();
    NOP();
    NOP();
    NOP();
    SPS0 = _0001_SAU_CK00_FCLK_1 | _0010_SAU_CK01_FCLK_1;
    R_UART0_Create();
}

/***********************************************************************************************************************
* Function Name: R_UART0_Create
* Description  : This function initializes the UART0 module.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_UART0_Create(void)
{
    ST0 |= _0002_SAU_CH1_STOP_TRG_ON | _0001_SAU_CH0_STOP_TRG_ON;    /* disable UART0 receive and transmit */
    STMK0 = 1U;    /* disable INTST0 interrupt */
    STIF0 = 0U;    /* clear INTST0 interrupt flag */
    SRMK0 = 1U;    /* disable INTSR0 interrupt */
    SRIF0 = 0U;    /* clear INTSR0 interrupt flag */
    /* Set INTST0 low priority */
    STPR10 = 1U;
    STPR00 = 1U;
    /* Set INTSR0 low priority */
    SRPR10 = 1U;
    SRPR00 = 1U;
    SMR00 = _0020_SAU_SMRMN_INITIALVALUE | _0000_SAU_CLOCK_SELECT_CK00 | _0000_SAU_TRIGGER_SOFTWARE |
            _0002_SAU_MODE_UART | _0000_SAU_TRANSFER_END;
    SCR00 = _8000_SAU_TRANSMISSION | _0000_SAU_PARITY_NONE | _0080_SAU_LSB | _0010_SAU_STOP_1 | _0007_SAU_LENGTH_8;
    SDR00 = _8800_UART0_TRANSMIT_DIVISOR;
    NFEN0 |= _01_SAU_RXD0_FILTER_ON;
    SIR01 = _0004_SAU_SIRMN_FECTMN | _0002_SAU_SIRMN_PECTMN | _0001_SAU_SIRMN_OVCTMN;    /* clear error flag */
    SMR01 = _0020_SAU_SMRMN_INITIALVALUE | _0000_SAU_CLOCK_SELECT_CK00 | _0100_SAU_TRIGGER_RXD | _0000_SAU_EDGE_FALL |
            _0002_SAU_MODE_UART | _0000_SAU_TRANSFER_END;
    SCR01 = _4000_SAU_RECEPTION | _0000_SAU_PARITY_NONE | _0080_SAU_LSB | _0010_SAU_STOP_1 | _0007_SAU_LENGTH_8;
    SDR01 = _8800_UART0_RECEIVE_DIVISOR;
    SO0 |= _0001_SAU_CH0_DATA_OUTPUT_1;
    SOL0 |= _0000_SAU_CHANNEL0_NORMAL;    /* output level normal */
    SOE0 |= _0001_SAU_CH0_OUTPUT_ENABLE;    /* enable UART0 output */
    /* Set RxD0 pin */
    PM1 |= 0x40U;
    /* Set TxD0 pin */
    P1 |= 0x20U;
    PM1 &= 0xDFU;
}

/***********************************************************************************************************************
* Function Name: R_UART0_Start
* Description  : This function starts the UART0 module operation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_UART0_Start(void)
{
    SO0 |= _0001_SAU_CH0_DATA_OUTPUT_1;    /* output level normal */
    SOE0 |= _0001_SAU_CH0_OUTPUT_ENABLE;    /* enable UART0 output */
    SS0 |= _0002_SAU_CH1_START_TRG_ON | _0001_SAU_CH0_START_TRG_ON;    /* enable UART0 receive and transmit */
    STIF0 = 0U;    /* clear INTST0 interrupt flag */
    SRIF0 = 0U;    /* clear INTSR0 interrupt flag */
    STMK0 = 0U;    /* enable INTST0 interrupt */
    SRMK0 = 0U;    /* enable INTSR0 interrupt */
}

/***********************************************************************************************************************
* Function Name: R_UART0_Stop
* Description  : This function stops the UART0 module operation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_UART0_Stop(void)
{
    STMK0 = 1U;    /* disable INTST0 interrupt */
    SRMK0 = 1U;    /* disable INTSR0 interrupt */
    ST0 |= _0002_SAU_CH1_STOP_TRG_ON | _0001_SAU_CH0_STOP_TRG_ON;    /* disable UART0 receive and transmit */
    SOE0 &= ~_0001_SAU_CH0_OUTPUT_ENABLE;    /* disable UART0 output */
    STIF0 = 0U;    /* clear INTST0 interrupt flag */
    SRIF0 = 0U;    /* clear INTSR0 interrupt flag */
}

/***********************************************************************************************************************
* Function Name: R_UART0_Receive
* Description  : This function receives UART0 data.
* Arguments    : rx_buf -
*                    receive buffer pointer
*                rx_num -
*                    buffer size
* Return Value : status -
*                    MD_OK or MD_ARGERROR
***********************************************************************************************************************/
MD_STATUS R_UART0_Receive(uint8_t * const rx_buf, uint16_t rx_num)
{
    MD_STATUS status = MD_OK;

    if (rx_num < 1U)
    {
        status = MD_ARGERROR;
    }
    else
    {
        g_uart0_rx_count = 0U;
        g_uart0_rx_length = rx_num;
        gp_uart0_rx_address = rx_buf;
    }

    return (status);
}

/***********************************************************************************************************************
* Function Name: R_UART0_Send
* Description  : This function sends UART0 data.
* Arguments    : tx_buf -
*                    transfer buffer pointer
*                tx_num -
*                    buffer size
* Return Value : status -
*                    MD_OK or MD_ARGERROR
***********************************************************************************************************************/
MD_STATUS R_UART0_Send(uint8_t * const tx_buf, uint16_t tx_num)
{
    MD_STATUS status = MD_OK;

    if (tx_num < 1U)
    {
        status = MD_ARGERROR;
    }
    else
    {
        gp_uart0_tx_address = tx_buf;
        g_uart0_tx_count = tx_num;
        STMK0 = 1U;    /* disable INTST0 interrupt */
        SDR00L = *gp_uart0_tx_address;
        gp_uart0_tx_address++;
        g_uart0_tx_count--;
        STMK0 = 0U;    /* enable INTST0 interrupt */
    }

    return (status);
}

/***********************************************************************************************************************
* Function Name: R_SAU1_Create
* Description  : This function initializes the SAU1 module.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_SAU1_Create(void)
{
    SAU1EN = 1U;    /* supply SAU1 clock */
    NOP();
    NOP();
    NOP();
    NOP();
    SPS1 = _0000_SAU_CK00_FCLK_0 | _0000_SAU_CK01_FCLK_0;
    R_IIC10_Create();
}

/***********************************************************************************************************************
* Function Name: R_IIC10_Create
* Description  : This function initializes the IIC10 module.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_IIC10_Create(void)
{
    ST1 |= _0001_SAU_CH0_STOP_TRG_ON;    /* disable IIC10 */
    IICMK10 = 1U;    /* disable INTIIC10 interrupt */
    IICIF10 = 0U;    /* clear INTIIC10 interrupt flag */
    /* Set INTIIC10 low priority */
    IICPR110 = 1U;
    IICPR010 = 1U;
    SIR10 = _0004_SAU_SIRMN_FECTMN | _0002_SAU_SIRMN_PECTMN | _0001_SAU_SIRMN_OVCTMN;    /* clear error flag */
    SMR10 = _0020_SAU_SMRMN_INITIALVALUE | _0000_SAU_CLOCK_SELECT_CK00 | _0000_SAU_CLOCK_MODE_CKS |
            _0000_SAU_TRIGGER_SOFTWARE | _0000_SAU_EDGE_FALL | _0004_SAU_MODE_IIC | _0000_SAU_TRANSFER_END;
    SCR10 = _0000_SAU_TIMING_1 | _0000_SAU_PARITY_NONE | _0000_SAU_MSB | _0010_SAU_STOP_1 | _0007_SAU_LENGTH_8;
    SDR10 = _4E00_IIC10_DIVISOR;
    SO1 |= _0100_SAU_CH0_CLOCK_OUTPUT_1 | _0001_SAU_CH0_DATA_OUTPUT_1;
    /* Set SCL10, SDA10 pin */
    P1 |= 0x03U;
    PM1 &= 0xFCU;
}

/***********************************************************************************************************************
* Function Name: R_IIC10_Master_Send
* Description  : This function starts transferring data for IIC10 in master mode.
* Arguments    : adr -
*                    set address for select slave
*                tx_buf -
*                    transfer buffer pointer
*                tx_num -
*                    buffer size
* Return Value : None
***********************************************************************************************************************/
void R_IIC10_Master_Send(uint8_t adr, uint8_t * const tx_buf, uint16_t tx_num)
{
    g_iic10_master_status_flag = _00_SAU_IIC_MASTER_FLAG_CLEAR;    /* clear IIC10 master status flag */
    adr &= 0xFEU;    /* send mode */
    g_iic10_master_status_flag = _01_SAU_IIC_SEND_FLAG;            /* set master status flag */
    SCR10 &= ~_C000_SAU_RECEPTION_TRANSMISSION;
    SCR10 |= _8000_SAU_TRANSMISSION;
    /* Set paramater */
    g_iic10_tx_count = tx_num;
    gp_iic10_tx_address = tx_buf;
    /* Start condition */
    R_IIC10_StartCondition();
    IICIF10 = 0U;   /* clear INTIIC10 interrupt flag */
    IICMK10 = 0U;   /* enable INTIIC10 */
    SDR10L = adr;
}

/***********************************************************************************************************************
* Function Name: R_IIC10_Master_Receive
* Description  : This function starts receiving data for IIC10 in master mode.
* Arguments    : adr -
*                    set address for select slave
*                rx_buf -
*                    receive buffer pointer
*                rx_num -
*                    buffer size
* Return Value : None
***********************************************************************************************************************/
void R_IIC10_Master_Receive(uint8_t adr, uint8_t * const rx_buf, uint16_t rx_num)
{
    g_iic10_master_status_flag = _00_SAU_IIC_MASTER_FLAG_CLEAR;    /* clear master status flag */
    adr |= 0x01U;    /* receive mode */
    g_iic10_master_status_flag = _02_SAU_IIC_RECEIVE_FLAG;         /* set master status flag */
    SCR10 &= ~_C000_SAU_RECEPTION_TRANSMISSION;
    SCR10 |= _8000_SAU_TRANSMISSION;
    /* Set parameter */
    g_iic10_rx_length = rx_num;
    g_iic10_rx_count = 0U;
    gp_iic10_rx_address = rx_buf;
    /* Start condition */
    R_IIC10_StartCondition();
    IICIF10 = 0U;   /* clear INTIIC10 interrupt flag */
    IICMK10 = 0U;   /* enable INTIIC10 */
    SDR10L = adr;
}

/***********************************************************************************************************************
* Function Name: R_IIC10_Stop
* Description  : This function stops the IIC10 operation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_IIC10_Stop(void)
{
    /* Stop transfer */
    IICMK10 = 1U;    /* disable INTIIC10 */
    ST1 |= _0001_SAU_CH0_STOP_TRG_ON;    /* disable IIC10 */
    IICIF10 = 0U;    /* clear INTIIC10 interrupt flag */
}

/***********************************************************************************************************************
* Function Name: R_IIC10_StartCondition
* Description  : This function starts IIC10 condition.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_IIC10_StartCondition(void)
{
    volatile uint8_t w_count;
    
    SO1 &= ~_0001_SAU_CH0_DATA_OUTPUT_1;    /* clear IIC10 SDA */
    
    /* Set delay to secure a hold time after SDA output low. The delay time depend on slave device.
       Here set 5us as default base on current clock */
    for (w_count = 0U; w_count <= IIC10_WAITTIME; w_count++)
    {
        NOP();
    }
    
    SO1 &= ~_0100_SAU_CH0_CLOCK_OUTPUT_1;    /* clear IIC10 SCL */
    SOE1 |= _0001_SAU_CH0_OUTPUT_ENABLE;            /* enable IIC10 output */
    SS1 |= _0001_SAU_CH0_START_TRG_ON;              /* enable IIC10 */

    /* Set delay to secure a hold time after SCL output low. The delay time depend on slave device.
       Here set 5us as default base on current clock */
    for (w_count = 0U; w_count <= IIC10_WAITTIME; w_count++)
    {
        NOP();
    }
}

/***********************************************************************************************************************
* Function Name: R_IIC10_StopCondition
* Description  : This function stops IIC10 condition.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_IIC10_StopCondition(void)
{
    volatile uint8_t w_count;
    
    ST1 |= _0001_SAU_CH0_STOP_TRG_ON;           /* disable IIC10 */
    SOE1 &= ~_0001_SAU_CH0_OUTPUT_ENABLE;       /* disable IIC10 output */
    SO1 &= ~_0001_SAU_CH0_DATA_OUTPUT_1;        /* clear IIC10 SDA */

    /* Set delay to secure a hold time after SDA output low. The delay time depend on slave device.
       Here set 5us as default base on current clock */
    for (w_count = 0U; w_count <= IIC10_WAITTIME; w_count++)
    {
        NOP();
    }

    SO1 |= _0100_SAU_CH0_CLOCK_OUTPUT_1; /* set IIC10 SCL */
    
    /* Set delay to secure a hold time after SCL output high. The delay time depend on slave device.
       Here set 5us as default base on current clock */
    for (w_count = 0U; w_count <= IIC10_WAITTIME; w_count++)
    {
        NOP();
    }
    
    SO1 |= _0001_SAU_CH0_DATA_OUTPUT_1;         /* set IIC10 SDA */

    /* Set delay to secure a hold time after SDA output high. The delay time depend on slave device.
       Here set 5us as default base on current clock */
    for (w_count = 0U; w_count <= IIC10_WAITTIME; w_count++)
    {
        NOP();
    }
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
