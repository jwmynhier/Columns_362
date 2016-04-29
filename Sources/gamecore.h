#include <stdlib.h> 

//set the size of the game grid
//at [5,16] X can be stored in 3 bits and Y can be stored in 4 bits
//which lets the coordinates be stored in 1 byte.
#define XSIZE 5
#define YSIZE 16 

//Allows data to be stored in a larger datatype if more bits
//are needed.
#define SQUARE unsigned char 

//LOCATION
#define XMASK 0xE0
#define YMASK 0x1F
#define YBITS 5

//SQUARE
#define MMASK 0x08 //mask for the motion bit
#define CMASK 0x07 //mask for the color bits
#define DMASK 0x10 //mask for a block that will be deleted




/****************************************************************
    The User Column is the user controled column of 3 squares. It
    can move down, left or right. When it stops moving down, user
    control is ended.
****************************************************************/

#define LOCATION unsigned char
#define LOC_INVLD 0xFF //cannot occur in grid




/****************************************************************
    The Falling List is a linked list that holds the location of
    any moving blocks not controlled by the user. 
****************************************************************/

#define FLSIZE XSIZE * YSIZE


/****************************************************************
    The Stopped Queue is a circular queue that holds the location
    of blocks that have just stopped moving.
****************************************************************/

#define SQSIZE XSIZE * YSIZE



/****************************************************************
    The Removal Queue is a circular queue that holds the location
    of blocks that need to be cleared.
****************************************************************/

#define RQSIZE XSIZE * YSIZE



/****************************************************************
    Clock Tick States are the functions, selected by a state var-
    iable, that execute game actions. One clock tick runs just one
    function, then sets state so the next tick will execute a 
    specific function.
****************************************************************/

//state codes
#define USERMOTION 1
#define STPANDCHK 2
#define MOTION 3
#define INSCOLUMN 4
#define START 5       //Should be default initialization
#define END 6 



/*************************Function declarations*****************/

/*                        Grid Manipulation Functions          */

/*  Function:   grid_init zeros out the grid                   */
void grid_init(void);

/*  Function:   get_x returns the x coordinate of a location    */
unsigned char get_x (LOCATION l);

/*  Function:   get_y returns the y coordinate of a location    */
unsigned char get_y (LOCATION l);

/*  Function:   is_mov returns true if the moving bit is set
    and false otherwise                                        */
unsigned char is_mov(SQUARE s);

/*  Function:   set_y changes the value of the y bits     */   
LOCATION set_y(LOCATION l, LOCATION y);

/*  Function:   set_x changes the value of the x bits     */   
LOCATION set_y(LOCATION l, LOCATION x);

/*  Function:   mk_loc takes a raw x, y pair, returns a location*/   
LOCATION mk_loc(LOCATION x, LOCATION y);

/*  Function:   gc_init initializes the game core.             */
void gc_init(char i);


/*                            Falling List Functions           */

/*  Function:   f_list_init sets the initial "pointer" values  */
void f_list_init(void);

/*  Function:   f_list_add add a new value to the list         */
void f_list_add(LOCATION l);

/*  Function:   f_list_remove remove the node at the current
    index.                                                     */
void f_list_remove(char curr, char prev);



/*                            Stopped List Functions           */

/*  Function:   stop_q_inc increments a counter modulus SQSIZE */
char stop_q_inc(unsigned char i);

/*  Function:   stop_q_add adds a location to the stop queue
    index.                                                     */
void stop_q_add(LOCATION l);

/*  Function:   stop_q_remove removes a location from the stop
queue index.                                                   */
LOCATION stop_q_remove(void);



/*                                Remove Queue Functions       */

/*  Function:   remove_q_inc increments a counter modulus RQSIZE */
char remove_q_inc(unsigned char i);

/*  Function:   remove_q_add adds a location to the removal queue
    index.                                                       */
void remove_q_add(LOCATION l);

/*  Function:   remove_q_remove removes a location from the removal 
queue index.                                                     */
LOCATION remove_q_remove(void);



/*                                Tick State Functions          */

/*  Function:   user_motion checks user input flags and moves the
    user column accordingly. If the user column is stopped, it 
    will set state for the Adjacency Check, otherwise it will 
    continue with another User Motion tick.                    */

char user_motion(char uleft, char uright, char udown, char uswap);

/*  Function:   stop_and_check dequeues locations from the Stopped
    Queue and checks for a match of three of a kind in a straight
    line from the block being checked. The score is set according-
    ly. Any matching blocks are added to the Removal Queue. After
    all checks are done, it loops through the Removal Queue and 
    deletes them, adding all blocks above a deleted block to the 
    Falling List.                                              */

char stop_and_check(void); 

/*  Function:   motion moves the squares in the Falling List down
    until they hit the bottom of the screen or a square that isn't
    moving.                                                    */

char motion(void);

/*  Function:   ins_user_column inserts a new user if there is 
    room. If there isn't room, it triggers the endgame state. 
    Initializations must seed random number generator          */

char ins_user_column(void);

/****************************************************************
    Grid motion functions update the game grid and relay update 
    info to the SCI output buffer.
****************************************************************/

/*  Function:   grid_swap swaps the block info in two sqares of
    the grid and updates the FPGA board                        */
void grid_swap(unsigned char x1, unsigned char y1,
        unsigned char x2, unsigned char y2);

/*  Function:   square_clear removes a block from play, zeroing
    out the grid space.                                        */ 

void square_clear(unsigned char x, unsigned char y);


/****************************************************************
    The Command Queue contains the two-byte commands that will be
    shifted out to the FPGA board and used to update the VGA
    display.
    Bit Map:
        Color Byte:     Sc->score bit, C->color bit
            [Sc:_:_:_:_:C:C:C]
        Location Byte:  X->x position bit, Y->y position bit
            [X:X:X:Y:Y:Y:Y:Y]

    If the Score Bit is set, the Location byte is read as an 
    offset to the score (score += location byte).
****************************************************************/
/*
#define CQSIZE 2 * XSIZE * YSIZE
#define SCMASK 0x80  //Mask for the score bit.
*/
/*  Function:   cmd_q_inc increments a counter modulus CQSIZE */

//char cmd_q_inc(unsigned char i);

/*  Function:   cmd_q_add adds a location to the command queue
    index.                                                       */
/*
void cmd_q_add(LOCATION l);
*/
/*  Function:   cmd_q_remove removes a location from the command 
queue index.                                                     */
/*
char cmd_q_remove(void);
*/
/*  Function: cmd_q_is_empty returns 1 if the command queue is 
    empty and 0 if it is not.                                    */   
/*
char cmd_q_is_empty(void);
*/

/****************************************************************
    An 8-bit Galois linear feedback shift register provides 
    pseudo-random number generation. 
****************************************************************/

/*  Function:   rand_init takes a byte to seed the sequence. If
    0 is given, a default vaue is used.                        */
void rand_init(char i);

/*  Function:   get_rand returns a value from the register and 
    increments to the next value.                               */
unsigned char get_rand(void);

/*  Function: grid_out Outputs the grid. Somehow.                */
SQUARE grid_out(int z);