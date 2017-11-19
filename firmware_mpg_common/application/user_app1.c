/**********************************************************************************************************************
File: user_app.c                                                                

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app as a template:
 1. Copy both user_app.c and user_app.h to the Application directory
 2. Rename the files yournewtaskname.c and yournewtaskname.h
 3. Add yournewtaskname.c and yournewtaskname.h to the Application Include and Source groups in the IAR project
 4. Use ctrl-h (make sure "Match Case" is checked) to find and replace all instances of "user_app" with "yournewtaskname"
 5. Use ctrl-h to find and replace all instances of "UserApp1" with "YourNewTaskName"
 6. Use ctrl-h to find and replace all instances of "USER_APP" with "YOUR_NEW_TASK_NAME"
 7. Add a call to YourNewTaskNameInitialize() in the init section of main
 8. Add a call to YourNewTaskNameRunActiveState() in the Super Loop section of main
 9. Update yournewtaskname.h per the instructions at the top of yournewtaskname.h
10. Delete this text (between the dashed lines) and update the Description below to describe your task
----------------------------------------------------------------------------------------------------------------------

Description:
This is a user_app.c file template 

------------------------------------------------------------------------------------------------------------------------
API:

Public functions:


Protected System functions:
void UserApp1Initialize(void)
Runs required initialzation for the task.  Should only be called once in main init section.

void UserApp1RunActiveState(void)
Runs current task state.  Should only be called once in main loop.


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32UserApp1Flags;                       /* Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern u32 G_u32AntApiCurrentDataTimeStamp;                       /* From ant_api.c */
extern AntApplicationMessageType G_eAntApiCurrentMessageClass;    /* From ant_api.c */
extern u8 G_au8AntApiCurrentMessageBytes[ANT_APPLICATION_MESSAGE_BYTES];  /* From ant_api.c */
extern AntExtendedDataType G_sAntApiCurrentMessageExtData;        /* From ant_api.c */

extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */



/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_" and be declared as static.
***********************************************************************************************************************/
static u32 UserApp1_u32DataMsgCount = 0;             /* Counts the number of ANT_DATA packets received */
static u32 UserApp1_u32TickMsgCount = 0;             /* Counts the number of ANT_TICK packets received */

static fnCode_type UserApp1_StateMachine;            /* The state machine function pointer */
static u32 UserApp1_u32Timeout;                      /* Timeout counter used across states */

static bool bMasterOrSlave = SLAVE;
static bool bFinded=FALSE;

/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Public functions                                                                                                   */
/*--------------------------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------------------*/
/* Protected functions                                                                                                */
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
Function: UserApp1Initialize

Description:
Initializes the State Machine and its variables.

Requires:
  -

Promises:
  - 
*/
void UserApp1Initialize_Slave(void)
{
  u8 au8WelcomeMessage[] = "Hide and Go Seek!";
  u8 au8Instructions[] = "Press B0 to Start";
  AntAssignChannelInfoType sAntSetupData;
  
  PWMAudioSetFrequency(BUZZER1, 500);
  
  /* Clear screen and place start messages */
  LCDCommand(LCD_CLEAR_CMD);
  LCDMessage(LINE1_START_ADDR, au8WelcomeMessage); 
  LCDMessage(LINE2_START_ADDR, au8Instructions); 

  /* Start with LED0 in RED state = channel is not configured */
  LedOn(RED);
  
 /* Configure ANT for this application */
  sAntSetupData.AntChannel          = ANT_CHANNEL_SLAVE;
  sAntSetupData.AntChannelType      = ANT_CHANNEL_TYPE_SLAVE;
  sAntSetupData.AntChannelPeriodLo  = ANT_CHANNEL_PERIOD_LO_USERAPP;
  sAntSetupData.AntChannelPeriodHi  = ANT_CHANNEL_PERIOD_HI_USERAPP;
  
  sAntSetupData.AntDeviceIdLo       = ANT_DEVICEID_LO_USERAPP;
  sAntSetupData.AntDeviceIdHi       = ANT_DEVICEID_HI_USERAPP;
  sAntSetupData.AntDeviceType       = ANT_DEVICE_TYPE_USERAPP;
  sAntSetupData.AntTransmissionType = ANT_TRANSMISSION_TYPE_USERAPP;
  sAntSetupData.AntFrequency        = ANT_FREQUENCY_USERAPP;
  sAntSetupData.AntTxPower          = ANT_TX_POWER_USERAPP;

  sAntSetupData.AntNetwork = ANT_NETWORK_DEFAULT;
  for(u8 i = 0; i < ANT_NETWORK_NUMBER_BYTES; i++)
  {
    sAntSetupData.AntNetworkKey[i] = ANT_DEFAULT_NETWORK_KEY;
  }
    
  /* If good initialization, set state to Idle */
  if( AntAssignChannel(&sAntSetupData) )
  {
    /* Channel assignment is queued so start timer */
    UserApp1_u32Timeout = G_u32SystemTime1ms;
    bMasterOrSlave = SLAVE;
    LedOn(RED);
    
    UserApp1_StateMachine = UserApp1SM_WaitChannelAssign;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    LedBlink(RED, LED_4HZ);

    UserApp1_StateMachine = UserApp1SM_Error;
  }

} /* end UserApp1Initialize_Slave() */

void UserApp1Initialize_Master(void)
{
  u8 au8WelcomeMessage[] = "Hide and Go Seek!";
  u8 au8Instructions[] = "Press B0 to Start";
  AntAssignChannelInfoType mAntSetupData;
  
  PWMAudioSetFrequency(BUZZER1, 500);
  
  /* Clear screen and place start messages */
  LCDCommand(LCD_CLEAR_CMD);
  LCDMessage(LINE1_START_ADDR, au8WelcomeMessage); 
  LCDMessage(LINE2_START_ADDR, au8Instructions); 

  /* Start with LED0 in RED state = channel is not configured */
  LedOn(RED);
  
 /* Configure ANT for this application */
  mAntSetupData.AntChannel          = ANT_CHANNEL_MASTER;
  mAntSetupData.AntChannelType      = ANT_CHANNEL_TYPE_MASTER;
  mAntSetupData.AntChannelPeriodLo  = ANT_CHANNEL_PERIOD_LO_USERAPP;
  mAntSetupData.AntChannelPeriodHi  = ANT_CHANNEL_PERIOD_HI_USERAPP;
  
  mAntSetupData.AntDeviceIdLo       = ANT_DEVICEID_LO_USERAPP;
  mAntSetupData.AntDeviceIdHi       = ANT_DEVICEID_HI_USERAPP;
  mAntSetupData.AntDeviceType       = ANT_DEVICE_TYPE_USERAPP;
  mAntSetupData.AntTransmissionType = ANT_TRANSMISSION_TYPE_USERAPP;
  mAntSetupData.AntFrequency        = ANT_FREQUENCY_USERAPP;
  mAntSetupData.AntTxPower          = ANT_TX_POWER_USERAPP;

  mAntSetupData.AntNetwork = ANT_NETWORK_DEFAULT;
  for(u8 i = 0; i < ANT_NETWORK_NUMBER_BYTES; i++)
  {
    mAntSetupData.AntNetworkKey[i] = ANT_DEFAULT_NETWORK_KEY;
  }
    
  /* If good initialization, set state to Idle */
  if( AntAssignChannel(&mAntSetupData) )
  {
    /* Channel assignment is queued so start timer */
    UserApp1_u32Timeout = G_u32SystemTime1ms;
    bMasterOrSlave = MASTER;
    LedOn(RED);
    
    UserApp1_StateMachine = UserApp1SM_WaitChannelAssign;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    LedBlink(RED, LED_4HZ);

    UserApp1_StateMachine = UserApp1SM_Error;
  }

} /* end UserApp1Initialize_Master() */

static void PrintSData(s8 s8Data)
{
	u8 u8aTemp[]={'-',1,1,'d','B','m','\0'};
	u8 temp=abs(s8Data);
	u8aTemp[1]=temp/10+48;
	u8aTemp[2]=temp%10+48;
	LCDMessage(LINE1_START_ADDR+15,u8aTemp);
}

static void AllLedOff(void)
{
    LedOff(WHITE);
    LedOff(PURPLE);
    LedOff(BLUE);
    LedOff(CYAN);
    LedOff(GREEN);
    LedOff(YELLOW);
    LedOff(ORANGE);
    LedOff(RED);
}

/*----------------------------------------------------------------------------------------------------------------------
Function UserApp1RunActiveState()

Description:
Selects and runs one iteration of the current state in the state machine.
All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
  - State machine function pointer points at current state

Promises:
  - Calls the function to pointed by the state machine function pointer
*/
void UserApp1RunActiveState(void)
{
  UserApp1_StateMachine();

} /* end UserApp1RunActiveState */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for the ANT channel assignment to finish */
static void UserApp1SM_WaitChannelAssign(void)
{
  /* Check if the channel assignment is complete */  
  if(AntRadioStatusChannel(ANT_CHANNEL_SLAVE) == ANT_CONFIGURED)
  {
    LedOff(RED);
    LedOn(YELLOW);

    UserApp1_StateMachine = UserApp1SM_Idle;
  }
  
  if(AntRadioStatusChannel(ANT_CHANNEL_MASTER) == ANT_CONFIGURED)
  {
    LedOff(RED);
    LedOn(YELLOW);

    UserApp1_StateMachine = UserApp1SM_Idle;
  }
  
  /* Monitor for timeout */
  if( IsTimeUp(&UserApp1_u32Timeout, 5000) )
  {
    DebugPrintf("\n\r***Channel assignment timeout***\n\n\r");
    UserApp1_StateMachine = UserApp1SM_Error;
  }
      
} /* end UserApp1SM_WaitChannelAssign() */


  /*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for a message to be queued */
static void UserApp1SM_Idle(void)
{
  /* Look for BUTTON 0 to open channel */
  if(WasButtonPressed(BUTTON0))
  {
    /* Got the button, so complete one-time actions before next state */
    ButtonAcknowledge(BUTTON0);
    
    if(bMasterOrSlave==SLAVE)
    {
        LCDCommand(LCD_CLEAR_CMD);
        LCDMessage(LINE1_START_ADDR, "Seeker"); 
        AntOpenChannelNumber(ANT_CHANNEL_SLAVE);
        
        UserApp1_u32Timeout = G_u32SystemTime1ms;
        UserApp1_StateMachine = UserApp1SM_WaitChannelOpen_Slave;
    }
    
    if(bMasterOrSlave==MASTER)
    {
        LCDCommand(LCD_CLEAR_CMD);
        LCDMessage(LINE1_START_ADDR, "Hide!"); 
        AntOpenChannelNumber(ANT_CHANNEL_MASTER);
            
        UserApp1_u32Timeout = G_u32SystemTime1ms;
        UserApp1_StateMachine = UserApp1SM_WaitChannelOpen_Master;
    }
    
    /* Queue open channel and change LED0 from yellow to blinking green to indicate channel is opening */
    LedOff(YELLOW);
    LedBlink(GREEN, LED_2HZ);
  }
    
} /* end UserApp1SM_Idle() */
     

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for channel to open */
static void UserApp1SM_WaitChannelOpen_Slave(void)
{
  /* Monitor the channel status to check if channel is opened */
  if(AntRadioStatusChannel(ANT_CHANNEL_SLAVE) == ANT_OPEN)
  {
    LedOn(GREEN);
    LCDMessage(LINE1_START_ADDR, "Ready or not?"); 
    
    UserApp1_StateMachine = UserApp1SM_ChannelOpen_Slave;      
  }

  /* Check for timeout */
  if( IsTimeUp(&UserApp1_u32Timeout, TIMEOUT_VALUE) )
  {
    AntCloseChannelNumber(ANT_CHANNEL_SLAVE);

    LedOff(GREEN);
    LedOn(YELLOW);
    
    UserApp1_StateMachine = UserApp1SM_Idle;
  }
    
} /* end UserApp1SM_WaitChannelOpen() */

static void UserApp1SM_WaitChannelOpen_Master(void)
{
  /* Monitor the channel status to check if channel is opened */
  if(AntRadioStatusChannel(ANT_CHANNEL_MASTER) == ANT_OPEN)
  {
    LedOn(GREEN);
    
    UserApp1_StateMachine = UserApp1SM_ChannelOpen_Master;      
  }
  
  /* Check for timeout */
  if( IsTimeUp(&UserApp1_u32Timeout, TIMEOUT_VALUE) )
  {
    AntCloseChannelNumber(ANT_CHANNEL_MASTER);

    LedOff(GREEN);
    LedOn(YELLOW);
    
    UserApp1_StateMachine = UserApp1SM_Idle;
  }
    
} /* end UserApp1SM_WaitChannelOpen() */

/*-------------------------------------------------------------------------------------------------------------------*/
/* Channel is open, so monitor data */
static void UserApp1SM_ChannelOpen_Slave(void)
{
  static u8 au8TestMessage[] = {0, 0, 0, 0, 0xA5, 0, 0, 0};
  LedNumberType LedNum[]={WHITE, PURPLE, BLUE, CYAN, GREEN, YELLOW, ORANGE, RED};
  static s8 s8Rssi;
  static u16 u16delay=0;
  static bool bsign=FALSE;
  
  if(bsign)
  {
     u16delay++;
     if(u16delay>=2000)
     {
        bsign=FALSE;
        u16delay=0;
        AntCloseChannelNumber(ANT_CHANNEL_SLAVE);
 
        UserApp1_u32Timeout = G_u32SystemTime1ms;
        UserApp1_StateMachine = UserApp1SM_WaitChannelClose_Slave;
     }
     return;    
  }
  
  /* Check for BUTTON0 to close channel */
  if(WasButtonPressed(BUTTON0))
  {
    /* Got the button, so complete one-time actions before next state */
    ButtonAcknowledge(BUTTON0);
    AllLedOff();
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, "Seeker"); 
    
    /* Queue close channel and change LED to blinking green to indicate channel is closing */
    AntCloseChannelNumber(ANT_CHANNEL_SLAVE);

    LedOff(YELLOW);
    LedOff(BLUE);
    LedBlink(GREEN, LED_2HZ);
    
    /* Set timer and advance states */
    UserApp1_u32Timeout = G_u32SystemTime1ms;
    UserApp1_StateMachine = UserApp1SM_WaitChannelClose_Slave;
  } /* end if(WasButtonPressed(BUTTON0)) */
  
  /* Always check for ANT messages */
  if( AntReadAppMessageBuffer() )
  {
     /* New data message: check what it is */
    if(G_eAntApiCurrentMessageClass == ANT_DATA)
    {
      UserApp1_u32DataMsgCount++;
      LCDCommand(LCD_CLEAR_CMD);
      LCDMessage(LINE1_START_ADDR, "Here I come!"); 
      s8Rssi=G_sAntApiCurrentMessageExtData.s8RSSI;
      PrintSData(s8Rssi);
      
      for(u8 i=0;i<8;i++)
      {
        if(s8Rssi>=(-45-i*5))
        {
            LedOn(LedNum[i]);
        }
      }
      
      for(u8 i=0;i<=7;i++)
      {
        if(s8Rssi<=(-45-i*5))
        {
            LedOff(LedNum[i]);
        }
      }
      
      if(s8Rssi>=-50)
      {
        AllLedOff();
        bFinded=TRUE;
        LedBlink(WHITE,LED_4HZ);
        LedBlink(PURPLE,LED_4HZ);
        LedBlink(BLUE,LED_4HZ);
        LedBlink(CYAN,LED_4HZ);
        LedBlink(GREEN,LED_4HZ);
        LedBlink(YELLOW,LED_4HZ);
        LedBlink(ORANGE,LED_4HZ);
        LedBlink(RED,LED_4HZ);
        au8TestMessage[0]=1;
        PWMAudioOn(BUZZER1);
        AntQueueBroadcastMessage(ANT_CHANNEL_SLAVE,au8TestMessage);
        LCDCommand(LCD_CLEAR_CMD);
        LCDMessage(LINE1_START_ADDR,"Found you!" );
        bsign=TRUE;
      }
      
    } /* end if(G_eAntApiCurrentMessageClass == ANT_DATA) */
    
     else if(G_eAntApiCurrentMessageClass == ANT_TICK)
    {
        UserApp1_u32TickMsgCount++;
    }  
  } /* end AntReadAppMessageBuffer() */
  
  /* A slave channel can close on its own, so explicitly check channel status */
  if(AntRadioStatusChannel(ANT_CHANNEL_SLAVE) != ANT_OPEN)
  {
    LedBlink(GREEN, LED_2HZ);
    LedOff(BLUE);
    
    UserApp1_u32Timeout = G_u32SystemTime1ms;
    UserApp1_StateMachine = UserApp1SM_WaitChannelClose_Slave;
  } /* if(AntRadioStatusChannel(ANT_CHANNEL_USERAPP) != ANT_OPEN) */     
}

static void UserApp1SM_ChannelOpen_Master(void)
{
  static u8 au8TestMessage[] = {0x5B, 0, 0, 0, 0xFF, 0, 0, 0};
  static u16 u16delay=0;
  static bool bsign=FALSE;
  
  if(bsign)
  {
     u16delay++;
     if(u16delay>=2000)
     {
        bsign=FALSE;
        u16delay=0;
        AntCloseChannelNumber(ANT_CHANNEL_MASTER);
        
        UserApp1_u32Timeout = G_u32SystemTime1ms;
        UserApp1_StateMachine = UserApp1SM_WaitChannelClose_Master;
     }
     return;    
  }
  
  if(WasButtonPressed(BUTTON0))
  {
    /* Got the button, so complete one-time actions before next state */
    ButtonAcknowledge(BUTTON0);
    AllLedOff();
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, "Hide!"); 
    
    /* Queue close channel and change LED to blinking green to indicate channel is closing */
    AntCloseChannelNumber(ANT_CHANNEL_MASTER);

    LedOff(YELLOW);
    LedOff(BLUE);
    LedBlink(GREEN, LED_2HZ);
    
    /* Set timer and advance states */
    UserApp1_u32Timeout = G_u32SystemTime1ms;
    UserApp1_StateMachine = UserApp1SM_WaitChannelClose_Master;
  } /* end if(WasButtonPressed(BUTTON0)) */
  
  
  if( AntReadAppMessageBuffer() )
  {
     /* New message from ANT task: check what it is */
    if(G_eAntApiCurrentMessageClass == ANT_DATA)
    {
      /* We got some data: parse it into au8DataContent[] */
        if(G_au8AntApiCurrentMessageBytes[0]==0x01)
        {
            bFinded=TRUE;
            AllLedOff();
            LedBlink(ORANGE,LED_1HZ);
            LCDCommand(LCD_CLEAR_CMD);
            LCDMessage(LINE1_START_ADDR,"You found me!" );
            bsign=TRUE;
        }
    }
    else if(G_eAntApiCurrentMessageClass == ANT_TICK)
    {
     /* Update and queue the new message data */
      au8TestMessage[7]++;
      
      if(au8TestMessage[7] == 0)
      {
        au8TestMessage[6]++;
        if(au8TestMessage[6] == 0)
        {
          au8TestMessage[5]++;
        }
      }
      
      AntQueueAcknowledgedMessage(ANT_CHANNEL_MASTER, au8TestMessage);

    }
  } /* end AntReadData() */
}

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for channel to close */
static void UserApp1SM_WaitChannelClose_Slave(void)
{
  AllLedOff();
  /* Monitor the channel status to check if channel is closed */
  if(AntRadioStatusChannel(ANT_CHANNEL_SLAVE) == ANT_CLOSED)
  {
    LedOff(GREEN);
    LedOn(YELLOW);
    PWMAudioOff(BUZZER1);
    
    if(bMasterOrSlave==SLAVE)
    {
        if(bFinded)
        {
            if(AntRadioStatusChannel(ANT_CHANNEL_MASTER) == ANT_CONFIGURED)
            {
                LCDMessage(LINE1_START_ADDR, "Hide and Go Seek!"); 
                LCDMessage(LINE2_START_ADDR, "Press B0 to Start"); 
                bMasterOrSlave=MASTER;
                UserApp1_StateMachine = UserApp1SM_Idle;
            }
            else
            {
                UserApp1_StateMachine = UserApp1Initialize_Master;     
            }
            
            bFinded=FALSE;            
        }
        else
        {
            UserApp1_StateMachine = UserApp1SM_Idle;
        }
    }
  }
 
  /* Check for timeout */
  if( IsTimeUp(&UserApp1_u32Timeout, TIMEOUT_VALUE) )
  {
    LedOff(GREEN);
    LedOff(YELLOW);
    LedBlink(RED, LED_4HZ);
    
    UserApp1_StateMachine = UserApp1SM_Error;
  }
    
} /* end UserApp1SM_WaitChannelClose() */

static void UserApp1SM_WaitChannelClose_Master(void)
{
  AllLedOff();
  if(AntRadioStatusChannel(ANT_CHANNEL_MASTER) == ANT_CLOSED)
  {
    LedOff(GREEN);
    LedOn(YELLOW);
    
    if(bMasterOrSlave==MASTER)
    {
      if(bFinded)
      {
        if(AntRadioStatusChannel(ANT_CHANNEL_SLAVE) == ANT_CONFIGURED)
        {
          LCDMessage(LINE1_START_ADDR, "Hide and Go Seek!"); 
          LCDMessage(LINE2_START_ADDR, "Press B0 to Start"); 
          bMasterOrSlave=SLAVE;
          UserApp1_StateMachine = UserApp1SM_Idle;
        }
        else
        {
          UserApp1_StateMachine = UserApp1Initialize_Slave;     
        }
        
        bFinded=FALSE;            
      }
      else
      {
        UserApp1_StateMachine = UserApp1SM_Idle;
      }
    }
  }
  
  if( IsTimeUp(&UserApp1_u32Timeout, TIMEOUT_VALUE) )
  {
    LedOff(GREEN);
    LedOff(YELLOW);
    LedBlink(RED, LED_4HZ);
    
    UserApp1_StateMachine = UserApp1SM_Error;
  }
}

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{

} /* end UserApp1SM_Error() */



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
