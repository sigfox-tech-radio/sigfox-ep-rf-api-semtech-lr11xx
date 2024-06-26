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
 
#include "board/lr11xx_hw_api.h"

#ifdef USE_SIGFOX_EP_FLAGS_H
#include "sigfox_ep_flags.h"
#endif
// Sigfox EP library.
#include "sigfox_error.h"
#include "sigfox_types.h"

LR11XX_HW_API_status_t __attribute__((weak)) LR11XX_HW_API_open(LR11XX_HW_irq_cb_t callback) {
    /* To be implemented by the device manufacturer */
#ifdef ERROR_CODES
    LR11XX_HW_API_status_t status = LR11XX_HW_API_SUCCESS;
#endif
    SFX_UNUSED(callback);
    // Configure all hardware pin of LR11XX chipset.
    // Configure SPI peripherial.
    // Configure interrupt pin to handle radio interrupt.
    // The callback function must be called when such event occurs.
    RETURN();
}

LR11XX_HW_API_status_t __attribute__((weak)) LR11XX_HW_API_close(void) {
    /* To be implemented by the device manufacturer */
#ifdef ERROR_CODES
    LR11XX_HW_API_status_t status = LR11XX_HW_API_SUCCESS;
#endif
    //Release all pins and peripherial opened.
    RETURN();
}

LR11XX_HW_API_status_t __attribute__((weak)) LR11XX_HW_API_delayMs(unsigned short delay_ms) {
	/* To be implemented by the device manufacturer */
#ifdef ERROR_CODES
    LR11XX_HW_API_status_t status = LR11XX_HW_API_SUCCESS;
#endif
    SFX_UNUSED(delay_ms);
    RETURN();
}

LR11XX_HW_API_status_t __attribute__((weak)) LR11XX_HW_API_get_fem_mask(LR11XX_HW_API_FEM_t fem, sfx_u8 *rfsw_dio_mask) {
	/* To be implemented by the device manufacturer */
#ifdef ERROR_CODES
    LR11XX_HW_API_status_t status = LR11XX_HW_API_SUCCESS;
#endif
    SFX_UNUSED(fem);
    SFX_UNUSED(rfsw_dio_mask);
    RETURN();
}

LR11XX_HW_API_status_t __attribute__((weak)) LR11XX_HW_API_get_xosc_cfg(LR11XX_HW_API_xosc_cfg_t *xosc_cfg) {
    /* To be implemented by the device manufacturer */
#ifdef ERROR_CODES
    LR11XX_HW_API_status_t status = LR11XX_HW_API_SUCCESS;
#endif
    SFX_UNUSED(xosc_cfg);
    RETURN();
}

LR11XX_HW_API_status_t __attribute__((weak)) LR11XX_HW_API_get_pa_pwr_cfg(LR11XX_HW_API_pa_pwr_cfg_t *pa_pwr_cfg, sfx_u32 rf_freq_in_hz, sfx_s8 expected_output_pwr_in_dbm){
    /* To be implemented by the device manufacturer */
#ifdef ERROR_CODES
    LR11XX_HW_API_status_t status = LR11XX_HW_API_SUCCESS;
#endif
    SFX_UNUSED(pa_pwr_cfg);
    SFX_UNUSED(rf_freq_in_hz);
    SFX_UNUSED(expected_output_pwr_in_dbm);
    RETURN();
}

LR11XX_HW_API_status_t __attribute__((weak)) LR11XX_HW_API_tx_on(void) {
	/* To be implemented by the device manufacturer */
#ifdef ERROR_CODES
    LR11XX_HW_API_status_t status = LR11XX_HW_API_SUCCESS;
#endif
    RETURN();
}

LR11XX_HW_API_status_t __attribute__((weak)) LR11XX_HW_API_tx_off(void) {
	/* To be implemented by the device manufacturer */
#ifdef ERROR_CODES
    LR11XX_HW_API_status_t status = LR11XX_HW_API_SUCCESS;
#endif
    RETURN();
}

LR11XX_HW_API_status_t __attribute__((weak)) LR11XX_HW_API_rx_on(void) {
	/* To be implemented by the device manufacturer */
#ifdef ERROR_CODES
    LR11XX_HW_API_status_t status = LR11XX_HW_API_SUCCESS;
#endif
    RETURN();
}

LR11XX_HW_API_status_t __attribute__((weak)) LR11XX_HW_API_rx_off(void) {
	/* To be implemented by the device manufacturer */
#ifdef ERROR_CODES
    LR11XX_HW_API_status_t status = LR11XX_HW_API_SUCCESS;
#endif
    RETURN();
}

#if (defined TIMER_REQUIRED) && (defined LATENCY_COMPENSATION)
/*******************************************************************/
LR11XX_HW_API_status_t __attribute__((weak)) LR11XX_HW_API_get_latency(LR11XX_HW_API_latency_t latency_type, sfx_u32 *latency_ms) {
    /* To be implemented by the device manufacturer */
#ifdef ERROR_CODES
    LR11XX_HW_API_status_t status = LR11XX_HW_API_SUCCESS;
#endif
    SFX_UNUSED(latency_type);
    SFX_UNUSED(latency_ms);
    RETURN();
}
#endif
