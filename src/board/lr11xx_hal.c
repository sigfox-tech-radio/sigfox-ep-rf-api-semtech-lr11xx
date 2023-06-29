/*!*****************************************************************
 * \file    lr11xx_hal.c
 * \brief   LR11XX library low level functions.
 *******************************************************************/
 
#include "lr11xx_hal.h"

lr11xx_hal_status_t lr11xx_hal_reset( const void* context )
{
	/* To be implemented by the device manufacturer */
    return LR11XX_HAL_STATUS_OK;
}

lr11xx_hal_status_t lr11xx_hal_wakeup( const void* context )
{
	/* To be implemented by the device manufacturer */
    return LR11XX_HAL_STATUS_OK;
}

lr11xx_hal_status_t lr11xx_hal_read( const void* context, const uint8_t* cbuffer, const uint16_t cbuffer_length,
                                     uint8_t* rbuffer, const uint16_t rbuffer_length )
{
	/* To be implemented by the device manufacturer */
    return LR11XX_HAL_STATUS_OK;
}

lr11xx_hal_status_t lr11xx_hal_write( const void* context, const uint8_t* cbuffer, const uint16_t cbuffer_length,
                                      const uint8_t* cdata, const uint16_t cdata_length )
{
	/* To be implemented by the device manufacturer */
    return LR11XX_HAL_STATUS_OK;
}

lr11xx_hal_status_t lr11xx_hal_direct_read( const void* context, uint8_t* buffer, const uint16_t length )
{
	/* To be implemented by the device manufacturer */
    return LR11XX_HAL_STATUS_OK;
}
