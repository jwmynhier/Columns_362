/*
************************************************************************
 ECE 362 - Mini-Project C Source File - Spring 2016
***********************************************************************
	 	   			 		  			 		  		
 Team ID: 06

 Project Name: Pillars

 Team Members:

   - Team/Doc Leader: Craig Plazony     Signature: ______________________
   
   - Software Leader: Joseph Mynheir    Signature: ______________________

   - Interface Leader: Bishop Sarkar    Signature: ______________________

   - Peripheral Leader: Dwezil D'souza  Signature: ______________________


 Academic Honesty Statement:  In signing above, we hereby certify that we 
 are the individuals who created this HC(S)12 source file and that we have
 not copied the work of any other student (past or present) while completing 
 it. We understand that if we fail to honor this agreement, we will receive 
 a grade of ZERO and be subject to possible disciplinary action.

***********************************************************************

 The objective of this Mini-Project is to create an interactive game based on
 the Sega Genesis game called "Columns" using game controllers as inputs and
 VGA and the method of display.


***********************************************************************

 List of project-specific success criteria (functionality that will be
 demonstrated):

 1.

 2.

 3.

 4.

 5.

***********************************************************************

  Date code started: 4-15-2016

  Update history (add an entry every time a significant change is made):

  Date: < ? >  Name: < ? >   Update: < ? >

  Date: < ? >  Name: < ? >   Update: < ? >

  Date: 4-17-2016  Name: Craig Plazony   Update: Added intializations, variables, fucntions for LCD display, MAIN logic
  
  Date: 4-19-2-16  Name: Joe Mynhier     Update: Included core game logic files. Still need testing and RNG. Interfacing prototype added.


***********************************************************************
*/

#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include <mc9s12c32.h>
#include "gamecore.h"

/* All functions after main should be initialized here */
char inchar(void);
void outchar(char x);
void shiftout(char x);
int size(char x[]);
void music(char x);
void musicdelay(int x); 
void funeral(void);
void arcade(void);
void sandstorm(void);
void bco(char x);
void send_i(char x);
void printmsg(char str[]);
void print_c(char x);
void chgline(char x);
void send_byte(char x);
void lcdwait(void);
void introdisp(void);
void scoredisp(void);
void updatescreen(void);

//Set text colors through PuTTY
/************Slows down screen refresh. Don't use without fixing*******/
void make_red();
void make_green();
void make_yellow();
void make_cyan();
void make_white();


/* Variable declarations */
//Prev state for controller buttons
char prevP1L = 1; //Left Button
char prevP1R = 1; //Right Button
char prevP1D = 1; //Down Button
char prevP1C = 1; //Change Colors Button
char prevP1S = 1; //Start/Select Button
char prevP2L = 1;
char prevP2R = 1;
char prevP2D = 1;
char prevP2C = 1;
char prevP2S = 1;
char P1Lb; //Raw button inputs
char P1Rb;
char P1Cb;
char P1Db;
char P1Sb;
char P2Lb;
char P2Rb;
char P2Cb;
char P2Db;
char P2Sb; 
char P1L = 0; //Button Presed Logic Variables
char P1R = 0;
char P1C = 0;
char P1D = 0;
char P1S = 0;
char P2L = 0;
char P2R = 0;
char P2C = 0;
char P2D = 0;
char P2S = 0;
char P1LR = 0;
char P1RR = 0;

int time = 0; //Time is Seconds since game started
int score1 = 0; //Scores for P1 and P2
int score2 = 0;
char tenths = 0;
char onesec = 0;
char tenthsflag = 0;  	   			 		  			 		       
char runstop = 0;
char pause = 0;
char reset = 0;
char gameover = 0;

int tin	= 0;	// SCI transmit display buffer IN pointer
int tout	= 0;	// SCI transmit display buffer OUT pointer   	   			 		  			 		       
char sciread;
#define TSIZE 1500	// transmit buffer size (80 characters)
char tbuf[TSIZE];	// SCI transmit display buffer
char outputchar;

//4-19-2016 JM: This variable controls the state of the next game tick.
char state;
char ul = 0;
char ur = 0;
char ud = 0;
char us = 0;


//music sounds from PWM
#define INTROM music(1);
#define STARTM music(2);
#define PAUSEDM music(3);
#define GAMEOVERM music(4);
long musictimer1;
long musictimer2;
int beat = 75; //beat for songs. 75 is Sandstorm speed
#define GAP SOUNDOFF; musicdelay(7);
#define SIXTEEN musicdelay(beat);
#define EIGHT musicdelay(beat*2);
#define QUART musicdelay(beat*4);
#define QUARTDOT musicdelay(beat*6);
#define HALF musicdelay(beat*8);
#define HALFDOT musicdelay(beat*12);
#define WHOLE musicdelay(beat*16);
#define CNL PWMSCLB = 90; SOUNDON;
#define CSNL PWMSCLB = 85; SOUNDON; 
#define DNL PWMSCLB = 80; SOUNDON;
#define DSNL PWMSCLB = 75; SOUNDON;
#define ENL PWMSCLB = 71; SOUNDON;
#define FNL PWMSCLB = 67; SOUNDON;
#define FSNL PWMSCLB = 63; SOUNDON;
#define GNL PWMSCLB = 60; SOUNDON;
#define GSNL PWMSCLB = 56; SOUNDON;
#define ANL PWMSCLB = 53; SOUNDON;
#define ASNL PWMSCLB = 50; SOUNDON;
#define BNL PWMSCLB = 47; SOUNDON;
#define CN PWMSCLB = 45; SOUNDON;
#define CSN PWMSCLB = 42; SOUNDON; 
#define DN PWMSCLB = 40; SOUNDON;
#define DSN PWMSCLB = 38; SOUNDON;
#define EN PWMSCLB = 36; SOUNDON;
#define FN PWMSCLB = 34; SOUNDON;
#define FSN PWMSCLB = 32; SOUNDON;
#define GN PWMSCLB = 30; SOUNDON;
#define GSN PWMSCLB = 28; SOUNDON;
#define AN PWMSCLB = 27; SOUNDON;
#define ASN PWMSCLB = 25; SOUNDON;
#define BN PWMSCLB = 24; SOUNDON;
#define SOUNDON PWMDTY3 = 0x80;
#define SOUNDOFF PWMDTY3 = 0x00;



#define PAPCLKH PTT_PTT7 = 1;
#define PAPCLKL PTT_PTT7 = 0;

/* Special ASCII characters */
#define CR 0x0D		// ASCII return 
#define LF 0x0A		// ASCII new line 



/* LCD INSTRUCTION CHARACTERS */
#define LCDON 0x0F	// LCD initialization command
#define LCDCLR 0x01	// LCD clear display command
#define TWOLINE 0x38	// LCD 2-line enable command
#define CURMOV 0xFE	// LCD cursor move instruction
#define LINE1 0x80	// LCD line 1 cursor position
#define LINE2 0xC0	// LCD line 2 cursor position
#define LCDINST PTT_PTT7 = 0; //RS Pin - Instruction Select
#define LCDPRINT PTT_PTT7 = 1; //RS Pin - Print Select
#define LCDCLKH PTT_PTT6 = 1;  //CLK pin High for LCD Display
#define LCDCLKL PTT_PTT6 = 0;  //CLK pin Low for LCD Display


//Output character definitions for terminal output
#define BORDERV 0x7c
#define BORDERH 0x2d
#define SQ1     0x23
#define SQ2     0xdb
#define SQ3     0xf0
#define SQ4     0xc5

	 	   		
/*	 	   		
***********************************************************************
 Initializations
***********************************************************************
*/

void  initializations(void) {

/* Set the PLL speed (bus clock = 24 MHz) */
  CLKSEL = CLKSEL & 0x80; //; disengage PLL from system
  PLLCTL = PLLCTL | 0x40; //; turn on PLL
  SYNR = 0x02;            //; set PLL multiplier
  REFDV = 0;              //; set PLL divider
  while (!(CRGFLG & 0x08)){  }
  CLKSEL = CLKSEL | 0x80; //; engage PLL

/* Disable watchdog timer (COPCTL register) */
  COPCTL = 0x40   ; //COP off; RTI and COP stopped in BDM-mode

/* Initialize asynchronous serial port (SCI) for 9600 baud, interrupts off initially */
  SCIBDH =  0x00; //set baud rate to 115200
  SCIBDL =  0x0D; //24,000,000 / 16 / 13 = 115200 (approx)  
  SCICR1 =  0x00; //$9C = 156
  SCICR2 =  0x0C; //initialize SCI for program-driven operation
  DDRB   =  0x10; //set PB4 for output mode
  PORTB  =  0x10; //assert DTR pin on COM port

/* Initialize peripherals */
  //ATD as Inputs
  DDRAD = 0xFF;
  

  //Input Pins initialization
  DDRT = 0xF9;//PTT3 and PTT7 as outputs
  DDRM = 0xFF;
  ATDDIEN = 0xFF;
            

  //RTI 2.048 ms interrupt rate
  CRGINT = 0x80;
  RTICTL = 0x1F;
  	      
	//TIM  10ms timer
	
	TIOS = 0x80;
  TSCR1 = 0x80;
  TSCR2 = 0x0F;
  TC7 = 37500;
  TIE = 0x80; 
  
    
  SPIBR = 0x01;
  SPICR1 = 0x50;
  SPICR2 = 0;
  
  
  //4-19-2016 JM: This variable sets the state for the next 0.1s game tick.
  state = START;
  
  
  //PWM
  asm{
    
  movb	#$08,MODRR; //PT0 used as PWM Ch 3 output
	movb	#$08,PWME; //Intially enabled (0x08 starts it)
	movb	#$08,PWMPOL; //set active high polarity
	movb	#$00,PWMCTL; //no concatenate (8-bit)
	movb	#$00,PWMCAE; //left-aligned output mode
	movb	#$FF,PWMPER3; //set maximum 8-bit period
	movb	#$00,PWMDTY3; //initially clear DUTY register
	movb	#$08,PWMCLK; //select Clock SB for Ch 3
	movb	#$10,PWMPRCLK; //set Clock B = 6 MHz (prescaler = 4) rate ($02)
	movb  #117,PWMSCLB; //additional divider to get to 100hz sample rate
	
  }
 
  
  //LCD
  LCDCLKH;
              //R/W' Pin should be connected to GND;
  send_i(LCDON);
  send_i(TWOLINE);
  send_i(LCDCLR);
  lcdwait();          
  
       
}

	 		  			 		  		
/*	 		  			 		  		
***********************************************************************
Main
***********************************************************************
*/
void main(void) {

  DisableInterrupts;
	initializations(); 		  			 		  		
	EnableInterrupts;
	
  //bco(0x39);
  
  sandstorm();
 
  introdisp();
  updatescreen();
  
  
  
 for(;;) {
  
 
  
 
  
  //GAME RUNNING
  if(runstop == 1){
   //GAME RUNNING LOGIC HERE 
   if(tenthsflag == 1){
    tenthsflag = 0;
      switch(state) {
        case USERMOTION:
          state = user_motion(ul, ur, ud, us);
          ul = 0;
          ur = 0;
          ud = 0;
          us = 0;
          if(state == END){
            funeral();
          }
            
          break;
        case STPANDCHK:          
          state = stop_and_check();
          if(state == END){
            funeral();
          }
          break;
        case MOTION:
          state = motion();
          if(state == END){
            funeral();
          }
          break;
        case INSCOLUMN:
          state = ins_user_column();
          if(state == END){
            funeral();
          }
          break;
        case START:
          //if game is running, set to INSCOLUMN, otherwise stay in START.
          if (runstop != 0) {
            state = INSCOLUMN;
          } else {
            state = START;  
          }
          break;
        case END:
          //stay on END until reset, Rerunning initializations should get
          //the game out of END.
          state = END;
          runstop = 0;
          gameover = 1;
          break;
        default:
          //Error state. Should not occur.
          break;    
      }
   
        //output
    updatescreen();                                                                            
  
      
   }
    
  }
  
    
  

  //One Second Update
  if(onesec == 1){
   onesec = 0;
   
   
   if(runstop == 1 || pause == 1 || gameover == 1){
    scoredisp();
    time++;
   }
   
  }
 
      
  //P1L Pressed Logic
      if(P1L == 1){
        P1L = 0;
        ul = 1;
      }
      
      //P1R Pressed Logic
      if(P1R == 1){
        P1R = 0;
        ur = 1;
      }  

      //P1D Pressed Logic
      if(P1D == 1){
        P1D = 0;
        ud = 1;
      }
      
      //P1C Pressed Logic
      if(P1C == 1){
        P1C = 0;
        us = 1;
                
      }
      
      
  
      
/***********************************************************************
Controller Buttons Logic Section
************************************************************************/
 
  //P1L Pressed Logic
 
    
  //P1S Pressed Logic
  if(P1S == 1){
    P1S = 0;
    if(runstop == 1){
      if(pause == 0){
        pause = 1;
        arcade();
      }else{
        pause = 0;
      }       
        
    }else{
      runstop = 1;
      gc_init(TCNT);
    }
     
  }
    


   //START GAME LOGIC
   //4-19-2016 JM: This is the general idea of the interfacing with the
   //game logic. Logic still needs to be debugged. More will need to be
   //done to interface with pushbuttons and the buffer. The buffer
   //may need to be declared in gamecore.h instead, so it is visible 
   //inside the gamecore funcitons as well as main.
   
   
   

 }
    
  
  
  
}/* loop forever */
   
   /* do not leave main */




/*
***********************************************************************                       
 RTI interrupt service routine: RTI_ISR
************************************************************************
*/

interrupt 7 void RTI_ISR(void)
{
  	// clear RTI interrupt flagt 
  	CRGFLG = CRGFLG | 0x80; 
    P1Lb = PORTAD0_PTAD0; //Defining the button inputs
    P1Rb = PORTAD0_PTAD1;
    P1Db = PORTAD0_PTAD2;
    P1Cb = PORTAD0_PTAD3;
    P1Sb = PORTAD0_PTAD4;
    
 
    //P1 Button Checks
    if(P1Lb == 0){
     if(prevP1L == 1){
      prevP1L = 0;
      P1L = 1;
      
     }else{
      P1L = 0;
     }
    }else{
      prevP1L = 1;
    }

    if(P1Rb == 0){
     if(prevP1R == 1){
      prevP1R = 0;
      P1R = 1;
      
     }else{
      P1R = 0;
     }
    }else{
      prevP1R = 1;
    }
    
    if(P1Db == 0){
     if(prevP1D == 1){
      prevP1D = 0;
      P1D = 1;
     }else{
      P1D = 0;
     }
    }else{
      prevP1D = 1;
    }
    
    if(P1Cb == 0){
     if(prevP1C == 1){
      prevP1C = 0;
      P1C = 1;
     }else{
      P1C = 0;
     }
    }else{
      prevP1C = 1;
    }
    
    if(P1Sb == 0){
     if(prevP1S == 1){
      prevP1S = 0;
      P1S = 1;
     }else{
      P1S = 0;
     }
    }else{
      prevP1S = 1;
    }
    
    
}

/*
***********************************************************************                       
  TIM interrupt service routine	  		
***********************************************************************
*/

interrupt 15 void TIM_ISR(void)
{
  	// clear TIM CH 7 interrupt flag 
 	TFLG1 = TFLG1 | 0x80; 
 if(pause == 0){
   PTT_PTT0 = PTT_PTT0 ^ 1;
   tenthsflag = 1;
   tenths++;
   if(tenths == 5){
    tenths = 0;
    onesec = 1;
    
   }
 }else{
  
 }

}

/*
***********************************************************************                       
  SCI (transmit section) interrupt service routine
                         
    - read status register to enable TDR write
    - check status of TBUF: if EMPTY, disable SCI transmit interrupts and exit; else, continue
    - access character from TBUF[TOUT]
    - output character to SCI TDR
    - increment TOUT mod TSIZE	

  NOTE: DO NOT USE OUTCHAR (except for debugging)	  			 		  		
***********************************************************************
*/

interrupt 20 void SCI_ISR(void)
{
    sciread = SCISR1;
    if(tout == tin){
      SCICR2 = SCICR2 & 0x7F;
      
    }else{
      outputchar = tbuf[tout];
      SCIDRL = tbuf[tout];
      tout++;
      tout = tout  % TSIZE;      
    }

}

/*
***********************************************************************                              
  SCI buffered character output routine - bco

  Places character x passed to it into TBUF

   - check TBUF status: if FULL, wait for space; else, continue
   - place character in TBUF[TIN]
   - increment TIN mod TSIZE
   - enable SCI transmit interrupts

  NOTE: DO NOT USE OUTCHAR (except for debugging)
***********************************************************************
*/

void bco(char x)
{
     while((tin+1) % TSIZE == tout);
     tbuf[tin] = x;
     tin++;
     tin = tin % TSIZE;
     SCICR2 = SCICR2 | 0x80;
}


/***********************************************************************
  scoredisp: Displays the timer and the current score
***********************************************************************
*/
void scoredisp(){
 int timemin = time / 60; //Get digits for time
 int timesec = time % 60;
 char timemintens = (timemin / 10) + 0x30;
 char timeminones = (timemin % 10) + 0x30;
 char timesectens = (timesec / 10) + 0x30;
 char timesecones = (timesec % 10) + 0x30;
 char score1hunds = (score1 / 100) + 0x30;
 char score1tens = ((score1 / 10) % 10) + 0x30;
 char score1ones = (score1 % 10) + 0x30;
 
 //Display first line - Players and Time Elapsed
 send_i(LCDCLR);
 lcdwait();
 printmsg("P1   ");
 print_c(timemintens);
 print_c(timeminones);
 print_c(':');
 print_c(timesectens);
 print_c(timesecones);
 
 
 //Display Second Line - Player Scores 
 chgline(LINE2);
 print_c(score1hunds);
 print_c(score1tens);
 print_c(score1ones);
 if(pause == 1){
   printmsg("  PAUSED  "); 
 }else if(gameover == 1){
   printmsg(" GAMEOVER "); 
 }else{
   printmsg("          "); 
 }
}

/***********************************************************************
  introdisp: Displays the intro message to players
***********************************************************************
*/

void introdisp(){
 send_i(LCDCLR);
 lcdwait();
 printmsg(" PRESS START TO ");
 chgline(LINE2);
 printmsg("       BEGIN    ");
}

/***********************************************************************
  shiftout: Transmits the character x to external shift 
            register using the SPI.  It should shift MSB first.  
             
            MISO = PM[4]
            SCK  = PM[5]
***********************************************************************
*/
 
void shiftout(char x)

{
  
  int count;
  if(SPISR_SPTEF){// read the SPTEF bit, continue if bit is 1
    // write data to SPI data register
    SPIDR = x;    
  // wait for 30 cycles for SPI data to shift out
    for(count = 30; count>0; count--){
      asm{
        nop;
        
      }
    }



  }
}

/*
***********************************************************************
  lcdwait: Delay for approx 2 ms
***********************************************************************
*/

void lcdwait()
{
  int count;
  for(count = 2500; count>0; count--){
    asm{
      nop;
      nop;
      nop;
    }
  }
}

/*
*********************************************************************** 
  send_byte: writes character x to the LCD
***********************************************************************
*/

void send_byte(char x)
{
     shiftout(x);// shift out character
     // pulse LCD clock line low->high->low
     LCDCLKL;
     LCDCLKH;
     LCDCLKL;
     // wait 2 ms for LCD to process data
     lcdwait();
}

/*
***********************************************************************
  send_i: Sends instruction byte x to LCD  
***********************************************************************
*/

void send_i(char x)
{
        // set the register select line low (instruction data)
        LCDINST;
        
        // send byte
        send_byte(x);
}

/*
***********************************************************************
  chgline: Move LCD cursor to position x
  NOTE: Cursor positions are encoded in the LINE1/LINE2 variables
***********************************************************************
*/

void chgline(char x)
{
     send_i(CURMOV);
     send_i(x);
}

/*
***********************************************************************
  print_c: Print (single) character x on LCD            
***********************************************************************
*/
 
void print_c(char x)
{
      LCDPRINT;
      send_byte(x);
}

/*
***********************************************************************
  pmsglcd: print character string str[] on LCD
***********************************************************************
*/

void printmsg(char str[])
{
      
     int length = size(str);
     int count = 0;
     LCDPRINT;
     while(count < length){
      send_byte(str[count]);
      count++;
     }
}

/*
***********************************************************************
 Character I/O Library Routines for 9S12C32 (for debugging only)
***********************************************************************
 Name:         inchar
 Description:  inputs ASCII character from SCI serial port and returns it
 Example:      char ch1 = inchar();
***********************************************************************
*/

char inchar(void) {
  /* receives character from the terminal channel */
        while (!(SCISR1 & 0x20)); /* wait for input */
    return SCIDRL;
}

/*
***********************************************************************
 Name:         outchar
 Description:  outputs ASCII character x to SCI serial port
 Example:      outchar('x');
***********************************************************************
*/

void outchar(char x) {
  /* sends a character to the terminal channel */
    while (!(SCISR1 & 0x80));  /* wait for output buffer empty */
    SCIDRL = x;
}

//returns the size of a character array using a pointer to the first element of the character array
int size(char *ptr)
{
    //variable used to access the subsequent array elements.
    int offset = 0;
    //variable that counts the number of elements in your array
    int count = 0;

    //While loop that tests whether the end of the array has been reached
    while (*(ptr + offset) != '\0')
    {
        //increment the count variable
        ++count;
        //advance to the next element of the array
        ++offset;
    }
    //return the size of the array
    return count;
}

/*************************************************************************
Music

  every change in PWMSCLB from 117 (100hz) changes the frequency by .7. 1/2 steps are around
  5-6hz (~8-9 on PWMSCLB) and whole steps around around 10-13hz (13-18 on PWMSCLB). C4 (middle C)
  is at 261.63hz (~45 CLB). Before SCLB the value is 11718.75hz. Use this number and music chart online to
  find right values 
**************************************************************************/

void music(char x){
 if(x == 1){
  
  //intro sounds
    sandstorm();
 
 }else if(x == 2){
  //game start
  
  
  
  
 }else if(x == 3){
  //game paused
    arcade();
  
  
 }else if(x == 4){
  //game over
    funeral();
  
  
 }
  
}

/*************************************************************************
Music delay
**************************************************************************/

void musicdelay(int x){
 for(musictimer1 = 800; musictimer1 > 0; musictimer1--){
  for(musictimer2 = x; musictimer2 > 0; musictimer2--){
    asm{
      nop;
    }
  }
   asm{
      nop;
   }
  
 }
  
}

void funeral(void){
  beat = 60;
  
  GN;
  HALF;
  GAP;
  
  GN;
  QUARTDOT;
  GAP;
  
  GN;
  EIGHT;
  GAP;
  
  GN;
  HALF;
  GAP;
  
  ASN;
  QUARTDOT;
  GAP
  
  AN;
  EIGHT;
  GAP;
  
  AN;
  QUARTDOT;
  GAP;
  
  GN;
  EIGHT;
  GAP;
  
  GN;
  QUARTDOT;
  GAP;
  
  FSN;
  EIGHT;
  
  GN;
  HALF;
  
  SOUNDOFF;
  
}

void arcade(void){
 beat = 10;
  CNL;
  QUART;
  CSNL;
  QUART;
  DNL;
  QUART;
  DSNL;
  QUART;
  ENL;
  QUART;
  FNL;
  QUART;
  FSNL;
  QUART;
  GNL;
  QUART;
  GSNL;
  QUART;
  ANL;
  QUART;
  ASNL;
  QUART;
  BNL;
  QUART;
  CN;
  QUART;
  CSN;
  QUART;
  DN;
  QUART;
  DSN;
  QUART;
  EN;
  QUART;
  FN;
  QUART;
  FSN;
  QUART;
  GN;
  QUART;
  GSN;
  QUART;
  AN;
  QUART;
  ASN;
  QUART;
  BN;
  QUART;
  PWMSCLB = 22;
  QUART;
  SOUNDOFF;
   
  
}

void sandstorm(void){
  int i;
  int j;
  beat = 75;
  
  for(j=2;j>0;j--){
    
  for(i=4;i>0;i--){
    
  CN;
  SIXTEEN;
  GAP;
  }
  
  CN;
  EIGHT;
  GAP;
  
   
  CN;
  SIXTEEN;
  GAP;
  
   
  CN;
  SIXTEEN;
  GAP;
 
 for(i=4;i>0;i--){
    
  CN;
  SIXTEEN;
  GAP;
  }
 
  CN;
  EIGHT;
  GAP;
  
  for(i=6;i>0;i--){
   
  FN;
  SIXTEEN;
  GAP;
  }
  
  FN;
  EIGHT;
  GAP;
  
    
  DSN;
  SIXTEEN;
  GAP;
  
    
  DSN;
  SIXTEEN;
  GAP;
  
   for(i=4;i>0;i--){
    
  DSN;
  SIXTEEN;
  GAP;
  }
  
    
  SOUNDON;
  PWMSCLB = 50; 
  EIGHT;
  
 }
  CN;
  HALF;
  SOUNDOFF; 
  
}



#include "gamecore.h"

/****************************************************************
    The Grid is the 2D matrix that holds the data for each square  
****************************************************************/

 /****************************************************************
    The Grid is the 2D matrix that holds the data for each square  
****************************************************************/
/*  SQUARE: Holds color and motion bits. Bit map (M=motion, C=
    color):
    [...:M:C:C:C]                                              */

SQUARE grid[XSIZE][YSIZE];



LOCATION user; //bits [X:X:X:Y:Y:Y:Y:Y]


short f_list[2][FLSIZE];
char f_head;
char f_unused_head;

LOCATION stop_q[SQSIZE];
unsigned char sq_in;
unsigned char sq_out;


LOCATION remove_q[RQSIZE];
unsigned char rq_in;
unsigned char rq_out;

unsigned char rand_val;


/****************************************************************
    The Grid is the 2D matrix that holds the data for each square  
****************************************************************/

/*  Function:   grid_init zeros out the grid                   */
void grid_init(void) { 
    int i, j;
    for (i=0; i<XSIZE; i++) {
        for (j=0; j<YSIZE; j++) {
            grid[i][j] = 0;
        }
    }

    //variable initializations
    sq_in = 0;
    sq_out = 0;
    rq_in = 0;
    rq_out = 0;


}

/*  Function:   get_x returns the x coordinate of a location   */
unsigned char get_x (LOCATION l) {
    return (l & XMASK) >> YBITS; 
}

/*  Function:   get_y returns the y coordinate of a location   */
unsigned char get_y (LOCATION l) {
    return (l & YMASK);
}

/*  Function:   is_mov returns true if the moving bit is set
    and false otherwise                                        */
unsigned char is_mov(SQUARE s) {
    return  s & MMASK;    
}

/*  Function:   set_y changes the value of the y bits     */   
LOCATION set_y(LOCATION l, LOCATION y) {
    return (l & XMASK) | (y & YMASK); 
}

/*  Function:   set_x changes the value of the x bits     */   
LOCATION set_x(LOCATION l, LOCATION x) {
    return (l & YMASK) | ( (x << YBITS) & XMASK); 
}

/*  Function:   mk_loc takes a raw x, y pair, returns a location*/   
LOCATION mk_loc(LOCATION x, LOCATION y) {
    return (y & YMASK) | ( (x << YBITS) & XMASK);
}

/*  Function:   gc_init initializes the game core.             */
void gc_init(char i) {
    rand_init(i);
    f_list_init();
    grid_init();
}

/****************************************************************
    The Falling List is a linked list that holds the location of
    any moving blocks not controlled by the user. 
****************************************************************/

/*  Function:   f_list_init sets the initial "pointer" values  */
void f_list_init(void) {
    int i;
    f_head = -1;
    f_unused_head = 0;
    
    for (i=0; i<FLSIZE-1; i++) {
       f_list[1][i] = i+1; //each node points to the next 
    }
    f_list[1][i] = -1;
}

/*  Function:   f_list_add add a new value to the list         */
void f_list_add(LOCATION l) {
    char temp = f_unused_head;
    f_unused_head = f_list[1][(int)f_unused_head]; 
    f_list[1][(int)temp] = f_head;
    f_head = temp;
    f_list[0][(int)f_head] = l;
}

/*  Function:   f_list_remove remove the node at the current
    index.                                                     */
void f_list_remove(char curr, char prev) {
    if (curr == f_head) {
        f_head = f_list[1][(int)curr];
    } else {
        f_list[1][(int)prev] = f_list[1][(int)curr];
    }
        f_list[1][(int)curr] = f_unused_head;
        f_unused_head = curr;
}

/****************************************************************
    The Stopped Queue is a circular queue that holds the location
    of blocks that have just stopped moving.
****************************************************************/

/*  Function:   stop_q_inc increments a counter modulus SQSIZE */
char stop_q_inc(unsigned char i) {
    int temp = SQSIZE;
    return (i + 1) % temp;
}

/*  Function:   stop_q_add adds a location to the stop queue
    index.                                                     */
void stop_q_add(LOCATION l) {
    if (stop_q_inc(sq_in) != sq_out) {
        stop_q[(int)sq_in] = l;
        sq_in = stop_q_inc(sq_in);
    } else {
        //output an error message, the queue is full
    }
}

/*  Function:   stop_q_remove removes a location from the stop
queue index. Returns LOC_INVLD when queue is empty             */
LOCATION stop_q_remove(void) {
    if (sq_in == sq_out)
        return LOC_INVLD;
    else {
        LOCATION temp = stop_q[(int)sq_out];
        sq_out = stop_q_inc(sq_out);
        return temp;
    }
}

/****************************************************************
    The Removal Queue is a circular queue that holds the location
    of blocks that need to be cleared.
****************************************************************/

/*  Function:   remove_q_inc increments a counter modulus RQSIZE */
char remove_q_inc(unsigned char i) {
    int temp = RQSIZE;
    return (i + 1) % temp;
}

/*  Function:   remove_q_add adds a location to the removal queue
    index.                                                     */
void remove_q_add(LOCATION l) {
    if (remove_q_inc(rq_in) != rq_out) {
        remove_q[(int)rq_in] = l;
        rq_in = remove_q_inc(rq_in);
    } else {
        //output an error message, the queue is full
    }
}

/*  Function:   remove_q_remove removes a location from the removal 
queue index. Returns LOC_INVLD when queue is empty             */
LOCATION remove_q_remove(void) {
    if (rq_in == rq_out)
        return LOC_INVLD;
    else {
        LOCATION temp = remove_q[(int)rq_out];
        rq_out = remove_q_inc(rq_out);
        return temp;
    }
}


/*  Function:   user_motion checks user input flags and moves the
    user column accordingly. If the user column is stopped, it 
    will set state for the Adjacency Check, otherwise it will 
    continue with another User Motion tick.                    */

char user_motion(char uleft, char uright, char udown, char uswap) {
    char tempx, tempy;
    char yoffset = 1, xoffset = 0;
    int i;

    char state_l = USERMOTION;

    tempx = get_x(user);
    tempy = get_y(user);
    if (uleft && (tempx + xoffset-1) > -1 && (tempx + xoffset-1) < XSIZE && \
            grid[(int)tempx+xoffset-1][(int)tempy+yoffset] == 0) {
        uleft = 0;
        xoffset--;
    } 
    if (uright && (tempx + xoffset+1) > -1 && (tempx + xoffset+1) < XSIZE && \
            grid[(int)tempx+xoffset+1][(int)tempy+yoffset] == 0) {
        uright = 0;
        xoffset++;
    }
    if (udown) {
        udown = 0;
        i = 1;
        while ((tempy+i)<YSIZE && grid[(int)tempx+xoffset][(int)tempy+i] == 0) 
            i++;
        yoffset = i>1 ? i-1 : 1;
        state_l = STPANDCHK;
    }
    if (grid[(int)tempx+xoffset][(int)tempy+yoffset] == 0) {
        grid_swap(tempx,tempy,tempx+xoffset,tempy+yoffset);
        user = mk_loc(tempx+xoffset,tempy+yoffset);
        grid_swap(tempx,tempy-1,tempx+xoffset,tempy+yoffset-1);
        grid_swap(tempx,tempy-2,tempx+xoffset,tempy+yoffset-2);

        if (tempy+yoffset == YSIZE-1 || grid[tempx+xoffset][tempy+yoffset+1] != 0) {
            for (i=0; i<3; i++) {
                stop_q_add(mk_loc(tempx+xoffset,tempy+yoffset-i));
                //unset the moving bit
                grid[(int)tempx+xoffset][(int)tempy+yoffset-i] &= ~MMASK; 
            }

            state_l = STPANDCHK;
        }
    } else {
        for (i=0; i<3; i++) {
            stop_q_add(mk_loc(tempx,tempy-i));
            //unset the moving bit
            grid[(int)tempx][(int)tempy-i] &= ~MMASK; 
        }
        state_l = STPANDCHK;
    }
    if (uswap) { //rotate the order of the squares
        uswap = 0;
        tempx = get_x(user);
        tempy = get_y(user);
        grid_swap(tempx,tempy,tempx,tempy-1);
        grid_swap(tempx,tempy-1,tempx,tempy-2);
    }



    return state_l;
}

/*  Function:   stop_and_check dequeues locations from the Stopped
    Queue and checks for a match of three of a kind in a straight
    line from the block being checked. The score is set according-
    ly. Any matching blocks are added to the Removal Queue. After
    all checks are done, it loops through the Removal Queue and 
    deletes them, adding all blocks above a deleted block to the 
    Moving List.                                               */

char stop_and_check(void) {
    LOCATION temp;
    char tempx, tempy; 
    int i,j,m,n;
    unsigned char count;
    char score_offset = 0;

    char state_l = INSCOLUMN;
    
    while (sq_in != sq_out) { //while stop queue not empty
        temp = stop_q_remove();
        tempx = get_x(temp);
        tempy = get_y(temp);
        for (i=-1; i<2; i++) {
            for (j=-1; j<2; j++) {
                if ((i==0 && j==0) || tempx + i >= XSIZE || \
                    tempx + i < 0 || tempy + j >= YSIZE || \
                    tempy + j < 0) continue;

                if ((CMASK & grid[(int)tempx][(int)tempy]) == \
                        (CMASK & grid[(int)tempx+i][(int)tempy+j])) {

                    //check if squares on opposite sides of the center match.
                    if (tempx - i >= 0 && tempx - i < XSIZE && \
                            tempy - j < YSIZE && tempy - j >= 0) {
                        if ((grid[(int)tempx][(int)tempy]&CMASK) == (grid[(int)tempx-i][(int)tempy-j]&CMASK)) {
                           
                            if (!(grid[(int)tempx][(int)tempy] & DMASK)) {         
                               remove_q_add(temp);
                               grid[(int)tempx][(int)tempy] |= DMASK; //mark for deletion to avoid repeated deletion
                            }
                            if (!(grid[(int)tempx+i][(int)tempy+j] & DMASK)) {
                               remove_q_add(mk_loc(tempx+i,tempy+j));
                               grid[(int)tempx+i][(int)tempy+j] |= DMASK; //mark for deletion to avoid repeated deletion
                            }
                            if (!(grid[(int)tempx-i][(int)tempy-j] & DMASK)) {
                               remove_q_add(mk_loc(tempx-i,tempy-j));
                               grid[(int)tempx-i][(int)tempy-j] |= DMASK; //mark for deletion to avoid repeated deletion
                            } 
                        }                    

                    }

                    //Do a psuedo-depth-first traversal to find sets of 3 matches
                    //radiating out from the center.
                    m = i;
                    n = j;
                    count = 0;
                    // travel in one direction, staying within the
                    // bounds, as long as you get the same color as
                    // the center. 
                    while (tempx+i+m<XSIZE && tempx+i+m>=0 && \
                           tempy+j+n<YSIZE && tempy+j+n>=0 && \
                           ((CMASK & grid[(int)tempx][(int)tempy]) == \
                           (CMASK & grid[(int)tempx+i+m][(int)tempy+j+n]))) {
                           
                       if (!(grid[(int)tempx+i+m][(int)tempy+j+n]&DMASK)) {
                           remove_q_add(mk_loc(tempx+i+m,tempy+j+n));
                           grid[(int)tempx+i+m][(int)tempy+j+n] |= DMASK;
                       }
                       count=1;
                       m += i;
                       n += j;
                    }
                    if (count) { //3 or more have been found
                       if (!(grid[(int)tempx+i][(int)tempy+j]&DMASK)) {
                           remove_q_add(mk_loc(tempx+i,tempy+j));
                           grid[(int)tempx+i][(int)tempy+j] |= DMASK;
                       }
                       if (!(grid[(int)tempx][(int)tempy]&DMASK)) {
                           remove_q_add(temp);
                           grid[(int)tempx][(int)tempy] |= DMASK;
                       }

                    }
                }
            }
        }
    }
    //Now remove from the removal queue.
    while (rq_in != rq_out) {
        temp = remove_q_remove();
        tempx = get_x(temp);
        tempy = get_y(temp);
        square_clear(tempx,tempy);
        i=0;
        while (i < tempy) {
            if (grid[(int)tempx][i] != 0  && !(grid[(int)tempx][i] & MMASK) && \
                    !(grid[(int)tempx][i] & DMASK)) { //exists,not about to be deleted
                                                      // and not already moving.
                f_list_add(mk_loc(tempx,i));
                grid[(int)tempx][(int)i] |= MMASK;
                state_l = MOTION;

            }
            i++;
        }
        score1++;
    }

    //Send the score offset
    //cmd_q_add(SCMASK); //color byte
    //cmd_q_add(score_offset); //location byte

    return state_l;
}

/*  Function:   motion moves the squares in the Falling List down
    until they hit the bottom of the screen or a square that isn't
    moving.                                                    */

char motion(void) {
    char curr = f_head;
    char prev = -1;
    LOCATION temp;
    unsigned char tempx, tempy;

    char state_l = STPANDCHK;

    while (f_head != -1) {
        temp = f_list[0][(int)curr];
        tempx = get_x(temp);
        tempy = get_y(temp);
        
        if (tempy == YSIZE - 1) {
            stop_q_add(f_list[0][(int)curr]);
            f_list_remove(curr, prev);
            curr = prev;
            grid[(int)tempx][(int)tempy] &= ~MMASK; //turn off moving bit
        } else if (grid[(int)tempx][(int)tempy+1] != 0) {
            if (!(grid[(int)tempx][(int)tempy+1] & MMASK)) { 
                //next block is not moving
                stop_q_add(f_list[0][(int)curr]);
                f_list_remove(curr,prev);
                curr = prev;                    
                grid[(int)tempx][(int)tempy] &= ~MMASK; //turn off moving bit
            } //if next is moving, go to next in queue and 
              //come back later.
        } else { //next square is empty 
            if (tempy+1 == YSIZE-1) { //if the next square is the bottom, stop.
                stop_q_add(mk_loc(tempx,tempy+1)); //add (x,y+1) to Stop List
                f_list_remove(curr,prev);
                curr = prev;                    
                grid[(int)tempx][(int)tempy] &= ~MMASK; //turn off moving bit
            } else {  // Update the location in f_list for the next pass
                f_list[0][(int)curr] = mk_loc(tempx,tempy+1);
            }

            grid_swap(tempx,tempy,tempx,tempy+1);
        }
        if (curr == -1) { //if you get to the end, go back to the head.
            curr = f_head;               
        } else {
            prev = curr;
            curr = f_list[1][(int)curr];
            if (curr == -1) {
                curr = f_head;
                prev = -1;
            }
        }
    }
    return state_l;
}

/*  Function:   ins_user_column inserts a new user if there is 
    room. If there isn't room, it triggers the endgame state.  */

char ins_user_column(void) {
    unsigned char column = XSIZE / 2;
    if (grid[(int)column][0] || grid[(int)column][1] || grid[(int)column][2])
        return END;
    else {
        //select a random color code from 1 to 7. Set the moving bit.
        grid[(int)column][0] = ((get_rand() % 4) + 1) | MMASK;

        grid[(int)column][1] = ((get_rand() % 4) + 1) | MMASK;
        grid[(int)column][2] = ((get_rand() % 4) + 1) | MMASK;
        user = mk_loc(column,2);
    }
    return USERMOTION;
}

/****************************************************************
    Grid motion functions update the game grid and relay update 
    info to the SCI output buffer.
****************************************************************/

/*  Function:   grid_swap swaps the block info in two sqares of
    the grid and updates the FPGA board                        */

void grid_swap(unsigned char x1, unsigned char y1,
        unsigned char x2, unsigned char y2) {
    
    grid[(int)x1][(int)y1] += grid[(int)x2][(int)y2];
    grid[(int)x2][(int)y2] = grid[(int)x1][(int)y1] - grid[(int)x2][(int)y2];
    grid[(int)x1][(int)y1] = grid[(int)x1][(int)y1] - grid[(int)x2][(int)y2];

    //cmd_q_add(grid[(int)x1][(int)y1] & CMASK); //color byte
    //cmd_q_add(mk_loc(x1,y1)); //location byte

    //cmd_q_add(grid[(int)x2][(int)y2] & CMASK); //color byte
    //cmd_q_add(mk_loc(x2,y2)); //location byte

}

/*  Function:   square_clear removes a block from play, zeroing
    out the grid space.                                        */ 

void square_clear(unsigned char x, unsigned char y) {
    grid[(int)x][(int)y] = 0;
    //cmd_q_add(0); //color byte
    //cmd_q_add(mk_loc(x,y));

}


/****************************************************************
    The Command Queue is a circular queue that holds the two byte
    codes to be sent to the FPGA board.
****************************************************************/

/*  Function:   cmd_q_inc increments a counter modulus CQSIZE */
/*
char cmd_q_inc(unsigned char i) {
    int temp = CQSIZE;
    return (i + 1) % temp;
}
*/
/*  Function:   cmd_q_add adds a command byte to the command queue
                                                               */
/*
void cmd_q_add(LOCATION l) {
    if (cmd_q_inc(cq_in) != cq_out) {
        cmd_q[(int)cq_in] = l;
        cq_in = cmd_q_inc(cq_in);
    } else {
        //output an error message, the queue is full
    }
}
*/
/*  Function:   cmd_q_remove removes a byte from the removal 
queue index.                                                   */
/*
char cmd_q_remove(void) {
    if (cq_in == cq_out)
        return LOC_INVLD;
    else {
        char temp = cmd_q[(int)cq_out];
        cq_out = cmd_q_inc(cq_out);
        return temp;
    }
}
 */

/*  Function: cmd_q_is_empty returns 1 if the command queue is 
    empty and 0 if it is not.                                    */   
/*char cmd_q_is_empty(void) {
    return (cq_in == cq_out);
}
*/
/****************************************************************
    An 8-bit Galois linear feedback shift register provides 
    pseudo-random number generation. 
****************************************************************/

/*  Function:   rand_init takes a byte to seed the sequence. If
    0 is given, a default vaue is used.                        */
void rand_init(char i) {
    if (i == 0)
        rand_val = 0xE3;
    else
        rand_val = i; 
}

/*  Function:   get_rand returns a value from the register and 
    increments to the next value.                               */
 unsigned char get_rand(void) {
    char temp = (0x01&rand_val);
    rand_val >>= 1;
    rand_val ^= 0xE1 & (-temp);
    return rand_val;
}


/*  Function: grid_out Outputs the grid. Somehow.                */
SQUARE grid_out(int z) {
  int x = z%7 - 1;
  int y = z/7 -1;
  if (x == -1 || x > XSIZE || y == -1 || y > YSIZE)
    return 255;
  
  return grid[x][y] & CMASK; 
  
}

void updatescreen(void){
   int pos;
   char c;
   pos = 0;
      while (pos < 126) {
        //if ( pos == 0) 
          //make_white();
        if ( pos < 6 || pos > 118)
          bco(BORDERH);
        else if (pos == 6){
          
          bco(BORDERH);
          bco(LF);
          bco(CR);
        }else if (pos%7 == 0)
          bco(BORDERV);
        else if (pos%7 ==  6) {
          //make_white();
          bco(BORDERV);
          bco(LF);
          bco(CR);
        } else {
          
          c = grid_out(pos);
          if (c == 1) {
            //make_red();
            bco('#');
          } else if (c == 2) {
            //make_green();
            bco('@');
          } else if (c == 3) {
            //make_cyan();
            bco('%');
          } else if (c == 4) {
            //make_yellow();
            bco('&');
          } else
            //something bad happened.
            bco(' ');
        }
        pos++;
      }
      bco(LF);
      bco(CR);
                
}
/**************
void make_red() {
  bco('\x1b');
  bco('[');
  bco('3');
  bco('1');
  bco('m');  
}

void make_green() {
  bco('\x1b');
  bco('[');
  bco('3');
  bco('2');
  bco('m');  
}

void make_yellow() {
  bco('\x1b');
  bco('[');
  bco('3');
  bco('3');
  bco('m');  
}

void make_cyan() {
  bco('\x1b');
  bco('[');
  bco('3');
  bco('6');
  bco('m');  
}

void make_white() {
  bco('\x1b');
  bco('[');
  bco('3');
  bco('7');
  bco('m');  
}
****************/