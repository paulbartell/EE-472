// MatrixKeypad.c
// Runs on LM3S1968
// Valvano
// August 2, 2011

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to the Arm Cortex M3",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2011
   
   Example 4.5, Section 4.10, Program 4.13 and Program 4.14
   Circuit Figure 4.42

 Copyright 2011 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
 
// columns interfaced to PA5-2
// rows interfaced to PD3-0


struct Row{
  unsigned char direction;
  unsigned char keycode[4];};
typedef const struct Row RowType;
RowType ScanTab[5]={
{   0x01, "123A" }, // row 0
{   0x02, "456B" }, // row 1
{   0x04, "789C" }, // row 2
{   0x08, "*0#D" }, // row 3
{   0x00, "    " }};

void MatrixKeypad_Init(void){ unsigned long volatile delay;
  SYSCTL_RCGC2_R |= 0x0009;        // enable Ports A and D
  delay = SYSCTL_RCGC2_R;
  GPIO_PORTA_DEN_R |= 0x3C;        // enable digital I/O on PA5-2
  GPIO_PORTA_DIR_R &= ~0x3C;       // make PA5-2 in (PA5-2 columns)
  GPIO_PORTD_DATA_R &= ~0x0F;      // DIRn=0, OUTn=HiZ; DIRn=1, OUTn=0
  GPIO_PORTD_DEN_R |= 0x0F;        // enable digital I/O on PD3-0
  GPIO_PORTD_DIR_R &= ~0x0F;       // make PD3-0 in (PD3-0 rows)
}

/* Returns ASCII code for key pressed,
   Num is the number of keys pressed
   both equal zero if no key pressed */
unsigned char MatrixKeypad_Scan(long *Num){
  RowType *pt;
  unsigned char column, key;
  short j;
  (*Num) = 0;
  key = 0;    // default values
  pt = &ScanTab[0];
  while(pt->direction){
    GPIO_PORTD_DIR_R = pt->direction; // one output
    GPIO_PORTD_DATA_R &= ~0x0F;   // DIRn=0, OUTn=HiZ; DIRn=1, OUTn=0
    for(j=1; j<=10; j++);         // very short delay
    column = ((GPIO_PORTA_DATA_R&0x3C)>>2);// read columns
    for(j=0; j<=3; j++){
      if((column&0x01)==0){
        key = pt->keycode[j];
        (*Num)++;
      }
      column>>=1;  // shift into position
    }
    pt++;
  }
  return key;
}
/* Waits for a key be pressed, then released
    Returns ASCII code for key pressed,
   Num is the number of keys pressed
   both equal zero if no key pressed */
unsigned char MatrixKeypad_In(void){ long n;
unsigned char letter;
  do{
    letter = MatrixKeypad_Scan(&n);
  } while (n != 1); // repeat until exactly one
  do{
    letter = MatrixKeypad_Scan(&n);
  } while (n != 0); // repeat until release
  return letter;
}
