
/*===os4Headers.h============================ 

contains prototypes for classes:
memoryNode, instruction, simulation
*/
//---Libraries--------------------------------
#include <iostream>
#include <fstream>
#include <queue>
#include <string>
#include <stdlib.h>
#include <iomanip> 

using namespace std;
//--------------------------------------------
//---ENUMS------------------------------------
enum state {FREE = 0, FULL = 1};
enum command {DEALLOCATE = 0, ALLOCATE = 1};
//--------------------------------------------
//---Formatting Constants------------------
const char A = 'A';
const char D = 'D'; 
const char Q = 'Q'; 
const char SP = ' ';
const int WDTH = 10; //format width
//=================================================
class memoryNode
{
public:
	//---variables-----
	int ID, size, addressBegin, addressEnd; //process data 
	state contents; //ENUM FREE or FULL
	
	//pointers to adjacent nodes in linked list
	memoryNode * prev; 
	memoryNode * next;    


	//---CTORs-----
	memoryNode(); //default FREE node 
	memoryNode(int myId, int mySize); //FULL node with data 

	//---functions-----
	bool nextIsFree(); //returns true if this->next = NULL, else false
	bool prevIsFree(); //returns true if this->prev = NULL, else false 

	//---DTOR------
	~memoryNode();
	
};
//=================================================
class instruction
{
public:
	//---variables-----
	command op; //ENUM ALLOCATE or DEALLOCATE
	int PID, size; //process id and size
	
	//---CTOR-----
	//instruction();
	instruction(command myOp, int myId, int mySize); //allocate instruction
	instruction(command myOp, int myId); //deallocate instruction
	
	//---DTOR-----
	~instruction();
	
};
//====================================================
class simulation
{
private: 
	//---variables---
	int MaxMemory; //highest mem index
	queue <instruction> InstQ; //holds all instructions for simulation
	int * skipArray; //holds skipped PIDs
	int skipArrayIndex; //size of array
	memoryNode * Head; //pointer to the head of the linked list

	//counters
	int numSkipped, //number of processes not allocated
		totalUsedMem, //tracks amount of memory currently in use
		totalFreeMem, //diference between max memory and total used
		numContigSpaces; //number of blocks of free memory at end of simulation
	float aveSizeFreeBlocks; //average size of memory blocks  


	//---funtions----
	memoryNode* findMemNode(memoryNode * currentNode, int wantedID); //searches linked list for memory node with corresponding PID.
																	//returns pointer to that node.
	void coalesceRight(memoryNode * centerNode); //combines center node with its next
	void coalesceLeft(memoryNode * centerNode); //combines center node with its prev
	bool IDwasSkipped(int lookingFor); //searches skipped array for input PID. If in array returns true, else returns false 

	void allocate(instruction me, memoryNode * currentNode); //searches linked list for next availiable space in memory and fills 
															//it with a node representing that process
															//if no space found, sends ID to skipArray
	void deallocate(int wantedID); //searches linked list for corresponding PID and sets that node to FREE
	
	void printBar(int wid); //prints "-" times width
	void getStats(memoryNode * thisNode); //traverses linked list and prints data for each node also tracks contiguous spaces

public: 

	//---CTOR-----
	//simulation(); 
	simulation(int memSize);
	
	//---functions---
	void readFile(); //asks user for input file, converts data in file to instructions and pushes them to instruction queue 
	void simulationGO(); //executes Allocate and deallocate instructions for simulation
	void printResults(); //prints table and simulation data to console
	
	//---DTOR-----
	~simulation();
};
//=====================================================