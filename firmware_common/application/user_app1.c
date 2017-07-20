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
	LCDCommand(LCD_CLEAR_CMD);
	LedOff(WHITE);
	LedOff(PURPLE);
	LedOff(BLUE);
	LedOff(CYAN);
	LedOff(GREEN);
	LedOff(YELLOW);
	LedOff(ORANGE);
	LedOff(RED);
 
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
	static u8 au8Message[255];//用于LCD输出
	static u8 au8PrintOut[255];//保存Debug输入的每个字符
	static u8 au8EnterIn[255];
	LedNumberType au8LedNumber[]={WHITE, PURPLE, BLUE, CYAN, GREEN, YELLOW, ORANGE, RED};
	static u16 u16TimeCounter=0;
	u8 u8Index=0;
	static u8 u8Index2=0;
	static u8 u8Index3=0;
	static u8 u8PrintCounter=0;
		
	if(G_u8DebugScanfCharCount>=1)//每次输入一个字符都将它保存在au8PrintOut中
 	{
 		DebugScanf(au8EnterIn);
 		au8PrintOut[u8Index2]=au8EnterIn[0];
		u8Index2++;
	}
	
	if(au8PrintOut[u8Index2-1]=='\r')//如果输入的字符是回车，即按下了确认键
	{
	  	u16TimeCounter++;
		
		if(u8Index2<20)
		{
		  LCDMessage(LINE1_START_ADDR,au8PrintOut);
		}
		else
		{
			if(u16TimeCounter==500)
			{
				for(u8Index=0;u8Index<20;u8Index++)//把au8PrintOut中的字符给au8Message,每过0.5s就左移一个
				{
					if(u8Index<=(19-u8PrintCounter))
					{
						au8Message[u8Index]=32;//屏幕左边没有字符的单元给（speace）
					}
					else
					{
						au8Message[u8Index]=au8PrintOut[u8Index-(19-u8PrintCounter)-1];
					}
				}
				
				if(au8Message[0]==au8PrintOut[u8Index2-1])//当要输入的字符最后一个到屏幕最开始时重置
				{
					u8PrintCounter=0;
				}
				else
				{
					LCDMessage(LINE1_START_ADDR, au8Message);
				}
				
				if(u8Index3<8)//每过0.5s就往后亮一个LED，直到八个都亮了后重置
				{
					LedOn(au8LedNumber[u8Index3]);
				}
				else
				{
					u8Index3=0;
					LedOff(PURPLE);
					LedOff(BLUE);
					LedOff(CYAN);
					LedOff(GREEN);
					LedOff(YELLOW);
					LedOff(ORANGE);
					LedOff(RED);
				}
				
				u16TimeCounter=0;
				u8PrintCounter++;
				u8Index3++;
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
