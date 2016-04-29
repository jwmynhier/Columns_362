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


***********************************************************************
*/

#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include <mc9s12c32.h>

/* All functions after main should be initialized here */
char inchar(void);
void outchar(char x);
void send_i(char x);
void shiftout(char x);
void lcdwait(void);
void send_byte(char x);
void chgline(char x);
void printmsg(char x[]);
void print_c(char x);
int size(char x[]);
void introdisp(void);
void scoredisp(void);



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
char P1Lb = 1; //Raw button inputs
char P1Rb = 1;
char P1Cb = 1;
char P1Db = 1;
char P1Sb = 1;
char P2Lb = 1;
char P2Rb = 1;
char P2Cb = 1;
char P2Db = 1;
char P2Sb = 1; 
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

char tin	= 0;	// SCI transmit display buffer IN pointer
char tout	= 0;	// SCI transmit display buffer OUT pointer   	   			 		  			 		       
char sciread;
#define TSIZE 81	// transmit buffer size (80 characters)
char tbuf[TSIZE];	// SCI transmit display buffer
char outputchar;


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
  SCIBDH =  0x00; //set baud rate to 9600
  SCIBDL =  0x9C; //24,000,000 / 16 / 156 = 9600 (approx)  
  SCICR1 =  0x00; //$9C = 156
  SCICR2 =  0x0C; //initialize SCI for program-driven operation
  DDRB   =  0x10; //set PB4 for output mode
  PORTB  =  0x10; //assert DTR pin on COM port

/* Initialize peripherals */
  //ATD as Inputs
  DDRAD = 0x00;
  ATDDIEN = 0xFF;
  
  
    //Input Pins initialization
  DDRT = 0xC0;
  DDRM = 0xFF;
  
            
/* Initialize interrupts */
  //SPI 6Mbs
  SPICR1 = 0x50;
  SPICR2 = 0x00;
  SPIBR =  0x01;

  //RTI 2.048 ms interrupt rate
  CRGINT = 0x80;
  RTICTL = 0x1F;
  	      
	//TIM  10ms timer
	
	TIOS = 0x80;
  TSCR1 = 0x80;
  TSCR2 = 0x0C;
  TC7 = 15000;
  TIE = 0x80;      

  
  
  
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
  DisableInterrupts
	initializations(); 		  			 		  		
	EnableInterrupts;

  introdisp(); 


 for(;;) {
  
 
  
  //GAME RUNNING
  if(runstop == 1){
   //GAME RUNNING LOGIC HERE 
    
  }
  
    
  
  //RESET
  if(reset == 1){
   reset = 0;
   DisableInterrupts
	 initializations(); 		  			 		  		
	 EnableInterrupts;
	 //FUNCTION FOR INTRO STUFF
	 //RESET TO PAPILIO ONE 
    
  }
  
  //One Second Update
  if(onesec == 1){
   onesec = 0;
   time++;
   
   //which display to use
   if(runstop == 1){
    scoredisp();
   }
   
  }
  
  
/***********************************************************************
Controller Buttons Logic Section
************************************************************************/
 
  //P1L Pressed Logic
  if(tenthsflag == 1){
    tenthsflag = 0;
  
  if(P1L == 1){
    P1L = 0;
    send_i(LCDCLR);
    lcdwait();
    printmsg("P1L");
  }
  
  //P1R Pressed Logic
  if(P1R == 1){
    P1R = 0;
    send_i(LCDCLR);
    printmsg("P1R");
  }  

  //P1D Pressed Logic
  if(P1D == 1){
    P1D = 0;
    send_i(LCDCLR);
    printmsg("P1D");
  }
  
  //P1C Pressed Logic
  if(P1C == 1){
    P1C = 0;
    send_i(LCDCLR);
    printmsg("P1C");
  }
    
  //P1S Pressed Logic
  if(P1S == 1){
    P1S = 0;
    send_i(LCDCLR);
    printmsg("P1S");
  }
    
  //P2L Pressed Logic
  if(P2L == 1){
    P2L = 0;
    send_i(LCDCLR);
    printmsg("P2L");
  }
  
  //P2R Pressed Logic
  if(P2R == 1){
    P2R = 0;
    send_i(LCDCLR);
    printmsg("P2R");
  }  

  //P2D Pressed Logic
  if(P2D == 1){
    P2D = 0;
    send_i(LCDCLR);
    printmsg("P2D");
  }
  
  //P2C Pressed Logic
  if(P2C == 1){
    P2C = 0;
    send_i(LCDCLR);
    printmsg("P2C");
  }
    
  //P2S Pressed Logic
  if(P2S == 1){
    P2S = 0;
    send_i(LCDCLR);
    lcdwait();
    printmsg("P2S");
  }  

   //START GAME LOGIC

  }
   } /* loop forever */
   
}   /* do not leave main */




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
    //P1Rb = PORTAD0_PTAD1;
    //P1Db = PORTAD0_PTAD2;
    //P1Cb = PORTAD0_PTAD3;
    //P1Sb = PORTAD0_PTAD4;
    //P2Lb = PORTAD0_PTAD5;
    //P2Rb = PORTAD0_PTAD6;
    //P2Db = PORTAD0_PTAD7;
    //P2Cb = PTT_PTT0;
    P2Sb = PTT_PTT1;
 
 
    //P1 Button Checks
    if(P1Lb == 0){
     if(prevP1L == 1){
      prevP1L = 0;
      P1L = 1;
     }else{
      prevP1L = 0;
     }
    }else{
     prevP1L = 1;
    }

    if(P1Rb == 0){
     if(prevP1R == 1){
      prevP1R = 0;
      P1R = 1;
     }      
     prevP1R = 1;
    }
    
    if(P1Cb == 0){
     if(prevP1C == 1){
      prevP1C = 0;
      P1C = 1;
     }      
     prevP1C = 1;
    }

    if(P1Db == 0){
     if(prevP1D == 1){
      prevP1D = 0;
      P1D = 1;
     }      
     prevP1D = 1;
    }

    if(P1Sb == 0){
     if(prevP1S == 1){
      prevP1S = 0;
      P1S = 1;
     }      
     prevP1S = 1;
    }

 //P2 Button Checks
    if(P2Lb == 0){
     if(prevP2L == 1){
      prevP2L = 0;
      P2L = 1;
     }      
     prevP2L = 1;
    }

    if(P2Rb == 0){
     if(prevP2R == 1){
      prevP2R = 0;
      P2R = 1;
     }      
     prevP2R = 1;
    }
    
    if(P2Cb == 0){
     if(prevP2C == 1){
      prevP2C = 0;
      P2C = 1;
     }      
     prevP2C = 1;
    }

    if(P2Db == 0){
     if(prevP2D == 1){
      prevP2D = 0;
      P2D = 1;
     }      
     prevP2D = 1;
    }

    if(P2Sb == 0){
     if(prevP2S == 1){
      prevP2S = 0;
      P2S = 1;
     }else{
      prevP2S = 0;
     }
    }else{
     prevP2S = 1;
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
 tenthsflag = 1;
 tenths++;
 if(tenths == 10){
  tenths = 0;
  onesec = 1;
  
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
 //char score2hunds = (score2 / 100) + 0x30;       ADD IF 2 PLAYER
 //char score2tens  = ((score2 / 10) % 10) + 0x30;
 //char score2ones  = (score2 % 10) + 0x30;
 
 //Display first line - Players and Time Elapsed
 send_i(LCDCLR);
 lcdwait();
 printmsg("P1   ");
 print_c(timemintens);
 print_c(timeminones);
 print_c(':');
 print_c(timesectens);
 print_c(timesecones);
 //printmsg("    P2");   ADD IF 2 PLAYER
 
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
 //print_c(score2hunds);
 //print_c(score2tens);
 //print_c(score2ones);
  
  
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
  for(count = 10; count>0; count--){
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