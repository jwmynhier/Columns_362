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
    if ((tempy + yoffset < YSIZE) && grid[(int)tempx+xoffset][(int)tempy+yoffset] == 0) {
        grid_swap(tempx,tempy,tempx+xoffset,tempy+yoffset);
        user = mk_loc(tempx+xoffset,tempy+yoffset);
        grid_swap(tempx,tempy-1,tempx+xoffset,tempy+yoffset-1);
        grid_swap(tempx,tempy-2,tempx+xoffset,tempy+yoffset-2);

        if (tempy+yoffset == YSIZE-1 || grid[tempx+xoffset][tempy+yoffset+1] != 0) {
            for (i=0; i<3; i++) {
                stop_q_add(mk_loc(tempx,tempy+yoffset-i));
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
    unsigned char tempx, tempy; 
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
                            tempy - j < YSIZE && tempy + j >= 0) {
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
                           (CMASK & grid[(int)tempx][(int)tempy]) == \
                           (CMASK & grid[(int)tempx+i+m][(int)tempy+j+n])) {
                           
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
        score_offset++;
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
SQUARE grid_out(int i) {
  int x = i%7 - 1;
  int y = i/18 -1;
  if (x == -1 || x > XSIZE || y == -1 || y > YSIZE)
    return 255;
  
  return grid[x][y] & CMASK; 
  
}