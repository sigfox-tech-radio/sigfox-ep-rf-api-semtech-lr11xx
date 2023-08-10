/*!*****************************************************************
 * \file    lr11xx_rf_api.c
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

#include "manuf/lr11xx_rf_api.h"
#include "manuf/smtc_dbpsk.h"
#include "manuf/smtc_sigfox_lr11xx.h"
#include "manuf/mcu_api.h"
#include "sigfox_error.h"
#include "board/lr11xx_hw_api.h"
#include "lr11xx_system.h"
#include "lr11xx_radio_types.h"
#include "lr11xx_radio.h"
#include "lr11xx_regmem.h"

#define T_RADIO_DELAY_ON 0
#define T_RADIO_BIT_LATENCY_ON (2)
#define T_RADIO_BIT_LATENCY_OFF (9)

#ifdef ASYNCHRONOUS
typedef struct {
    RF_API_process_cb_t process_cb;
    RF_API_error_cb_t error_cb;
    RF_API_tx_cplt_cb_t tx_cplt_cb;
#ifdef BIDIRECTIONAL
    RF_API_rx_data_received_cb_t rx_data_received_cb;
#endif
#if (defined REGULATORY) && (defined SPECTRUM_ACCESS_LBT)
    RF_API_channel_free_cb_t channel_free_cb;
#endif
} callback_t;
#endif

typedef struct {
#ifdef ASYNCHRONOUS
    callback_t callbacks;
#endif
    sfx_bool tx_done_flag;
    sfx_bool rx_done_flag;
    sfx_bool error_flag;
    volatile sfx_bool irq_flag;
    volatile sfx_bool irq_en;
    sfx_u16 backup_bit_rate_bps_patch;
}lr1110_ctx_t;

#ifdef VERBOSE
static const sfx_u8 LR11XX_RF_API_VERSION[] = "v1.1";
#endif

static lr1110_ctx_t lr1110_ctx = {
#ifdef ASYNCHRONOUS
        .callbacks.process_cb = SFX_NULL,
        .callbacks.error_cb = SFX_NULL,
        .callbacks.tx_cplt_cb = SFX_NULL,
#ifdef BIDIRECTIONAL
        .callbacks.rx_data_received_cb = SFX_NULL,
#endif
#if (defined REGULATORY) && (defined SPECTRUM_ACCESS_LBT)    
        .callbacks.channel_free_cb = SFX_NULL,
#endif
#endif
        .tx_done_flag   = 0,
        .rx_done_flag   = 0,
        .error_flag     = 0,
        .irq_flag = 0,
        .irq_en = SFX_FALSE,
        .backup_bit_rate_bps_patch = 0,
};


static void LR11XX_irq(void)
{
    if (lr1110_ctx.irq_en == 1) {
        lr1110_ctx.irq_flag = 1;
#ifdef ASYNCHRONOUS
        if (lr1110_ctx.callbacks.process_cb != SFX_NULL)
            lr1110_ctx.callbacks.process_cb();
#endif
    }
}

#if (defined ASYNCHRONOUS) || (defined LOW_LEVEL_OPEN_CLOSE)
RF_API_status_t LR11XX_RF_API_open(RF_API_config_t *config) {
#ifdef ERROR_CODES
    RF_API_status_t status = RF_API_SUCCESS;
    LR11XX_HW_API_status_t lr11xx_hw_api_status = LR11XX_HW_API_SUCCESS;
#endif
#ifdef ASYNCHRONOUS
    lr1110_ctx.callbacks.process_cb = config->process_cb;
    lr1110_ctx.callbacks.error_cb = config->error_cb;
#endif
#ifdef ERROR_CODES
    LR11XX_HW_API_open(&LR11XX_irq);
    LR11XX_HW_API_check_status(LR11XX_RF_API_ERROR_DRIVER_LR11XX_HW_API);
#else
    LR11XX_HW_API_open(&LR11XX_irq);
#endif
#ifdef ERROR_CODES
errors:
#endif
    RETURN();
}
#endif

RF_API_status_t LR11XX_RF_API_process(void)
{
#ifdef ERROR_CODES
    RF_API_status_t status = RF_API_SUCCESS;
    LR11XX_HW_API_status_t lr11xx_hw_api_status = LR11XX_HW_API_SUCCESS;
#endif
    lr11xx_system_irq_mask_t lr11xx_system_irq_mask;
    lr11xx_status_t lr11xx_status;
    if (lr1110_ctx.irq_flag != 1)
        EXIT_ERROR(LR11XX_RF_API_ERROR_STATE);
    lr1110_ctx.irq_flag = 0;

    lr11xx_status = lr11xx_system_get_and_clear_irq_status( SFX_NULL, &lr11xx_system_irq_mask );
    if (lr11xx_status != LR11XX_STATUS_OK)
        EXIT_ERROR(LR11XX_RF_API_ERROR_CHIP_SYSTEM_REG);
    if (lr11xx_system_irq_mask & LR11XX_SYSTEM_IRQ_TX_DONE) {
#ifdef ERROR_CODES
        lr11xx_hw_api_status = LR11XX_HW_API_tx_off();
        LR11XX_HW_API_check_status(LR11XX_RF_API_ERROR_DRIVER_LR11XX_HW_API);
#else
        LR11XX_HW_API_tx_off();
#endif
        lr1110_ctx.tx_done_flag = 1;
#ifdef ASYNCHRONOUS
        if (lr1110_ctx.callbacks.tx_cplt_cb != SFX_NULL)
            lr1110_ctx.callbacks.tx_cplt_cb();
#endif
    }
    if (lr11xx_system_irq_mask & LR11XX_SYSTEM_IRQ_RX_DONE) {
#ifdef ERROR_CODES
        lr11xx_hw_api_status = LR11XX_HW_API_rx_off();
        LR11XX_HW_API_check_status(LR11XX_RF_API_ERROR_DRIVER_LR11XX_HW_API);
#else
        LR11XX_HW_API_rx_off();
#endif
        lr1110_ctx.rx_done_flag = 1;
#if (defined ASYNCHRONOUS) && (defined BIDIRECTIONAL)
        if (lr1110_ctx.callbacks.rx_data_received_cb != SFX_NULL)
            lr1110_ctx.callbacks.rx_data_received_cb();
#endif
    }
    if (lr11xx_system_irq_mask & LR11XX_SYSTEM_IRQ_ERROR) {
        lr1110_ctx.error_flag = 1;
#ifdef ASYNCHRONOUS
        if (lr1110_ctx.callbacks.error_cb != SFX_NULL)
#ifdef ERROR_CODES
            lr1110_ctx.callbacks.error_cb(LR11XX_RF_API_ERROR_CHIP_IRQ);
#else
            lr1110_ctx.callbacks.error_cb();
#endif
#endif
    }
    RETURN();
errors:
#ifdef ASYNCHRONOUS
#ifdef ERROR_CODES
            lr1110_ctx.callbacks.error_cb(RF_API_ERROR);
#else
            lr1110_ctx.callbacks.error_cb();
#endif
#endif
    RETURN();
}

#ifdef LOW_LEVEL_OPEN_CLOSE
RF_API_status_t LR11XX_RF_API_close(void) {
#ifdef ERROR_CODES
    RF_API_status_t status = RF_API_SUCCESS;
    LR11XX_HW_API_status_t lr11xx_hw_api_status = LR11XX_HW_API_SUCCESS;
#endif
#ifdef ERROR_CODES
    lr11xx_hw_api_status = LR11XX_HW_API_close();
    LR11XX_HW_API_check_status(LR11XX_RF_API_ERROR_DRIVER_LR11XX_HW_API);
#else
    LR11XX_HW_API_close();
#endif
#ifdef ERROR_CODES
errors:
#endif
    RETURN();
}
#endif

RF_API_status_t LR11XX_RF_API_wake_up(void) {
#ifdef ERROR_CODES
    RF_API_status_t status = RF_API_SUCCESS;
    LR11XX_HW_API_status_t lr11xx_hw_api_status = LR11XX_HW_API_SUCCESS;
#endif
    lr11xx_status_t lr11xx_status;
    lr11xx_system_errors_t errors;
    lr11xx_system_rfswitch_cfg_t rf_switch_setup = { 0 };
    lr11xx_system_version_t lr11xx_system_version;
    lr11xx_status = lr11xx_system_reset(SFX_NULL);
    sfx_u8 rfsw_dio_mask;
    if ( lr11xx_status != LR11XX_STATUS_OK)
        EXIT_ERROR(LR11XX_RF_API_ERROR_CHIP_RESET);
    lr11xx_status = lr11xx_system_wakeup(SFX_NULL);
    if ( lr11xx_status != LR11XX_STATUS_OK)
        EXIT_ERROR(LR11XX_RF_API_ERROR_CHIP_WAKEUP);
#ifdef ERROR_CODES
    lr11xx_hw_api_status = LR11XX_HW_API_delayMs(150);
    LR11XX_HW_API_check_status(LR11XX_RF_API_ERROR_DRIVER_LR11XX_HW_API);
#else
    LR11XX_HW_API_delayMs(100);
#endif
    lr11xx_status = lr11xx_system_set_reg_mode( SFX_NULL, LR11XX_SYSTEM_REG_MODE_LDO );
    if ( lr11xx_status != LR11XX_STATUS_OK)
        EXIT_ERROR(LR11XX_RF_API_ERROR_CHIP_SYSTEM_REG);
#ifdef ERROR_CODES
    lr11xx_hw_api_status = LR11XX_HW_API_get_fem_mask(LR11XX_HW_API_FEM_PIN_USED, &rfsw_dio_mask);
    LR11XX_HW_API_check_status(LR11XX_RF_API_ERROR_DRIVER_LR11XX_HW_API);
    rf_switch_setup.enable = rfsw_dio_mask;
    lr11xx_hw_api_status = LR11XX_HW_API_get_fem_mask(LR11XX_HW_API_FEM_STBY, &rfsw_dio_mask);
    LR11XX_HW_API_check_status(LR11XX_RF_API_ERROR_DRIVER_LR11XX_HW_API);
    rf_switch_setup.standby = rfsw_dio_mask;
    lr11xx_hw_api_status = LR11XX_HW_API_get_fem_mask(LR11XX_HW_API_FEM_TX, &rfsw_dio_mask);
    LR11XX_HW_API_check_status(LR11XX_RF_API_ERROR_DRIVER_LR11XX_HW_API);
    rf_switch_setup.tx = rfsw_dio_mask;
    lr11xx_hw_api_status = LR11XX_HW_API_get_fem_mask(LR11XX_HW_API_FEM_TXHP, &rfsw_dio_mask);
    LR11XX_HW_API_check_status(LR11XX_RF_API_ERROR_DRIVER_LR11XX_HW_API);
    rf_switch_setup.tx_hp = rfsw_dio_mask;
    lr11xx_hw_api_status = LR11XX_HW_API_get_fem_mask(LR11XX_HW_API_FEM_RX, &rfsw_dio_mask);
    LR11XX_HW_API_check_status(LR11XX_RF_API_ERROR_DRIVER_LR11XX_HW_API);
    rf_switch_setup.rx = rfsw_dio_mask;
    lr11xx_hw_api_status = LR11XX_HW_API_get_fem_mask(LR11XX_HW_API_FEM_WIFI, &rfsw_dio_mask);
    LR11XX_HW_API_check_status(LR11XX_RF_API_ERROR_DRIVER_LR11XX_HW_API);
    rf_switch_setup.wifi = rfsw_dio_mask;
    lr11xx_hw_api_status = LR11XX_HW_API_get_fem_mask(LR11XX_HW_API_FEM_GNSS, &rfsw_dio_mask);
    LR11XX_HW_API_check_status(LR11XX_RF_API_ERROR_DRIVER_LR11XX_HW_API);
    rf_switch_setup.gnss = rfsw_dio_mask;
#else
    LR11XX_HW_API_get_fem_mask(LR11XX_HW_API_FEM_PIN_USED, &rfsw_dio_mask);
    rf_switch_setup.enable = rfsw_dio_mask;
    LR11XX_HW_API_get_fem_mask(LR11XX_HW_API_FEM_STBY, &rfsw_dio_mask);
    rf_switch_setup.standby = rfsw_dio_mask;
    LR11XX_HW_API_get_fem_mask(LR11XX_HW_API_FEM_TX, &rfsw_dio_mask);
    rf_switch_setup.tx = rfsw_dio_mask;
    LR11XX_HW_API_get_fem_mask(LR11XX_HW_API_FEM_TXHP, &rfsw_dio_mask);
    rf_switch_setup.tx_hp = rfsw_dio_mask;
    LR11XX_HW_API_get_fem_mask(LR11XX_HW_API_FEM_RX, &rfsw_dio_mask);
    rf_switch_setup.rx = rfsw_dio_mask;
    LR11XX_HW_API_get_fem_mask(LR11XX_HW_API_FEM_WIFI, &rfsw_dio_mask);
    rf_switch_setup.wifi = rfsw_dio_mask;
    LR11XX_HW_API_get_fem_mask(LR11XX_HW_API_FEM_GNSS, &rfsw_dio_mask);
    rf_switch_setup.gnss = rfsw_dio_mask;
#endif
    lr11xx_status = lr11xx_system_set_dio_as_rf_switch( SFX_NULL, &rf_switch_setup );
    if ( lr11xx_status != LR11XX_STATUS_OK)
        EXIT_ERROR(LR11XX_RF_API_ERROR_CHIP_SYSTEM_REG);
    lr11xx_status = lr11xx_system_cfg_lfclk( SFX_NULL, LR11XX_SYSTEM_LFCLK_RC, true );
    if ( lr11xx_status != LR11XX_STATUS_OK)
        EXIT_ERROR(LR11XX_RF_API_ERROR_CHIP_SYSTEM_REG);
    lr11xx_status = lr11xx_system_clear_errors( SFX_NULL );
    if ( lr11xx_status != LR11XX_STATUS_OK)
        EXIT_ERROR(LR11XX_RF_API_ERROR_CHIP_SYSTEM_REG);
    lr11xx_status = lr11xx_system_set_tcxo_mode( SFX_NULL, LR11XX_SYSTEM_TCXO_CTRL_1_8V, 1);
    if ( lr11xx_status != LR11XX_STATUS_OK)
        EXIT_ERROR(LR11XX_RF_API_ERROR_CHIP_SYSTEM_REG);
    lr11xx_status = lr11xx_system_calibrate( SFX_NULL, LR11XX_SYSTEM_CALIB_HF_RC_MASK |
            LR11XX_SYSTEM_CALIB_PLL_MASK | LR11XX_SYSTEM_CALIB_PLL_MASK |
            LR11XX_SYSTEM_CALIB_ADC_MASK | LR11XX_SYSTEM_CALIB_IMG_MASK |
            LR11XX_SYSTEM_CALIB_PLL_TX_MASK);
    if ( lr11xx_status != LR11XX_STATUS_OK)
        EXIT_ERROR(LR11XX_RF_API_ERROR_CHIP_SYSTEM_REG);
#ifdef ERROR_CODES
    lr11xx_hw_api_status = LR11XX_HW_API_delayMs(50);
    LR11XX_HW_API_check_status(LR11XX_RF_API_ERROR_DRIVER_LR11XX_HW_API);
#else
    LR11XX_HW_API_delayMs(100);
#endif
    //Set standby on TCXO oscillator to keep the TCXO powered in standby mode.
    lr11xx_status = lr11xx_system_set_standby(SFX_NULL, LR11XX_SYSTEM_STANDBY_CFG_XOSC);
    if ( lr11xx_status != LR11XX_STATUS_OK)
        EXIT_ERROR(LR11XX_RF_API_ERROR_CHIP_SYSTEM_REG);
#ifdef ERROR_CODES
    lr11xx_hw_api_status = LR11XX_HW_API_delayMs(100);
    LR11XX_HW_API_check_status(LR11XX_RF_API_ERROR_DRIVER_LR11XX_HW_API);
#else
    LR11XX_HW_API_delayMs(100);
#endif
    lr11xx_status = lr11xx_system_set_dio_irq_params(SFX_NULL, LR11XX_SYSTEM_IRQ_TX_DONE | LR11XX_SYSTEM_IRQ_RX_DONE | LR11XX_SYSTEM_IRQ_ERROR, 0);
    if ( lr11xx_status != LR11XX_STATUS_OK)
        EXIT_ERROR(LR11XX_RF_API_ERROR_CHIP_SYSTEM_REG);
    lr11xx_status = lr11xx_system_clear_irq_status( SFX_NULL, LR11XX_SYSTEM_IRQ_ALL_MASK );
    if ( lr11xx_status != LR11XX_STATUS_OK)
        EXIT_ERROR(LR11XX_RF_API_ERROR_CHIP_SYSTEM_REG);
    lr11xx_status = lr11xx_system_get_version(SFX_NULL, &lr11xx_system_version);
    if ( lr11xx_status != LR11XX_STATUS_OK)
        EXIT_ERROR(LR11XX_RF_API_ERROR_CHIP_SYSTEM_REG);
    lr11xx_status = lr11xx_system_get_errors( SFX_NULL, &errors );
    if ( (lr11xx_status != LR11XX_STATUS_OK) || (errors != 0))
        EXIT_ERROR(LR11XX_RF_API_ERROR_CHIP_SYSTEM_REG);
    lr1110_ctx.irq_en = 1;
errors:
    RETURN();
}


RF_API_status_t LR11XX_RF_API_sleep(void) {
#ifdef ERROR_CODES
    RF_API_status_t status = RF_API_SUCCESS;
#endif
    lr11xx_status_t lr11xx_status;
    lr11xx_system_sleep_cfg_t lr11xx_system_sleep_cfg;
    lr1110_ctx.irq_en = 0;
    lr11xx_system_sleep_cfg.is_warm_start = 0;
    lr11xx_system_sleep_cfg.is_rtc_timeout= 0;
    lr11xx_status = lr11xx_system_set_sleep(SFX_NULL, lr11xx_system_sleep_cfg, 0);
    if ( lr11xx_status != LR11XX_STATUS_OK)
        EXIT_ERROR(LR11XX_RF_API_ERROR_CHIP_SYSTEM_REG);
        
errors:
    RETURN();
}

RF_API_status_t LR11XX_RF_API_init(RF_API_radio_parameters_t *radio_parameters) {
#ifdef ERROR_CODES
    RF_API_status_t status = RF_API_SUCCESS;
#endif
    lr11xx_radio_mod_params_bpsk_t lr11xx_radio_mod_params_bpsk;
    lr11xx_radio_pa_cfg_t   lr11xx_radio_pa_cfg;
    lr11xx_radio_mod_params_gfsk_t lr11xx_radio_mod_params_gfsk;
    lr11xx_status_t lr11xx_status;
    lr11xx_status = lr11xx_radio_set_rf_freq(SFX_NULL, radio_parameters->frequency_hz);
    if ( lr11xx_status != LR11XX_STATUS_OK)
        EXIT_ERROR(LR11XX_RF_API_ERROR_CHIP_RADIO_REG);

    switch (radio_parameters->modulation) {
        case RF_API_MODULATION_DBPSK :
            lr11xx_status = lr11xx_radio_set_pkt_type( SFX_NULL, LR11XX_RADIO_PKT_TYPE_BPSK);
            if ( lr11xx_status != LR11XX_STATUS_OK)
                EXIT_ERROR(LR11XX_RF_API_ERROR_CHIP_RADIO_REG);
            lr11xx_radio_mod_params_bpsk.br_in_bps = radio_parameters->bit_rate_bps;
            lr11xx_radio_mod_params_bpsk.pulse_shape = LR11XX_RADIO_DBPSK_PULSE_SHAPE;
            lr11xx_status = lr11xx_radio_set_bpsk_mod_params( SFX_NULL, &lr11xx_radio_mod_params_bpsk );
            if ( lr11xx_status != LR11XX_STATUS_OK)
                EXIT_ERROR(LR11XX_RF_API_ERROR_CHIP_RADIO_REG);
            break;
        case RF_API_MODULATION_GFSK :
            lr11xx_status = lr11xx_radio_set_pkt_type( SFX_NULL, LR11XX_RADIO_PKT_TYPE_GFSK);
            if ( lr11xx_status != LR11XX_STATUS_OK)
                EXIT_ERROR(LR11XX_RF_API_ERROR_CHIP_RADIO_REG);
            lr11xx_radio_mod_params_gfsk.br_in_bps = radio_parameters->bit_rate_bps;
#ifdef BIDIRECTIONAL
            lr11xx_radio_mod_params_gfsk.fdev_in_hz = radio_parameters->deviation_hz;
#endif
            lr11xx_radio_mod_params_gfsk.pulse_shape = LR11XX_RADIO_GFSK_PULSE_SHAPE_BT_1;
            lr11xx_radio_mod_params_gfsk.bw_dsb_param = LR11XX_RADIO_GFSK_BW_4800;
            lr11xx_status = lr11xx_radio_set_gfsk_mod_params(SFX_NULL, &lr11xx_radio_mod_params_gfsk);
            if ( lr11xx_status != LR11XX_STATUS_OK)
                EXIT_ERROR(LR11XX_RF_API_ERROR_CHIP_RADIO_REG);
            break;
        case RF_API_MODULATION_NONE :
            break;
        default:
            EXIT_ERROR(LR11XX_RF_API_ERROR_MODULATION);
    }

    if (radio_parameters->rf_mode == RF_API_MODE_TX) {
        lr1110_ctx.backup_bit_rate_bps_patch = radio_parameters->bit_rate_bps;
        if (radio_parameters->tx_power_dbm_eirp > 14 ) {
            lr11xx_radio_pa_cfg.pa_reg_supply = LR11XX_RADIO_PA_REG_SUPPLY_VBAT;
            lr11xx_radio_pa_cfg.pa_sel = LR11XX_RADIO_PA_SEL_HP;
            lr11xx_radio_pa_cfg.pa_hp_sel = 0x07;
        } else {
            lr11xx_radio_pa_cfg.pa_reg_supply = LR11XX_RADIO_PA_REG_SUPPLY_VREG;
            lr11xx_radio_pa_cfg.pa_sel = LR11XX_RADIO_PA_SEL_LP;
            lr11xx_radio_pa_cfg.pa_hp_sel = 0x00;
        }
        lr11xx_radio_pa_cfg.pa_duty_cycle = 0x04;

        lr11xx_status = lr11xx_radio_set_pa_cfg(SFX_NULL,&lr11xx_radio_pa_cfg);
        if ( lr11xx_status != LR11XX_STATUS_OK)
            EXIT_ERROR(LR11XX_RF_API_ERROR_CHIP_RADIO_REG);
        lr11xx_status = lr11xx_radio_set_tx_params(SFX_NULL, radio_parameters->tx_power_dbm_eirp, LR11XX_RADIO_RAMP_208_US);
        if ( lr11xx_status != LR11XX_STATUS_OK)
            EXIT_ERROR(LR11XX_RF_API_ERROR_CHIP_RADIO_REG);

    }
    if (radio_parameters->rf_mode == RF_API_MODE_RX) {

    }
errors:
    RETURN();
}

RF_API_status_t LR11XX_RF_API_de_init(void) {
#ifdef ERROR_CODES
    RF_API_status_t status = RF_API_SUCCESS;
    LR11XX_HW_API_status_t lr11xx_hw_api_status = LR11XX_HW_API_SUCCESS;
#endif
    lr11xx_status_t lr11xx_status;
    lr11xx_status = lr11xx_system_set_standby(SFX_NULL, LR11XX_SYSTEM_STANDBY_CFG_XOSC);
    if ( lr11xx_status != LR11XX_STATUS_OK)
        EXIT_ERROR(LR11XX_RF_API_ERROR_CHIP_SYSTEM_REG);
#ifdef ERROR_CODES
    lr11xx_hw_api_status = LR11XX_HW_API_rx_off();
    LR11XX_HW_API_check_status(LR11XX_RF_API_ERROR_DRIVER_LR11XX_HW_API);
#else
    LR11XX_HW_API_rx_off();
#endif
#ifdef ERROR_CODES
    lr11xx_hw_api_status = LR11XX_HW_API_tx_off();
    LR11XX_HW_API_check_status(LR11XX_RF_API_ERROR_DRIVER_LR11XX_HW_API);
#else
    LR11XX_HW_API_tx_off();
#endif
errors:
    RETURN();
}

RF_API_status_t LR11XX_RF_API_send(RF_API_tx_data_t *tx_data) {
#ifdef ERROR_CODES
    RF_API_status_t status = RF_API_SUCCESS;
    LR11XX_HW_API_status_t lr11xx_hw_api_status = LR11XX_HW_API_SUCCESS;
#endif
    lr11xx_radio_pkt_params_bpsk_t lr11xx_radio_pkt_params_bpsk;
    lr11xx_status_t lr11xx_status;
    sfx_u8 buffer[SIGFOX_UL_BITSTREAM_SIZE_BYTES + 1];
    sfx_u8 i;
#ifdef ASYNCHRONOUS
    lr1110_ctx.callbacks.tx_cplt_cb = tx_data->cplt_cb;
#endif
    lr1110_ctx.tx_done_flag = 0;
    lr1110_ctx.error_flag = 0;
    for (i = 0; i < tx_data->bitstream_size_bytes; i++) {
        buffer[i] = tx_data->bitstream[i];
    }
    buffer[tx_data->bitstream_size_bytes] = 0x80;
    dbpsk_encode_buffer(buffer, tx_data->bitstream_size_bytes * 8 + 2, buffer);
    /*Set the BPSK packet param*/
    lr11xx_radio_pkt_params_bpsk.pld_len_in_bits = dbpsk_get_pld_len_in_bits(tx_data->bitstream_size_bytes * 8);
    lr11xx_radio_pkt_params_bpsk.pld_len_in_bytes = dbpsk_get_pld_len_in_bytes(tx_data->bitstream_size_bytes * 8);
    if (lr1110_ctx.backup_bit_rate_bps_patch == 100) {
        lr11xx_radio_pkt_params_bpsk.ramp_down_delay = SIGFOX_DBPSK_RAMP_DOWN_TIME_100_BPS;
        lr11xx_radio_pkt_params_bpsk.ramp_up_delay = SIGFOX_DBPSK_RAMP_UP_TIME_100_BPS;
    } else if (lr1110_ctx.backup_bit_rate_bps_patch == 600) {
        lr11xx_radio_pkt_params_bpsk.ramp_down_delay = SIGFOX_DBPSK_RAMP_DOWN_TIME_600_BPS;
        lr11xx_radio_pkt_params_bpsk.ramp_up_delay = SIGFOX_DBPSK_RAMP_UP_TIME_600_BPS;
    }

    lr11xx_status = lr11xx_radio_set_bpsk_pkt_params(SFX_NULL, &lr11xx_radio_pkt_params_bpsk);
    if ( lr11xx_status != LR11XX_STATUS_OK)
        EXIT_ERROR(LR11XX_RF_API_ERROR_CHIP_RADIO_REG);
    lr11xx_status = lr11xx_regmem_write_buffer8(SFX_NULL, buffer, lr11xx_radio_pkt_params_bpsk.pld_len_in_bytes);
    if ( lr11xx_status != LR11XX_STATUS_OK)
        EXIT_ERROR(LR11XX_RF_API_ERROR_CHIP_REGMEM_REG);
#ifdef ERROR_CODES
    lr11xx_hw_api_status = LR11XX_HW_API_tx_on();
    LR11XX_HW_API_check_status(LR11XX_RF_API_ERROR_DRIVER_LR11XX_HW_API);
#else
    LR11XX_HW_API_tx_on();
#endif
    lr11xx_status = lr11xx_radio_set_tx( SFX_NULL, 5000);
    if ( lr11xx_status != LR11XX_STATUS_OK)
        EXIT_ERROR(LR11XX_RF_API_ERROR_CHIP_RADIO_REG);
#ifndef ASYNCHRONOUS
    while(lr1110_ctx.tx_done_flag != 1) {
        if (lr1110_ctx.irq_flag == 1) {
#ifdef ERROR_CODES
            status = LR11XX_RF_API_process();
            CHECK_STATUS(RF_API_SUCCESS);
#else
            LR11XX_RF_API_process();
#endif
            if(lr1110_ctx.error_flag == 1) {
                EXIT_ERROR(RF_API_ERROR);
            }
        }
    }
#endif
errors:
    RETURN();
}

#ifdef BIDIRECTIONAL
RF_API_status_t LR11XX_RF_API_receive(RF_API_rx_data_t *rx_data) {
#ifdef ERROR_CODES
    RF_API_status_t status = RF_API_SUCCESS;
    LR11XX_HW_API_status_t lr11xx_hw_api_status = LR11XX_HW_API_SUCCESS;
#ifndef ASYNCHRONOUS
    MCU_API_status_t mcu_api_status = MCU_API_SUCCESS;
#endif
#endif
    lr11xx_status_t lr11xx_status;
    lr11xx_radio_pkt_params_gfsk_t lr11xx_radio_pkt_params_gfsk;
    sfx_u8 sync_world[2] = SIGFOX_DL_FT;
#ifdef ASYNCHRONOUS
    lr1110_ctx.callbacks.rx_data_received_cb = rx_data->data_received_cb;
#else
    sfx_bool timer_has_elapsed;
#endif  
    lr1110_ctx.rx_done_flag = 0;
    lr1110_ctx.error_flag = 0;
    lr11xx_radio_pkt_params_gfsk.address_filtering = LR11XX_RADIO_GFSK_ADDRESS_FILTERING_DISABLE;
    lr11xx_radio_pkt_params_gfsk.crc_type = LR11XX_RADIO_GFSK_CRC_OFF;
    lr11xx_radio_pkt_params_gfsk.dc_free = LR11XX_RADIO_GFSK_DC_FREE_OFF;
    lr11xx_radio_pkt_params_gfsk.header_type = LR11XX_RADIO_GFSK_PKT_FIX_LEN;
    lr11xx_radio_pkt_params_gfsk.pld_len_in_bytes = SIGFOX_DL_PHY_CONTENT_SIZE_BYTES;
    lr11xx_radio_pkt_params_gfsk.preamble_detector = LR11XX_RADIO_GFSK_PREAMBLE_DETECTOR_MIN_16BITS;
    lr11xx_radio_pkt_params_gfsk.preamble_len_in_bits = 16;
    lr11xx_radio_pkt_params_gfsk.sync_word_len_in_bits = SIGFOX_DL_FT_SIZE_BYTES * 8;
    lr11xx_status = lr11xx_radio_set_gfsk_pkt_params(SFX_NULL, &lr11xx_radio_pkt_params_gfsk);
    if ( lr11xx_status != LR11XX_STATUS_OK)
        EXIT_ERROR(LR11XX_RF_API_ERROR_CHIP_RADIO_REG);
    lr11xx_status = lr11xx_radio_set_gfsk_sync_word(SFX_NULL, sync_world);
    if ( lr11xx_status != LR11XX_STATUS_OK)
        EXIT_ERROR(LR11XX_RF_API_ERROR_CHIP_RADIO_REG);
    lr11xx_status = lr11xx_radio_cfg_rx_boosted(SFX_NULL, 0x01);
    if ( lr11xx_status != LR11XX_STATUS_OK)
        EXIT_ERROR(LR11XX_RF_API_ERROR_CHIP_RADIO_REG);
#ifdef ERROR_CODES
    lr11xx_hw_api_status = LR11XX_HW_API_rx_on();
    LR11XX_HW_API_check_status(LR11XX_RF_API_ERROR_DRIVER_LR11XX_HW_API);
#else
    LR11XX_HW_API_rx_on();
#endif
    lr11xx_status = lr11xx_radio_set_rx_with_timeout_in_rtc_step(SFX_NULL, 0xFFFFFF);
    if ( lr11xx_status != LR11XX_STATUS_OK)
        EXIT_ERROR(LR11XX_RF_API_ERROR_CHIP_RADIO_REG);
#ifndef ASYNCHRONOUS
    while(1) {
        if (lr1110_ctx.irq_flag == 1) {
#ifdef ERROR_CODES
            status = LR11XX_RF_API_process();
            CHECK_STATUS(RF_API_SUCCESS);
#else
            LR11XX_RF_API_process();
#endif
            if(lr1110_ctx.error_flag == 1) {
                EXIT_ERROR(RF_API_ERROR);
            }
            if(lr1110_ctx.rx_done_flag == 1) {
                rx_data->data_received = SFX_TRUE;
                break;
            }
        }
#ifdef ERROR_CODES
        mcu_api_status = MCU_API_timer_status(MCU_API_TIMER_INSTANCE_T_RX, &timer_has_elapsed);
        MCU_API_check_status(LR11XX_RF_API_ERROR_DRIVER_MCU_API);
#else
        MCU_API_timer_status(MCU_API_TIMER_INSTANCE_T_RX, &timer_has_elapsed);
#endif
        if (timer_has_elapsed == SFX_TRUE) {
            break;
        }
    }
#endif
errors:
    RETURN();
}
#endif

#ifdef BIDIRECTIONAL
RF_API_status_t LR11XX_RF_API_get_dl_phy_content_and_rssi(sfx_u8 *dl_phy_content, sfx_u8 dl_phy_content_size, sfx_s16 *dl_rssi_dbm) {
#ifdef ERROR_CODES
    RF_API_status_t status = RF_API_SUCCESS;
#endif
    lr11xx_status_t lr11xx_status;
    lr11xx_radio_pkt_status_gfsk_t lr11xx_radio_pkt_status_gfsk;
    lr11xx_radio_rx_buffer_status_t lr11xx_radio_rx_buffer_status;
#ifdef PARAMETERS_CHECK
    // Check parameters.
    if ((dl_phy_content == SFX_NULL) || (dl_rssi_dbm == SFX_NULL)) {
        EXIT_ERROR(LR11XX_RF_API_ERROR_NULL_PARAMETER);
    }
    if (dl_phy_content_size > SIGFOX_DL_PHY_CONTENT_SIZE_BYTES) {
        EXIT_ERROR(LR11XX_RF_API_ERROR_BUFFER_SIZE);
    }
#endif
    if (lr1110_ctx.rx_done_flag != SFX_TRUE)
        EXIT_ERROR(LR11XX_RF_API_ERROR_STATE);
    lr11xx_status = lr11xx_radio_get_gfsk_pkt_status(SFX_NULL, &lr11xx_radio_pkt_status_gfsk);
    if ( lr11xx_status != LR11XX_STATUS_OK)
        EXIT_ERROR(LR11XX_RF_API_ERROR_CHIP_RADIO_REG);
    if (lr11xx_radio_pkt_status_gfsk.is_received == 1) {
        *dl_rssi_dbm = (sfx_s16)lr11xx_radio_pkt_status_gfsk.rssi_avg_in_dbm;
        lr11xx_status = lr11xx_radio_get_rx_buffer_status(SFX_NULL, &lr11xx_radio_rx_buffer_status);
        if ( lr11xx_status != LR11XX_STATUS_OK)
            EXIT_ERROR(LR11XX_RF_API_ERROR_CHIP_RADIO_REG);
        lr11xx_regmem_read_buffer8(SFX_NULL, dl_phy_content, lr11xx_radio_rx_buffer_status.buffer_start_pointer,
                dl_phy_content_size);
        lr11xx_status = lr11xx_regmem_clear_rxbuffer(SFX_NULL);
        if ( lr11xx_status != LR11XX_STATUS_OK)
            EXIT_ERROR(LR11XX_RF_API_ERROR_CHIP_REGMEM_REG);
    }
errors:
    RETURN();
}
#endif

#if (defined REGULATORY) && (defined SPECTRUM_ACCESS_LBT)
RF_API_status_t LR11XX_RF_API_carrier_sense(RF_API_carrier_sense_parameters_t *carrier_sense_params){
    /*TODO*/
#ifdef ERROR_CODES
    RF_API_status_t status = RF_API_SUCCESS;
#endif
    RETURN();
}
#endif

#if (defined TIMER_REQUIRED) && (defined LATENCY_COMPENSATION)
RF_API_status_t LR11XX_RF_API_get_latency(RF_API_latency_t latency_type, sfx_u32 *latency_ms) {
#ifdef ERROR_CODES
    RF_API_status_t status = RF_API_SUCCESS;
    LR11XX_HW_API_status_t lr11xx_hw_api_status = LR11XX_HW_API_SUCCESS;
#endif
    sfx_u32 latency_tmp;

    switch(latency_type) {
        case RF_API_LATENCY_WAKE_UP :
            *latency_ms = 532;
#ifdef ERROR_CODES
            lr11xx_hw_api_status = LR11XX_HW_API_get_latency(LR11XX_HW_API_LATENCY_RESET, &latency_tmp);
            LR11XX_HW_API_check_status(LR11XX_RF_API_ERROR_DRIVER_LR11XX_HW_API);
#else
            LR11XX_HW_API_get_latency(LR11XX_HW_API_LATENCY_RESET, &latency_tmp);
#endif
            (*latency_ms) += latency_tmp;
#ifdef ERROR_CODES
            lr11xx_hw_api_status = LR11XX_HW_API_get_latency(LR11XX_HW_API_LATENCY_WAKEUP, &latency_tmp);
            LR11XX_HW_API_check_status(LR11XX_RF_API_ERROR_DRIVER_LR11XX_HW_API);
#else
            LR11XX_HW_API_get_latency(LR11XX_HW_API_LATENCY_RESET, &latency_tmp);
#endif
            (*latency_ms) += latency_tmp;
            break;
        case RF_API_LATENCY_INIT_TX:
            *latency_ms = T_RADIO_DELAY_ON;
            break;
        case RF_API_LATENCY_SEND_START:
            *latency_ms = (T_RADIO_BIT_LATENCY_ON * 1000) / lr1110_ctx.backup_bit_rate_bps_patch;
            break;
        case RF_API_LATENCY_SEND_STOP:
            *latency_ms = (T_RADIO_BIT_LATENCY_OFF * 1000) / lr1110_ctx.backup_bit_rate_bps_patch;
            break;
        case RF_API_LATENCY_DE_INIT_TX:
            *latency_ms = 0;
            break;
        case RF_API_LATENCY_SLEEP :
            *latency_ms = 0;
            break;
#ifdef BIDIRECTIONAL
        case RF_API_LATENCY_INIT_RX:
            *latency_ms = 0;
            break;
        case RF_API_LATENCY_RECEIVE_START:
            *latency_ms = 0;
            break;
        case RF_API_LATENCY_RECEIVE_STOP:
            *latency_ms = 5;
            break;
        case RF_API_LATENCY_DE_INIT_RX:
            *latency_ms = 0;
            break;
#endif
        default :
            *latency_ms = 0;
    }
#ifdef ERROR_CODES
errors:
#endif
    RETURN();
}
#endif

#ifdef VERBOSE
RF_API_status_t LR11XX_RF_API_get_version(sfx_u8 **version, sfx_u8 *version_size_char) {
#ifdef ERROR_CODES
    RF_API_status_t status = RF_API_SUCCESS;
#endif
    (*version) = (sfx_u8*) LR11XX_RF_API_VERSION;
    (*version_size_char) = (sfx_u8) sizeof(LR11XX_RF_API_VERSION);
    RETURN();
}
#endif

#ifdef ERROR_CODES
void LR11XX_RF_API_error(void) {
    lr11xx_regmem_clear_rxbuffer(SFX_NULL);
    LR11XX_RF_API_de_init();
    LR11XX_RF_API_sleep();
}
#endif

/*** LR11XX RF API functions mapping ***/

#ifndef DYNAMIC_RF_API

#if (defined ASYNCHRONOUS) || (defined LOW_LEVEL_OPEN_CLOSE)
/*******************************************************************/
inline RF_API_status_t RF_API_open(RF_API_config_t *rf_api_config) {
	return LR11XX_RF_API_open(rf_api_config);
}
#endif

#ifdef LOW_LEVEL_OPEN_CLOSE
/*******************************************************************/
inline RF_API_status_t RF_API_close(void) {
	return LR11XX_RF_API_close();
}
#endif

#ifdef ASYNCHRONOUS
/*******************************************************************/
inline RF_API_status_t RF_API_process(void) {
	return LR11XX_RF_API_process();
}
#endif

/*******************************************************************/
inline RF_API_status_t RF_API_wake_up(void) {
	return LR11XX_RF_API_wake_up();
}

/*******************************************************************/
inline RF_API_status_t RF_API_sleep(void) {
	return LR11XX_RF_API_sleep();
}

/*******************************************************************/
inline RF_API_status_t RF_API_init(RF_API_radio_parameters_t *radio_parameters) {
	return LR11XX_RF_API_init(radio_parameters);
}

/*******************************************************************/
inline RF_API_status_t RF_API_de_init(void) {
	return LR11XX_RF_API_de_init();
}

/*******************************************************************/
inline RF_API_status_t RF_API_send(RF_API_tx_data_t *tx_data) {
	return LR11XX_RF_API_send(tx_data);
}

#ifdef BIDIRECTIONAL
/*******************************************************************/
inline RF_API_status_t RF_API_receive(RF_API_rx_data_t *rx_data) {
	return LR11XX_RF_API_receive(rx_data);
}
#endif

#ifdef BIDIRECTIONAL
/*******************************************************************/
inline RF_API_status_t RF_API_get_dl_phy_content_and_rssi(sfx_u8 *dl_phy_content, sfx_u8 dl_phy_content_size, sfx_s16 *dl_rssi_dbm) {
	return LR11XX_RF_API_get_dl_phy_content_and_rssi(dl_phy_content, dl_phy_content_size, dl_rssi_dbm);
}
#endif

#if (defined REGULATORY) && (defined SPECTRUM_ACCESS_LBT)
/*******************************************************************/
inline RF_API_status_t RF_API_carrier_sense(RF_API_carrier_sense_parameters_t *carrier_sense_params) {
	return LR11XX_RF_API_carrier_sense(carrier_sense_params);
}
#endif

#if (defined TIMER_REQUIRED) && (defined LATENCY_COMPENSATION)
/*******************************************************************/
inline RF_API_status_t RF_API_get_latency(RF_API_latency_t latency_type, sfx_u32 *latency_ms) {
	return LR11XX_RF_API_get_latency(latency_type, latency_ms);
}
#endif

#ifdef VERBOSE
/*******************************************************************/
inline RF_API_status_t RF_API_get_version(sfx_u8 **version, sfx_u8 *version_size_char) {
	return LR11XX_RF_API_get_version(version, version_size_char);
}
#endif

#ifdef ERROR_CODES
/*******************************************************************/
inline void RF_API_error(void) {
	LR11XX_RF_API_error();
}
#endif

#endif /* DYNAMIC_RF_API */
