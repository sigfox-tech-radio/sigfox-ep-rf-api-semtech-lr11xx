/*!*****************************************************************
 * \file    lr11xx_rf_api.h
 * \brief   Sigfox lr11xx RF api.
 *******************************************************************
 * \copyright
 *
 * Copyright (c) 2022, UnaBiz SAS
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  1 Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  2 Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  3 Neither the name of UnaBiz SAS nor the names of its contributors may be
 *    used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY
 * THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 *******************************************************************/
 
#ifndef __LR11XX_HW_H__
#define __LR11XX_HW_H__

#ifdef USE_SIGFOX_EP_FLAGS_H
#include "sigfox_ep_flags.h"
#endif
#include "sigfox_types.h"

/*** LR11XX HW API structures ***/

#define LR11XX_HW_API_RFSW0_DIO5  ( 1 << 0 )
#define LR11XX_HW_API_RFSW1_DIO6  ( 1 << 1 )
#define LR11XX_HW_API_RFSW2_DIO7  ( 1 << 2 )
#define LR11XX_HW_API_RFSW3_DIO8  ( 1 << 3 )
#define LR11XX_HW_API_RFSW4_DIO10 ( 1 << 4 )


/*!******************************************************************
 * \enum LR11XX_HW_API_FEM_t
 * \brief Front end module configuration strucure.
 *******************************************************************/
typedef enum
{    
    LR11XX_HW_API_FEM_PIN_USED = 1,
    LR11XX_HW_API_FEM_STBY   ,
    LR11XX_HW_API_FEM_RX     ,
    LR11XX_HW_API_FEM_TX     ,
    LR11XX_HW_API_FEM_TXHP   ,
    LR11XX_HW_API_FEM_WIFI   ,
    LR11XX_HW_API_FEM_GNSS   ,
} LR11XX_HW_API_FEM_t;

#if (defined TIMER_REQUIRED) && (defined LATENCY_COMPENSATION)
/*!******************************************************************
 * \enum LR11XX_HW_API_latency_t
 * \brief LR11XX hardware functions latency delay type.
 *******************************************************************/
typedef enum {
	LR11XX_HW_API_LATENCY_RESET = 0,
	LR11XX_HW_API_LATENCY_WAKEUP,
	LR11XX_HW_API_LATENCY_LAST
} LR11XX_HW_API_latency_t;
#endif

/********************************
 * \brief LR11xx driver callback functions.
 * \fn LR11XX_HW_irq_cb_t To be called when a rising edge is detected on the DIO9 pin.
 *******************************/
typedef void (LR11XX_HW_irq_cb_t)(void);

/*** LR11XX HW API functions ***/

/*!******************************************************************
 * \fn void LR11XX_HW_API_open(LR11XX_HW_irq_cb_t LR11XX_HW_irq_cb_t);
 * \brief Open the LR11XX hardware interface. This function is called during RF_API_open() function of the manufacturer layer.
 * \param[in]  	LR11XX_HW_irq_cb_t: GPIO interrupt callback that must be called on LR11XX GPIOx pin interrupt.
 * \param[out] 	none
 * \retval		void.
 *******************************************************************/
void LR11XX_HW_API_open(LR11XX_HW_irq_cb_t callback);

/*!******************************************************************
 * \fn void LR11XX_HW_API_close(void)
 * \brief Close the LR11XX hardware interface. This function is called during RF_API_close() function of the manufacturer layer.
 * \param[in]  	none
 * \param[out] 	none
 * \retval		void.
 *******************************************************************/
void LR11XX_HW_API_close(void);

/*!******************************************************************
 * \fn void LR11XX_HW_API_delayMs(unsigned short delay_ms);
 * \brief Delay. This function is called during multiples function of the manufacturer layer.
 * \param[in]  	delay_ms: delay in ms
 * \param[out] 	none
 * \retval		void.
 *******************************************************************/
void LR11XX_HW_API_delayMs(unsigned short delay_ms);

/*!******************************************************************

 * \fn void LR11XX_HW_API_get_fem_mask(LR11XX_HW_API_FEM_t fem, sfx_u8 *rfsw_dio_mask);
 * \brief  Lock configuration of several pins for a dedicated port.
 * \param[in]  fem front end module or switch configuration.
 * \param[out] rfsw_dio_mask This parameter can be a combination of the following values:
 *      \arg \ref LR11XX_HW_API_RFSW0_DIO5
 *      \arg \ref LR11XX_HW_API_RFSW1_DIO6
 *      \arg \ref LR11XX_HW_API_RFSW2_DIO7
 *      \arg \ref LR11XX_HW_API_RFSW3_DIO8
 *      \arg \ref LR11XX_HW_API_RFSW4_DIO10
 * \retval void
 *******************************************************************/
void LR11XX_HW_API_get_fem_mask(LR11XX_HW_API_FEM_t fem, sfx_u8 *rfsw_dio_mask);

/*!******************************************************************
 * \fn void LR11XX_HW_API_tx_on(void);
 * \brief Radio chipset will be start the TX.
 * \param[in]  	none
 * \param[out] 	none
 * \retval		void.
 *******************************************************************/
void LR11XX_HW_API_tx_on(void);

/*!******************************************************************
 * \fn void LR11XX_HW_API_tx_off(void);
 * \brief Radio chipset just stopped the TX.
 * \param[in]  	none
 * \param[out] 	none
 * \retval		void.
 *******************************************************************/
void LR11XX_HW_API_tx_off(void);

/*!******************************************************************
 * \fn void LR11XX_HW_API_rx_on(void);
 * \brief Radio chipset will be start the RX.
 * \param[in]  	none
 * \param[out] 	none
 * \retval		void.
 *******************************************************************/
void LR11XX_HW_API_rx_on(void);

/*!******************************************************************
 * \fn void LR11XX_HW_API_rx_off(void);
 * \brief Radio chipset just stopped the RX.
 * \param[in]  	none
 * \param[out] 	none
 * \retval		void.
 *******************************************************************/
void LR11XX_HW_API_rx_off(void);

#if (defined TIMER_REQUIRED) && (defined LATENCY_COMPENSATION)
/*!******************************************************************
 * \fn void LR11XX_HW_API_get_latency(LR11XX_HW_API_latency_t latency_type, sfx_u32 *latency_ms)
 * \brief Read HW functions latency in milliseconds.
 * \param[in]	latency_type: Type of latency to get.
 * \param[out] 	latency_ms: Pointer to integer that will contain the latency in milliseconds.
 * \retval		void.
 *******************************************************************/
void LR11XX_HW_API_get_latency(LR11XX_HW_API_latency_t latency_type, sfx_u32 *latency_ms);
#endif


#endif /* __LR11XX_RF_API_H__ */
