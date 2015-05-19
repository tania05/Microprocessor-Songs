/**********************************************************************************************************************
File: mpgl1_song.c                                                                

Description:
Source code for an application that plays a song.
**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_"
***********************************************************************************************************************/
/* New variables */
volatile fnCode_type G_MpgSongStateMachine;            /* The state machine function pointer */
volatile u32 G_u32MpgSongFlags;                        /* Global state flags */

static u32 G_Timer;

u16 MEASURE_TIME;
u16 FULL_NOTE;      
u16 HALF_NOTE;       
u16 QUARTER_NOTE;     
u16 EIGHTH_NOTE;       
u16 SIXTEENTH_NOTE;

u16 FN;
u16 HN;
u16 QN;
u16 EN;
u16 SN;

/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "MpgSong_" and be declared as static.
***********************************************************************************************************************/

static u32 MpgSong_u32Timeout;                      /* Timeout counter used across states */

  
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
Function: MpgSongInitialize

Description:
Initializes the State Machine and its variables.

Requires:
  -

Promises:
  - 
   
*/
void MpgSongInitialize(void)
{
  
    G_MpgSongStateMachine = MpgSongIdle;
  
 MEASURE_TIME  =           1200 ; /* Time in ms for 1 measure (1 full note) - should be divisible by 8*/
 FULL_NOTE      =         (MEASURE_TIME);
 HALF_NOTE       =        (MEASURE_TIME / 2);
 QUARTER_NOTE     =       (MEASURE_TIME / 4);
 EIGHTH_NOTE       =      (MEASURE_TIME / 8);
SIXTEENTH_NOTE     =     (MEASURE_TIME / 16);

 FN                    =    FULL_NOTE;                 
 HN                     =   HALF_NOTE ;                
 QN                      =  QUARTER_NOTE;              
 EN                       = EIGHTH_NOTE;               
 SN                        =SIXTEENTH_NOTE;            

  
} /* end MpgSongInitialize() */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/


/**********************************************************************************************************************
State Machine Function Definitions   ,,
**********************************************************************************************************************/

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for a message to be queued */


void MpgSongIdle(void)
{
FULL_NOTE      =         (MEASURE_TIME);
 HALF_NOTE       =        (MEASURE_TIME / 2);
 QUARTER_NOTE     =       (MEASURE_TIME / 4);
 EIGHTH_NOTE       =      (MEASURE_TIME / 8);
SIXTEENTH_NOTE     =     (MEASURE_TIME / 16);

 FN                    =    FULL_NOTE;                 
 HN                     =   HALF_NOTE ;                
 QN                      =  QUARTER_NOTE;              
 EN                       = EIGHTH_NOTE;               
 SN                        =SIXTEENTH_NOTE;            

 
 
 u16 noteSequence[] ={NO,B3,NO,G4,NO,G4,NO,D4,NO,C4,NO,G4,NO,NO,G4,NO,D5,NO,A4S,NO,G4,NO,B3,NO,G4,NO,G4,NO,D4,NO,C4,NO,G4,NO,NO,G4,NO,D5,NO,A4S,NO,G4,NO,B3,NO,G4,NO,G4,NO,D4,NO,C4,NO,G4,NO,NO,G4,NO,D5,NO,A4S,NO,G4,NO,B3,NO,G4,NO,G4,NO,D4,NO,C4,NO,G4,NO,NO,G4,NO,D5,NO,A4S,NO,G4,NO,G4,NO,    G4,NO,A4,NO, D4,NO,  C4S,NO, NO,NO};
 u16 noteTimes[]= {FN,   EN,EN,SN,SN,SN,SN,EN,EN,EN,EN,EN,EN,HT,SN,SN,SN,SN,EN, EN,EN,EN,EN,EN,SN,SN,SN,SN,EN,EN,EN,EN,EN,EN,HT,SN,SN,SN,SN, EN,EN,EN,EN,EN,EN,SN,SN,SN,SN,EN,EN,EN,EN,EN,EN,HT,SN,SN,SN,SN, EN,EN,EN,EN,EN,EN,SN,SN,SN,SN,EN,EN,EN,EN,EN,EN,HT,SN,SN,SN,SN, EN,EN,EN,EN,QN*3,QN*3,EN,EN,EN,EN, FN,FN,  FN,FN, FN,FN,};
static int u8Index = 0;

 u16 noteSequence2[]={NO,NO,D5,NO,C5,NO,D5,NO,C5,NO,A4S,NO,G4,NO,G4,NO,G4,NO,G4,NO,NO,NO, D5,NO, D5,NO, C5,NO, D5,NO, C5,NO, A4S,NO, D5,NO, C5,NO, NO,NO, D5,NO, NO,NO, D5,NO, NO,NO, D5,NO,  C5,NO, NO,NO, D5,NO,  D5,NO, C5,NO, D5 ,NO    ,C5,NO,  A4S,NO, NO,NO, D5,NO, NO,NO, D5,NO, NO,NO, C5,NO, A4S,NO, G4,NO, G4,NO, NO,NO, NO,NO, NO,NO };
 u16 noteTimes2[]={FN,FN*8,EN,EN,SN,SN,SN,SN,EN,EN,EN,EN,QN,QN,EN,EN,EN,EN,EN,EN,SN,SN,   SN,SN, SN,SN, SN,SN, SN,SN, EN,EN, EN,EN,  EN,EN, QN,QN, SN,SN, SN,SN, SN,SN, SN,SN, SN,SN, SN,SN,  EN,EN, EN,EN, SN,SN,  SN,SN, SN,SN, SN ,SN,    SN,SN,   EN,EN,  SN,SN, SN,SN, SN,SN, SN,SN, SN,SN, EN,EN, EN,EN,	 EN,EN, EN,EN, EN,EN, SN,SN, SN,SN  };
static int u8Index2=0;
 static int state=0;
 
 
//static int timeToStart=0;

  
 if (IsTimeUp(&G_Timer,noteTimes[u8Index] ) )
  {
 
  //Turn off the LED
    PWMAudioOff(BUZZER1);
  //adjust the index
    
    
    if (u8Index == (sizeof((noteSequence))/sizeof((noteSequence[0])))) {
      u8Index =0;
      //PWMAudioOff(BUZZER1);
      }

      u8Index ++;

   /* Turn on BUZZER1 and indicator LED */
      PWMAudioOn(BUZZER1);
    PWMAudioSetFrequency(AT91C_PWMC_CHID1, noteSequence[u8Index]);
   
    
      if(noteSequence[u8Index]== NO){
      LedOn(LCD_BLUE);
     
    }
    else{
      LedOff(LCD_BLUE);
    }

    if(noteSequence[u8Index]== D4){
      LedOn(YELLOW);
     
    }
    else{
      LedOff(YELLOW);
    }
    if(noteSequence[u8Index]== G4){
      LedOn(PURPLE);
      LedOn(LCD_RED);
     
    }
    else{
      LedOff(PURPLE);
      LedOff(LCD_RED);
     
    }
    if(noteSequence[u8Index]== A4S){
      LedOn(RED);
     
    }
    else{
      LedOff(RED);}
      if(noteSequence[u8Index]== A3S){
      LedOn(CYAN);
     
    }
    else{
      LedOff(CYAN);
    }
    if(noteSequence[u8Index]== C5){
      LedOn(WHITE);
     
    }
    else{
      LedOff(WHITE);
    }
    
    if(noteSequence[u8Index]== C4){
      LedOn(GREEN);
     
    }
    else{
      LedOff(GREEN);
    }
    if(noteSequence[u8Index]==D5){
      LedOn(ORANGE);
     
    }
    else{
      LedOff(ORANGE);
    }
    
    if(noteSequence[u8Index]== D4){
      LedOn(BLUE);
     LedOn(LCD_GREEN);
     
    }
    else{
      LedOff(BLUE);
      LedOff(LCD_GREEN);
     
    }
     
 /* Reset the counter to the current time */
    G_Timer = G_u32SystemTime1ms;

  }
 
 if (IsTimeUp(&MpgSong_u32Timeout,noteTimes2[u8Index2] ) )
    {
 
  //Turn off the BUZZER
    PWMAudioOff(BUZZER0);
 
    
    //adjust the index
    if (u8Index2 == (sizeof((noteSequence2))/sizeof((noteSequence2[0])))) {
      u8Index2 =0;
      }

      u8Index2 ++;

   /* Turn on BUZZER1 and indicator LED */
    PWMAudioOn(BUZZER0);
    PWMAudioSetFrequency(AT91C_PWMC_CHID0, noteSequence2[u8Index2]);
   
 /* Reset the counter to the current time */
    MpgSong_u32Timeout = G_u32SystemTime1ms;
    if(noteSequence2[u8Index2]== D4){
      LedOn(LCD_GREEN);
     
    }
    else{
      LedOff(LCD_GREEN);
    }
    if(noteSequence2[u8Index2]== F4){
      LedOn(LCD_BLUE);
     
    }
    else{
      LedOff(LCD_BLUE);
    }
    if(noteSequence2[u8Index2]== G4){
      LedOn(LCD_RED);
     
    }
    else{
      LedOff(LCD_RED);
    }
    
    } 

  
 //for replay from the beginning
  if( IsButtonHeld(BUTTON0,1000) )
  {
    if (IsTimeUp(&MpgSong_u32Timeout,10 ) ){
      G_Timer = G_u32SystemTime1ms;
      MpgSong_u32Timeout=G_u32SystemTime1ms;  
      G_MpgSongStateMachine=  Reset;
  }
  }
  if( IsButtonPressed(BUTTON0) )
  {
    PWMAudioOff(BUZZER0);
    PWMAudioOff(BUZZER1);
    u8Index=0;
    u8Index2=0;
    
  }
 
 //Go Slower
      if (IsButtonPressed(BUTTON1)){
        if (IsTimeUp(&MpgSong_u32Timeout,10 ) ){
      G_Timer = G_u32SystemTime1ms;
      MpgSong_u32Timeout=G_u32SystemTime1ms;  
      G_MpgSongStateMachine=  Slow;
     //slow down
    }
   }
   
//Go to the next song. 
 if (IsButtonPressed(BUTTON2)){
    PWMAudioOff(BUZZER0);
    PWMAudioOff(BUZZER1);
    u8Index=0;
    u8Index2=0;
    G_Timer = G_u32SystemTime1ms;
    MpgSong_u32Timeout=G_u32SystemTime1ms;
    G_MpgSongStateMachine=  Smoke;  
 }
 
 
 //Record your own song
 if (IsButtonPressed(BUTTON3)){
    
    PWMAudioOff(BUZZER0);
    PWMAudioOff(BUZZER1);
    u8Index=0;
    u8Index2=0;
    G_Timer = G_u32SystemTime1ms;
    MpgSong_u32Timeout=G_u32SystemTime1ms;
    G_MpgSongStateMachine=  Skip;
   
  //G_MpgSongStateMachine=  OwnMusic ; 
  }
 
} /* end MpgSongIdle() */


void Smoke(void)
{
 FULL_NOTE      =         (MEASURE_TIME);
 HALF_NOTE       =        (MEASURE_TIME / 2);
 QUARTER_NOTE     =       (MEASURE_TIME / 4);
 EIGHTH_NOTE       =      (MEASURE_TIME / 8);
 SIXTEENTH_NOTE     =     (MEASURE_TIME / 16);

 FN                    =    FULL_NOTE;                 
 HN                     =   HALF_NOTE ;                
 QN                      =  QUARTER_NOTE;              
 EN                       = EIGHTH_NOTE;               
 SN                        =SIXTEENTH_NOTE;            

 
 
 u16 noteSequence[] ={NO,G4,NO,NO,NO,A4S,NO,NO,NO,C5,NO,NO,NO,G4,NO,NO,NO,A4S,NO,NO,NO,C5S,NO,C5,NO,NO,NO,G4,NO,NO,NO,A4S,NO,NO,NO,C5,NO,NO,NO,A4S,NO,NO,NO,G4,NO,   G4,NO,  NO,NO};
 u16 noteTimes[]= {   FN,EN,EN,EN,EN,EN, EN,EN,EN,QN,QN,EN,EN,EN,EN,EN,EN,EN, EN,EN,EN,EN, EN,EN,EN,QN,QN,EN,EN,EN,EN,EN, EN,EN,EN,EN,EN,EN,EN,EN, EN,EN,EN,EN*3,EN*3,QN,QN, QN,QN};
 static int u8Index = 0;

 u16 noteSequence2[]={NO,D4,NO,NO,NO,F4, NO,NO,NO,G4,NO,NO,NO,D4,NO,NO,NO,F4, NO,NO,NO,A4S,NO,G4,NO,NO,NO,D4,NO,NO,NO,F4, NO,NO,NO,G4,NO,NO,NO,F4, NO,NO,NO,D4,NO,    D4,NO,NO,NO};
 u16 noteTimes2[]={   FN,EN,EN,EN,EN,EN, EN,EN,EN,QN,QN,EN,EN,EN,EN,EN,EN,EN, EN,EN,EN,EN, EN,EN,EN,QN,QN,EN,EN,EN,EN,EN, EN,EN,EN,EN,EN,EN,EN,EN, EN,EN,EN,EN*3,EN*3,QN,QN,QN,QN};
 static int u8Index2=0;
 
 if (IsTimeUp(&G_Timer,noteTimes[u8Index] ) )
  {
 
  //Turn off the LED
    PWMAudioOff(BUZZER1);
  //adjust the index
    
    
    if (u8Index == (sizeof((noteSequence))/sizeof((noteSequence[0])))) {
      u8Index =0;
      //PWMAudioOff(BUZZER1);
      }

      u8Index ++;

   /* Turn on BUZZER1 and indicator LED */
      PWMAudioOn(BUZZER1);
    PWMAudioSetFrequency(AT91C_PWMC_CHID1, noteSequence[u8Index]);
   
    if(noteSequence[u8Index]== NO){
      LedOn(LCD_BLUE);
     
    }
    else{
      LedOff(LCD_BLUE);
    }
    if(noteSequence[u8Index]== D4){
      LedOn(YELLOW);
     
    }
    else{
      LedOff(YELLOW);
    }
    if(noteSequence[u8Index]== G4){
      LedOn(PURPLE);
     
    }
    else{
      LedOff(PURPLE);
    }
    if(noteSequence[u8Index]== A4S){
      LedOn(RED);
     
    }
    else{
      LedOff(RED);}
      if(noteSequence[u8Index]== A3S){
      LedOn(CYAN);
     
    }
    else{
      LedOff(CYAN);
    }
    if(noteSequence[u8Index]== C5){
      LedOn(WHITE);
     
    }
    else{
      LedOff(WHITE);
    }
    
    if(noteSequence[u8Index]== C4){
      LedOn(GREEN);
     
    }
    else{
      LedOff(GREEN);
    }
    if(noteSequence[u8Index]== D5){
      LedOn(ORANGE);
     
    }
    else{
      LedOff(ORANGE);
    }
    
    if(noteSequence[u8Index]== D4){
      LedOn(BLUE);
     
    }
    else{
      LedOff(BLUE);
    }
     
 /* Reset the counter to the current time */
    G_Timer = G_u32SystemTime1ms;

  }
 
 if (IsTimeUp(&MpgSong_u32Timeout,noteTimes2[u8Index2] ) )
    {
 
  //Turn off the BUZZER
    PWMAudioOff(BUZZER0);
 
    
    //adjust the index
    if (u8Index2 == (sizeof((noteSequence2))/sizeof((noteSequence2[0])))) {
      u8Index2 =0;
      }

      u8Index2 ++;

   /* Turn on BUZZER1 and indicator LED */
    PWMAudioOn(BUZZER0);
    PWMAudioSetFrequency(AT91C_PWMC_CHID0, noteSequence2[u8Index2]);
  
        if(noteSequence2[u8Index2]== D4){
      LedOn(LCD_GREEN);
     
    }
    else{
      LedOff(LCD_GREEN);
    }
    if(noteSequence2[u8Index2]== F4){
      LedOn(LCD_BLUE);
     
    }
    else{
      LedOff(LCD_BLUE);
    }
    if(noteSequence2[u8Index2]== G4){
      LedOn(LCD_RED);
     
    }
    else{
      LedOff(LCD_RED);
    }
    
     

    
    
 /* Reset the counter to the current time */
    MpgSong_u32Timeout = G_u32SystemTime1ms;


  
  
  
    } 
  if( IsButtonHeld(BUTTON0,1000) )
  {
    if (IsTimeUp(&MpgSong_u32Timeout,10 ) ){
      G_Timer = G_u32SystemTime1ms;
      MpgSong_u32Timeout=G_u32SystemTime1ms;  
      G_MpgSongStateMachine=  Reset;
  }
  }
  
 //for replay from the beginning
  if( IsButtonPressed(BUTTON0) )
  {
    PWMAudioOff(BUZZER0);
    PWMAudioOff(BUZZER1);
    u8Index=0;
    u8Index2=0;
    
  }
  
      //Go Slower
      if (IsButtonPressed(BUTTON1)){
        if (IsTimeUp(&MpgSong_u32Timeout,10 ) ){
      G_Timer = G_u32SystemTime1ms;
      MpgSong_u32Timeout=G_u32SystemTime1ms;  
      G_MpgSongStateMachine=  Slow;
     //slow down
    }
   }
   
//Go to the next song. 
 if (IsButtonPressed(BUTTON2)){
    PWMAudioOff(BUZZER0);
    PWMAudioOff(BUZZER1);
    u8Index=0;
    u8Index2=0;
    G_Timer = G_u32SystemTime1ms;
    MpgSong_u32Timeout=G_u32SystemTime1ms;
    G_MpgSongStateMachine=  MpgSongIdle;  
 }
 
 
 
 if (IsButtonPressed(BUTTON3)){
     PWMAudioOff(BUZZER0);
    PWMAudioOff(BUZZER1);
    u8Index=0;
    u8Index2=0;
    G_Timer = G_u32SystemTime1ms;
    MpgSong_u32Timeout=G_u32SystemTime1ms;
    G_MpgSongStateMachine=  Skip;
   
  
  
}
}

void Skip(void){
 PWMAudioOff(BUZZER0);
    PWMAudioOff(BUZZER1);
     LedOff(WHITE);
    LedOff(PURPLE);
     LedOff(BLUE);
      LedOff(GREEN);
       LedOff(YELLOW);
        LedOff(ORANGE);
         LedOff(CYAN);
          LedOff(RED);
      LedOff(LCD_RED);
     LedOff(LCD_BLUE);
     LedOff(LCD_GREEN);
}

void Slow(void) {
  if (IsTimeUp(&G_Timer,100 ) )
  {
  //Turn off the LED
      MEASURE_TIME *=2;
        G_MpgSongStateMachine = MpgSongIdle;
  }
  
}


void Reset(void) {
  if (IsTimeUp(&G_Timer,100 ) )
  {
  //Turn off the LED
      MEASURE_TIME =1200;
        G_MpgSongStateMachine = MpgSongIdle;
  }
  
}



#if 0
void OwnMusic(void) {
  
 FULL_NOTE      =         (MEASURE_TIME);
 HALF_NOTE       =        (MEASURE_TIME / 2);
 QUARTER_NOTE     =       (MEASURE_TIME / 4);
 EIGHTH_NOTE       =      (MEASURE_TIME / 8);
 SIXTEENTH_NOTE     =     (MEASURE_TIME / 16);

 FN                   =    FULL_NOTE;                 
 HN                    =   HALF_NOTE ;                
 QN                     =  QUARTER_NOTE;              
 EN                      = EIGHTH_NOTE;               
 SN                       =SIXTEENTH_NOTE;            

  
  
  u16 noteSequence[3] ;
  u16 noteTimes[3] ;
  int index=0;
  int i=0;
  int state =1;
  int z=0;
  // Things to consider: 
  // First ones:
  //
  //White LED: C4
  //Purple LED: D4
  //blue LED:   E4
  //Green LED:  F4
  //Yellow LED: G4 
  //Orange LED: A4
  //CYAN LED : B4
  //RED LED: C5
  //Second ones:
  
  //White LED: full note
  //Purple LED: half note
  //blue LED:   quarter note
  //Green LED:  eight note
  //Yellow LED: 
   PWMAudioOff(BUZZER0);
   PWMAudioOff(BUZZER1);  
   LedOff(WHITE);
    LedOff(PURPLE);
     LedOff(BLUE);
      LedOff(GREEN);
       LedOff(YELLOW);
        LedOff(ORANGE);
         LedOff(CYAN);
          LedOff(RED);
      LedOff(LCD_RED);
     LedOff(LCD_BLUE);
     LedOff(LCD_GREEN);
      
     if(state==1){
     if (IsTimeUp(&MpgSong_u32Timeout,1000 ) )
    {
 
      if(IsButtonPressed(BUTTON0)){
        noteSequence[index]=C4;
      }
      
       if(IsButtonPressed(BUTTON1)){
        noteSequence[index]=D4;
      }
       
          if(IsButtonPressed(BUTTON2)){
        noteSequence[index]=F4;
      }
        
      if(IsButtonPressed(BUTTON3)){
        noteSequence[index]=E4;
      }
        
      index ++;
         
         if (index==4){
           state=2;
         }
    }
   
  }
  
   
         if(state==2){      
   if (IsTimeUp(&MpgSong_u32Timeout,1000 ) )
    {
 
      
    PWMAudioOff(BUZZER0);
  //adjust the index
    
    
    if (i== (sizeof((noteSequence))/sizeof((noteSequence[0])))) {
     i =0;
      }

      i++;
   /* Turn on BUZZER1 and indicator LED */
    PWMAudioOn(BUZZER0);
    PWMAudioSetFrequency(AT91C_PWMC_CHID0, noteSequence[i]);
   
 /* Reset the counter to the current time */
    MpgSong_u32Timeout = G_u32SystemTime1ms;
   

  }
  }     
      
         
   
   
   
   
       

}
#endif

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
void MpgSongError(void)          
{
  G_MpgSongStateMachine = MpgSongIdle;
 
 

  
  
} /* end MpgSongError() */



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/

