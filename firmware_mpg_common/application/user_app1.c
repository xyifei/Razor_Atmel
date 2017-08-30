/**********************************************************************************************************************
File: user_app1.c                                                                

Description:
Provides a Tera-Term driven system to display, read and write an LED command list.

Test1.
Test2 from Engenuics.
Test3.

------------------------------------------------------------------------------------------------------------------------
API:

Public functions:
None.

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
extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */

extern u8 G_au8DebugScanfBuffer[DEBUG_SCANF_BUFFER_SIZE]; /* From debug.c */
extern u8 G_u8DebugScanfCharCount;                        /* From debug.c */


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
  u8 au8UserApp1Start1[] = "LED program task started\n\r";
  
  /* Turn off the Debug task command processor and announce the task is ready */
  DebugSetPassthrough();
  DebugPrintf(au8UserApp1Start1);
  
    /* If good initialization, set state to Idle */
  if( 1 )
  {
    UserApp1_StateMachine = UserApp1SM_Idle;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    UserApp1_StateMachine = UserApp1SM_FailedInit;
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

bool bJudge(u8 *au8Command,u8 u8CMDBit)
{
    bool bIsRight=TRUE;
    u8 u8EndTimeLogo=0;
    bool bContinueJudge=TRUE;
    u8 au8ValidLed[]={'W','P','B','C','G','Y','O','R','w','p','b','c','g','y','o','r'};
    u32 u32StartTime=0;
    u32 u32EndTime=0;
    u32 u32Mul=1;
    
    if(au8Command[1]=='-')//determine whether the commond is correct
    {
        for(u8 i=2;i<u8CMDBit;i++)
        {
            if(au8Command[i]=='-')
            {
                u8EndTimeLogo=i;
                bContinueJudge=TRUE;
                break;
            }
            else
            {
                if(i==(u8CMDBit-1))
                {
                    bIsRight=FALSE;
                    bContinueJudge=FALSE;
                }
            }
        }
        
        if((au8Command[2]=='-')||(au8Command[u8EndTimeLogo+1]=='-'))
        {
            bIsRight=FALSE;
        }
        
        if(bContinueJudge)
        {
            for(u8 i=0;i<sizeof(au8ValidLed);i++)
            {
                if(au8Command[0]==au8ValidLed[i])
                {
                    break;
                }
                else
                {
                    if(i==(sizeof(au8ValidLed)-1))
                    {
                        bIsRight=FALSE;
                    }
                }	
            }
            
            for(u8 i=2;i<u8EndTimeLogo;i++)
            {
                if(au8Command[i]<48&&au8Command[i]>57)
                {
                    bIsRight=FALSE;
                }
            }
            
            for(u8 i=u8EndTimeLogo+1;i<u8CMDBit;i++)
            {
                if(au8Command[i]<48&&au8Command[i]>57)
                {
                    bIsRight=FALSE;
                }
            }
            
            for(u8 i=u8EndTimeLogo-1;i>1;i--)
            {
                u32StartTime=u32StartTime+(au8Command[i]-48)*u32Mul;
                u32Mul=u32Mul*10;
            }
            
            u32Mul=1;
            
            for(u8 i=u8CMDBit-1;i>u8EndTimeLogo;i--)
            {
                u32EndTime=u32EndTime+(au8Command[i]-48)*u32Mul;
                u32Mul=u32Mul*10;
            }
            
            u32Mul=1;
            
            if(u32StartTime>=u32EndTime)
            {
                bIsRight=FALSE;
            }
            
            bContinueJudge=FALSE;
        }
        
    }
    else
    {
        bIsRight=FALSE;
    }
    
    return bIsRight;
}


/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for input */
static void UserApp1SM_Idle(void)
{
    static u8 au8Command[12];
	static u8 au8EnterIn[]={0};
	static u8 u8CMDBit=0;
	static u8 u8CMDNum=0;
	static u8 u8EndTimeLogo=0;
	static bool bWriteOrDisplay=TRUE;
	static bool bPrint=TRUE;
	static u8 u8OneOrTwo=0;
	static u32 u32Mul=1;
	static bool bIsRight=TRUE;
	static u32 u32Num=1;
	LedNumberType eLedNum[]={WHITE, PURPLE, BLUE, CYAN, GREEN, YELLOW, ORANGE, RED};
	u8 au8ValidLed[]={'W','P','B','C','G','Y','O','R','w','p','b','c','g','y','o','r'};
	LedCommandType aeUserList[100];
	static bool bIsInit=FALSE;
	
	if(!bIsInit)//Initialize the aeUserList
	{
	  	bIsInit=TRUE;
		
		for(u8 i=0;i<100;i++)
		{
			aeUserList[i].u32Time=0;
		}	
	}
	
	if(bPrint)
	{
		DebugLineFeed();
	  	DebugPrintf("****************************************************************");
		DebugLineFeed();
		DebugPrintf("LED Programing Interface");  
		DebugLineFeed();
		DebugPrintf("Press 1 to program LED command sequence");
		DebugLineFeed();
		DebugPrintf("Press 2 to show current USER program");
		DebugLineFeed();
		DebugPrintf("****************************************************************");
		DebugLineFeed();
		bPrint=FALSE;
	}

	if(bWriteOrDisplay)//determine whether to enterin the commonds or diaplay
	{
		if(G_u8DebugScanfCharCount>=1)
		{
			DebugScanf(au8EnterIn);
			
			if(au8EnterIn[0]=='1')
			{
				for(u8 i=0;i<=(2*u8CMDNum);i++)
                {
                    aeUserList[i].u32Time=0;
                }
              
                LedDisplayStartList();
                u8CMDNum=0;
                u8OneOrTwo=1;
				u32Num=1;
				DebugLineFeed();
				DebugLineFeed();
				DebugPrintf("Enter commands as LED-ONTIME-OFFTIME and press Enter");
				DebugLineFeed();
				DebugPrintf("Time is in milliseconds, max 100 commands");
				DebugLineFeed();
				DebugPrintf("LED colours: R, O, Y, G, C, B, P, W");
				DebugLineFeed();
				DebugPrintf("Example: R-100-200 (Red on at 100ms and off at 200ms)");
				DebugLineFeed();
				DebugPrintf("Press Enter on blank line to end");
				DebugLineFeed();
				DebugPrintNumber(u32Num);
				DebugPrintf(":");
			}
			
			if(au8EnterIn[0]=='2')
			{
				u8OneOrTwo=2;
			}
		}
	}
	
	if(u8OneOrTwo==1)//enter in commands	
	{
		bWriteOrDisplay=FALSE;
		
	  	if(G_u8DebugScanfCharCount>=1)//use Debug to enter in
		{
			DebugScanf(au8EnterIn);
		
			if(au8EnterIn[0]!='\r')
			{
				au8Command[u8CMDBit]=au8EnterIn[0];
				u8CMDBit++;
			}
			else                      //when one command is entered
			{
			  	DebugPrintNumber(u32Num);
				DebugPrintf(":");
				
			  	if(au8Command[0]==0)//whether the commond is empty
				{
					bWriteOrDisplay=TRUE;
					bPrint=TRUE;
					u8OneOrTwo=0;
				}
				else
				{     
                    if(bJudge(au8Command,u8CMDBit))
                    {
                        bIsRight=TRUE;
                    }
                    else
                    {
                        bIsRight=FALSE;
                    }
					
					if(bIsRight)//if commond is right, entering it to UserList
					{
						DebugPrintf(au8Command);
						
						for(u8 i=0;i<u8CMDBit;i++)
						{
							if(au8Command[i]=='-')
							{
								u8EndTimeLogo=i;
							}
						}
						
						for(u8 i=0;i<16;i++)
						{
							if((au8Command[0]==au8ValidLed[i])&&i<8)
							{
								aeUserList[2*u8CMDNum].eLED=eLedNum[i];
								aeUserList[2*u8CMDNum+1].eLED=eLedNum[i];
							}
							
							if((au8Command[0]==au8ValidLed[i])&&i>=8)
							{
								aeUserList[2*u8CMDNum].eLED=eLedNum[i%8];
								aeUserList[2*u8CMDNum+1].eLED=eLedNum[i%8];
							}
						}
						
						for(u8 i=u8EndTimeLogo-1;i>1;i--)
						{
							aeUserList[2*u8CMDNum].u32Time=aeUserList[2*u8CMDNum].u32Time+(au8Command[i]-48)*u32Mul;
							u32Mul=u32Mul*10;
						}
						
						u32Mul=1;
						
						for(u8 i=u8CMDBit-1;i>u8EndTimeLogo;i--)
						{
							aeUserList[2*u8CMDNum+1].u32Time=aeUserList[2*u8CMDNum+1].u32Time+(au8Command[i]-48)*u32Mul;
							u32Mul=u32Mul*10;
						}
						
						u32Mul=1;
						aeUserList[2*u8CMDNum].bOn=TRUE;
						aeUserList[2*u8CMDNum].eCurrentRate=LED_PWM_0;
						aeUserList[2*u8CMDNum+1].bOn=FALSE;
						aeUserList[2*u8CMDNum+1].eCurrentRate=LED_PWM_100;
						
						for(u8 i=0;i<u8CMDBit;i++)
						{
							au8Command[i]=0;
						}
						
						u8CMDBit=0;
						u8CMDNum++;
						u32Num++;
						DebugLineFeed(); 
						DebugPrintNumber(u32Num);
						DebugPrintf(":");
					}
					else//if the commond is wrong, enter in commond again
					{
						for(u8 i=0;i<u8CMDBit;i++)
						{
							au8Command[i]=0;
						}
						
						aeUserList[2*u8CMDNum].u32Time=0;
						aeUserList[2*u8CMDNum+1].u32Time=0;
						u8CMDBit=0;
						DebugLineFeed();
						DebugPrintf("Invalid command: incorrect format. Please use L-ONTIME-OFFTIME");
						DebugLineFeed();
						DebugPrintNumber(u32Num);
						DebugPrintf(":");
					}
				}
			}
		}
	}
	
	if(u8OneOrTwo==2)//when 2 is entered, display these commands	
	{
	  	bWriteOrDisplay=FALSE;
		u8OneOrTwo=0;
		DebugLineFeed();
		
		for(u8 i = 0; i < (2*u8CMDNum); i++)
		{
			LedDisplayAddCommand(USER_LIST, &aeUserList[i]);
		}
        
        DebugLineFeed();
        DebugPrintf("Current USER program");
        DebugLineFeed();
        DebugPrintf("LED  ON TIME  OFF TIME");
        DebugLineFeed();                   
        DebugPrintf("----------------------");
        DebugLineFeed();
        
        for(u8 i=0;i<u8CMDNum;i++)
        {
            LedDisplayPrintListLine(i);
        }
        
		bWriteOrDisplay=TRUE;
	}
} /* end UserApp1SM_Idle() */
                      
            
#if 0
/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
  
} /* end UserApp1SM_Error() */
#endif


/*-------------------------------------------------------------------------------------------------------------------*/
/* State to sit in if init failed */
static void UserApp1SM_FailedInit(void)          
{
    
} /* end UserApp1SM_FailedInit() */


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
