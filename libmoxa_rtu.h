/*******************************************************************************
 * Copyright Moxa Inc.
 *
 * Moxa Library
 *
 * Date          Author            Comment
 * 04-10-2013   Eddy Kao     Created.
 ******************************************************************************/

#ifndef _MOXA_RTU_H
#define _MOXA_RTU_H

#ifdef __cplusplus
extern "C"{
#endif

#include "common.h"
    
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <time.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <asm/ioctls.h>
#include <linux/rtc.h>
#include <linux/tty.h>
#include <linux/can.h>
#include <linux/watchdog.h>
#include <netinet/in.h>
#include <net/if.h>
#include <semaphore.h>
#include <mqueue.h>
#include <dirent.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/time.h>
#include <sys/wait.h>

#define USB_VID_MOXA                    0x110a  //Moxa

    /* I/O Module */
#define USB_PID_RTU_85M_1602            0x5200  //16ch, DI
#define USB_PID_RTU_85M_2600            0x5240  //16ch, DO
#define USB_PID_RTU_85M_3800            0x52c0  //8ch, 4-20mA
#define USB_PID_RTU_85M_3800_CRRC       0x5402  //8ch, 0-50mA
#define USB_PID_RTU_85M_3810            0x52c1  //8ch, 0-10V
#define USB_PID_RTU_85M_3801            0x5300  //8ch, 5KHz 4-2-mA
#define USB_PID_RTU_85M_3811            0x5301  //8ch, 5KHz 0-10V
#define USB_PID_RTU_85M_6810            0x5340  //8ch, TC
#define USB_PID_RTU_85M_6600            0x5380  //6ch, RTD
#define USB_PID_RTU_AIO_8di_8dio_8ai    0x50e0  //all-in-one module
#define USB_PID_RTU_86M_5870D           0x52c2  //HART 4-20mA
#define USB_PID_RTU_86M_4420            0x53a0  //AO
#define USB_PID_RTU_86M_2604D           0x5243  //Relay
#define USB_PID_RTU_86M_1832D           0x5201  //DI ch-to-ch isolation
#define USB_PID_RTU_86M_2830D           0x5241  //DO ch-to-ch isolation
#define USB_PID_RTU_86M_1620D           0x5202  //DI wide voltage
#define USB_PID_RTU_86M_2821D           0x5242  //DO wide voltage

    /* Communication Module */
#define USB_PID_RTU_85M_5401            0x53e0  //Serial
#define USB_PID_RTU_85M_5220            0x53e1  //IBIS
#define USB_PID_RTU_85M_5142            0x53e2  //HSPA
#define USB_PID_RTU_85M_5130            0x53e3  //WIFI
#define USB_PID_RTU_86M_5212U           0x53e5  // 2-wire switch
#define USB_PID_RTU_86M_5811M           0x53e6  //Managed switch
#define USB_PID_RTU_86M_5814U           0x53e7  // 8-port Gigabit PoE switch
#define USB_PID_RTU_86M_5250            0x53e8  //CAN Bus

    /* Misc Module */
#define USB_PID_RTU_PW10_15W            0x5400
#define USB_PID_RTU_PW10_30W            0x5401
#define USB_PID_RTU_PW20_50W            0x5403

#define MAX_SLOT                        12
#define MAX_CHANNEL                     24

#define MODULE_SLOT_CPU_POWER           0
#define MODULE_SLOT_CPU                 0


    /*********************************************IO*********************************************/
    typedef enum
    {
        IO_ERR_OK = 0,                              
        IO_ERR_DEVICE,                              
        IO_ERR_CMD,                             
        IO_ERR_TYPE,                                
        IO_ERR_ITEM,                                
        IO_ERR_COPY,                                
        IO_ERR_SLOT,                                
        IO_ERR_CHANNEL,                         
        IO_ERR_ARGUMENT,                            
        IO_ERR_RW,                                  
        IO_ERR_ACTION,                              
        IO_ERR_UPDATE,                              
        IO_ERR_FAST_AI_BUF_EMPTY,               
        IO_ERR_FAST_AI_BUF_OVERFLOW,            
        IO_ERR_FAST_AI_NO_BUF,                  
        IO_ERR_IO_MODULE_REQUEST,               
        IO_ERR_COMMUNICATION_MODULE_REQUEST,    
        IO_ERR_SEMAPHORE_LOCK,                  
        IO_ERR_TIMEOUT,                         
        IO_ERR_SYSTEM_INFO,                     
        IO_ERR_MODULE_INFO,                     
        IO_ERR_HOT_PLUG,                            
        IO_ERR_DUPLICATE_REGISTER,              
        IO_ERR_IO_EVENT_QUEUE_EMPTY,                
        IO_ERR_CELLULAR_POWER_INDICATOR,        
        IO_ERR_CELLULAR_EMERGENCY_OFF,          
        IO_ERR_CELLULAR_IGT_ON,                 
        IO_ERR_POWER,                               
        IO_ERR_COMMUNICATION_MODULE_SUPPORT,    
        IO_ERR_AMOUNT                               
    } IO_ERR_CODE;  

    typedef enum
    {
        MODULE_RW_ERR_OK = 0,                               
        MODULE_RW_ERR_DEVICE,                               
        MODULE_RW_ERR_SLOT,                                 
        MODULE_RW_ERR_CMD,                                  
        MODULE_RW_ERR_ARGUMENT,                             
        MODULE_RW_ERR_SYSTEM_INFO,                          
        MODULE_RW_ERR_RW,                                   
        MODULE_RW_ERR_VERSION,                              
        MODULE_RW_ERR_TIMEOUT,                              
        MODULE_RW_ERR_PACKET,                               
        MODULE_RW_ERR_INSUFFICIENT_BUFFER,                  
        MODULE_RW_ERR_REQUEST_FAIL,                         
        MODULE_RW_ERR_REQUEST_EXCEPTION,                    
        MODULE_RW_ERR_BUSY,                                 
        MODULE_RW_ERR_COMMUNICATION_MODULE_REQUEST,         
        MODULE_RW_ERR_SEMAPHORE_LOCK,                       
        MODULE_RW_ERR_HOT_PLUG,                             
        MODULE_RW_ERR_TIMESTAMP,                            
        MODULE_RW_ERR_TIMESTAMP_NOT_SYNC,                   
        MODULE_RW_ERR_IO_PARAM,                             
        MODULE_RW_ERR_IO_TYPE,                              
        MODULE_RW_ERR_IO_CH,                                
        MODULE_RW_ERR_IO_MODE,                              
        MODULE_RW_ERR_IO_FILTER,                            
        MODULE_RW_ERR_IO_TRIGGER,                           
        MODULE_RW_ERR_IO_SIG_WIDTH,                         
        MODULE_RW_ERR_IO_PWM,                               
        MODULE_RW_ERR_IO_BURNOUT,                           
        MODULE_RW_ERR_IO_CALIBRATION,                       
        MODULE_RW_ERR_IO_BUF,                               
        MODULE_RW_ERR_IO_FAST_AI_BUF_EMPTY,                 
        MODULE_RW_ERR_IO_FAST_AI_BUF_OVERFLOW,              
        MODULE_RW_ERR_IO_FAST_AI_NOT_READY,                 
        MODULE_RW_ERR_POWER,                                
        MODULE_RW_ERR_NO_EXTERNAL_POWER,                    
        MODULE_RW_ERR_IO_FPGA,                              
        MODULE_RW_ERR_AMOUNT                                
    } MODULE_RW_ERR_CODE;

    typedef enum
    {
        AI_TRANSFORM_ERR_OK = 0,                    
        AI_TRANSFORM_ERR_RANGE,                 
        AI_TRANSFORM_ERR_VALUE,                 
        AI_TRANSFORM_ERR_ARGUMENT,              
        AI_TRANSFORM_ERR_AMOUNT             
    } AI_TRANSFORM_ERR_CODE;

    typedef enum
    {
        AIO_TRANSFORM_ERR_OK = 0,               
        AIO_TRANSFORM_ERR_RANGE,                
        AIO_TRANSFORM_ERR_BITSRESOLUTION,       
        AIO_TRANSFORM_ERR_VALUE,                
        AIO_TRANSFORM_ERR_ARGUMENT,         
        AIO_TRANSFORM_ERR_AMOUNT                
    } AIO_TRANSFORM_ERR_CODE;

    struct IO_Info {
        UINT8 di_channels;
        UINT8 do_channels;
        UINT8 dio_channels;
        UINT8 ai_channels;
        UINT8 fast_ai_channels;
        UINT8 ao_channels;
        UINT8 tc_channels;
        UINT8 rtd_channels;
        UINT8 counter_channels;
        UINT8 pulse_channels;
        UINT8 relay_channels;
    } __attribute__((packed));

    struct Module_Info {
        UINT8 slot;
        UINT8 unit_id;
        UINT16 vendor_id;
        UINT16 product_id;
        UINT32 serial_number;   
        UINT16 hw_version;
        UINT16 fw_version;
        struct IO_Info io_info;
    } __attribute__((packed));

    struct Timestamp {
        UINT32 msec;    
        UINT32 sec;
        UINT32 min;
        UINT32 hour;
        UINT32 day;
        UINT32 mon;
        UINT32 year;
    } __attribute__((packed));

    //--------------------------------------------------------------------------------------------------------
    //ETHERNET ADAPTER TYPE
#define ETHERNET_ADAPTER_RJ45       0
#define ETHERNET_ADAPTER_M12            1
    //SWITCH MODE
#define SWITCH_DUMP_MODE            0
#define SWITCH_VLAN_MODE                1

#define SRAM_START_ADDRESS          0x0
#define SRAM_END_ADDRESS            0x20000 //128KB

#define FRAM_START_ADDRESS          0x0
#define FRAM_END_ADDRESS                0x10000 //64KB

    IO_ERR_CODE MX_RTU_Module_Info_Get(UINT8 slot, struct Module_Info *module_info);
    IO_ERR_CODE MX_RTU_Total_Slots_Get(UINT32 *num);
    IO_ERR_CODE MX_RTU_Slot_Inserted_Get(UINT32 *state); //Example: 0x6 -> slot 1 and 2 are inserted
    IO_ERR_CODE MX_RTU_Ethernet_Adapter_Type_Get(UINT32 *type); //ETHERNET ADAPTER TYPE
    IO_ERR_CODE MX_RTU_CPU_Switch_Mode_Get(UINT32 *mode);

    IO_ERR_CODE MX_RTU_SRAM_Read(UINT32 start_address, UINT32 length, UINT8 *buf);
    IO_ERR_CODE MX_RTU_SRAM_Write(UINT32 start_address, UINT32 length, UINT8 *buf);

    IO_ERR_CODE MX_RTU_FRAM_Read(UINT32 start_address, UINT32 length, UINT8 *buf);
    IO_ERR_CODE MX_RTU_FRAM_Write(UINT32 start_address, UINT32 length, UINT8 *buf);
    //--------------------------------------------------------------------------------------------------------
#define IO_EVENT_QUEUE_MAX              100

    //DI EVENT TRIGGER CONDITION
#define DI_EVENT_TOGGLE_L2H             0
#define DI_EVENT_TOGGLE_H2L             1
#define DI_EVENT_TOGGLE_BOTH                2

    //AI EVENT TRIGGER CONDITION
#define AI_TC_RTD_EVENT_GREATER         0
#define AI_TC_RTD_EVENT_SMALLER         1
#define AI_TC_RTD_EVENT_EQUAL           2

    IO_ERR_CODE MX_RTU_DI_Event_Register(UINT8 slot, UINT8 channel, UINT32 trigger, int *handle); //DI EVENT TRIGGER CONDITION
    IO_ERR_CODE MX_RTU_DI_Event_Unregister(int handle);
    IO_ERR_CODE MX_RTU_DI_Event_Get(int handle, UINT32 *status, struct Timestamp *time);
    IO_ERR_CODE MX_RTU_DI_Event_Set_Trigger(int handle, UINT32 trigger); //DI EVENT TRIGGER CONDITION
    IO_ERR_CODE MX_RTU_DI_Event_Get_Trigger(int handle, UINT32 *trigger); //DI EVENT TRIGGER CONDITION
    IO_ERR_CODE MX_RTU_DI_Event_Count(int handle, UINT32 *count);
    IO_ERR_CODE MX_RTU_DI_Event_Clear(int handle);
    IO_ERR_CODE MX_RTU_DI_Event_Reset();

    IO_ERR_CODE MX_RTU_DIO_DI_Event_Register(UINT8 slot, UINT8 channel, UINT32 trigger, int *handle); //DI EVENT TRIGGER CONDITION
    IO_ERR_CODE MX_RTU_DIO_DI_Event_Unregister(int handle);
    IO_ERR_CODE MX_RTU_DIO_DI_Event_Get(int handle, UINT32 *status, struct Timestamp *time);
    IO_ERR_CODE MX_RTU_DIO_DI_Event_Set_Trigger(int handle, UINT32 trigger); //DI EVENT TRIGGER CONDITION
    IO_ERR_CODE MX_RTU_DIO_DI_Event_Get_Trigger(int handle, UINT32 *trigger); //DI EVENT TRIGGER CONDITION
    IO_ERR_CODE MX_RTU_DIO_DI_Event_Count(int handle, UINT32 *count);
    IO_ERR_CODE MX_RTU_DIO_DI_Event_Clear(int handle);
    IO_ERR_CODE MX_RTU_DIO_DI_Event_Reset();

    IO_ERR_CODE MX_RTU_AI_Event_Register(UINT8 slot, UINT8 channel, float value, UINT32 condition, int *handle); //AI EVENT TRIGGER CONDITION
    IO_ERR_CODE MX_RTU_AI_Event_Unregister(int handle);
    IO_ERR_CODE MX_RTU_AI_Event_Get(int handle, float *value, struct Timestamp *time);
    IO_ERR_CODE MX_RTU_AI_Event_Set_Value(int handle, float value, UINT32 condition); //AI EVENT TRIGGER CONDITION
    IO_ERR_CODE MX_RTU_AI_Event_Get_Value(int handle, float *value, UINT32 *condition); //AI EVENT TRIGGER CONDITION
    IO_ERR_CODE MX_RTU_AI_Event_Count(int handle, UINT32 *count);
    IO_ERR_CODE MX_RTU_AI_Event_Clear(int handle);
    IO_ERR_CODE MX_RTU_AI_Event_Reset(void);

    IO_ERR_CODE MX_RTU_TC_Event_Register(UINT8 slot, UINT8 channel, float value, UINT32 condition, int *handle); //AI EVENT TRIGGER CONDITION
    IO_ERR_CODE MX_RTU_TC_Event_Unregister(int handle);
    IO_ERR_CODE MX_RTU_TC_Event_Get(int handle, float *value, struct Timestamp *time);
    IO_ERR_CODE MX_RTU_TC_Event_Set_Value(int handle, float value, UINT32 condition); //AI EVENT TRIGGER CONDITION
    IO_ERR_CODE MX_RTU_TC_Event_Get_Value(int handle, float *value, UINT32 *condition); //AI EVENT TRIGGER CONDITION
    IO_ERR_CODE MX_RTU_TC_Event_Count(int handle, UINT32 *count);
    IO_ERR_CODE MX_RTU_TC_Event_Clear(int handle);
    IO_ERR_CODE MX_RTU_TC_Event_Reset(void);

    IO_ERR_CODE MX_RTU_RTD_Event_Register(UINT8 slot, UINT8 channel, float value, UINT32 condition, int *handle); //AI EVENT TRIGGER CONDITION
    IO_ERR_CODE MX_RTU_RTD_Event_Unregister(int handle);
    IO_ERR_CODE MX_RTU_RTD_Event_Get(int handle, float *value, struct Timestamp *time);
    IO_ERR_CODE MX_RTU_RTD_Event_Set_Value(int handle, float value, UINT32 condition); //AI EVENT TRIGGER CONDITION
    IO_ERR_CODE MX_RTU_RTD_Event_Get_Value(int handle, float *value, UINT32 *condition); //AI EVENT TRIGGER CONDITION
    IO_ERR_CODE MX_RTU_RTD_Event_Count(int handle, UINT32 *count);
    IO_ERR_CODE MX_RTU_RTD_Event_Clear(int handle);
    IO_ERR_CODE MX_RTU_RTD_Event_Reset(void);
    //--------------------------------------------------------------------------------------------------------
    //DI MODE
#define DI_MODE_DI                      0
#define DI_MODE_COUNTER             1
#define DI_MODE_FREQUENCY           2

    //DI COUNTER TRIGGER CONDITION
#define DI_TOGGLE_L2H                   0
#define DI_TOGGLE_H2L                   1
#define DI_TOGGLE_BOTH              2

    MODULE_RW_ERR_CODE MX_RTU_Module_DI_Value_Get(UINT8 slot, UINT32 *value, struct Timestamp *time);

    MODULE_RW_ERR_CODE MX_RTU_Module_DI_Mode_Get(UINT8 slot, UINT8 start, UINT8 count, UINT8 *buf);

    MODULE_RW_ERR_CODE MX_RTU_Module_DI_Filter_Get(UINT8 slot, UINT8 start, UINT8 count, UINT32 *buf);

    MODULE_RW_ERR_CODE MX_RTU_Module_DI_Counter_Start_Get(UINT8 slot, UINT32 *start);

    MODULE_RW_ERR_CODE MX_RTU_Module_DI_Counter_Trigger_Get(UINT8 slot, UINT8 start, UINT8 count, UINT8 *buf);

    MODULE_RW_ERR_CODE MX_RTU_Module_DI_Counter_Value_Get(UINT8 slot, UINT8 start, UINT8 count, UINT32 *buf, struct Timestamp *time);

    MODULE_RW_ERR_CODE MX_RTU_Module_DI_Counter_Overflow_Get(UINT8 slot, UINT32 *overflow);

    MODULE_RW_ERR_CODE MX_RTU_Module_DI_Frequency_Start_Get(UINT8 slot, UINT32 *start);

    MODULE_RW_ERR_CODE MX_RTU_Module_DI_Frequency_Measurement_Time_Get(UINT8 slot, UINT8 start, UINT8 count, UINT32 *buf);

    MODULE_RW_ERR_CODE MX_RTU_Module_DI_Frequency_Get(UINT8 slot, UINT8 start, UINT8 count, float *buf);

    MODULE_RW_ERR_CODE MX_RTU_Module_DI_Debouncing_Enable_Get(UINT8 slot, UINT32 *enable);

    MODULE_RW_ERR_CODE MX_RTU_Module_DI_Debouncing_Time_Get(UINT8 slot, UINT8 start, UINT8 count, float *buf);

    MODULE_RW_ERR_CODE MX_RTU_Module_DI_Ext_Power_Get(UINT8 slot, UINT32 *value);

    MODULE_RW_ERR_CODE MX_RTU_Module_DI_Mode_Set(UINT8 slot, UINT8 start, UINT8 count, UINT8 *buf);

    MODULE_RW_ERR_CODE MX_RTU_Module_DI_Filter_Set(UINT8 slot, UINT8 start, UINT8 count, UINT32 *buf);

    MODULE_RW_ERR_CODE MX_RTU_Module_DI_Counter_Start_Set(UINT8 slot, UINT32 start);

    MODULE_RW_ERR_CODE MX_RTU_Module_DI_Counter_Trigger_Set(UINT8 slot, UINT8 start, UINT8 count, UINT8 *buf);

    MODULE_RW_ERR_CODE MX_RTU_Module_DI_Counter_Value_Set(UINT8 slot, UINT8 start, UINT8 count, UINT32 *buf);

    MODULE_RW_ERR_CODE MX_RTU_Module_DI_Counter_Overflow_Reset(UINT8 slot, UINT32 overflow);

    MODULE_RW_ERR_CODE MX_RTU_Module_DI_Frequency_Start_Set(UINT8 slot, UINT32 start);

    MODULE_RW_ERR_CODE MX_RTU_Module_DI_Frequency_Measurement_Time_Set(UINT8 slot, UINT8 start, UINT8 count, UINT32 *buf);

    MODULE_RW_ERR_CODE MX_RTU_Module_DI_Debouncing_Enable_Set(UINT8 slot, UINT32 enable);

    MODULE_RW_ERR_CODE MX_RTU_Module_DI_Debouncing_Time_Set(UINT8 slot, UINT8 start, UINT8 count, float *buf);


    //--------------------------------------------------------------------------------------------------------
    //DO MODE
#define DO_MODE_DO                  0
#define DO_MODE_PWM                 1

    //DO_DIAGNOSTIC_STATUS
#define DO_DIAGNOSTIC_STATUS_NORMAL     0x00
#define DO_DIAGNOSTIC_STATUS_ERROR      0x01

    MODULE_RW_ERR_CODE MX_RTU_Module_DO_Value_Get(UINT8 slot, UINT32 *value);

    MODULE_RW_ERR_CODE MX_RTU_Module_DO_Mode_Get(UINT8 slot, UINT8 start, UINT8 count, UINT8 *buf);

    /*
      \param[in] slot slot 0: Built-in IO Ports, slot 1 ~ 12: Expansion IO Ports.
      \param[in] start    The first channel to get.
      \param[in] count    The number of channels to get.
      \param[out] buf The DO PWM <b>LOW</b> signal width for the corresponding channels, Low = On.
      \li 85M-2600: One unit = 100us.
      \li 86M-2830D: One unit = 500us.
      \li ioPAC 5542: One unit = 500us.
      \li ioPAC 5542-HSPA: One unit = 500us.  
      \return         \link MODULE_RW_ERR_CODE MODULE_RW_ERR_CODE \endlink
    */
    MODULE_RW_ERR_CODE MX_RTU_Module_DO_SigW_L_Get(UINT8 slot, UINT8 start, UINT8 count, UINT32 *buf);

    /*
      \param[in] slot slot 0: Built-in IO Ports, slot 1 ~ 12: Expansion IO Ports.
      \param[in] start    The first channel to get.
      \param[in] count    The number of channels to get.
      \param[out] buf The DO PWM <b>HIGH</b> signal width for the corresponding channels, High = Off.
      \li 85M-2600: One unit = 100us.
      \li 86M-2830D: One unit = 500us.
      \li ioPAC 5542: One unit = 500us.
      \li ioPAC 5542-HSPA: One unit = 500us.  
      \return         \link MODULE_RW_ERR_CODE MODULE_RW_ERR_CODE \endlink
    */
    MODULE_RW_ERR_CODE MX_RTU_Module_DO_SigW_H_Get(UINT8 slot, UINT8 start, UINT8 count, UINT32 *buf);

    MODULE_RW_ERR_CODE MX_RTU_Module_DO_PWM_Start_Get(UINT8 slot, UINT32 *start);

    MODULE_RW_ERR_CODE MX_RTU_Module_DO_PWM_Count_Get(UINT8 slot, UINT8 start, UINT8 count, UINT32 *buf);

    MODULE_RW_ERR_CODE MX_RTU_Module_DO_PWM_Config_Get(UINT8 slot, UINT8 start, UINT8 count, float *frequency, float *dutyCycle);

    MODULE_RW_ERR_CODE MX_RTU_Module_DO_Diagnostic_Status_Get(UINT8 slot, UINT8 start, UINT8 count, UINT8 *buf);

    MODULE_RW_ERR_CODE MX_RTU_Module_DO_Value_Set(UINT8 slot, UINT32 value);

    MODULE_RW_ERR_CODE MX_RTU_Module_DO_Mode_Set(UINT8 slot, UINT8 start, UINT8 count, UINT8 *buf); //DO MODE

    /*
      \param[in] slot slot 0: Built-in IO Ports, slot 1 ~ 12: Expansion IO Ports.
      \param[in] start    The first channel to set.
      \param[in] count    The number of channels to set.
      \param[in] buf  The DO PWM <b>LOW</b> signal width for the corresponding channels, Low = On.
      \li 85M-2600: One unit = 100us.
      \li 86M-2830D: One unit = 500us.
      \li ioPAC 5542: One unit = 500us.
      \li ioPAC 5542-HSPA: One unit = 500us.  
      \return         \link MODULE_RW_ERR_CODE MODULE_RW_ERR_CODE \endlink
    */
    MODULE_RW_ERR_CODE MX_RTU_Module_DO_SigW_L_Set(UINT8 slot, UINT8 start, UINT8 count, UINT32 *buf);

    /*
      \param[in] slot slot 0: Built-in IO Ports, slot 1 ~ 12: Expansion IO Ports.
      \param[in] start    The first channel to set.
      \param[in] count    The number of channels to set.
      \param[in] buf  The DO PWM <b>HIGH</b> signal width for the corresponding channels, High = Off.
      \li 85M-2600: One unit = 100us.
      \li 86M-2830D: One unit = 500us.
      \li ioPAC 5542: One unit = 500us.
      \li ioPAC 5542-HSPA: One unit = 500us.  
      \return         \link MODULE_RW_ERR_CODE MODULE_RW_ERR_CODE \endlink
    */
    MODULE_RW_ERR_CODE MX_RTU_Module_DO_SigW_H_Set(UINT8 slot, UINT8 start, UINT8 count, UINT32 *buf);

    MODULE_RW_ERR_CODE MX_RTU_Module_DO_PWM_Start_Set(UINT8 slot, UINT32 start);

    MODULE_RW_ERR_CODE MX_RTU_Module_DO_PWM_Stop_Set(UINT8 slot, UINT32 stop);

    MODULE_RW_ERR_CODE MX_RTU_Module_DO_PWM_Count_Set(UINT8 slot, UINT8 start, UINT8 count, UINT32 *buf);

    MODULE_RW_ERR_CODE MX_RTU_Module_DO_PWM_Config_Set(UINT8 slot, UINT8 start, UINT8 count, float *frequency, float *dutyCycle);

    MODULE_RW_ERR_CODE MX_RTU_Module_DO_Diagnostic_Status_Clean_Set(UINT8 slot, UINT32 clean);

    //--------------------------------------------------------------------------------------------------------
    //RELAY MODE
#define RELAY_MODE_RELAY                    0
#define RELAY_MODE_PWM                  1

    MODULE_RW_ERR_CODE MX_RTU_Module_Relay_Value_Get(UINT8 slot, UINT32 *value);

    MODULE_RW_ERR_CODE MX_RTU_Module_Relay_Mode_Get(UINT8 slot, UINT8 start, UINT8 count, UINT8 *buf);

    /*
      \param[in] slot slot 0: Built-in IO Ports, slot 1 ~ 12: Expansion IO Ports.
      \param[in] start    The first channel to get.
      \param[in] count    The number of channels to get.
      \param[out] buf The RELAY PWM <b>LOW</b> signal width for the corresponding channels, Low = On.
      \li ioPAC 8500/8600: One unit = 1.5sec.
      \return         \link MODULE_RW_ERR_CODE MODULE_RW_ERR_CODE \endlink
    */
    MODULE_RW_ERR_CODE MX_RTU_Module_Relay_SigW_L_Get(UINT8 slot, UINT8 start, UINT8 count, UINT32 *buf);

    /*
      \param[in] slot slot 0: Built-in IO Ports, slot 1 ~ 12: Expansion IO Ports.
      \param[in] start    The first channel to get.
      \param[in] count    The number of channels to get.
      \param[out] buf The RELAY PWM <b>HIGH</b> signal width for the corresponding channels, High = Off.
      \li ioPAC 8500/8600: One unit = 1.5sec.
      \return         \link MODULE_RW_ERR_CODE MODULE_RW_ERR_CODE \endlink
    */
    MODULE_RW_ERR_CODE MX_RTU_Module_Relay_SigW_H_Get(UINT8 slot, UINT8 start, UINT8 count, UINT32 *buf);

    MODULE_RW_ERR_CODE MX_RTU_Module_Relay_PWM_Start_Get(UINT8 slot, UINT32 *start);

    MODULE_RW_ERR_CODE MX_RTU_Module_Relay_PWM_Count_Get(UINT8 slot, UINT8 start, UINT8 count, UINT32 *buf);

    MODULE_RW_ERR_CODE MX_RTU_Module_Relay_PWM_Config_Get(UINT8 slot, UINT8 start, UINT8 count, float *frequency, float *dutyCycle);

    MODULE_RW_ERR_CODE MX_RTU_Module_Relay_Total_Count_Get(UINT8 slot, UINT8 start, UINT8 count, UINT32 *buf);

    MODULE_RW_ERR_CODE MX_RTU_Module_Relay_Current_Count_Get(UINT8 slot, UINT8 start, UINT8 count, UINT32 *buf);

    MODULE_RW_ERR_CODE MX_RTU_Module_Relay_Value_Set(UINT8 slot, UINT32 value);

    MODULE_RW_ERR_CODE MX_RTU_Module_Relay_Mode_Set(UINT8 slot, UINT8 start, UINT8 count, UINT8 *buf); //DO MODE

    /*
      \param[in] slot slot 0: Built-in IO Ports, slot 1 ~ 12: Expansion IO Ports.
      \param[in] start    The first channel to set.
      \param[in] count    The number of channels to set.
      \param[in] buf  The RELAY PWM <b>LOW</b> signal width for the corresponding channels, Low = On.
      \li ioPAC 8500/8600: One unit = 1.5sec.
      \return         \link MODULE_RW_ERR_CODE MODULE_RW_ERR_CODE \endlink
    */
    MODULE_RW_ERR_CODE MX_RTU_Module_Relay_SigW_L_Set(UINT8 slot, UINT8 start, UINT8 count, UINT32 *buf);

    /*
      \param[in] slot slot 0: Built-in IO Ports, slot 1 ~ 12: Expansion IO Ports.
      \param[in] start    The first channel to set.
      \param[in] count    The number of channels to set.
      \param[in] buf  The RELAY PWM <b>HIGH</b> signal width for the corresponding channels, High = Off.
      \li ioPAC 8500/8600: One unit = 1.5sec.
      \return         \link MODULE_RW_ERR_CODE MODULE_RW_ERR_CODE \endlink
    */
    MODULE_RW_ERR_CODE MX_RTU_Module_Relay_SigW_H_Set(UINT8 slot, UINT8 start, UINT8 count, UINT32 *buf);

    MODULE_RW_ERR_CODE MX_RTU_Module_Relay_PWM_Start_Set(UINT8 slot, UINT32 start);

    MODULE_RW_ERR_CODE MX_RTU_Module_Relay_PWM_Stop_Set(UINT8 slot, UINT32 stop);

    MODULE_RW_ERR_CODE MX_RTU_Module_Relay_PWM_Count_Set(UINT8 slot, UINT8 start, UINT8 count, UINT32 *buf);

    MODULE_RW_ERR_CODE MX_RTU_Module_Relay_PWM_Config_Set(UINT8 slot, UINT8 start, UINT8 count, float *frequency, float *dutyCycle);

    MODULE_RW_ERR_CODE MX_RTU_Module_Relay_Current_Count_Set(UINT8 slot, UINT8 start, UINT8 count, UINT32 *buf);


    //--------------------------------------------------------------------------------------------------------
    MODULE_RW_ERR_CODE MX_RTU_Module_DIO_Map_Get(UINT8 slot, UINT32 *map);
    MODULE_RW_ERR_CODE MX_RTU_Module_DIO_Map_Set(UINT8 slot, UINT32 map);

    MODULE_RW_ERR_CODE MX_RTU_Module_DIO_DI_Value_Get(UINT8 slot, UINT32 *value, struct Timestamp *time);
    MODULE_RW_ERR_CODE MX_RTU_Module_DIO_DI_Mode_Get(UINT8 slot, UINT8 start, UINT8 count, UINT8 *buf);
    MODULE_RW_ERR_CODE MX_RTU_Module_DIO_DI_Filter_Get(UINT8 slot, UINT8 start, UINT8 count, UINT32 *buf);
    MODULE_RW_ERR_CODE MX_RTU_Module_DIO_DI_Counter_Start_Get(UINT8 slot, UINT32 *start);
    MODULE_RW_ERR_CODE MX_RTU_Module_DIO_DI_Counter_Trigger_Get(UINT8 slot, UINT8 start, UINT8 count, UINT8 *buf);
    MODULE_RW_ERR_CODE MX_RTU_Module_DIO_DI_Counter_Value_Get(UINT8 slot, UINT8 start, UINT8 count, UINT32 *buf, struct Timestamp *time);
    MODULE_RW_ERR_CODE MX_RTU_Module_DIO_DI_Counter_Overflow_Get(UINT8 slot, UINT32 *overflow);

    MODULE_RW_ERR_CODE MX_RTU_Module_DIO_DI_Frequency_Start_Get(UINT8 slot, UINT32 *start);

    MODULE_RW_ERR_CODE MX_RTU_Module_DIO_DI_Frequency_Measurement_Time_Get(UINT8 slot, UINT8 start, UINT8 count, UINT32 *buf);

    MODULE_RW_ERR_CODE MX_RTU_Module_DIO_DI_Frequency_Get(UINT8 slot, UINT8 start, UINT8 count, float *buf);

    MODULE_RW_ERR_CODE MX_RTU_Module_DIO_DI_Mode_Set(UINT8 slot, UINT8 start, UINT8 count, UINT8 *buf);
    MODULE_RW_ERR_CODE MX_RTU_Module_DIO_DI_Filter_Set(UINT8 slot, UINT8 start, UINT8 count, UINT32 *buf);
    MODULE_RW_ERR_CODE MX_RTU_Module_DIO_DI_Counter_Start_Set(UINT8 slot, UINT32 start);
    MODULE_RW_ERR_CODE MX_RTU_Module_DIO_DI_Counter_Trigger_Set(UINT8 slot, UINT8 start, UINT8 count, UINT8 *buf);
    MODULE_RW_ERR_CODE MX_RTU_Module_DIO_DI_Counter_Value_Set(UINT8 slot, UINT8 start, UINT8 count, UINT32 *buf);
    MODULE_RW_ERR_CODE MX_RTU_Module_DIO_DI_Counter_Overflow_Reset(UINT8 slot, UINT32 overflow);

    MODULE_RW_ERR_CODE MX_RTU_Module_DIO_DI_Frequency_Start_Set(UINT8 slot, UINT32 start);

    MODULE_RW_ERR_CODE MX_RTU_Module_DIO_DI_Frequency_Measurement_Time_Set(UINT8 slot, UINT8 start, UINT8 count, UINT32 *buf);

    /* Same as DO*/
    MODULE_RW_ERR_CODE MX_RTU_Module_DIO_DO_Value_Get(UINT8 slot, UINT32 *value);
    MODULE_RW_ERR_CODE MX_RTU_Module_DIO_DO_Mode_Get(UINT8 slot, UINT8 start, UINT8 count, UINT8 *buf);
    /*
      \param[in] slot slot 0: Built-in IO Ports, slot 1 ~ 12: Expansion IO Ports.
      \param[in] start    The first channel to get.
      \param[in] count    The number of channels to get.
      \param[out] buf The DO PWM <b>LOW</b> signal width for the corresponding channels, Low = On.
      \li 85M-2600: One unit = 100us.
      \li 86M-2830D: One unit = 500us.
      \li ioPAC 5542: One unit = 500us.
      \li ioPAC 5542-HSPA: One unit = 500us.  
      \return         \link MODULE_RW_ERR_CODE MODULE_RW_ERR_CODE \endlink
    */
    MODULE_RW_ERR_CODE MX_RTU_Module_DIO_DO_SigW_L_Get(UINT8 slot, UINT8 start, UINT8 count, UINT32 *buf);
    /*
      \param[in] slot slot 0: Built-in IO Ports, slot 1 ~ 12: Expansion IO Ports.
      \param[in] start    The first channel to get.
      \param[in] count    The number of channels to get.
      \param[out] buf The DO PWM <b>HIGH</b> signal width for the corresponding channels, High = Off.
      \li 85M-2600: One unit = 100us.
      \li 86M-2830D: One unit = 500us.
      \li ioPAC 5542: One unit = 500us.
      \li ioPAC 5542-HSPA: One unit = 500us.  
      \return         \link MODULE_RW_ERR_CODE MODULE_RW_ERR_CODE \endlink
    */
    MODULE_RW_ERR_CODE MX_RTU_Module_DIO_DO_SigW_H_Get(UINT8 slot, UINT8 start, UINT8 count, UINT32 *buf);
    MODULE_RW_ERR_CODE MX_RTU_Module_DIO_DO_PWM_Start_Get(UINT8 slot, UINT32 *start);
    MODULE_RW_ERR_CODE MX_RTU_Module_DIO_DO_PWM_Count_Get(UINT8 slot, UINT8 start, UINT8 count, UINT32 *buf);

    MODULE_RW_ERR_CODE MX_RTU_Module_DIO_DO_PWM_Config_Get(UINT8 slot, UINT8 start, UINT8 count, float *frequency, float *dutyCycle);

    /* Same as DO*/
    MODULE_RW_ERR_CODE MX_RTU_Module_DIO_DO_Value_Set(UINT8 slot, UINT32 value);
    MODULE_RW_ERR_CODE MX_RTU_Module_DIO_DO_Mode_Set(UINT8 slot, UINT8 start, UINT8 count, UINT8 *buf);
    /*
      \param[in] slot slot 0: Built-in IO Ports, slot 1 ~ 12: Expansion IO Ports.
      \param[in] start    The first channel to set.
      \param[in] count    The number of channels to set.
      \param[in] buf  The DO PWM <b>LOW</b> signal width for the corresponding channels, Low = On.
      \li ioPAC 8500/8600: One unit = 100us.
      \li ioPAC 5542: One unit = 500us.
      \li ioPAC 5542-HSPA: One unit = 500us.  
      \return         \link MODULE_RW_ERR_CODE MODULE_RW_ERR_CODE \endlink
    */
    MODULE_RW_ERR_CODE MX_RTU_Module_DIO_DO_SigW_L_Set(UINT8 slot, UINT8 start, UINT8 count, UINT32 *buf);
    /*
      \param[in] slot slot 0: Built-in IO Ports, slot 1 ~ 12: Expansion IO Ports.
      \param[in] start    The first channel to set.
      \param[in] count    The number of channels to set.
      \param[in] buf  The DO PWM <b>HIGH</b> signal width for the corresponding channels, High = Off.
      \li ioPAC 8500/8600: One unit = 100us.
      \li ioPAC 5542: One unit = 500us.
      \li ioPAC 5542-HSPA: One unit = 500us.  
      \return         \link MODULE_RW_ERR_CODE MODULE_RW_ERR_CODE \endlink
    */
    MODULE_RW_ERR_CODE MX_RTU_Module_DIO_DO_SigW_H_Set(UINT8 slot, UINT8 start, UINT8 count, UINT32 *buf);
    MODULE_RW_ERR_CODE MX_RTU_Module_DIO_DO_PWM_Start_Set(UINT8 slot, UINT32 start);
    MODULE_RW_ERR_CODE MX_RTU_Module_DIO_DO_PWM_Stop_Set(UINT8 slot, UINT32 stop);
    MODULE_RW_ERR_CODE MX_RTU_Module_DIO_DO_PWM_Count_Set(UINT8 slot, UINT8 start, UINT8 count, UINT32 *buf);

    MODULE_RW_ERR_CODE MX_RTU_Module_DIO_DO_PWM_Config_Set(UINT8 slot, UINT8 start, UINT8 count, float *frequency, float *dutyCycle);

    //--------------------------------------------------------------------------------------------------------
    //AI RANGE
    //#define   AI_RANGE_5V                 0 //not support
#define AI_RANGE_10V                    1
#define AI_RANGE_0_10V                  2
#define AI_RANGE_0_20mA             3
#define AI_RANGE_4_20mA             4
#define AI_RANGE_0_50mA             5

    //AI STATUS
#define AI_STATUS_BURNOUT           0
#define AI_STATUS_UNDER_RANGE       1
#define AI_STATUS_NORMAL_RANGE      2
#define AI_STATUS_OVER_RANGE            3

    //AI enable mode
#define AI_ENABLE_MODE_1ch          0
#define AI_ENABLE_MODE_2ch          1
#define AI_ENABLE_MODE_4ch          2
#define AI_ENABLE_MODE_8ch          3

    AI_TRANSFORM_ERR_CODE MX_RTU_AI_Raw_to_Eng(UINT32 range, int rawValue, float *engValue);

    AI_TRANSFORM_ERR_CODE MX_RTU_AI_Eng_to_Raw(UINT32 range, float engValue, int *rawValue);

    AIO_TRANSFORM_ERR_CODE MX_RTU_AIO_Raw_to_Eng(UINT32 range, UINT32 bitsResolution, int rawValue, float *engValue);

    AIO_TRANSFORM_ERR_CODE MX_RTU_AIO_Eng_to_Raw(UINT32 range, UINT32 bitsResolution, float engValue, int *rawValue);

    void MX_RTU_AI_Raw_to_Eng_0_10V(int rawValue, float *engValue);

    void MX_RTU_AI_Raw_to_Eng_4_20mA(int rawValue, float *engValue);

    MODULE_RW_ERR_CODE MX_RTU_Module_AI_Raw_Value_Get(UINT8 slot, UINT8 start, UINT8 count, UINT32 *buf, struct Timestamp *time);

    MODULE_RW_ERR_CODE MX_RTU_Module_AI_Eng_Value_Get(UINT8 slot, UINT8 start, UINT8 count, float *buf, struct Timestamp *time);

    MODULE_RW_ERR_CODE MX_RTU_Module_AI_Range_Get(UINT8 slot, UINT8 start, UINT8 count, UINT8 *buf); //AI RANGE

    MODULE_RW_ERR_CODE MX_RTU_Module_AI_Min_Raw_Get(UINT8 slot, UINT8 start, UINT8 count, UINT32 *buf);

    MODULE_RW_ERR_CODE MX_RTU_Module_AI_Max_Raw_Get(UINT8 slot, UINT8 start, UINT8 count, UINT32 *buf);

    MODULE_RW_ERR_CODE MX_RTU_Module_AI_Min_Eng_Get(UINT8 slot, UINT8 start, UINT8 count, float *buf);

    MODULE_RW_ERR_CODE MX_RTU_Module_AI_Max_Eng_Get(UINT8 slot, UINT8 start, UINT8 count, float *buf);

    MODULE_RW_ERR_CODE MX_RTU_Module_AI_Burnout_Value_Get(UINT8 slot, UINT8 start, UINT8 count, float *buf);

    MODULE_RW_ERR_CODE MX_RTU_Module_AI_Status_Get(UINT8 slot, UINT8 start, UINT8 count, UINT8 *buf);

    MODULE_RW_ERR_CODE MX_RTU_Module_AI_Range_Set(UINT8 slot, UINT8 start, UINT8 count, UINT8 *buf);

    MODULE_RW_ERR_CODE MX_RTU_Module_AI_Min_Raw_Reset(UINT8 slot, UINT32 channel);

    MODULE_RW_ERR_CODE MX_RTU_Module_AI_Max_Raw_Reset(UINT8 slot, UINT32 channel);

    MODULE_RW_ERR_CODE MX_RTU_Module_AI_Min_Eng_Reset(UINT8 slot, UINT32 channel);

    MODULE_RW_ERR_CODE MX_RTU_Module_AI_Max_Eng_Reset(UINT8 slot, UINT32 channel);

    MODULE_RW_ERR_CODE MX_RTU_Module_AI_Burnout_Value_Set(UINT8 slot, UINT8 start, UINT8 count, float *buf);

    //--------------------------------------------------------------------------------------------------------
#define FAST_AI_BATCH_DATA_SIZE     60*1000

    MODULE_RW_ERR_CODE MX_RTU_Module_Fast_AI_Raw_Value_Get(UINT8 slot, UINT8 start, UINT8 count, UINT32 *buf, struct Timestamp *time);

    MODULE_RW_ERR_CODE MX_RTU_Module_Fast_AI_Eng_Value_Get(UINT8 slot, UINT8 start, UINT8 count, float *buf, struct Timestamp *time);

    MODULE_RW_ERR_CODE MX_RTU_Module_Fast_AI_Range_Get(UINT8 slot, UINT8 start, UINT8 count, UINT8 *buf);

    MODULE_RW_ERR_CODE MX_RTU_Module_Fast_AI_Min_Raw_Get(UINT8 slot, UINT8 start, UINT8 count, UINT32 *buf);

    MODULE_RW_ERR_CODE MX_RTU_Module_Fast_AI_Max_Raw_Get(UINT8 slot, UINT8 start, UINT8 count, UINT32 *buf);

    MODULE_RW_ERR_CODE MX_RTU_Module_Fast_AI_Min_Eng_Get(UINT8 slot, UINT8 start, UINT8 count, float *buf);

    MODULE_RW_ERR_CODE MX_RTU_Module_Fast_AI_Max_Eng_Get(UINT8 slot, UINT8 start, UINT8 count, float *buf);

    MODULE_RW_ERR_CODE MX_RTU_Module_Fast_AI_Burnout_Value_Get(UINT8 slot, UINT8 start, UINT8 count, float *buf);

    MODULE_RW_ERR_CODE MX_RTU_Module_Fast_AI_Status_Get(UINT8 slot, UINT8 start, UINT8 count, UINT8 *buf);

    MODULE_RW_ERR_CODE MX_RTU_Module_Fast_AI_Enable_Mode_Get(UINT8 slot, UINT32 *mode);

    MODULE_RW_ERR_CODE MX_RTU_Module_Fast_AI_Range_Set(UINT8 slot, UINT8 start, UINT8 count, UINT8 *buf);

    MODULE_RW_ERR_CODE MX_RTU_Module_Fast_AI_Min_Raw_Reset(UINT8 slot, UINT32 channel);

    MODULE_RW_ERR_CODE MX_RTU_Module_Fast_AI_Max_Raw_Reset(UINT8 slot, UINT32 channel);

    MODULE_RW_ERR_CODE MX_RTU_Module_Fast_AI_Min_Eng_Reset(UINT8 slot, UINT32 channel);

    MODULE_RW_ERR_CODE MX_RTU_Module_Fast_AI_Max_Eng_Reset(UINT8 slot, UINT32 channel);

    MODULE_RW_ERR_CODE MX_RTU_Module_Fast_AI_Burnout_Value_Set(UINT8 slot, UINT8 start, UINT8 count, float *buf);

    MODULE_RW_ERR_CODE MX_RTU_Module_Fast_AI_Sampling_Rate_Get(UINT8 slot, UINT32 *rate);

    MODULE_RW_ERR_CODE MX_RTU_Module_Fast_AI_Buf_Overflow_Get(UINT8 slot, UINT32 *overflow);

    MODULE_RW_ERR_CODE MX_RTU_Module_Fast_AI_Batch_Data_Get(UINT8 slot, UINT32 channel, UINT32 fore_interval, UINT32 back_interval, UINT8 *buf, struct Timestamp *time);

    MODULE_RW_ERR_CODE MX_RTU_Module_Fast_AI_Batch_Data_Check(UINT8 slot, UINT32 channel, UINT32 *count);


    MODULE_RW_ERR_CODE MX_RTU_Module_Fast_AI_Sampling_Rate_Set(UINT8 slot, UINT32 rate);

    MODULE_RW_ERR_CODE MX_RTU_Module_Fast_AI_Buf_Overflow_Reset(UINT8 slot, UINT32 overflow);

    MODULE_RW_ERR_CODE MX_RTU_Module_Fast_AI_Buf_Reset(UINT8 slot);

    MODULE_RW_ERR_CODE MX_RTU_Module_Fast_AI_Trigger_Set(UINT8 slot, UINT32 channel_trigger, UINT32 fore_interval, UINT32 back_interval, UINT32 *required_buf_size);

    MODULE_RW_ERR_CODE MX_RTU_Module_Fast_AI_Trigger_Stop_Set(UINT8 slot, UINT32 channel_trigger, struct Timestamp *time);

    MODULE_RW_ERR_CODE MX_RTU_Module_Fast_AI_Enable_Mode_Set(UINT8 slot, UINT32 mode);


    //--------------------------------------------------------------------------------------------------------
#define ENG_UNIT_CELSIUS        0 // Celsius (degree of C)
#define ENG_UNIT_FAHRENHEIT     1 // Fahrenheit (degree of F)
#define ENG_UNIT_MILLIVOLT      2 // millivolt
#define ENG_UNIT_OHM            3 // ohm

#define BURNOUT_STATUS_NORMAL           0
#define BURNOUT_STATUS_BURNOUT          1


#define TC_TYPE_78_126mV                0   //(+/- 78.126 mV)
#define TC_TYPE_39_062mV                1   //(+/- 39.062 mV)
#define TC_TYPE_19_532mV                2   //(+/- 19.532 mV)
#define TC_TYPE_J                       3   //(0 to 750 Celsius degree)
#define TC_TYPE_K                       4   //(-200 to 1250 Celsius degree)
#define TC_TYPE_T                       5   //(-200 to 350  Celsius degree)
#define TC_TYPE_E                       6   //(-200 to 900  Celsius degree)
#define TC_TYPE_R                       7   //(-50 to 1600  Celsius degree)
#define TC_TYPE_S                       8   //(-50 to 1760  Celsius degree)
#define TC_TYPE_B                       9   //(600 to 1700  Celsius degree)
#define TC_TYPE_N                       10  //(-200 to 1300 Celsius degree)

    MODULE_RW_ERR_CODE MX_RTU_Module_TC_Eng_Value_Get(UINT8 slot, UINT8 start, UINT8 count, float *buf, struct Timestamp *time);
    MODULE_RW_ERR_CODE MX_RTU_Module_TC_Type_Get(UINT8 slot, UINT8 start, UINT8 count, UINT8 *buf);
    MODULE_RW_ERR_CODE MX_RTU_Module_TC_Unit_Get(UINT8 slot, UINT8 start, UINT8 count, UINT8 *buf);
    MODULE_RW_ERR_CODE MX_RTU_Module_TC_Min_Eng_Get(UINT8 slot, UINT8 start, UINT8 count, float *buf);
    MODULE_RW_ERR_CODE MX_RTU_Module_TC_Max_Eng_Get(UINT8 slot, UINT8 start, UINT8 count, float *buf);
    MODULE_RW_ERR_CODE MX_RTU_Module_TC_Burnout_Status_Get(UINT8 slot, UINT8 start, UINT8 count, UINT8 *buf);
    MODULE_RW_ERR_CODE MX_RTU_Module_TC_Offset_Get(UINT8 slot, UINT8 start, UINT8 count, float *buf);

    MODULE_RW_ERR_CODE MX_RTU_Module_TC_Type_Set(UINT8 slot, UINT8 start, UINT8 count, UINT8 *buf); //TC TYPE
    MODULE_RW_ERR_CODE MX_RTU_Module_TC_Unit_Set(UINT8 slot, UINT8 start, UINT8 count, UINT8 *buf);
    MODULE_RW_ERR_CODE MX_RTU_Module_TC_Min_Eng_Reset(UINT8 slot, UINT32 channel_flags);
    MODULE_RW_ERR_CODE MX_RTU_Module_TC_Max_Eng_Reset(UINT8 slot, UINT32 channel_flags);
    MODULE_RW_ERR_CODE MX_RTU_Module_TC_Offset_Set(UINT8 slot, UINT8 start, UINT8 count, float *buf);
    MODULE_RW_ERR_CODE MX_RTU_Module_TC_Calibration_Set(UINT8 slot, UINT8 start, UINT8 count, float *buf);
    //--------------------------------------------------------------------------------------------------------
#define RTD_TYPE_2200ohm                0   //(2200 ohm)
#define RTD_TYPE_1250ohm                1   //(1250 ohm)
#define RTD_TYPE_620ohm                 2   //(620 ohm)
#define RTD_TYPE_310ohm                 3   //(310 ohm)
#define RTD_TYPE_PT50                   4   //(-200 to 850 Celsius degree)
#define RTD_TYPE_PT100                  5   //(-200 to 850 Celsius degree)
#define RTD_TYPE_PT200                  6   //(-200 to 850 Celsius degree)
#define RTD_TYPE_PT500                  7   //(-200 to 850 Celsius degree)
#define RTD_TYPE_PT1000                 8   //(-200 to 350 Celsius degree)
#define RTD_TYPE_JPT100                 9   //(-200 to 640 Celsius degree)
#define RTD_TYPE_JPT200                 10  //(-200 to 640 Celsius degree)
#define RTD_TYPE_JPT500                 11  //(-200 to 640 Celsius degree)
#define RTD_TYPE_JPT1000                    12  //(-200 to 350 Celsius degree)
#define RTD_TYPE_NI100                  13  //(-60 to 250  Celsius degree)
#define RTD_TYPE_NI200                  14  //(-60 to 250  Celsius degree)
#define RTD_TYPE_NI500                  15  //(-60 to 250  Celsius degree)
#define RTD_TYPE_NI1000                 16  //(-60 to 150  Celsius degree)
#define RTD_TYPE_NI120                  17  //(-80 to 260  Celsius degree)

    MODULE_RW_ERR_CODE MX_RTU_Module_RTD_Eng_Value_Get(UINT8 slot, UINT8 start, UINT8 count, float *buf, struct Timestamp *time);
    MODULE_RW_ERR_CODE MX_RTU_Module_RTD_Type_Get(UINT8 slot, UINT8 start, UINT8 count, UINT8 *buf); //RTD TYPE
    MODULE_RW_ERR_CODE MX_RTU_Module_RTD_Unit_Get(UINT8 slot, UINT8 start, UINT8 count, UINT8 *buf);
    MODULE_RW_ERR_CODE MX_RTU_Module_RTD_Min_Eng_Get(UINT8 slot, UINT8 start, UINT8 count, float *buf);
    MODULE_RW_ERR_CODE MX_RTU_Module_RTD_Max_Eng_Get(UINT8 slot, UINT8 start, UINT8 count, float *buf);
    MODULE_RW_ERR_CODE MX_RTU_Module_RTD_Burnout_Status_Get(UINT8 slot, UINT8 start, UINT8 count, UINT8 *buf);
    MODULE_RW_ERR_CODE MX_RTU_Module_RTD_Offset_Get(UINT8 slot, UINT8 start, UINT8 count, float *buf);
    MODULE_RW_ERR_CODE MX_RTU_Module_RTD_Type_Set(UINT8 slot, UINT8 start, UINT8 count, UINT8 *buf); //RTD TYPE
    MODULE_RW_ERR_CODE MX_RTU_Module_RTD_Unit_Set(UINT8 slot, UINT8 start, UINT8 count, UINT8 *buf);
    MODULE_RW_ERR_CODE MX_RTU_Module_RTD_Min_Eng_Reset(UINT8 slot, UINT32 channel_flags);
    MODULE_RW_ERR_CODE MX_RTU_Module_RTD_Max_Eng_Reset(UINT8 slot, UINT32 channel_flags);
    MODULE_RW_ERR_CODE MX_RTU_Module_RTD_Offset_Set(UINT8 slot, UINT8 start, UINT8 count, float *buf);
    MODULE_RW_ERR_CODE MX_RTU_Module_RTD_Calibration_Set(UINT8 slot, UINT8 start, UINT8 count, float *buf);
    //--------------------------------------------------------------------------------------------------------

    //AO RANGE
    //#define   AO_RANGE_5V                 0 //not support
#define AO_RANGE_10V                    1
#define AO_RANGE_0_10V                  2
#define AO_RANGE_0_20mA                 3
#define AO_RANGE_4_20mA                 4

    //AO STATUS
#define AO_STATUS_NORMAL                0x00
#define AO_STATUS_OVER_TEMPERATURE      0x01
#define AO_STATUS_CURRENT_OPEN_CIRCUIT  0x04

    //AO waveform type
#define AO_WAVEFORM_TYPE_SINE           0
#define AO_WAVEFORM_TYPE_TRIANGLE       1
#define AO_WAVEFORM_TYPE_SQUARE         2

    MODULE_RW_ERR_CODE MX_RTU_Module_AO_Enable_Get(UINT8 slot, UINT8 start, UINT8 count, UINT8 *buf);

    MODULE_RW_ERR_CODE MX_RTU_Module_AO_Range_Get(UINT8 slot, UINT8 start, UINT8 count, UINT8 *buf); //AO RANGE

    MODULE_RW_ERR_CODE MX_RTU_Module_AO_Raw_Value_Get(UINT8 slot, UINT8 start, UINT8 count, UINT32 *buf);

    MODULE_RW_ERR_CODE MX_RTU_Module_AO_Eng_Value_Get(UINT8 slot, UINT8 start, UINT8 count, float *buf);

    MODULE_RW_ERR_CODE MX_RTU_Module_AO_Status_Get(UINT8 slot, UINT8 start, UINT8 count, UINT8 *buf);

    MODULE_RW_ERR_CODE MX_RTU_Module_AO_Waveform_Type_Get(UINT8 slot, UINT8 start, UINT8 count, UINT8 *buf);

    MODULE_RW_ERR_CODE MX_RTU_Module_AO_Waveform_Config_Get(UINT8 slot, UINT8 start, UINT8 count, float *frequency, float *amplitude, float*offset);

    MODULE_RW_ERR_CODE MX_RTU_Module_AO_Waveform_Count_Get(UINT8 slot, UINT8 start, UINT8 count, UINT32 *buf);

    MODULE_RW_ERR_CODE MX_RTU_Module_AO_Waveform_Start_Get(UINT8 slot, UINT32 *start);


    MODULE_RW_ERR_CODE MX_RTU_Module_AO_Enable_Set(UINT8 slot, UINT8 start, UINT8 count, UINT8 *buf);

    MODULE_RW_ERR_CODE MX_RTU_Module_AO_Range_Set(UINT8 slot, UINT8 start, UINT8 count, UINT8 *buf);

    MODULE_RW_ERR_CODE MX_RTU_Module_AO_Raw_Value_Set(UINT8 slot, UINT8 start, UINT8 count, UINT32 *buf);

    MODULE_RW_ERR_CODE MX_RTU_Module_AO_Eng_Value_Set(UINT8 slot, UINT8 start, UINT8 count, float *buf);

    MODULE_RW_ERR_CODE MX_RTU_Module_AO_Waveform_Type_Set(UINT8 slot, UINT8 start, UINT8 count, UINT8 *buf);

    MODULE_RW_ERR_CODE MX_RTU_Module_AO_Waveform_Config_Set(UINT8 slot, UINT8 start, UINT8 count, float *frequency, float *amplitude, float *offset);

    MODULE_RW_ERR_CODE MX_RTU_Module_AO_Waveform_Count_Set(UINT8 slot, UINT8 start, UINT8 count, UINT32 *buf);

    MODULE_RW_ERR_CODE MX_RTU_Module_AO_Waveform_Start_Set(UINT8 slot, UINT32 start);

    MODULE_RW_ERR_CODE MX_RTU_Module_AO_Waveform_Stop_Set(UINT8 slot, UINT32 stop);


    /********************************************************************************************/

    /********************************************Misc********************************************/
    //LED STATE
#define LED_DARK            0
#define LED_GREEN           1
#define LED_RED             2

#define SOFTWARE_WATCHDOG_MIN_TIME      1           // 1 second
#define SOFTWARE_WATCHDOG_MAX_TIME      60          // 60 seconds

    typedef enum
    {
        MISC_ERR_OK = 0,                        
        MISC_ERR_DEVICE,                        
        MISC_ERR_ARGUMENT,                  
        MISC_ERR_RW,                            
        MISC_ERR_ACTION,                        
        MISC_ERR_COPY,                          
        MISC_ERR_TYPE,                          
        MISC_ERR_SEMAPHORE_LOCK,            
        MISC_ERR_SYSTEM_INFO,               
        MISC_ERR_NOT_SUPPORT_PRODUCT,       
        MISC_ERR_AMOUNT                     
    } MISC_ERR_CODE;

    struct swtd_setting
    {
        int enable;
        unsigned int time;
    };

    void MX_RTU_Timestamp_to_Timeval(struct Timestamp *timestamp, struct timeval *tv);

    MISC_ERR_CODE MX_RTU_RTC_Get(struct rtc_time *rtc);
    MISC_ERR_CODE MX_RTU_RTC_Set(struct rtc_time *rtc);

    MISC_ERR_CODE MX_RTU_SWTD_Enable(UINT32 swtdtime);
    MISC_ERR_CODE MX_RTU_SWTD_Get_Setting(struct swtd_setting *setting);
    MISC_ERR_CODE MX_RTU_SWTD_Disable();
    MISC_ERR_CODE MX_RTU_SWTD_Ack();

    MISC_ERR_CODE MX_RTU_LED1_Get(UINT8 *state); //LED STATE
    MISC_ERR_CODE MX_RTU_LED2_Get(UINT8 *state); //LED STATE
    MISC_ERR_CODE MX_RTU_LED1_Set(UINT8 state); //LED STATE
    MISC_ERR_CODE MX_RTU_LED2_Set(UINT8 state); //LED STATE

    MISC_ERR_CODE MX_RTU_Toggle_Switch_Get(UINT8 *state); //0: Mode1, 1: Mode 2

    MISC_ERR_CODE MX_RTU_Rotary_Switch_Mode_Get(UINT32 *mode);

    MISC_ERR_CODE MX_RTU_Dual_Power1_Get(UINT8 *state); //0: power disconnect, 1: power connect
    MISC_ERR_CODE MX_RTU_Dual_Power2_Get(UINT8 *state); //0: power disconnect, 1: power connect
    MODULE_RW_ERR_CODE MX_RTU_Module_Dual_Power1_Get(UINT8 slot, UINT8 *state); //0: power disconnect, 1: power connect
    MODULE_RW_ERR_CODE MX_RTU_Module_Dual_Power2_Get(UINT8 slot, UINT8 *state); //0: power disconnect, 1: power connect

    UINT32 MX_RTU_API_Version_Get(void); //Example: 0x01000000 -> 1.0.0 , final byte is always 0
    UINT32 MX_RTU_API_BuildDate_Get(void); //Example: 0x0d03010e -> 2013.03.01-14:00:00
    UINT32 MX_RTU_System_Version_Get(void); //Example: 0x01000000 -> 1.0.0 , final byte is always 0
    UINT32 MX_RTU_System_BuildDate_Get(void); //Example: 0x0d03010e -> 2013.03.01-14:00:00
    IO_ERR_CODE MX_RTU_System_Hot_Plug_Connect_Get(UINT32 *state); //Example: 0x6 -> slot 1 and 2 are hot-plug connecting
    IO_ERR_CODE MX_RTU_System_Hot_Plug_Disconnect_Get(UINT32 *state); //Example: 0x6 -> slot 1 and 2 are hot-plug disconnecting
    MODULE_RW_ERR_CODE MX_RTU_Module_FWR_BuildDate_Get(UINT8 slot, UINT32 *date); // Format: (0x)YYMMDDHH
    /********************************************************************************************/

    /*******************************************Serial********************************************/
    //SERIAL MODE
#define RS232_MODE                      0
#define RS485_2WIRE_MODE                1
#define RS422_MODE                      2
#define RS485_4WIRE_MODE                3

    //SERIAL BAUD RATE
#define BAUD_RATE_921600                921600
#define BAUD_RATE_460800                460800
#define BAUD_RATE_230400                230400
#define BAUD_RATE_115200                115200
#define BAUD_RATE_57600             57600
#define BAUD_RATE_38400             38400
#define BAUD_RATE_19200             19200
#define BAUD_RATE_9600                  9600
#define BAUD_RATE_4800                  4800
#define BAUD_RATE_2400                  2400
#define BAUD_RATE_1800                  1800
#define BAUD_RATE_1200                  1200
#define BAUD_RATE_600                   600
#define BAUD_RATE_300                   300

    //SERIAL DATA BITS
#define SERIAL_DATA_BITS_5              5
#define SERIAL_DATA_BITS_6              6
#define SERIAL_DATA_BITS_7              7
#define SERIAL_DATA_BITS_8              8

    //SERIAL PARITY
#define SERIAL_PARITY_NONE              0
#define SERIAL_PARITY_ODD               1
#define SERIAL_PARITY_EVEN              2

    //SERIAL STOP BIT
#define SERIAL_STOP_BIT_1               1
#define SERIAL_STOP_BIT_2               2

    //SERIAL FLOW CONTROL
#define NO_FLOW_CONTROL             0
#define HW_FLOW_CONTROL             1
#define SW_FLOW_CONTROL             2

#define MX_RTU_SerialRead               MX_RTU_SerialBlockRead

#define SERIAL_MAX_OQUEUE_LENGTH    4095

    typedef enum
    {
        PORT1 = 0,          
        PORT2,              
        PORT3,              
        PORT4,              
        PORT5,              
        PORT6,              
        PORT7,              
        PORT8,              
        PORT_AMOUNT     
    } SERIAL_PORT;

    typedef enum
    {
        SERIAL_ERR_OK = 0,                      
        SERIAL_ERR_FD,                          
        SERIAL_ERR_OPEN,                        
        SERIAL_ERR_CLOSE,                       
        SERIAL_ERR_PARAMETER,               
        SERIAL_ERR_SLOT,                        
        SERIAL_ERR_PORT,                        
        SERIAL_ERR_READ,                        
        SERIAL_ERR_WRITE,                       
        SERIAL_ERR_IOCTL,                       
        SERIAL_ERR_MODE,                        
        SERIAL_ERR_ttyUSB,                      
        SERIAL_ERR_NOT_SUPPORT_PRODUCT, 
        SERIAL_ERR_SYSTEM_INFO,             
        SERIAL_ERR_HOT_PLUG,                    
        SERIAL_ERR_FLUSH,                       
        SERIAL_ERR_AMOUNT                   
    } SERIAL_ERR_CODE;

    SERIAL_ERR_CODE MX_RTU_SerialOpen( UINT8 slot, SERIAL_PORT port);

    SERIAL_ERR_CODE MX_RTU_SerialWrite( UINT8 slot, SERIAL_PORT port, UINT32 len, UINT8* str, UINT32 *write_bytes);

    SERIAL_ERR_CODE MX_RTU_SerialNonBlockRead( UINT8 slot, SERIAL_PORT port, UINT32 len, UINT8* buf, UINT32 *read_bytes);

    SERIAL_ERR_CODE MX_RTU_SerialBlockRead( UINT8 slot, SERIAL_PORT port, UINT32 len, UINT8* buf, UINT32 *read_bytes);

    SERIAL_ERR_CODE MX_RTU_SerialClose( UINT8 slot, SERIAL_PORT port);

    SERIAL_ERR_CODE MX_RTU_SerialFlowControl( UINT8 slot, SERIAL_PORT port, UINT8 control); //SERIAL FLOW CONTROL

    SERIAL_ERR_CODE MX_RTU_SerialSetSpeed( UINT8 slot, SERIAL_PORT port, UINT32 speed); //SERIAL BAUD RATE

    SERIAL_ERR_CODE MX_RTU_SerialSetMode( UINT8 slot, SERIAL_PORT port, UINT8 mode); //SERIAL MODE

    SERIAL_ERR_CODE MX_RTU_SerialGetMode( UINT8 slot, SERIAL_PORT port, UINT8 *mode); //SERIAL MODE

    SERIAL_ERR_CODE MX_RTU_SerialSetParam( UINT8 slot, SERIAL_PORT port, UINT8 parity, UINT8 databits, UINT8 stopbit); //SERIAL PARITY, SERIAL DATA BITS, SERIAL STOP BIT

    SERIAL_ERR_CODE MX_RTU_SerialDataInInputQueue( UINT8 slot, SERIAL_PORT port, UINT32 *bytes);

    SERIAL_ERR_CODE MX_RTU_SerialDataInOutputQueue( UINT8 slot, SERIAL_PORT port, UINT32 *bytes);

    SERIAL_ERR_CODE MX_RTU_SerialInputQueueFlush(UINT8 slot, SERIAL_PORT port);

    SERIAL_ERR_CODE MX_RTU_SerialOutputQueueFlush(UINT8 slot, SERIAL_PORT port);

    SERIAL_ERR_CODE MX_RTU_FindFD( UINT8 slot, SERIAL_PORT port, INT32 *fd);
    /********************************************************************************************/

    /*******************************************Cellular*******************************************/
#define MODEM_BRAND_PH8                 1

#define SIZE_PIN                            4
#define SIZE_APN                            32
#define SIZE_USERNAME                       128
#define SIZE_PASSWORD                       128
#define SIZE_HOSTNAME                       128
#define SIZE_IMEI                           17
#define SIZE_IP_ADDR                        16

    //MODEM BAND
#define MODEM_BAND_PH8_GSM900           1
#define MODEM_BAND_PH8_GSM1800          2
#define MODEM_BAND_PH8_GSM850           4
#define MODEM_BAND_PH8_GSM1900          8
#define MODEM_BAND_PH8_WCDMA2100        16
#define MODEM_BAND_PH8_WCDMA1900        32
#define MODEM_BAND_PH8_WCDMA850         64
#define MODEM_BAND_PH8_WCDMA800         256
#define MODEM_BAND_PH8_WCDMA1700        512
#define MODEM_BAND_PH8_AUTO             895

    //MODEM STATE
#define MODEM_STATE_INIT                    0
#define MODEM_STATE_READY               1
#define MODEM_STATE_CONNECTING          2
#define MODEM_STATE_CONNECTED           3
#define MODEM_STATE_DISCONNECT          4

    //modem error code
    typedef enum
    {
        MODEM_ERR_OK = 0,                        //successfully
        MODEM_ERR_INIT,                      //not init
        MODEM_ERR_PARAM,                     //invalid parameter
        MODEM_ERR_OPEN,                      //open device error
        MODEM_ERR_SIM,                       //SIM card error
        MODEM_ERR_PIN,                       //PIN code error
        MODEM_ERR_BAND,                      //Set band error
        MODEM_ERR_ECHO_OFF,                  //turn off echo error
        MODEM_ERR_CELLULAR_DENIED,           //register to cellular network is denied
        MODEM_ERR_RESET,                         //reset modem error
        MODEM_ERR_THREAD,                    //create thread error
        MODEM_ERR_APN,                       //set APN error
        MODEM_ERR_CREDENTIAL,                //generate PPP credential file error 
        MODEM_ERR_ATTACH,                    //attach to 3G network error
        MODEM_ERR_IF,                            //ppp interface is not established
        MODEM_ERR_CMD,                       //err command(wrong state, or previous cmd is executing)
        MODEM_ERR_TIMEOUT,                       //connect to 3G timeout(PPP)
        MODEM_ERR_STATE,                         //invalid cmd state
        MODEM_ERR_BREAK,                         //user break
        MODEM_ERR_SEM,                       //create named semaphore for modem failed  
        MODEM_ERR_ttyUSB,                        //can't find ttyUSB of modem
        MODEM_ERR_SYSTEM_INFO,               //can't get system info
        MODEM_ERR_NOT_SUPPORT_PRODUCT,   //this product is not support cellular
        MODEM_ERR_GPS_SWITCH,                //start/stop GPS function err
        MODEM_ERR_QUERY_REG,                 //query modem registration to station err  
        MODEM_ERR_AMOUNT                    
    } MODEM_ERR_CODE;   

    typedef struct Check_Info
    {
        UINT8 autoCheckEnable;  
        UINT8 pingHostname[SIZE_HOSTNAME];      
        UINT32 pingIntervalS;
        UINT32 pingMaxFail; 
    }CheckInfo;

    MODEM_ERR_CODE MX_RTU_Cellular_Modem_Init(INT8 *pin, UINT32 band); //MODEM BAND

    MODEM_ERR_CODE MX_RTU_Cellular_Net_Start(INT8 *apn, INT8 *userName, INT8 *password, CheckInfo* autoCheck);

    MODEM_ERR_CODE MX_RTU_Cellular_Net_Stop(void);

    MODEM_ERR_CODE MX_RTU_Cellular_Net_IP_Address(UINT8 *ipAddress);

    MODEM_ERR_CODE MX_RTU_Cellular_Modem_Reset(void);//do not reset modem while MX_RTU_Cellular_Net_State()!=(MODEM_STATE_DISCONNECT|MODEM_STATE_READY)

    INT32 MX_RTU_Cellular_Net_State(void); //MODEM STATE

    MODEM_ERR_CODE MX_RTU_Cellular_Modem_RSSI(UINT8 *rssi);

    void MX_RTU_Cellular_Modem_IMEI(INT8 *imei);

    void MX_RTU_Cellular_Set_Debug(UINT8 debug);
    /********************************************GPS********************************************/
    typedef struct Gps_Time
    {
        int     year;       
        int     mon;        
        int     day;        
        int     hour;       
        int     min;        
        int     sec;        
        int     hsec;       
    }GPS_TIME;

    typedef struct Gps_Data
    {
        //GPS info
        double  lat;            
        double  lon;            
        int fix;            
        int satInUse;       
        int satInView;      
        GPS_TIME time;  
    }GPS_DATA;

    INT32 MX_RTU_GPS_Start(UINT8 activeAntenna);

    INT32 MX_RTU_GPS_Stop(void);

    void MX_RTU_GPS_Get_Info(GPS_DATA *gpsData);
    /********************************************************************************************/

    /********************************************SMS********************************************/
    typedef enum
    {
        SMS_ERR_OK = 0,                 
        SMS_ERR_OPEN,                        //open modem device fail
        SMS_ERR_ERR,                         //send SMS fail
        SMS_ERR_PIN,                         //maybe it's a pin code err or SIM err
        SMS_ERR_PDU,                         //set modem to PDU mode
        SMS_ERR_MODEM,                   //check modem err
        SMS_ERR_LENGTH,                      //message exceeds the maximum limitation 
        SMS_ERR_SYSTEM_INFO,             //can't get system info
        SMS_ERR_NOT_SUPPORT_PRODUCT,     //this product is not support sms
        SMS_ERR_ISP,                         //register to ISP station err
        SMS_ERR_AMOUNT                  
    } SMS_ERR_CODE;

    SMS_ERR_CODE MX_RTU_SMS_Send_GSM_7bits_Default_Alphabet(INT8 *phone, INT8 *pin, INT8 *msg, INT32 msgLength );//maximum message length is 160 bytes

    SMS_ERR_CODE MX_RTU_SMS_Send_Ucs2(INT8 *phone, INT8 *pin, INT8 *ucs2Msg, INT8 msgLength);//maximum message length is 140 bytes
    /********************************************************************************************/

    /*******************************************CAN********************************************/
#define RTU_MAX_CAN_BUS_PORT                        2

#define CAN_MAX_PAYLOAD_LEN                         8
#define CAN_MAX_PDO_DATA_LEN                            CAN_MAX_PAYLOAD_LEN
#define CAN_MAX_SDO_DATA_LEN                            4

#define CAN_MAX_PDO                                 4
#define CANOPEN_SLAVE_MAX_SDO                       1000
#define CANOPEN_SLAVE_MAX_PDO                       CAN_MAX_PDO

    typedef enum
    {
        CAN_ERR_OK = 0,                 
        CAN_ERR_SLOT,
        CAN_ERR_PORT,   
        CAN_ERR_PARAM,
        CAN_ERR_GET,
        CAN_ERR_SET,
        CAN_ERR_NOT_SUPPORT_PRODUCT,
        CAN_ERR_SYSTEM_INFO,
        CAN_ERR_HOT_PLUG,
        CAN_ERR_TTYACM_PORT,    
        CAN_ERR_SOCKETCAN_INDEX,    
        CAN_ERR_SOCKETCAN_INIT,
        CAN_ERR_SOCKET,
        CAN_ERR_IOCTL,  
        CAN_ERR_BIND,   
        CAN_ERR_NMT_NODE_STATE,
        CAN_ERR_HANDLE,     
        CAN_ERR_DUPLICATE_OPEN,
        CAN_ERR_WRITE,
        CAN_ERR_READ,       
        CAN_ERR_SLAVE_NODE_ID,      
        CAN_ERR_EITHER_NODEGUARDING_OR_HEARTBEAT,       
        CAN_ERR_NEITHER_NODEGUARDING_NOR_HEARTBEAT, 
        CAN_ERR_THREAD,            
        CAN_ERR_RW_TIMEOUT, 
        CAN_ERR_SDO_RW_ABORT,
        CAN_ERR_FNCTL,
        CAN_ERR_NO_DATA_RECEIVE,
        CAN_ERR_DATA_LENGTH,
        CAN_ERR_TAG,  
        CAN_ERR_SLAVE_NODE_ID_IS_NOT_SET,   
        CAN_ERR_SLAVE_IS_NOT_LISTENING,         
        CAN_ERR_SLAVE_SDO_REGISTER_FULL,    
        CAN_ERR_SLAVE_SDO_DUPLICATE_REGISTER,       
        CAN_ERR_SLAVE_PDO_REGISTER_FULL,    
        CAN_ERR_SLAVE_PDO_DUPLICATE_REGISTER,       
        CAN_ERR_SLAVE_BOTH_SDO_AND_PDO_ARE_NOT_REGISTERED,      
        CAN_ERR_AMOUNT                  
    } CAN_ERR_CODE;

    typedef enum
    {
        CAN_NODE_START = 0x1,
        CAN_NODE_STOP = 0x2,
        CAN_NODE_PRE_OPERATIONAL = 0x80,
        CAN_NODE_RESET_NODE = 0x81,
        CAN_NODE_RESET_COMMUNICATION = 0x82,
        CAN_NODE_AMOUNT
    } CAN_NODE_STATE;

    typedef enum
    {
        CAN_NMT_ERROR_OK = 0,
        CAN_NMT_ERROR_NODE_GUARDING,
        CAN_NMT_ERROR_HEARTBEAT,
        CAN_NMT_ERROR_AMOUNT
    } CAN_NMT_ERROR_STATUS;

    typedef enum
    {
        CAN_NMT_NODE_GUARDING_INITIALISING = 0,
        CAN_NMT_NODE_GUARDING_DISCONNECTED,
        CAN_NMT_NODE_GUARDING_CONNECTING,
        CAN_NMT_NODE_GUARDING_PREPARING,
        CAN_NMT_NODE_GUARDING_STOPPED,
        CAN_NMT_NODE_GUARDING_OPERATIONAL,
        CAN_NMT_NODE_GUARDING_PREOPERATIONAL = 127, 
        CAN_NMT_NODE_GUARDING_AMOUNT
    } CAN_NMT_NODE_GUARDING_STATE;

    typedef enum
    {
        CAN_NMT_HEARTBEAT_BOOTUP = 0,
        CAN_NMT_HEARTBEAT_STOPPED = 4,
        CAN_NMT_HEARTBEAT_OPERATIONAL = 5,
        CAN_NMT_HEARTBEAT_PREOPERATIONAL = 127,
        CAN_NMT_HEARTBEAT_AMOUNT
    } CAN_NMT_HEARTBEAT_STATE;

    struct CAN_Emcy_Message {
        UINT32 cobID;
        UINT16 emergencyErrorCode;
        UINT8 errorRegister;
        UINT8 manufacturerSpecificErrorField[5];
    } __attribute__((packed));

    // CAN_STATUS
#define CAN_STATUS_BUS_OFF      0x00000080  // CAN controller has entered a Bus Off state.
#define CAN_STATUS_EWARN        0x00000040  // CAN controller error level has reached warning level.
#define CAN_STATUS_EPASS        0x00000020  // CAN controller error level has reached error passive level.
#define CAN_STATUS_RXOK         0x00000010  // A message was received successfully since the last read of this status.
#define CAN_STATUS_TXOK         0x00000008  // A message was transmitted successfully since the last read of this status.
#define CAN_STATUS_LEC_MASK     0x00000007  // This is the mask for the CAN Last Error Code (LEC).
#define CAN_STATUS_LEC_NONE     0x00000000  // There was no error.
#define CAN_STATUS_LEC_STUFF    0x00000001  // A bit stuffing error has occurred.
#define CAN_STATUS_LEC_FORM     0x00000002  // A formatting error has occurred.
#define CAN_STATUS_LEC_ACK      0x00000003  // An acknowledge error has occurred.
#define CAN_STATUS_LEC_BIT1     0x00000004  // The bus remained a bit level of 1 for longer than is allowed.
#define CAN_STATUS_LEC_BIT0     0x00000005  // The bus remained a bit level of 0 for longer than is allowed.
#define CAN_STATUS_LEC_CRC      0x00000006  // A CRC error has occurred.

    //CAN BAUD RATE
#define CAN_BAUD_RATE_1000K             1000000
#define CAN_BAUD_RATE_800K              800000
#define CAN_BAUD_RATE_500K              500000
#define CAN_BAUD_RATE_250K              250000
#define CAN_BAUD_RATE_125K              125000
#define CAN_BAUD_RATE_100K              100000
#define CAN_BAUD_RATE_50K               50000
#define CAN_BAUD_RATE_20K               20000
#define CAN_BAUD_RATE_10K               10000

    CAN_ERR_CODE MX_RTU_CanSetBaudrate(UINT8 slot, UINT8 port, UINT32 baudrate);

    CAN_ERR_CODE MX_RTU_CanGetBaudrate(UINT8 slot, UINT8 port, UINT32 *baudrate);

    CAN_ERR_CODE MX_RTU_CanSetAccCode(UINT8 slot, UINT8 port, UINT32 accCode);

    CAN_ERR_CODE MX_RTU_CanGetAccCode(UINT8 slot, UINT8 port, UINT32 *accCode);

    CAN_ERR_CODE MX_RTU_CanSetAccMask(UINT8 slot, UINT8 port, UINT32 accMask);

    CAN_ERR_CODE MX_RTU_CanGetAccMask(UINT8 slot, UINT8 port, UINT32 *accMask);

    CAN_ERR_CODE MX_RTU_CanGetStatus(UINT8 slot, UINT8 port, UINT32 *status);

    //--------------------------------------------------------------------------------------------------------

    CAN_ERR_CODE MX_RTU_CanOpen(UINT8 slot, UINT8 port, int *handle);
    CAN_ERR_CODE MX_RTU_CanClose(int handle);

    //CANopen Master
    CAN_ERR_CODE MX_RTU_CanNMTSetState(int handle, UINT32 slaveNodeID, CAN_NODE_STATE state);
    CAN_ERR_CODE MX_RTU_CanNMTNodeGuarding(int handle, UINT32 slaveNodeID, UINT16 guardTime, UINT8 lifeTimeFactor);
    CAN_ERR_CODE MX_RTU_CanNMTHeartbeat(int handle, UINT32 slaveNodeID, UINT16 producerTime, UINT16 consumerTime);
    CAN_ERR_CODE MX_RTU_CanGetNMTError(int handle, UINT32 slaveNodeID, CAN_NMT_ERROR_STATUS *status, CAN_NMT_NODE_GUARDING_STATE *nodeGuardingState);
    CAN_ERR_CODE MX_RTU_CanSYNCSend(int handle, UINT32 CobID);
    CAN_ERR_CODE MX_RTU_CanCyclicSYNCSend(int handle, UINT32 CobID, UINT16 Timer, UINT32 Times);
    CAN_ERR_CODE MX_RTU_CanGetAllEmcyMessage(int handle, UINT32 slaveNodeID, struct CAN_Emcy_Message *Msg, UINT32 *nMsg);
    CAN_ERR_CODE MX_RTU_CanSDORead(int handle, UINT32 slaveNodeID, UINT16 index, UINT8 subIndex, UINT8 *rData, UINT32 *rLen, UINT32 *abortCode);
    CAN_ERR_CODE MX_RTU_CanSDOWrite(int handle, UINT32 slaveNodeID, UINT16 index, UINT8 subIndex, UINT8 *wData, UINT32 wLen, UINT8 *rData, UINT32 *rLen, UINT32 *abortCode);
    CAN_ERR_CODE MX_RTU_CanPDORemote(int handle, UINT8 pdoNumber, UINT32 slaveNodeID, UINT8 *rData, UINT32 *rLen);
    CAN_ERR_CODE MX_RTU_CanPDOSend(int handle, UINT8 pdoNumber, UINT32 slaveNodeID, UINT8 *wData, UINT32 wLen);
    CAN_ERR_CODE MX_RTU_CanCyclicPDOSend(int handle, UINT8 pdoNumber, UINT32 slaveNodeID, UINT8 *wData, UINT32 wLen, UINT16 Timer, UINT32 Times);

    //CANopen Slave
    CAN_ERR_CODE MX_RTU_CanSlaveNodeIDSet(int handle, UINT32 slaveNodeID);
    CAN_ERR_CODE MX_RTU_CanSlaveNodeIDGet(int handle, UINT32 *slaveNodeID);
    CAN_ERR_CODE MX_RTU_CanSlaveSDORegister(int handle, UINT16 index, UINT8 subIndex, UINT8 *tagName);
    CAN_ERR_CODE MX_RTU_CanSlavePDORegister(int handle, UINT8 pdoNumber, UINT8 *tagName);
    CAN_ERR_CODE MX_RTU_CanSlaveStartListening(int handle); //sending NMT Boot-up message
    CAN_ERR_CODE MX_RTU_CanSlaveStopListening(int handle);
    CAN_ERR_CODE MX_RTU_CanSlaveNMTStateGet(int handle, CAN_NODE_STATE *state);
    CAN_ERR_CODE MX_RTU_CanSlaveEmcyMessageSend(int handle, struct CAN_Emcy_Message *Msg);
    CAN_ERR_CODE MX_RTU_CanSlaveSYNCGet(int handle, UINT32 *CobID, UINT32 *nCobID);
    CAN_ERR_CODE MX_RTU_CanSlaveHeartbeatGet(int handle, UINT16 *producerTime);  //Active: 0x1017 = producerTime
    CAN_ERR_CODE MX_RTU_CanSlaveHeartbeatStateGet(int handle, CAN_NMT_HEARTBEAT_STATE *state);
    CAN_ERR_CODE MX_RTU_CanSlaveNodeGuardingGet(int handle, UINT16 *guardTime, UINT8 *lifeTimeFactor); //Passive: 0x100C = guardTime, 0x100D = lifeTimeFactor.
    CAN_ERR_CODE MX_RTU_CanSlaveNodeGuardingStateGet(int handle, CAN_NMT_NODE_GUARDING_STATE *state);
    CAN_ERR_CODE MX_RTU_CanSlaveNodeGuardingStateSet(int handle, CAN_NMT_NODE_GUARDING_STATE state);

    /********************************************************************************************/


    /****************************************Modbus Master****************************************/
    typedef enum
    {
        MODBUS_MASTER_ERR_OK = 0,               
        MODBUS_MASTER_ERR_PARAM,                 //invalid parameters
        MODBUS_MASTER_ERR_PORT,              //invalid port
        MODBUS_MASTER_ERR_OPENED,                //port is opened
        MODBUS_MASTER_ERR_NOT_OPENED,            //port is not opened
        MODBUS_MASTER_ERR_UART_OPEN,             //open uart device err
        MODBUS_MASTER_ERR_UART_CLOSE,            //close uart device err
        MODBUS_MASTER_ERR_UART_BAUDRATE,         //set uart baudrate err
        MODBUS_MASTER_ERR_UART_MODE,             //set uart 232/422/485 mode err
        MODBUS_MASTER_ERR_UART_FORMAT,       //set uart data format err
        MODBUS_MASTER_ERR_UART_FLOW,             //set uart flow control err
        MODBUS_MASTER_ERR_UART_WRITE,            //write data err
        MODBUS_MASTER_ERR_RESP,              //invalid modbus RTU response
        MODBUS_MASTER_ERR_CRC,                   //check response CRC error
        MODBUS_MASTER_ERR_TIMEOUT,               //timeout
        MODBUS_MASTER_ERR_SOCKET,                //socket error
        MODBUS_MASTER_ERR_CONNECT,           //failed to connect to modbus server
        MODBUS_MASTER_ERR_HANDLE,                //out of space for connection handle
        MODBUS_MASTER_ERR_EXCEPTION,         //modbus standard excpetion occurred
        MODBUS_MASTER_ERR_LIB_INIT,              //please call MX_RTU_Modbus_Init() first
        MODBUS_MASTER_ERR_SYSTEM_INFO,       //can't get system info
        MODBUS_MASTER_ERR_HOT_PLUG,         
        MODBUS_MASTER_ERR_KEEPALIVE,            
        MODBUS_MASTER_ERR_AMOUNT                
    } MODBUS_MASTER_ERR_CODE;   

#define MODBUS_EXCEPTION_CODE_NONE                                      0x00
#define MODBUS_EXCEPTION_CODE_ILLEGAL_FUNCTION                          0x01
#define MODBUS_EXCEPTION_CODE_ILLEGAL_DATA_ADDRESS                      0x02
#define MODBUS_EXCEPTION_CODE_ILLEGAL_DATA_VALUE                        0x03
#define MODBUS_EXCEPTION_CODE_SLAVE_DEVICE_FAILURE                      0x04
#define MODBUS_EXCEPTION_CODE_ACKNOWLEDGE                               0x05
#define MODBUS_EXCEPTION_CODE_SLAVE_DEVICE_BUSY                         0x06
#define MODBUS_EXCEPTION_CODE_MEMORY_PARITY_ERROR                       0x08
#define MODBUS_EXCEPTION_CODE_GATEWAY_PATH_UNAVAILABLE                  0x0A
#define MODBUS_EXCEPTION_CODE_GATEWAY_TARGET_DEVICE_FAILED_TO_RESPOND   0x0B

    MODBUS_MASTER_ERR_CODE MX_RTU_Modbus_Master_Init(void);

    void MX_RTU_Modbus_Master_Uninit(void);

    //-------------------RTU-------------------
    typedef struct
    {
        UINT32 baudrate;
        INT32 parity;
        INT32 databits;
        INT32 stopbit;
        INT32 mode;
        INT32 flowCtrl;
    }TTY_PARAM;

    MODBUS_MASTER_ERR_CODE MX_RTU_Modbus_Rtu_Master_Open(UINT8 slot, SERIAL_PORT port, TTY_PARAM *param);

    MODBUS_MASTER_ERR_CODE MX_RTU_Modbus_Rtu_Master_Close(UINT8 slot, SERIAL_PORT port);

    //coil
    MODBUS_MASTER_ERR_CODE MX_RTU_Modbus_Rtu_Master_Read_Coils(UINT8 slot, SERIAL_PORT port, UINT8 unitId, UINT16 startAddr , UINT16 coilCount, UINT8 byteCoils[], UINT32 timeoutMs, UINT8 *exceptionCode);

    MODBUS_MASTER_ERR_CODE MX_RTU_Modbus_Rtu_Master_Write_Coils(UINT8 slot, SERIAL_PORT port, UINT8 unitId, UINT16 startAddr, UINT16 coilCount, UINT8 byteCoils[], UINT32 timeoutMs, UINT8 *exceptionCode);

    MODBUS_MASTER_ERR_CODE MX_RTU_Modbus_Rtu_Master_Write_Coil(UINT8 slot, SERIAL_PORT port, UINT8 unitId, UINT16 addr, UINT8 coil, UINT32 timeoutMs, UINT8 *exceptionCode);

    //input discrete
    MODBUS_MASTER_ERR_CODE MX_RTU_Modbus_Rtu_Master_Read_Discrete_Inputs(UINT8 slot, SERIAL_PORT port, UINT8 unitId, UINT16 startAddr , UINT16 coilCount, UINT8 byteCoils[], UINT32 timeoutMs, UINT8 *exceptionCode);

    //input register
    MODBUS_MASTER_ERR_CODE MX_RTU_Modbus_Rtu_Master_Read_Input_Regs(UINT8 slot, SERIAL_PORT port, UINT8 unitId, UINT16 startAddr, UINT16 regCount, UINT16 regs[], UINT32 timeoutMs, UINT8 *exceptionCode);

    //holding register
    MODBUS_MASTER_ERR_CODE MX_RTU_Modbus_Rtu_Master_Read_Holding_Regs(UINT8 slot, SERIAL_PORT port, UINT8 unitId, UINT16 startAddr, UINT16 regCount, UINT16 regs[], UINT32 timeoutMs, UINT8 *exceptionCode);

    MODBUS_MASTER_ERR_CODE MX_RTU_Modbus_Rtu_Master_Write_Holding_Regs(UINT8 slot, SERIAL_PORT port, UINT8 unitId, UINT16 startAddr, UINT16 regCount, UINT16 regs[], UINT32 timeoutMs, UINT8 *exceptionCode);

    MODBUS_MASTER_ERR_CODE MX_RTU_Modbus_Rtu_Master_Write_Holding_Reg(UINT8 slot, SERIAL_PORT port, UINT8 unitId, UINT16 addr, UINT16 reg, UINT32 timeoutMs, UINT8 *exceptionCode);
    //-------------------TCP-------------------
    MODBUS_MASTER_ERR_CODE MX_RTU_Modbus_Tcp_Master_Open(UINT8 ipAddress[], UINT16 tcpPort, UINT32 cTimeoutMs, UINT32 *sHandle);

    MODBUS_MASTER_ERR_CODE MX_RTU_Modbus_Tcp_Master_Close(UINT32 sHandle);

    MODBUS_MASTER_ERR_CODE MX_RTU_Modbus_Tcp_Master_Ioctl(UINT32 sHandle, UINT8 unitId, UINT32 rwTimeoutMs);
    //coil
    MODBUS_MASTER_ERR_CODE MX_RTU_Modbus_Tcp_Master_Read_Coils(UINT32 sHandle, UINT16 startAddr , UINT16 coilCount, UINT8 byteCoils[], UINT8 *exceptionCode);
    MODBUS_MASTER_ERR_CODE MX_RTU_Modbus_Tcp_Master_Write_Coils(UINT32 sHandle, UINT16 startAddr , UINT16 coilCount, UINT8 byteCoils[], UINT8 *exceptionCode);
    MODBUS_MASTER_ERR_CODE MX_RTU_Modbus_Tcp_Master_Write_Coil(UINT32 sHandle, UINT16 addr , UINT8 coil, UINT8 *exceptionCode);
    //input discrete inputs
    MODBUS_MASTER_ERR_CODE MX_RTU_Modbus_Tcp_Master_Read_Discrete_Inputs(UINT32 sHandle, UINT16 startAddr , UINT16 coilCount, UINT8 byteCoils[], UINT8 *exceptionCode);
    //input register
    MODBUS_MASTER_ERR_CODE MX_RTU_Modbus_Tcp_Master_Read_Input_Regs(UINT32 sHandle, UINT16 startAddr , UINT16 regCount, UINT16 regs[], UINT8 *exceptionCode);
    //holding register
    MODBUS_MASTER_ERR_CODE MX_RTU_Modbus_Tcp_Master_Read_Holding_Regs(UINT32 sHandle, UINT16 startAddr , UINT16 regCount, UINT16 regs[], UINT8 *exceptionCode);
    MODBUS_MASTER_ERR_CODE MX_RTU_Modbus_Tcp_Master_Write_Holding_Regs(UINT32 sHandle, UINT16 startAddr , UINT16 regCount, UINT16 regs[], UINT8 *exceptionCode);
    MODBUS_MASTER_ERR_CODE MX_RTU_Modbus_Tcp_Master_Write_Holding_Reg(UINT32 sHandle, UINT16 addr , UINT16 regValue, UINT8 *exceptionCode);
    /********************************************************************************************/


    /****************************************Modbus Slave****************************************/
#define MAKE_WORD(a,b)  (((UINT16)a<<8) +(UINT16)b)

#define pfnModbusRead int (*pfnModRead)(UINT8 *pData, UINT16 nth, void *pUserData)
#define pfnModbusWrite int (*pfnModWrite)(UINT8 *pData, UINT16 nth, void *pUserData)

#define MODBUS_LISTEN_PORTS                 1   //Only one port can be registered
#define MODBUS_MAX_CONNECTION               10  //Maximum connections for each port

#define MODBUS_MAX_MAP_SIZE                 3000    //Maximum entries for each Map Type
#define MODBUS_MIN_IDLE_TIMEOUT_SECOND      10

    //Exception Code
#define ILLEGAL_FUNCTION                        0x01
#define ILLEGAL_DATA_ADDRESS                    0x02
#define ILLEGAL_DATA_VALUE                  0x03
#define SLAVE_DEVICE_FAILURE                    0x04
#define SLAVE_DEVICE_BUSY                       0x06

    //Return Code
#define RETURN_OK                               0
#define RETURN_ERROR_ADDRESS                1
#define RETURN_ERROR_FUNCTION               2
#define RETURN_ERROR_VALUE                  3

    //Function Code
#define MODBUS_READ_COILS                   0x01 //COIL
#define MODBUS_READ_DISCRETE                    0x02 //INPUT COIL
#define MODBUS_READ_HOLDINGREGISTERS        0x03 //HOLDING REGISTER
#define MODBUS_READ_INPUTREGISTERS          0x04 //INPUT REGISTER
#define MODBUS_WRITE_COIL                       0x05 //COIL
#define MODBUS_WRITE_REGISTER               0x06 //HOLDING REGISTER
#define MODBUS_WRITE_COILS                  0x0F //COIL
#define MODBUS_WRITE_REGISTERS              0x10 //HOLDING REGISTER

    //MAP TYPE
#define MODBUS_COIL                         0x01
#define MODBUS_INPUT_COIL                       0x02
#define MODBUS_HOLDING_REGISTER             0x03
#define MODBUS_INPUT_REGISTER               0x04

    typedef enum
    {
        MODBUS_SLAVE_ERR_OK = 0,                        
        MODBUS_SLAVE_ERR_ADDRESS,                   
        MODBUS_SLAVE_ERR_ADDRESS_COLLISION,     
        MODBUS_SLAVE_ERR_BIND,                      
        MODBUS_SLAVE_ERR_CREATE_SOCKET,         
        MODBUS_SLAVE_ERR_CREATE_THREAD,         
        MODBUS_SLAVE_ERR_EXCEPTION,             
        MODBUS_SLAVE_ERR_EXCEED_MAP_SIZE,           
        MODBUS_SLAVE_ERR_FUNCTION,                  
        MODBUS_SLAVE_ERR_ILLEGAL_HANDLE,            
        MODBUS_SLAVE_ERR_IDLE_TIMEOUT,              
        MODBUS_SLAVE_ERR_ILLEGAL_ACTION,            
        MODBUS_SLAVE_ERR_LISTEN,                    
        MODBUS_SLAVE_ERR_LISTEN_PORT_OVERFLOW,  
        MODBUS_SLAVE_ERR_MAP_TYPE,                  
        MODBUS_SLAVE_ERR_MAP_EMPTY,             
        MODBUS_SLAVE_ERR_MEMORY_LEAK,               
        MODBUS_SLAVE_ERR_NO_START,                  
        MODBUS_SLAVE_ERR_NO_REGISTER,               
        MODBUS_SLAVE_ERR_PORT_LISTENING,            
        MODBUS_SLAVE_ERR_REGISTERED_PORT,           
        MODBUS_SLAVE_ERR_SIZE,                      
        MODBUS_SLAVE_ERR_SYSTEM_TIMEOUT,            
        MODBUS_SLAVE_ERR_SET_SOCKET_MODE,       
        MODBUS_SLAVE_ERR_SET_SOCKET_OPTION,     
        MODBUS_SLAVE_ERR_NO_CONNECTIONS,            
        MODBUS_SLAVE_ERR_SYSTEM_INFO,               
        MODBUS_SLAVE_ERR_AMOUNT                 
    } MODBUS_SLAVE_ERR_CODE;

    /***************************************Modbus TCP Slave***************************************/
    struct Modbus_TCP_Master_Connection_Info
    {
        UINT8 accepted_connections;              // example: 5 -> five connections right now
        char ip[MODBUS_MAX_CONNECTION][17];      // example: ip[4] -> 192.168.127.254. ip[5] ~ ip[9] are invalid.
        UINT16 port[MODBUS_MAX_CONNECTION];  // example: port[4] -> 12345. port[5] ~ port[9] are invalid.
    } __attribute__((packed));

    MODBUS_SLAVE_ERR_CODE MX_RTU_Modbus_Tcp_Slave_Init();

    MODBUS_SLAVE_ERR_CODE MX_RTU_Modbus_Tcp_Slave_Uninit();

    MODBUS_SLAVE_ERR_CODE MX_RTU_Modbus_Tcp_Slave_Register(UINT16 port, UINT16 map_size, UINT32 idle_timeout_second, UINT32 *sHandle);

    MODBUS_SLAVE_ERR_CODE MX_RTU_Modbus_Tcp_Slave_Unregister(UINT32 sHandle);

    MODBUS_SLAVE_ERR_CODE MX_RTU_Modbus_Tcp_Slave_Start(UINT32 sHandle);

    MODBUS_SLAVE_ERR_CODE MX_RTU_Modbus_Tcp_Slave_Stop(UINT32 sHandle);

    MODBUS_SLAVE_ERR_CODE MX_RTU_Modbus_Tcp_Slave_Add_Entry(UINT32 sHandle, UINT8 map_type, UINT16 address, void *pUserData, pfnModbusRead, pfnModbusWrite); //MAP TYPE

    MODBUS_SLAVE_ERR_CODE MX_RTU_Modbus_Tcp_Slave_Delete_Entry(UINT32 sHandle, UINT8 map_type, UINT16 address); //MAP TYPE

    MODBUS_SLAVE_ERR_CODE MX_RTU_Modbus_Tcp_Slave_Map_Count(UINT32 sHandle, UINT16 *count);

    MODBUS_SLAVE_ERR_CODE MX_RTU_Modbus_Tcp_Slave_Map_Dump(UINT32 sHandle);

    MODBUS_SLAVE_ERR_CODE MX_RTU_Modbus_Tcp_Slave_Connection_Info(UINT32 sHandle, struct Modbus_TCP_Master_Connection_Info *connection_info);

    /***************************************Modbus RTU Slave***************************************/
    MODBUS_SLAVE_ERR_CODE MX_RTU_Modbus_Rtu_Slave_Init();

    MODBUS_SLAVE_ERR_CODE MX_RTU_Modbus_Rtu_Slave_Uninit();

    MODBUS_SLAVE_ERR_CODE MX_RTU_Modbus_Rtu_Slave_Register(UINT8 slot, SERIAL_PORT port, INT32 slaveID, UINT16 map_size, TTY_PARAM *param, UINT32 *handle);

    MODBUS_SLAVE_ERR_CODE MX_RTU_Modbus_Rtu_Slave_Unregister(UINT32 handle);
    //MODBUS_SLAVE_ERR_CODE MX_RTU_Modbus_Rtu_Slave_Unregister(UINT32 handle, UINT8 slot, SERIAL_PORT port);

    MODBUS_SLAVE_ERR_CODE MX_RTU_Modbus_Rtu_Slave_Start(UINT32 handle);

    MODBUS_SLAVE_ERR_CODE MX_RTU_Modbus_Rtu_Slave_Stop(UINT32 handle);

    MODBUS_SLAVE_ERR_CODE MX_RTU_Modbus_Rtu_Slave_Add_Entry(UINT32 handle, UINT8 map_type, UINT16 address, void *pUserData, pfnModbusRead, pfnModbusWrite);

    MODBUS_SLAVE_ERR_CODE MX_RTU_Modbus_Rtu_Slave_Delete_Entry(UINT32 handle, UINT8 map_type, UINT16 address);

    /*********************************************************************************************/

    /*********************************************AOPC*********************************************/
#define AOPC_DEFAULT_PORT   9900


#define     AOPC_MAX_DEV_NAME               59
#define AOPC_MAX_TAG_NAME               30
#define AOPC_MAX_TAG_DESC               16
#define AOPC_MAX_TAG_UNIT               16
#define AOPC_MAX_MULTIVALUES_UPDATE 256

    //AOPC tag value type
#define TAG_TYPE_BOOL           1
#define TAG_TYPE_WORD       2
#define TAG_TYPE_INT            3
#define TAG_TYPE_DWORD      4
#define TAG_TYPE_FLOAT      5
#define TAG_TYPE_STRING     6
#define TAG_TYPE_SHORT          7

    //AOPC tag access right
#define TAG_ACC_READ            0   //read only
    //#define TAG_ACC_WRITE 1   //write only
#define TAG_ACC_READ_WRITE  2   //read/writable

    //AOPC tag quality
#define TAG_QUALITY_GOOD        0x0000
#define TAG_QUALITY_BAD     0x8000

    typedef struct TagInfo
    {
        UINT8 strTagName[AOPC_MAX_TAG_NAME+1];
        UINT8 strTagDescription[AOPC_MAX_TAG_DESC+1];   
        UINT8 strTagUnit[AOPC_MAX_TAG_UNIT+1];      
        UINT8 tagAccessRight;           //0:read only, 1:write only, 2:read/writable
        UINT16 tagValueType;
        UINT16 tagQuality;              //(0x0000-0x7FFF:Good, 0x8000-0xFFFF:Bad)
        void *tagValue;
        UINT8 (*tagCallBack)(UINT8 *tagName, UINT16 tagValueType, UINT8 *tagValue); 
    }TAG;

    typedef enum
    {
        AOPC_ERR_OK = 0,                 //successfully
        AOPC_ERR_LIB_INIT,               //please call MX_RTU_AOPC_Init() first
        AOPC_ERR_PARAM,              //invalid parameters
        AOPC_ERR_SOCKET,                 //network error
        AOPC_ERR_TIMEOUT,                //timeout
        AOPC_ERR_CONNECT,            //connect to AOPC server error
        AOPC_ERR_CREATE_THREAD,      //create thread  error
        AOPC_ERR_NO_MEMORY,          //alloc memory error
        AOPC_ERR_NO_QUEUE,           //queue lenght reaches maximum limitation: 65536 items   
        AOPC_ERR_INTR,                   //interrupt by signal
        AOPC_ERR_FRAME_PENDING,      //waiting for a complete frame data
        AOPC_ERR_FRAME_ERROR,        //frame data format error
        AOPC_ERR_SERVER_ERROR,       //this is a temp error for API developments
        AOPC_ERR_CLOSE,              //connection is closed   
        AOPC_ERR_NO_TAG,                 //no such tag
        AOPC_ERR_EXIST_TAG,          //tag exists
        AOPC_ERR_NO_WRITE_FUNC,      //user does not register a write-callback for this tag
        AOPC_ERR_ACCESS,                 //no write access right
        AOPC_ERR_USER_WRITE,             //user's wirte callback returns error condition
        AOPC_ERR_NOT_SUPPORT,        //the function is not supported
        AOPC_ERR_NOT_WRITABLE,       //the tag is marked writable but the write-callback function is not assigned
        AOPC_ERR_FAIL,                   //generic error
        AOPC_ERR_NOT_REGISTERED,     //device is not registered yet
        AOPC_ERR_INVALID_ATTR,       //invalid tag attribute    
        AOPC_ERR_SYSTEM_INFO,        //can't get system info
        AOPC_ERR_STR_LEN,                //string length exceed 255 bytes
        AOPC_ERR_AMOUNT             
    } AOPC_ERR_CODE;

    AOPC_ERR_CODE MX_RTU_AOPC_Init(void);

    void MX_RTU_AOPC_Uninit(void);

    AOPC_ERR_CODE MX_RTU_AOPC_Connect(UINT8 *deviceName, UINT32 heartBeatS, UINT8 *ipAddress, UINT16 port, UINT32 timeoutMs, UINT32 *sHandle);

    AOPC_ERR_CODE MX_RTU_AOPC_Disconnect(UINT32 sHandle);

    AOPC_ERR_CODE MX_RTU_AOPC_Reconnect(UINT32 sHandle, UINT32 timeoutMs);

    AOPC_ERR_CODE MX_RTU_AOPC_AddTag(UINT32 sHandle, TAG *tagInfo, struct Timestamp *tagTime, UINT32 timeoutMs);

    AOPC_ERR_CODE MX_RTU_AOPC_DelTag(UINT32 sHandle, UINT8 *tagName, UINT32 timeoutMs);

    AOPC_ERR_CODE MX_RTU_AOPC_DelAllTag(UINT32 sHandle, UINT32 timeoutMs);

    AOPC_ERR_CODE MX_RTU_AOPC_UpdateTag(UINT32 sHandle, TAG *tagInfo, struct Timestamp *tagTime, UINT32 timeoutMs);

    AOPC_ERR_CODE MX_RTU_AOPC_UpdateValue(UINT32 sHandle, UINT8 *tagName, void *tagValue, struct Timestamp *tagTime, UINT32 timeoutMs);

    AOPC_ERR_CODE MX_RTU_AOPC_UpdateMultiValue(UINT32 sHandle, UINT16 nTags, UINT8 **tagName, void **tagValue, struct Timestamp *tagTime, UINT32 timeoutMs);

    AOPC_ERR_CODE MX_RTU_AOPC_UpdateHeartbeat(UINT32 sHandle, UINT32 heartbeatS, UINT32 timeoutMs);
    /*********************************************************************************************/

    /*********************************************TAG*********************************************/
#define TAG_MAX_NAME_SIZE   64

#define TAG_DATA_TYPE_COIL      0
#define TAG_DATA_TYPE_INT8      1
#define TAG_DATA_TYPE_INT16     2
#define TAG_DATA_TYPE_INT32     3
#define TAG_DATA_TYPE_UINT8     4
#define TAG_DATA_TYPE_UINT16        5
#define TAG_DATA_TYPE_UINT32        6
#define TAG_DATA_TYPE_FLOAT     7
#define TAG_DATA_TYPE_STR           8

#define TAG_EVENT_CONDITION_CHANGE          0
#define TAG_EVENT_CONDITION_EDGE                1
#define TAG_EVENT_CONDITION_RAISING_EDGE        2
#define TAG_EVENT_CONDITION_FALLING_EDGE        3
#define TAG_EVENT_CONDITION_GREATER         4   
#define TAG_EVENT_CONDITION_SMALLER         5   

    typedef enum
    {
        TAG_ERR_OK = 0,              //successfully
        TAG_ERR_INIT,                // Tag init error
        TAG_ERR_SEM,                 // 
        TAG_ERR_MSGQUEUE,            // message queue access err 
        TAG_ERR_ALLOC,               //
        TAG_ERR_PARAM,               // invalid parameters
        TAG_ERR_REG,                     //
        TAG_ERR_DESTROY,             //
        TAG_ERR_BIND,                // bind resource
        TAG_ERR_RESET,               //
        TAG_ERR_AGAIN,               //
        TAG_ERR_IO,                  // access IO err
        TAG_ERR_READY,               // Tag framework has been init
        TAG_ERR_NO_TAG,              //
        TAG_ERR_MSGQUEUE_FULL,       // message queue full
        TAG_ERR_NOT_SUPPORT,         // this usage is not supported
        TAG_ERR_EVENT_INIT,          // Tag event init error
        TAG_ERR_EVENT_REGISTER,      // Tag event register error
        TAG_ERR_EVENT_SPACE,         // Too many events
        TAG_ERR_EVENT_HANDLE,        // No such event
        TAG_ERR_EVENT_EMPTY,         // No event occurred
        TAG_ERR_AMOUNT              
    } TAG_ERR_CODE;

    //Service and Profile status
    typedef enum
    {
        STATUS_INIT = 0,                    
        STATUS_READY,                       
        STATUS_RUNNING,                 
        STATUS_SUCCESS,                 
        STATUS_FAIL,                        
        STATUS_ERR_AMOUNT               
    } STATUS_CODE;

    typedef struct TAG_INFO_T
    {
        UINT8 tagName[TAG_MAX_NAME_SIZE];
        UINT8 resId[TAG_MAX_NAME_SIZE];
        UINT32 tagId;
        UINT32 tagSize;
        UINT32 dataType;    
    } TAG_INFO;

    typedef struct TAG_EVENT_CONDITION_T
    {
        UINT32 condition;   
        float threshold;
        float hysteresis;
    } TAG_EVENT_CONDITION;

    TAG_ERR_CODE MX_RTU_Tag_Init(void);

    TAG_ERR_CODE MX_RTU_Tag_Uninit(void);

    TAG_ERR_CODE MX_RTU_Tag_List_All(void);

    TAG_ERR_CODE MX_RTU_Tag_Get_List(TAG_INFO ***list, UINT32 *quantity);

    TAG_ERR_CODE MX_RTU_Tag_Get_Info(UINT8 *tagName, TAG_INFO *info);

    TAG_ERR_CODE MX_RTU_Tag_Read(UINT8 *tagName, void *tagValue, UINT32 tagSize, UINT32 *readBytes, struct Timestamp *time);

    TAG_ERR_CODE MX_RTU_Tag_Write(UINT8 *tagName, void *tagValue, UINT32 tagSize);

    TAG_ERR_CODE MX_RTU_MultiTag_Read(UINT8 *prefixTagName, UINT8 *postfixTagName, UINT16 start, UINT16 count, UINT8 *tagValue, struct Timestamp *tagTimeStamp);

    TAG_ERR_CODE MX_RTU_MultiTag_Write(UINT8 *prefixTagName, UINT8 *postfixTagName, UINT16 start, UINT16 count, UINT8 *tagValue);

    TAG_ERR_CODE MX_RTU_Tag_Event_Register(UINT8 *tagName, TAG_EVENT_CONDITION *condition, int *handle);

    TAG_ERR_CODE MX_RTU_Tag_Event_Unregister(int handle);

    TAG_ERR_CODE MX_RTU_Tag_Event_Get(int handle, void *tagValue, struct Timestamp *time);

    TAG_ERR_CODE MX_RTU_Tag_Event_Clear(int handle);

    /*********************************************************************************************/



#ifdef __cplusplus
}//extern "C"
#endif

#endif
