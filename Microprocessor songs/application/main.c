/***********************************************************************************************************************
File: main.c                                                                

Description:
Container for the MPG firmware.  
***********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32SystemFlags = 0;                     /* Global system flags */
volatile u32 G_u32ApplicationFlags = 0;                /* Global applications flags: set when application is successfully initialized */

/*--------------------------------------------------------------------------------------------------------------------*/
/* External global variables defined in other files (must indicate which file they are defined in) */
extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */

extern volatile fnCode_type G_ButtonStateMachine;      /* From buttons.c           */
extern volatile fnCode_type G_UartStateMachine;        /* From sam3u_uart.c        */

extern volatile fnCode_type G_AudioTestStateMachine;   /* From mpgl1_audio_test */
extern volatile fnCode_type G_MpgSongStateMachine;     /* From mpgl1_song.c */

/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "Main_" and be declared as static.
***********************************************************************************************************************/


/***********************************************************************************************************************
Main Program
Main has two sections:

1. Initialization which is run once on power-up or reset.  All drivers and applications are setup here without timing
contraints but must complete execution regardless of success or failure of starting the application. 

2. Super loop which runs infinitely giving processor time to each application.  The total loop time should not exceed
1ms of execution time counting all application execution.  SystemSleep() will execute to complete the remaining time in
the 1ms period.
***********************************************************************************************************************/

void main(void)
{
  G_u32SystemFlags |= _SYSTEM_INITIALIZING;
  // Check for watch dog restarts

  /* Low level initialization */
  WatchDogSetup(); /* During development, set to not reset processor if timeout */
  GpioSetup();
  ClockSetup();
  InterruptSetup();
  SysTickSetup();

  /* Driver initialization */
  UartInitialize();
  //LedInitialize();
  ButtonInitialize();

  /* Application initialization */
  AudioTestInitialize();
  MpgSongInitialize();
  
  /* Exit initialization */
  G_u32SystemFlags &= ~_SYSTEM_INITIALIZING;
  
  /* Super loop */  
  while(1)
  {
    WATCHDOG_BONE();
    
    /* Drivers */
    LedUpdate();
    G_ButtonStateMachine();
    G_UartStateMachine();
   
    /* Applications */
   // G_AudioTestStateMachine();
    G_MpgSongStateMachine();
       
    /* System sleep*/
    AT91C_BASE_PIOA->PIO_SODR = PA_31_HEARTBEAT;
    SystemSleep();
    AT91C_BASE_PIOA->PIO_CODR = PA_31_HEARTBEAT;
    
  } /* end while(1) main super loop */
  
} /* end main() */


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File */
/*--------------------------------------------------------------------------------------------------------------------*/
