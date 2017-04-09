// filename ******** FIFO.h ************** 
// Lab 9 Spring 2017
// Matt Owens & Rebecca Ho
// 4/7/17


//------------FIFO_Put------------
// Puts a new value into the FIFO array
// Input: value to put into array
// Output: false if FIFO is full and value cannot be
// put into array, true otherwise
bool FIFO_Put(uint32_t);

//------------FIFO_Get-----------
// Retrieves next value from the FIFO array
// Input: pointer to value
// Output: false if FIFO is empty, true otherwise
bool FIFO_Get(uint32_t*);
