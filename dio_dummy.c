/*******************************************************************************
 *
 * Dummy DIO API functions so we can compile and link without error
 *
 ******************************************************************************/



#include "libmoxa_rtu.h"


MODULE_RW_ERR_CODE MX_RTU_Module_DIO_DI_Mode_Set(UINT8 slot, UINT8 start, UINT8 count, UINT8 *buf)
{
    return(MODULE_RW_ERR_OK);
}


MODULE_RW_ERR_CODE MX_RTU_Module_DI_Value_Get(UINT8 slot, UINT32 *value, struct Timestamp *time)
{
    *value = 0;
    return(MODULE_RW_ERR_OK);
}


