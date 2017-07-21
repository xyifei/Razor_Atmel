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
	LedOff(GREEN);
	LCDCommand(LCD_CLEAR_CMD);
 
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
  static u32 u32TimeCounter=0;
  static u16 u16DisplayCounter=0;
  static u8 au8Time[4];
  static u8 au8TimeChar[4];
  static u8 au8EnterIn[]={0};
  static u8 au8Message[]=" 0 0 0 0 0 0 0 0";
  LedNumberType eLedNum[8]={WHITE, PURPLE, BLUE, CYAN, GREEN, YELLOW, ORANGE, RED};
  LedCommandType aeDemoList[]=
  {
	{RED,1000,TRUE,LED_PWM_100},
	{RED,6000,FALSE,LED_PWM_0},
	{GREEN,3000,TRUE,LED_PWM_100},
	{GREEN,9000,FALSE,LED_PWM_0},
	{YELLOW,1000,TRUE,LED_PWM_100},
	{YELLOW,7000,FALSE,LED_PWM_0}
  };
  
  
  DebugScanf(au8EnterIn);
  
  if(au8EnterIn[0]=='2')
  {
	if(IsButtonPressed(BUTTON1) )
	{
	  u32TimeCounter++;
	  u16DisplayCounter++;	
	 
	  if(u32TimeCounter==10000)//10s一个周期
	  {
		u32TimeCounter=0;
	  }
	  
	  for(u8 i=0;i<6;i++)//有六种亮灯的情况，逐一判断
	  {
		if(u32TimeCounter==aeDemoList[i].u32Time)
		{
		  LedPWM(aeDemoList[i].eLed,aeDemoList[i].eCurrentRate);//什么时候点亮什么灯
		  
		  for(u8 k=0;k<8;k++)
		  {
			if(aeDemoList[i].eLed==eLedNum[k])//判断哪一个灯亮，LCD上哪一个数字变化
			{
			  if(aeDemoList[i].bOn)//判断该数字是变为一还是变为零
			  {
				au8Message[2*k+1]='1';
			  }
			  else
			  {
				au8Message[2*k+1]='0';
			  }
			}
		  }
		  LCDMessage(LINE1_START_ADDR,au8Message );//在LCD上显示哪个灯亮
		  LCDClearChars(LINE1_START_ADDR + 16, 4);
		}
	  }
	  
	  if(u16DisplayCounter==100)//每过100ms显示的时间变化一次
	  {
		au8Time[0]=u32TimeCounter/1000;
		au8Time[1]=u32TimeCounter/100-au8Time[0]*10;
		au8Time[2]=u32TimeCounter/10-au8Time[0]*100-au8Time[1]*10;//把要显示的时间的每一位提取出来
		au8Time[3]=u32TimeCounter%10;
		for(u8 j=0;j<4;j++)
		{
		  au8TimeChar[j]=au8Time[j]+48;//把时间的每一位都转化位字符
		}
		LCDMessage(LINE2_START_ADDR + 8, au8TimeChar);//在第二行中间显示时间
		LCDClearChars(LINE2_START_ADDR + 12, 8);
		LCDClearChars(LINE2_START_ADDR, 8);
		u16DisplayCounter=0;
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
