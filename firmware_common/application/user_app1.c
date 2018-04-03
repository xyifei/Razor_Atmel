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
    AT91C_BASE_PIOA->PIO_PER = PIOA_PER_ADD_INIT;
    AT91C_BASE_PIOA->PIO_PDR = PIOA_PDR_ADD_INIT;
    AT91C_BASE_PIOA->PIO_OER = PIOA_OER_ADD_INIT;
    AT91C_BASE_PIOA->PIO_ODR = PIOA_ODR_ADD_INIT;
    AT91C_BASE_PIOB->PIO_PER = PIOB_PER_ADD_INIT;
    AT91C_BASE_PIOB->PIO_PDR = PIOB_PDR_ADD_INIT;
    AT91C_BASE_PIOB->PIO_OER = PIOB_OER_ADD_INIT;
    AT91C_BASE_PIOB->PIO_ODR = PIOB_ODR_ADD_INIT;
    AT91C_BASE_PIOA->PIO_SODR = PIOA_SODR_ADD_INIT;
    AT91C_BASE_PIOB->PIO_CODR = PIOB_RE;
    
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
static void Volume_adjusting(void)
{
    if(WasButtonPressed(BUTTON0))
    {
        ButtonAcknowledge(BUTTON0);
        AT91C_BASE_PIOA->PIO_SODR = Volume_Up_SODR;
        AT91C_BASE_PIOA->PIO_CODR = Volume_Up_CODR;
        LedOn(RED);
        
        for(u8 i=0;i<20;i++) 
        {
            AT91C_BASE_PIOA->PIO_CODR = PIOA_INC;
            
            for(u8 i=0;i<=5;i++)
            {
                for(u8 i=0;i<48;i++);
            }
            
            AT91C_BASE_PIOA->PIO_SODR = PIOA_INC;
        }
    }
    
    if(WasButtonPressed(BUTTON1))
    {
        ButtonAcknowledge(BUTTON1);
        AT91C_BASE_PIOA->PIO_SODR = Volume_Down_SODR;
        AT91C_BASE_PIOA->PIO_CODR = Volume_Down_CODR;
        LedOn(RED);
        
        for(u8 i=0;i<20;i++) 
        {
            AT91C_BASE_PIOA->PIO_CODR = PIOA_INC;
            
            for(u8 i=0;i<=5;i++)
            {
                for(u8 i=0;i<48;i++);
            }
            
            AT91C_BASE_PIOA->PIO_SODR = PIOA_INC;
        }
    }
}

/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for ??? */
static void UserApp1SM_Idle(void)
{
    static u8 u8Mode=0;
    static float fData=0;
    static bool bTest=FALSE;
    
    LedOff(RED);
    
    if(WasButtonPressed(BUTTON3))
    {
        ButtonAcknowledge(BUTTON3);
        LedOn(RED);
        
        if(u8Mode == 2)
        {
            u8Mode = 0;
        }
        else
        {
            u8Mode++;
        }
        
        switch(u8Mode)
        {
            case 0:
                LCDClearChars(LINE1_START_ADDR,20);
                LCDMessage(LINE1_START_ADDR, "MUTE");
                break;
            case 1:
                LCDClearChars(LINE1_START_ADDR,20);
                LCDMessage(LINE1_START_ADDR, "MIC");
                break;
            case 2:
                LCDClearChars(LINE1_START_ADDR,20);
                LCDMessage(LINE1_START_ADDR, "PHONE");
        }
    }
    
    if(u8Mode == 0)
    {
        AT91C_BASE_PIOA->PIO_SODR = Mute_SODR;
        AT91C_BASE_PIOA->PIO_CODR = Mute_CODR;
        LedOn(PURPLE);
        LedOff(BLUE);
        LedOff(GREEN);
    }
    
    if(u8Mode == 1)
    {
        AT91C_BASE_PIOA->PIO_SODR = AUD1_SODR;
        AT91C_BASE_PIOA->PIO_CODR = AUD1_CODR;
        LedOn(BLUE);
        LedOff(GREEN);
        LedOff(PURPLE);
        Volume_adjusting();      
    }
    
    if(u8Mode == 2)
    {
        AT91C_BASE_PIOA->PIO_SODR = AUD2_SODR;
        AT91C_BASE_PIOA->PIO_CODR = AUD2_CODR;
        LedOn(GREEN);
        LedOff(BLUE);
        LedOff(PURPLE);
        Volume_adjusting();    
    }
    
    if(WasButtonPressed(BUTTON2))
    {
        ButtonAcknowledge(BUTTON2);
        LedOn(RED);
        AT91C_BASE_PIOA->PIO_SODR = PIOA_CY;
        bTest = TRUE;      
    }  
    
    if(bTest)
    {
        AT91C_BASE_PIOB->PIO_SODR = PIOB_RE;
        LedOn(WHITE);
  
        if(Adc12StartConversion(ADC12_CH2))
        {
            for(u8 i=0;i<=200;i++)
            {
                for(u8 i=0;i<48;i++);
            }
            
            fData=3.3/4096*AT91C_BASE_ADC12B->ADC12B_CDR[2];
        }
        
        bTest = FALSE;
    }
    else
    {
         LedOff(WHITE);
         AT91C_BASE_PIOB->PIO_CODR = PIOB_RE;
         AT91C_BASE_PIOA->PIO_CODR = PIOA_CY;
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
