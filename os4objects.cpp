
/*===os4objects.cpp============================ 

contains CTORS, DTOR, and methods for classes:
memoryNode, instruction, simulation
*/
//---Libraries--------------------------------
#include <iostream>
#include "os4Headers.h"

using namespace std;

//===== Memory Node ================
//---CTORS----
memoryNode::memoryNode()
{
	ID = 0;  
	size = 0; 

	addressBegin = 0; 
	addressEnd = 0; 
	
	contents = FREE; 
	
	prev = NULL; 
	next = NULL;
}
//--------------------------
memoryNode::memoryNode(int myId, int mySize)
{
	ID = myId;  
	size = mySize; 

	addressBegin = 0; 
	addressEnd = 0; 
	
	contents = FULL; 
	
	prev = NULL; 
	next = NULL;
}
//---DTOR-----
memoryNode::~memoryNode()
{} 
//--------------------------
//---Memory Node functions---------------
bool memoryNode::nextIsFree()
{
	return (this->next->contents == FREE); 
}
bool memoryNode::prevIsFree()
{
	return (this->prev->contents == FREE); 
}
//--------------------------------------
//======================================

//===== Instruction =====================
//---CTORs-----
instruction::instruction(command myOp, int myId, int mySize)
{
	op = myOp; 
	PID = myId;  
	size = mySize; 
}
//--------------------------
instruction::instruction(command myOp, int myId)
{
	op = myOp; 
	PID = myId;  
	size = 0; 
}
//--------------------------
//---DTOR-----
instruction::~instruction()
{}
//--------------------------
//===================================

//===== Simulation ===================
//---CTOR----
simulation::simulation(int memSize)
{
	MaxMemory = memSize;
	
	//initialize in simulationGO after filling InstQ
	//skipArray = new int[memSize]; 
	//skipArrayIndex = 0; //initialize in simafter get num instructions
	
	Head = new memoryNode();
	Head->size = memSize; 
	Head->addressEnd = memSize - 1;

	numSkipped = 0; 
	totalUsedMem = 0;  
	totalFreeMem = memSize;
	
	numContigSpaces = 0;
	aveSizeFreeBlocks = 0; 
}
//--------------------------
//---DTOR-----
simulation::~simulation()
{}
//--------------------------
//---simulation private functions-----------
memoryNode* simulation::findMemNode(memoryNode * currentNode, int wantedID)
{
	if (currentNode->ID == wantedID)
	{
		return currentNode; 
	}//end if found it!
	else if (currentNode->next == NULL)
	{
		return NULL; 
	}//end else if I'm the last node
	else
	{
		return (findMemNode(currentNode->next, wantedID)); 
	}//end else keep looking
}
//----------------------
void simulation::coalesceRight(memoryNode * centerNode)
{
	memoryNode * rightNode = centerNode->next; 

	//adjust values
	centerNode->size += rightNode->size; 
	centerNode->addressEnd = rightNode->addressEnd; 
	
	//reassign pointers
	rightNode->next->prev = centerNode; 
	centerNode->next = rightNode->next; 
	
	//remove right node 
	delete rightNode; 

} 
//-----------------------
void simulation::coalesceLeft(memoryNode * centerNode)
{
	memoryNode * leftNode = centerNode->prev; 
	
	//adjust values
	centerNode->size += leftNode->size; 
	centerNode->addressBegin = leftNode->addressBegin; 

	//reassign pointers
	leftNode->prev->next = centerNode; 
	centerNode->prev = leftNode->prev; 

	//remove left node
	delete leftNode; 
}
//----------------------
bool simulation::IDwasSkipped(int lookingFor)
{
	bool foundID = false; 
	for (int i = 0; i<skipArrayIndex; i++)
	{
		if(skipArray[i] == lookingFor)
		{
			return !foundID; 
		} //end if found id
	}//end for each element in array

	//if got here, did not find id
	return foundID; 
}
//-----------------------
void simulation::allocate(instruction me, memoryNode * currentNode)
{
	 bool mustSkip = false;
	 if (currentNode->contents == FREE)
	 {
	 	//check size
	 	if (currentNode->size > me.size) //yay! a bit big, but yay!
	 	{
	 		//make new node
			memoryNode * newNode = new memoryNode(me.PID, me.size);
			
			//set new pointers 
	 		newNode->prev = currentNode->prev;
	 		currentNode->prev = newNode; 
			newNode->next = currentNode; 

			//set new values
	 		newNode->addressBegin = currentNode->addressBegin; 
	 		newNode->addressEnd = newNode->addressBegin + (newNode->size -1); 
			currentNode->addressBegin = newNode->addressEnd + 1; 
			currentNode->size -= me.size; 

			totalUsedMem += me.size; 
			
			//check if new node is new head
			if(newNode->prev == NULL)
			{
				Head = newNode; 
			}//end if newNode is new head
			else
			{//need to link left's next pointer
				newNode->prev->next = newNode; 
			}

	 	}//end if found large free space
	 	else if (currentNode->size == me.size) //just right!
	 	{
	 		//fill info into current node
			currentNode->ID = me.PID; 
			currentNode->size = me.size; 
			currentNode->contents = FULL; 

			totalUsedMem += me.size;
	 	}//end else if found perfect free space
	 	else //too small! :(
	 	{
			if (currentNode->next != NULL)
			{
				allocate (me, currentNode->next); 
			}//end if there is another node
			else
			{
				mustSkip = true; 
			}//end else this free space was too small and is the last one
	 	}

	 }//end if this mem node is free
	 else 
	 {
	 	if (currentNode->next != NULL)
			{
				allocate (me, currentNode->next); 
			}//end if there is another node
			else
			{
				mustSkip = true; 
			}//end else this space is full and is the last one
	 }//end else mem node if not free 

	 if (mustSkip) 
	 {
		 skipArray[skipArrayIndex] = me.PID; 
		 skipArrayIndex++; 
	 }//end store skipped Id in array. 

}//end allocate
//--------------------------
void simulation::deallocate(int wantedID)
{
	memoryNode * foundNode = findMemNode(Head, wantedID); 

	if (foundNode != NULL)
	{
		//empty ID and contents of found node 
		//keep size, node pointers, and mem addresses  
		foundNode->ID = 0; 
		foundNode->contents = FREE; 

		totalUsedMem -= foundNode->size; 
		
		//coalescing
		if (foundNode->nextIsFree())
		{
			coalesceRight(foundNode); 
		} 
		
		if (foundNode->prevIsFree())
		{
			coalesceLeft(foundNode); 
		}

	}//end if found node
	else //node not found
	{
		cout << "Error: no instance of " << wantedID << endl; 
	}//end else bad deallocation! 
}
//--------------------------
void simulation::printBar(int wid)
{
	string line = ""; 
	for (int j = 0; j < wid; j++)
	{
		line = line + "-"; 
	}
	cout << right << setw(wid) << line << endl; 
}
//----------------------------
void simulation::getStats(memoryNode * thisNode)
{
	do{
		if (thisNode->contents == FULL)
		{
			cout << right << setw(WDTH) << thisNode->ID \
			<< right << setw(WDTH) << thisNode->addressBegin \
			<< right << setw(WDTH) << thisNode->addressEnd \
			<< right << setw(WDTH) << thisNode->size << endl; 
		} //end if this node represents a process


		else
		{
			cout << right << setw(WDTH) << "Free" \
			<< right << setw(WDTH) << thisNode->addressBegin \
			<< right << setw(WDTH) << thisNode->addressEnd \
			<< right << setw(WDTH) << thisNode->size << endl;
			//count free space
			numContigSpaces ++; 

		}//end else this is a free block

		thisNode = thisNode->next; //look at next node
	}while(thisNode != NULL); //end while not at end of linked list
}
//--------------------------
//---simulation public functions-------------
void simulation::readFile()
{
	ifstream FIN; //fstream variable
	string infileName; //name of input file

	//fetch file name
	cout << "Enter input file: " << endl;
	cin >> infileName; 

	//attempt to open
	FIN.open(infileName.c_str()); 

	//if failure to open
	if (FIN.fail())
	{
		cout << "Error: did not find file " << infileName << endl;
		exit (-1); 
	}//end if fail to open
	else
	{
		//got file
		char thisChar; 
		int thisID, thisSize; 

		
		//fetch first char in line (delimitor = SP)
		FIN >> thisChar;
		while (thisChar != Q)
		{
			//getline(FIN, thisChar, SP);
			if (thisChar == A)
			{
				//fetch ID and size
				FIN >> thisID >> thisSize;    
				//make new instruction and push to instructuion queue
				instruction newInstruct(ALLOCATE, thisID, thisSize); 
				InstQ.push(newInstruct);   
			}//end if allocating
			else if (thisChar == D)
			{
				//fetch ID
				FIN >> thisID; 
				//make new instruction and push to queue
				instruction newInstruct(DEALLOCATE, thisID); 
				InstQ.push(newInstruct); 
			}//end else if deallocating
			else
			{
				cout << "Error! bad input: no command for " << thisChar << endl; 
			}//end else error

			//getline(FIN, thisChar, SP);
			FIN >> thisChar; // >> thisID >> thisSize;
		}//end while thisChar not Q

		//close file
		FIN.close(); 


	}//end else got file

}//end readFile()
//--------------------------
void simulation::simulationGO()
{	
	//initialize array here. Most possible number of skipped 
	//allocations = number of instructions in simulation(size of queue)
	skipArray = new int[InstQ.size() + 1]; 
	skipArrayIndex = 0; 

	while(!InstQ.empty())
	{
		instruction currentInstruct = InstQ.front();//get next instruction 
		if (currentInstruct.op == ALLOCATE)
		{
			//check if enough memory
			if(totalUsedMem < MaxMemory)
			{
				allocate(currentInstruct, Head); 
			}//end if there is enough memory
			else
			{
				//send id to skip array
				skipArray[skipArrayIndex] = currentInstruct.PID; 
				skipArrayIndex ++;
			}//end else not enough mem to allocate

		}//end if allocating
		else //deallocating
		{
			if( !IDwasSkipped(currentInstruct.PID) )
			{
				deallocate(currentInstruct.PID);

			}//end if id was not skipped 

			//else ignore instruction
		}//end else deallocating

		InstQ.pop(); //remove instruction from queue 

	}//end while instruction queue not empty

}
//end simulationGo!
//-------------------------
void simulation::printResults()
{
	//calculating stats
	numSkipped = skipArrayIndex; 
	//totalUsedMem kept track of through sim
	totalFreeMem -= totalUsedMem; 

	//print header
	cout << right << setw(2*WDTH +6) << "Memory Table\n" << endl; 
	cout << right << setw(WDTH) << "PID" \
	<< right << setw(WDTH) << "BEGIN" \
	<< right << setw(WDTH) << "END" \
	<< right << setw(WDTH) << "SIZE" << endl; 
	
	printBar(WDTH*4); 

	getStats(Head); //traverse and print data from each node 
	
	//check for full memory
	if (totalFreeMem > 0)
	{
		aveSizeFreeBlocks = totalFreeMem / numContigSpaces;
	}//end if mem completly filled
	else
	{
		aveSizeFreeBlocks = 0; 
	}//end else no blocks
	
	printBar(WDTH*4); 

	//display stats
	cout << "\nSkipped Processes: " << endl; 
	for (int i=0; i < numSkipped; i++)
	{
		cout << skipArray[i] << endl;
	}
	cout << "Total skipped: " << numSkipped << endl;

	cout << "\nMemory in Use: " << totalUsedMem << " spaces." << endl;
	cout << "Memory Remaining: " << totalFreeMem << " spaces." << endl; 
	cout << "Number of Blocks: " << numContigSpaces << endl; 
	cout << "Average Size of Memory Blocks: " << aveSizeFreeBlocks << endl;
}
//--------------------------
//===================================
