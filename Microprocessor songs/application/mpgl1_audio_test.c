/**********************************************************************************************************************
File: mpgl1_audio_test.c                                                                

Description:
Tests the buzzers on the MPG Level 1 Atmel MPG board.
**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_<type>AudioTest"
***********************************************************************************************************************/
/* New variables */
volatile fnCode_type G_AudioTestStateMachine;           /* The state machine function pointer */
volatile u32 G_u32AudioTestFlags;                       /* Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "AudioTest_" and be declared as static.
***********************************************************************************************************************/

static u32 AudioTest_u32Timeout;                      /* Timeout counter used across states */


/***********************************************************************************************************************
Function Definitions
***********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Public functions                                                                                                   */
/*--------------------------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------------------*/
/* Protected functions                                                                                                */
/*--------------------------------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------------------------------
Function: AudioTestInitialize

Description:
Initializes the State Machine and its variables.

Requires:
  -

Promises:
  - 
*/
void AudioTestInitialize(void)
{
  u8* pu8Parser;
  u8 au8AudioTestStartupMsg[] = "Audio test task started\n\r";

  /* Starting LEDs */
  LedOn(WHITE);
  LedOff(PURPLE);
  LedOff(ORANGE);
  LedOff(CYAN);
  LedOff(BLUE);
  LedOff(GREEN);
  LedOff(YELLOW);
  LedOff(RED);
  LedOff(LCD_RED);
  LedOff(LCD_GREEN);
  LedOff(LCD_BLUE);
  
  /* Starting frequencies */
  PWMAudioSetFrequency(AT91C_PWMC_CHID0, NOTE_C4);
  PWMAudioSetFrequency(AT91C_PWMC_CHID1, NOTE_E4);

  /* Init complete: set function pointer and application flag */
  AudioTest_u32Timeout = G_u32SystemTime1ms;
  pu8Parser = &au8AudioTestStartupMsg[0];
  while(*pu8Parser != NULL)
  {
    /* Attempt to queue the character */
    if( Uart_putc(*pu8Parser) )
    {
      /* Advance only if character has been sent */
      pu8Parser++;
    }
       
    /* Watch for timeout */
    if( IsTimeUp(&AudioTest_u32Timeout, UTEST_INIT_MSG_TIMEOUT) )
    {
      break;
    }
  }
  
  AudioTest_u32Timeout = G_u32SystemTime1ms;
  G_AudioTestStateMachine = AudioTestSM_Idle;

} /* end AudioTestInitialize() */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/


/***********************************************************************************************************************
State Machine Function Definitions
***********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
void AudioTestSM_Idle(void)
{  
  /* Activate BUZZER1 when BUTTON0 is pressed */
  if( WasButtonPressed(BUTTON0) )
  {
    ButtonAcknowledge(BUTTON0);

    /* Turn on BUZZER1 and indicator LED */
    PWMAudioOn(BUZZER1);
    LedOn(BLUE);
    LedOn(LCD_BLUE);
  }
  
  if(!IsButtonPressed(BUTTON0))
  {
    /* Turn off BUZZER1 */
    PWMAudioOff(BUZZER1);
    
    /* Make sure LED is off */
    LedOff(BLUE);
    LedOff(LCD_BLUE);
  }

  /* Activate BUZZER0 when BUTTON3 is pressed */
  if( WasButtonPressed(BUTTON3) )
  {
    ButtonAcknowledge(BUTTON3);

    /* Turn on BUZZER0 and indicator LED */
    PWMAudioOn(BUZZER0);
    LedOn(RED);
    LedOn(LCD_RED);
  }
  
  if(!IsButtonPressed(BUTTON3))
  {
    /* Turn off BUZZER0 */
    PWMAudioOff(BUZZER0);
    
    /* Make sure LED is off */
    LedOff(RED);
    LedOff(LCD_RED);
  }
  
} /* end AudioTestSM_Idle() */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
void AudioTestSM_Error(void)          
{
  G_AudioTestStateMachine = AudioTestSM_Idle;
  
} /* end AudioTestSM_Error() */



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
