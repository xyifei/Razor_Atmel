/**********************************************************************************************************************
File: user_app1.c                                                                

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app1 as a template:
 1. Copy both user_app1.c and user_app1.h to the Application directory
 2. Rename the files yournewtaskname.c and yournewtaskname.h
 3. Add yournewtaskname.c and yournewtaskname.h to the Application Include and Source groups in the IAR project
 4. Use ctrl-h (make sure "Match Case" is checked) to find and replace all instances of "user_app1" with "yournewtaskname"
 5. Use ctrl-h to find and replace all instances of "UserApp1" with "YourNewTaskName"
 6. Use ctrl-h to find and replace all instances of "USER_APP1" with "YOUR_NEW_TASK_NAME"
 7. Add a call to YourNewTaskNameInitialize() in the init section of main
 8. Add a call to YourNewTaskNameRunActiveState() in the Super Loop section of main
 9. Update yournewtaskname.h per the instructions at the top of yournewtaskname.h
10. Delete this text (between the dashed lines) and update the Description below to describe your task
----------------------------------------------------------------------------------------------------------------------

Description:
This is a user_app1.c file template 

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
All Global variable names shall start with "G_UserApp1"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32UserApp1Flags;                       /* Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */

extern u8 G_u8DebugScanfCharCount;

/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp1_StateMachine;            /* The state machine function pointer */

static u8 u8EnterIn[2];
static bool bOneOrTwo=TRUE;
static bool bOn=TRUE;
static bool bOpen=FALSE;
//static u32 UserApp1_u32Timeout;                      /* Timeout counter used across states */


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
void UserApp1Initialize(void)
{
    LCDCommand(LCD_CLEAR_CMD);
    LedOff(WHITE);
    LedOff(PURPLE);
    LedOff(BLUE);
    LedOff(CYAN);
    LedOff(GREEN);
    LedOff(YELLOW);
    LedOff(ORANGE);
    LedOff(RED);
    LedOff(LCD_RED);
    LedOff(LCD_GREEN);
    LedOff(LCD_BLUE);
    PWMAudioSetFrequency(BUZZER2,200);
 
  /* If good initialization, set state to Idle */
  if( 1 )
  {
    UserApp1_StateMachine = UserApp1SM_state1;  //Initial state
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    UserApp1_StateMachine = UserApp1SM_Error;
  }

} /* end UserApp1Initialize() */

  
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
/* Wait for ??? */
static void UserApp1SM_state1(void)
{
    if(bOn)
    {
        LCDCommand(LCD_CLEAR_CMD);
        LCDMessage(7,"STATE 1");
        DebugPrintf("Entering state 1");
        DebugLineFeed();
        LedOn(WHITE);
        LedOn(PURPLE);
        LedOn(BLUE);
        LedOn(CYAN);
        LedOff(GREEN);
        LedOff(YELLOW);
        LedOff(ORANGE);
        LedOff(RED);
        LedOn(LCD_RED);
        LedOff(LCD_GREEN);
        LedOn(LCD_BLUE);
        PWMAudioOff(BUZZER2);
        
        bOn=FALSE;
    }
    
    if(G_u8DebugScanfCharCount==2)//when you enter in two character
    {
        DebugScanf(u8EnterIn);
        
        if((u8EnterIn[0]=='2')&&(u8EnterIn[1]=='\r'))
        {
            bOneOrTwo=FALSE;
        }
    }
    
    if(WasButtonPressed(BUTTON2))//judge whether button is pressed
    {
        ButtonAcknowledge(BUTTON2);
        bOneOrTwo=FALSE;
    }
    
    if(bOneOrTwo==FALSE)//change the state
    {
        bOn=TRUE;
        UserApp1_StateMachine = UserApp1SM_state2;
        bOpen=TRUE;
    }
}

static void UserApp1SM_state2(void)
{
    static u16 u16TimeCounter=0;
    
    if(bOn)
    {
        LCDMessage(7,"STATE 2");
        DebugPrintf("Entering state 2");
        DebugLineFeed();
        LedBlink(GREEN,LED_1HZ);
        LedBlink(YELLOW,LED_2HZ);
        LedBlink(ORANGE,LED_4HZ);
        LedBlink(RED,LED_8HZ);
        LedOff(WHITE);
        LedOff(PURPLE);
        LedOff(BLUE);
        LedOff(CYAN);
        LedPWM(LCD_RED,LED_PWM_100);
        LedPWM(LCD_GREEN,LED_PWM_50);
        LedOff(LCD_BLUE);
        
        bOn=FALSE;
    }
    
    if(bOpen)          //open the buzzer
    {
        if(u16TimeCounter==0)
        {
            PWMAudioOn(BUZZER2);
        }
        
        if(u16TimeCounter==100)
        {
            PWMAudioOff(BUZZER2);
        }
        
        if(u16TimeCounter==1000)
        {
            PWMAudioOn(BUZZER2);
            u16TimeCounter=0;
        }
        
        u16TimeCounter++;
    }
    
    if(G_u8DebugScanfCharCount==2)
    {
        DebugScanf(u8EnterIn);
        
        if((u8EnterIn[0]=='1')&&(u8EnterIn[1]=='\r'))
        {
            bOneOrTwo=TRUE;
        }
    }
    
    if(WasButtonPressed(BUTTON1))
    {
        ButtonAcknowledge(BUTTON1);
        bOneOrTwo=TRUE;
    }
    
    if(bOneOrTwo==TRUE)
    {
        bOn=TRUE;
        UserApp1_StateMachine = UserApp1SM_state1;
        bOpen=FALSE;
    }
    
} /* end UserApp1SM_Idle() */
    

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
  
} /* end UserApp1SM_Error() */



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
