/**********************************************************************************************************************
File: user_app.h                                                                

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app as a template:
1. Follow the instructions at the top of user_app.c
2. Use ctrl-h to find and replace all instances of "user_app" with "yournewtaskname"
3. Use ctrl-h to find and replace all instances of "UserApp" with "YourNewTaskName"
4. Use ctrl-h to find and replace all instances of "USER_APP" with "YOUR_NEW_TASK_NAME"
5. Add #include yournewtaskname.h" to configuration.h
6. Add/update any special configurations required in configuration.h (e.g. peripheral assignment and setup values)
7. Delete this text (between the dashed lines)
----------------------------------------------------------------------------------------------------------------------

Description:
Header file for yournewtaskname.c

**********************************************************************************************************************/

#ifndef __USER_APP_H
#define __USER_APP_H

/**********************************************************************************************************************
Type Definitions
**********************************************************************************************************************/


/**********************************************************************************************************************
Constants / Definitions
**********************************************************************************************************************/
/* Required constants for ANT channel configuration */
#define ANT_CHANNEL_SLAVE             ANT_CHANNEL_0        /* Channel 0 - 7 */
#define ANT_CHANNEL_MASTER            ANT_CHANNEL_1
#define ANT_CHANNEL_TYPE_SLAVE        CHANNEL_TYPE_SLAVE    /* ANT SLAVE */
#define ANT_CHANNEL_TYPE_MASTER       CHANNEL_TYPE_MASTER
#define ANT_DEVICEID_LO_USERAPP         (u8)0xD7                /* Low byte of two-byte Device # */
#define ANT_DEVICEID_HI_USERAPP         (u8)0x1F                /* High byte of two-byte Device # */
#define ANT_DEVICE_TYPE_USERAPP         (u8)1                 /* 1 - 255 */
#define ANT_TRANSMISSION_TYPE_USERAPP   (u8)1                /* 1-127 (MSB is pairing bit) */
#define ANT_CHANNEL_PERIOD_LO_USERAPP   (u8)0x00              /* Low byte of two-byte channel period 0x0001 - 0x7fff */
#define ANT_CHANNEL_PERIOD_HI_USERAPP   (u8)0x20              /* High byte of two-byte channel period */
#define ANT_FREQUENCY_USERAPP           (u8)50                /* 2400MHz + this number 0 - 99 */
#define ANT_TX_POWER_USERAPP            RADIO_TX_POWER_4DBM   /* RADIO_TX_POWER_0DBM, RADIO_TX_POWER_MINUS5DBM, RADIO_TX_POWER_MINUS10DBM, RADIO_TX_POWER_MINUS20DBM */

#define TIMEOUT_VALUE                   (u32)2000             /* Maximum allowed timeout value for any transition state */

/* G_u32UserAppFlags */
#define _CHANNEL_SYNCED                 0x00000001            /* Set when channel is synced */

#define MASTER                          (bool)TRUE
#define SLAVE                           (bool)FALSE
/**********************************************************************************************************************
Function Declarations
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Public functions                                                                                                   */
/*--------------------------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------------------*/
/* Protected functions                                                                                                */
/*--------------------------------------------------------------------------------------------------------------------*/
void UserApp1Initialize_Slave(void);
void UserApp1Initialize_Master(void);
void UserApp1RunActiveState(void);


/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/


/***********************************************************************************************************************
State Machine Declarations
***********************************************************************************************************************/
static void UserApp1SM_WaitChannelAssign(void);
static void UserApp1SM_Idle(void);    
static void UserApp1SM_WaitChannelOpen_Slave(void);
static void UserApp1SM_WaitChannelOpen_Master(void);
static void UserApp1SM_ChannelOpen_Slave(void);
static void UserApp1SM_ChannelOpen_Master(void);
static void UserApp1SM_WaitChannelClose(void);

static void UserApp1SM_Error(void);         
static void UserApp1SM_FailedInit(void);        

static void PrintSData(s8 s8Data);

static void AllLedOff(void);

#endif /* __USER_APP_H */


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
