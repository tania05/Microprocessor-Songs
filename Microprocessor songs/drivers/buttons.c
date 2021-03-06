/***********************************************************************************************************************
File: buttons.c                                                                

Description:
Button functions and state machine.  The application handles all debouncing and glitch filtering.

------------------------------------------------------------------------------------------------------------------------
API:
Types:
The argument u32Button_ is either BUTTON0 or BUTTON1.  

Public:
bool IsButtonPressed(u32 u32Button_)
Returns TRUE if Button_aeCurrentState[eButton_] is currently pressed (and debounced).

bool WasButtonPressed(u32 u32Button_)
Returns TRUE if a particular button was pressed since last time it was checked even if it is no longer pressed.
ButtonAcknowledge is typically called immediately after WasButtonPressed() returns TRUE to clear the button
pressed state.

void ButtonAcknowledge(u32 u32Button_)
Clears the New Press state of a button -- generally always called after WasButtonPressed() returns TRUE.

bool IsButtonHeld(u32 u32Button_, u32 u32ButtonHeldTime_)
Returns TRUE if a button has been held for u32ButtonHeldTime_ time in milliseconds.

Protected:
void ButtonInitialize(void)
Configures the button system for the product including enabling button GPIO interrupts.  

u32 GetButtonBitLocation(u8 u8Button_, ButtonPortType ePort_)
Returns the location of the button within its port (should be required only for interrupt service routines).  

DISCLAIMER: THIS CODE IS PROVIDED WITHOUT ANY WARRANTY OR GUARANTEES.  USERS MAY
USE THIS CODE FOR DEVELOPMENT AND EXAMPLE PURPOSES ONLY.  ENGENUICS TECHNOLOGIES
INCORPORATED IS NOT RESPONSIBLE FOR ANY ERRORS, OMISSIONS, OR DAMAGES THAT COULD
RESULT FROM USING THIS FIRMWARE IN WHOLE OR IN PART.

***********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_"
***********************************************************************************************************************/
/* New variables */
volatile fnCode_type G_ButtonStateMachine;                       /* The Button application state machine */

volatile bool G_abButtonDebounceActive[TOTAL_BUTTONS];           /* Flags for buttons being debounced */
volatile u32 G_au32ButtonDebounceTimeStart[TOTAL_BUTTONS];       /* Button debounce start time */

/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemTime1ms;        /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;         /* From board-specific source file */

extern volatile u32 G_u32SystemFlags;          /* From main.c */
extern volatile u32 G_u32ApplicationFlags;     /* From main.c */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "Button_" and be declared as static.
***********************************************************************************************************************/
static u32 Button_u32Timer;                                 /* Counter used across states */

static ButtonStateType Button_aeCurrentState[TOTAL_BUTTONS];/* Current pressed state of button */
static ButtonStateType Button_aeNewState[TOTAL_BUTTONS];    /* New (pending) pressed state of button */
static u32 Button_au32HoldTimeStart[TOTAL_BUTTONS];         /* System 1ms time when a button press started */
static bool Button_abNewPress[TOTAL_BUTTONS];               /* Flags to indicate a button was pressed */    


/************ %BUTTON% EDIT BOARD-SPECIFIC GPIO DEFINITIONS BELOW ***************/
/* Add all of the GPIO pin names for the buttons in the system.  
The order of the definitions below must match the order of the definitions provided in configuration.h */ 

#ifdef MPGL1
static const u32 Button_au32ButtonPins[TOTAL_BUTTONS] = 
{
  PA_17_BUTTON0, PB_00_BUTTON1, PB_01_BUTTON2, PB_02_BUTTON3
};

/* Control array for all buttons in system initialized for ButtonInitialize().  Array values correspond to ButtonConfigType fields: 
     eActiveState       ePort                   */
static ButtonConfigType Buttons_asArray[TOTAL_BUTTONS] = 
{{BUTTON_ACTIVE_LOW, BUTTON_PORTA}, /* BUTTON0  */
 {BUTTON_ACTIVE_LOW, BUTTON_PORTB}, /* BUTTON1  */
 {BUTTON_ACTIVE_LOW, BUTTON_PORTB}, /* BUTTON2  */
 {BUTTON_ACTIVE_LOW, BUTTON_PORTB}, /* BUTTON3  */
};   
#endif /* MPGL1 */

#ifdef MPGL2
static const u32 Button_au32ButtonPins[TOTAL_BUTTONS] = 
{
  PA_17_BUTTON0, PB_00_BUTTON1
};

/* Control array for all buttons in system initialized for ButtonInitialize().  Array values correspond to ButtonConfigType fields: 
     eActiveState       ePort                   */
static ButtonConfigType Buttons_asArray[TOTAL_BUTTONS] = 
{{BUTTON_ACTIVE_LOW, BUTTON_PORTA}, /* BUTTON0  */
 {BUTTON_ACTIVE_LOW, BUTTON_PORTB} /* BUTTON1  */
};   
#endif /* MPGL2 */

/************ EDIT BOARD-SPECIFIC GPIO DEFINITIONS ABOVE ***************/


/***********************************************************************************************************************
Function Definitions
***********************************************************************************************************************/
/*--------------------------------------------------------------------------------------------------------------------*/
/* Public Functions */
/*--------------------------------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------------------------------
Function: IsButtonPressed

Description:
Determine if a particular button is currently pressed at this moment in time.
The button must still be pressed at the time of this inquiry for the function
to return TRUE.

Requires:
  - u32Button_ is a valid button index
  - Button_aeCurrentState[u32Button_] is valid
 
Promises:
  - Returns TRUE if Button_aeCurrentState[eButton_] is pressed; otherwise returns FALSE
*/
bool IsButtonPressed(u32 u32Button_)
{
  if( Button_aeCurrentState[u32Button_] == PRESSED)
  {
    return(TRUE);
  }
  else
  {
    return(FALSE);
  }

} /* end IsButtonPressed() */


/*----------------------------------------------------------------------------------------------------------------------
Function: WasButtonPressed

Description:
Determines if a particular button was pressed since last time it was checked. 
The button may or may not still be pressed when this inquiry is made.  Mulitple
button presses are not tracked.  The user should call ButtonAcknowledge immediately
following this function to clear the state.

Requires:
  - u32 u32Button_ is a valid button index
  - Button_aeCurrentState[u32Button_] is valid
 
Promises:
  - Returns TRUE if Button_abNewPress[u32Button_] is TRUE; other wise returns FALSE
*/
bool WasButtonPressed(u32 u32Button_)
{
  if( Button_abNewPress[u32Button_] == TRUE)
  {
    return(TRUE);
  }
  else
  {
    return(FALSE);
  }

} /* end WasButtonPressed() */


/*----------------------------------------------------------------------------------------------------------------------
Function: ButtonAcknowledge

Description:
Clears the New Press state of a button.

Requires:
  - u32Button_ is a valid button index
 
Promises:
  - The flag at Button_abNewPress[eButton_] is set to FALSE
*/
void ButtonAcknowledge(u32 u32Button_)
{
  Button_abNewPress[u32Button_] = FALSE;

} /* end ButtonAcknowledge() */


/*----------------------------------------------------------------------------------------------------------------------
Function: IsButtonHeld

Description:
Queries to see if a button has been held for a certain time.  The button
must still be pressed when this function is called if it is to return TRUE.

Requires:
  - u32Button_ is a valid button index
  - u32ButtonHeldTime is a time in ms 
 
Promises:
  - Returns TRUE if eButton_ has been held longer than u32ButtonHeldTime_
*/
bool IsButtonHeld(u32 u32Button_, u32 u32ButtonHeldTime_)
{
 if( IsButtonPressed(u32Button_) && 
     IsTimeUp(&Button_au32HoldTimeStart[u32Button_], u32ButtonHeldTime_ ) )
 {
   return(TRUE);
 }
 else
 {
   return(FALSE);
 }

} /* end IsButtonHeld() */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Protected Functions */
/*--------------------------------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------------------------------
Function: ButtonInitialize

Description:
Configures the button system for the product including enabling button GPIO interrupts.  

Requires:
  - GPIO configuration is already complete for all button inputs
  - Button interrupt initializations and handler functions are ready
 
Promises:
  - G_abButtonDebounceActive, LGaeButtonPreviousState and Button_aeCurrentState 
    are intialized
  - GGstButtonTrackballPosition fields are all initialized to default values
  - The button state machine is initialized to Idle
*/
void ButtonInitialize(void)
{
  u32 u32PortAInterruptMask = 0;
  u32 u32PortBInterruptMask = 0;
  u8* pu8Parser;
  u8 au8ButtonStartupMsg[] = "Button task ready\n\r";
  
  /* Setup default data for all of the buttons in the system */
  for(u8 i = 0; i < TOTAL_BUTTONS; i++)
  {
    G_abButtonDebounceActive[i] = FALSE;
    Button_aeCurrentState[i]    = RELEASED;
    Button_aeNewState[i]        = RELEASED;
  }
  
  /* Create masks based on any buttons in the system.  It's ok to have an empty mask. */
  for(u8 i = 0; i < TOTAL_BUTTONS; i++)
  {
    if(Buttons_asArray[i].ePort == BUTTON_PORTA)
    {
      u32PortAInterruptMask |= Button_au32ButtonPins[i];
    }
    else if(Buttons_asArray[i].ePort == BUTTON_PORTB)
    {
      u32PortBInterruptMask |= Button_au32ButtonPins[i];
    }
  }

  /* Enable PIO interrupts */
  AT91C_BASE_PIOA->PIO_IER = u32PortAInterruptMask;
  AT91C_BASE_PIOB->PIO_IER = u32PortBInterruptMask;
  
  /* Read the ISR register to clear all the current flags */
  u32PortAInterruptMask = AT91C_BASE_PIOA->PIO_ISR;
  u32PortBInterruptMask = AT91C_BASE_PIOB->PIO_ISR;

  /* Configure the NVIC to ensure the PIOA and PIOB interrupts are active */
  NVIC_ClearPendingIRQ(IRQn_PIOA);
  NVIC_ClearPendingIRQ(IRQn_PIOB);
  NVIC_EnableIRQ(IRQn_PIOA);
  NVIC_EnableIRQ(IRQn_PIOB);
    
  /* Init complete: set function pointer and application flag */
  Button_u32Timer = G_u32SystemTime1ms;
  pu8Parser = &au8ButtonStartupMsg[0];
  while(*pu8Parser != NULL)
  {
    /* Attempt to queue the character */
    if( Uart_putc(*pu8Parser) )
    {
      /* Advance only if character has been sent */
      pu8Parser++;
    }
       
    /* Watch for timeout */
    if( IsTimeUp(&Button_u32Timer, BUTTON_INIT_MSG_TIMEOUT) )
    {
      break;
    }
  }

  G_ButtonStateMachine = ButtonSM_Idle;
  G_u32ApplicationFlags |= _APPLICATION_FLAGS_BUTTON;

} /* end ButtonInitialize() */


/*----------------------------------------------------------------------------------------------------------------------
Function: GetButtonBitLocation

Description:
Returns the location of the button within its port.  
The GPIO interrupt requires access to this function.

Requires:
  - u8Button_ is a valid ButtonNumberType.
  - ePort_ is the port where the button is located

Promises:
  - Returns a value that has a bit set in the corresponding position of u32Button_ on the button's port
  - Returns 0 if no match
*/
u32 GetButtonBitLocation(u8 u8Button_, ButtonPortType ePort_)
{
  /* Make sure the index is valid */
  if(u8Button_ < TOTAL_BUTTONS) 
  {
    /* Index is valid so check that the button exists on the port */
    if(Buttons_asArray[u8Button_].ePort == ePort_)
    {
      /* Return the button position if the index is the correct port */
      return(Button_au32ButtonPins[u8Button_]);
    }
  }
  
  /* Otherwise return 0 */
  return(0);
  
} /* end GetButtonBitLocation() */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions */
/*--------------------------------------------------------------------------------------------------------------------*/


/***********************************************************************************************************************
State Machine Function Definitions

The button state machine monitors button activity and manages debouncing and
maintaining the global button states.
***********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Do nothing but wait for a debounce time to start */
static void ButtonSM_Idle(void)                
{
  for(u8 i = 0; i < TOTAL_BUTTONS; i++)
  {
    if(G_abButtonDebounceActive[i])
    {
      G_ButtonStateMachine = ButtonSM_ButtonActive;
    }
  }
  
} /* end ButtonSM_Idle(void) */


/*--------------------------------------------------------------------------------------------------------------------*/
static void ButtonSM_ButtonActive(void)         
{
  u32 *pu32PortAddress;
  u32 *pu32InterruptAddress;

  /* Start by resseting back to Idle in case no buttons are active */
  G_ButtonStateMachine = ButtonSM_Idle;

  /* Check for buttons that are debouncing */
  for(u8 i = 0; i < TOTAL_BUTTONS; i++)
  {
    /* Load address offsets for the current button */
    pu32PortAddress = (u32*)(&(AT91C_BASE_PIOA->PIO_PDSR) + Buttons_asArray[i].ePort);
    pu32InterruptAddress = (u32*)(&(AT91C_BASE_PIOA->PIO_IER) + Buttons_asArray[i].ePort);
    
    if( G_abButtonDebounceActive[i] )
    {
      /* Still have an active button */
      G_ButtonStateMachine = ButtonSM_ButtonActive;
      
      if( IsTimeUp((u32*)&G_au32ButtonDebounceTimeStart[i], BUTTON_DEBOUNCE_TIME) )
      {
        /* Active low: get current state of button */
        if(Buttons_asArray[i].eActiveState == BUTTON_ACTIVE_LOW)
        {
          if( ~(*pu32PortAddress) & Button_au32ButtonPins[i] )
          {          
            Button_aeNewState[i] = PRESSED;
          }
          else
          {
            Button_aeNewState[i] = RELEASED;
          }
        }
        /* Active high */
        else
        {
          if( *pu32PortAddress & Button_au32ButtonPins[i] )
          {          
            Button_aeNewState[i] = PRESSED;
          }
          else
          {
            Button_aeNewState[i] = RELEASED;
          }
        }
        
        /* Update if the button state has changed */
        if( Button_aeNewState[i] != Button_aeCurrentState[i] )
        {
          Button_aeCurrentState[i] = Button_aeNewState[i];
          if(Button_aeCurrentState[i] == PRESSED)
          {
            Button_abNewPress[i] = TRUE;
            Button_au32HoldTimeStart[i] = G_u32SystemTime1ms;
          }
        }

        /* Regardless of a good press or not, clear the debounce active flag and re-enable the interrupts */
        G_abButtonDebounceActive[i] = FALSE;
        *pu32InterruptAddress |= Button_au32ButtonPins[i];
        
      } /* end if( IsTimeUp...) */
    } /* end if(G_abButtonDebounceActive[index]) */
  } /* end for i */
  
} /* end ButtonSM_ButtonActive() */



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File */
/*--------------------------------------------------------------------------------------------------------------------*/
