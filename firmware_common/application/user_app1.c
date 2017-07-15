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
    LedOff(RED);
	LedOff(WHITE);
	LedOff(PURPLE);
	LedOff(BLUE);
	PWMAudioSetFrequency(BUZZER1,1000);
	
  /* If good initialization, set state to Idle */
  if( 1 )
  {
    UserApp1_StateMachine = UserApp1SM_Idle;
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
static void UserApp1SM_Idle(void)
{
	static u8 u8RealPassword[]={'1','2','3','1','2','3'};
	static u8 u8UserPassword[6];
	static u8 u8Index=0;
	static u8 u8Comfirm=0;
	static u16 u16Counter=0;
	static bool bPressed=FALSE;
	static bool bIsOk=TRUE;
	static bool bWrite=FALSE;
	u8 u8TempIndex;
	
	if(WasButtonPressed(BUTTON3))
	{
		ButtonAcknowledge(BUTTON0);
	  	ButtonAcknowledge(BUTTON1);
	  	ButtonAcknowledge(BUTTON2);
	  	ButtonAcknowledge(BUTTON3);		
		u8Comfirm++;
	}
	
	if(IsButtonHeld(BUTTON3,2000))    //determine whether to change the password
	{
		bWrite=TRUE;
		u8Comfirm=0;
	}
		
	if(bWrite)
	{
		if(bIsOk)                    //only when you know the right password can you change the password
		{
			LedOn(YELLOW);
			LedOff(BLUE);
			LedOff(WHITE);
			LedOff(PURPLE);
			LedOff(WHITE);
			
			if(G_u8DebugScanfCharCount>=6)  //Use debug to enter the password
			{
				DebugScanf(u8RealPassword);
				LedOff(YELLOW);
				bWrite=FALSE;
			}
		}
	}
	
	if(u8Comfirm==2)   //determine whether the password is right or not
	{
		for(u8TempIndex=0;u8TempIndex<6;u8TempIndex++)
		{
			if(u8RealPassword[u8TempIndex]!=u8UserPassword[u8TempIndex])
			{
				bIsOk=FALSE;
				break;
			}
			else
			{
				bIsOk=TRUE;
			}
		}
		
		if(bIsOk)
		{
			LedOn(WHITE);
			LedOff(PURPLE);
		}
		else
		{
			LedOff(WHITE);
			LedOn(PURPLE);
		}
		LedOff(BLUE);
		u8Index=0;
		u8Comfirm=0;
	}
	
	if(u8Comfirm==1)    //press the button and save the keyvalue
	{
		LedOn(BLUE);
		LedOff(PURPLE);
		LedOff(WHITE);
		LedOff(YELLOW);
		
		if(u8Index<6)
		{   
			if(WasButtonPressed(BUTTON0))
			{
				ButtonAcknowledge(BUTTON0);
				LedOn(RED);
				bPressed=TRUE;
				PWMAudioOn(BUZZER1);
				u8UserPassword[u8Index]='1';
				u8Index++;
			}
		
			if(WasButtonPressed(BUTTON1))
			{
				ButtonAcknowledge(BUTTON1);
				LedOn(RED);
				bPressed=TRUE;
				PWMAudioOn(BUZZER1);
				u8UserPassword[u8Index]='2';
				u8Index++;
			}
		
			if(WasButtonPressed(BUTTON2))
			{
				ButtonAcknowledge(BUTTON2);
				LedOn(RED);
				bPressed=TRUE;
				PWMAudioOn(BUZZER1);
				u8UserPassword[u8Index]='3';
				u8Index++;
			}
		}
		
		if(bPressed==TRUE)  //let the LED turn for 10ms
		{
			u16Counter++;
				
			if(u16Counter==100)
			{
				u16Counter=0;
				LedOff(RED);
				PWMAudioOff(BUZZER1);
				bPressed=FALSE;
			}
		}
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
