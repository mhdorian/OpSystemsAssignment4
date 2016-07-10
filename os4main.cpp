/* 
Operating Systems: Project 4 "Spacing Out"
Programmer: Meghan H Dorian
I, Meghan Dorian, have abided by the Wheaton Honor
Code in this work. 
----------------------------------------------------------------
PURPOSE: Simulate a first-fit memory allocation scheme for 
		 comparative performance analysis against other possible
		 schemes for the osX10LinWin operating system. 
INPUT: Text file containing instructions for the simulation in 
	   the form, <operation> <PID> <size>.
	     - Operations include: 'A'(allocate), 'D'(deallocate), or 'Q'(end of file)
	     - only PID will follow deallocate operations.
OUTPUT: (to console) 
	   (0) Formated memory table indicating process locations in memory at 
		   the end of the simulations
	   (1) List of processes which did not fit in memory
	   (2) Total number of skipped processes
	   (3) Total memory remaining
	   (4) number of free blocks of memory
	   (5) Average size of free memory blocks
-------------------------------------------------------------------
NOTES: Max memory is set to 1M or 2^20. If desired max
	   memory exceeds 10^8 digits, const char WDTH 
	  (in os4Headers.h) should be increased to properly 
	  format table. 
----------------------------------------------------
*/ 
//---Libraries--------------------------------
#include <iostream>
#include "os4Headers.h"
#include <math.h> 

using namespace std;
//------------------------------------------------
//---Constants------------------------------------
const int MEMORY_SIZE =  int (pow(2.0 , 20.0)); 
//==================================================
int main()
{
	//make new instance of simulation
	simulation mySim(MEMORY_SIZE);

	mySim.readFile();
	mySim.simulationGO();
	mySim.printResults();

	cout << "\nDone." << endl;

	return 0; 
}//---end main----
//==================================================