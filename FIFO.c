// filename ******** FIFO.c ************** 
// Lab 9 Spring 2017
// Matt Owens & Rebecca Ho
// 4/7/17

#include <stdint.h>
#include <stdbool.h>

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

#define FIFO_SIZE 1000

uint32_t Size = 0;
uint32_t FIFO[FIFO_SIZE];
uint16_t PutI = 0;
uint16_t GetI = 0;

bool FIFO_Put(uint32_t data) {
	DisableInterrupts();
	if(Size == FIFO_SIZE) {		// FIFO is full
		return false;
	}
	FIFO[PutI] = data;
	PutI++;
	PutI = PutI%FIFO_SIZE;
	Size++;
	EnableInterrupts();
	return true;
}

bool FIFO_Get(uint32_t* data) {
	DisableInterrupts();
	if(GetI == PutI) { 	// nothing new in FIFO
		return false;
	}
	*data = FIFO[GetI];
	GetI++;
	GetI = GetI%FIFO_SIZE;
	Size = Size-1;
	EnableInterrupts();
	return true;
}
