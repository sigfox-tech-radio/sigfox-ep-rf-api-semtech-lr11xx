#ifndef __SMTC_SIGFOX_LR11XX_H__
#define __SMTC_SIGFOX_LR11XX_H__

/*!
* \brief Ramp-up delay for the power amplifier
*
* This parameter configures the delay to fine tune the
* ramp-up time of the power amplifier for BPSK operation.
*/
enum
{
    SIGFOX_DBPSK_RAMP_UP_TIME_DEFAULT = 0x0000,
    SIGFOX_DBPSK_RAMP_UP_TIME_100_BPS = 0x1306,
    SIGFOX_DBPSK_RAMP_UP_TIME_600_BPS = 0x0325,
};
/*!
* \brief Ramp-down delay for the power amplifier
*
* This parameter configures the delay to fine tune the
* ramp-down time of the power amplifier for BPSK operation.
*/
enum
{
    SIGFOX_DBPSK_RAMP_DOWN_TIME_DEFAULT = 0x0000,
    SIGFOX_DBPSK_RAMP_DOWN_TIME_100_BPS = 0x1D70,
    SIGFOX_DBPSK_RAMP_DOWN_TIME_600_BPS = 0x04E1,
};

#endif// __SMTC_SIGFOX_LR11XX_H__
