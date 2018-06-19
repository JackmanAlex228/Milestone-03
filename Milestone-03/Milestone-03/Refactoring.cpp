// Refactoring.cpp : Defines the entry point for the console application.
/*
I/O:
READ        10    read into memory
Write       11
Load/store operations
LOAD        20    load from memory location into accumulator
STORE       21
Arithmetic operation
ADD         30    number in memory location + number in accumulator (keep result in accumulator)
SUB         31
DIV         32
MULT        33
REMAINDER   34
EXPONENT    35
Control operations
BRANCH      40    branch into specific location in memory
BRANCHNEG   41
BRANCHZERO  42
HALT        43
Debugging operations
MEMDUMP            print the memory image on the screen
Terminate        -99999
*/

//You may have to comment this line if you're using MacOS.
#include "stdafx.h"

#include <iostream>
#include <string>
#include <vector>
#include <iterator>
#include <array>
#include <iomanip>
#include <cstdlib>


using namespace std;

#define MIM_DIGITS 3
#define MAX_DIGITS 5
#define NUM_ZERO 0
#define MEMORY_SIZE 1000
#define TERMINATE -99999
#define PROGRAMEND -11111


int calculateOperations(int command, int location);
string fixMyString(string rawValue);
string fixMyString(int rawValue);
int instructCount = 0;
int accumulator = 0;

//Pseudo-accums to hold data for use in main accumulator
int accumOneHolder = 0;
int accumTwoHolder = 0;

//To hold the operation value until the for-loop loops for the memory loc.
int operationHolder = 0;

//To keep track of our place in each program.
int programOneCtr;
int programTwoCtr;

//Where the -11111 split is.
int programTwoStartLoc = 0;

//To signal Program 1 or 2 is done.
bool isProgramOneDone;
bool isProgramTwoDone;

//To disallow additional -11111 codes after one has been used.
bool isProgramEndFlagged = false;


int instructRegisterDisplay = 0;
int operationCodeDisplay = 0;
int operandDisplay = 0;


//Menu Object for outputting the options available
class Menu
{
private:
	int code;
	string name;
public:
	Menu(int c, string n) : code(c), name(n) {};
	Menu(const Menu &m) : code(m.code), name(m.name) { }

	//The numerical operation code (i.e. 10)
	void setCode(int c) {
		this->code = c;
	}

	//The name of the operation (i.e. STORE)
	void setName(string n) {
		this->name = n;
	}

	//The numerical operation code (i.e. 10)
	int getCode() {
		return code;
	}

	//The name of the operation (i.e. STORE)
	string getName() {
		return name;
	}
};

//Startup to output the possible operations available 
//for end user to use.-Scott's take on this method
void showMenu(vector<Menu> &v)
{
	for (int i = 0; i < v.size(); i++)
	{
		cout << v.at(i).getCode() << " - " << v.at(i).getName() << endl;
	}

}

//Class to build Instruction objects, which contain an operation and an operand, 
//also a Value for outputting at the end -Scott's take on this method
class Instruction
{
private:
	int operation = 0;
	int operand = 0;
	int value = 0;
public:
	Instruction(int opt, int opn) : operation(opt), operand(opn) {};
	Instruction(int opt, int opn, int v) : operation(opt), operand(opn), value(v) {};
	Instruction(const Instruction &i) : operation(i.operation), operand(i.operand), value(i.value) { }

	void setOperation(int opt) {
		this->operation = opt;
	}

	void setOperand(int opn) {
		this->operand = opn;
	}

	void setValue(int v) {
		this->value = v;
	}

	int getOperation() {
		return this->operation;
	}

	int getOperand() {
		return this->operand;
	}

	int getValue() {
		return this->value;
	}
};

//Output that shows the instruction count, most recent memory value, its instructions, and all 1000 memory locations. 
void memoryDump(vector<Instruction> &memory)
{
	string operationDisplayCheck = to_string(operationCodeDisplay);
	if (operationDisplayCheck.length() < 2)
	{
		operationDisplayCheck = "0" + operationDisplayCheck;
	}

	string operandDisplayCheck = to_string(operandDisplay);
	if (operandDisplayCheck.length() < 3)
	{
		if (operandDisplayCheck.length() < 2)
		{
			operandDisplayCheck = "00" + operandDisplayCheck;
		}
		else
		{
			operandDisplayCheck = "0" + operandDisplayCheck;
		}

	}
	string operationOperandCombined = operationDisplayCheck + operandDisplayCheck;

	cout << "*** PROGRAM COMPLETED ***" << endl;

	// Print out all the basic information summary
	cout << "\n \n REGISTERS:" << endl
		<< setw(20) << left << "Accumulator:" << setw(10) << right << accumulator << endl
		<< setw(20) << left << "InstructionCounter:" << setw(10) << right << instructCount << endl
		<< setw(20) << left << "InstructionRegister:" << setw(10) << right << operationOperandCombined << endl
		<< setw(20) << left << "OperationCode:" << setw(10) << right << operationCodeDisplay << endl
		<< setw(20) << left << "Operand:" << setw(10) << right << operandDisplayCheck << endl << endl;

	// Print all the memory headers
	cout << "\n\nMEMORY:\n";
	cout << setw(9.5) << "00" << setw(9.5) << "01" << setw(9.5) << "02" << setw(9.5)
		<< "03" << setw(9.5) << "04" << setw(9.5) << "05" << setw(9.5) << "06"
		<< setw(9.5) << "07" << setw(9.5) << "08" << setw(9.5) << "09" << endl;

	//Print each row, pulling the data from main memory, checking format, and outputting it to the screen.	
	for (int column = 0; column < MEMORY_SIZE; column++)
	{
		cout << setfill('0') << setw(3) << column << setfill(' ') << setw(9.5);
		for (int row = 0; row < 10; row++)
		{
			if (memory.at(row + column).getValue() == 0)
			{
				if (memory.at(row + column).getOperation() != 0 || memory.at(row + column).getOperand() != 0)
				{
					string operationMemDumpDisplayCheck = to_string(memory.at(row + column).getOperation());
					if (operationMemDumpDisplayCheck.length() < 2)
					{
						operationMemDumpDisplayCheck = "0" + operationMemDumpDisplayCheck;
					}

					string operandMemDumpDisplayCheck = to_string(memory.at(row + column).getOperand());
					if (operandMemDumpDisplayCheck.length() < 3)
					{
						if (operandMemDumpDisplayCheck.length() < 2)
						{
							operandMemDumpDisplayCheck = "00" + operandMemDumpDisplayCheck;
						}
						else
						{
							operandMemDumpDisplayCheck = "0" + operandMemDumpDisplayCheck;
						}

					}
					string operationOperandMemDumpCombined = operationMemDumpDisplayCheck + operandMemDumpDisplayCheck;
					cout << operationOperandMemDumpCombined << setw(9.5);
				}
				else
				{
					cout << "00000" << setw(9.5);
				}
			}
			else
			{
				string memoryValueString = fixMyString(memory.at(row + column).getValue());
				cout << memoryValueString << setw(9.5);
			}
		}
		cout << endl;
		column += 9;
	}
}

//Checks to see if the input from the keyboard was 4 digits? 
//(what do we do with the + and - signs besides on -99999?)-Scott's take on this method
void parseInput(string &input, vector<Menu> &vMenu, vector<Instruction> &vMemory, bool &menuQuit)
{
	// operation is the command and operand is the location
	int operation;
	int operand;

	// checking if the input is 0, greater than 0, or if it is terminate command
	if (stoi(input) >= NUM_ZERO)
	{
		// checking if the input has 5 digits
		if (input.length() > MIM_DIGITS && input.length() < MAX_DIGITS) {

			//For when users want to intentionally make a memory instruction 
			//spot blank(usually that spot is used for storing data later)
			if (stoi(input) == NUM_ZERO)
			{
				vMemory.push_back(Instruction(0, 0));
				operationHolder = -1;
			}
			//Operation
			else if (input.at(0) != '0')
			{
				if (operationHolder == 0 || operationHolder == -1)
				{
					operation = stoi(input.substr(0, 2));

					//Checking if the command (variable operation) exists in the Menu vector,
					//If it exists, store in mainMemory and flag it as a good instruction.
					bool isInstructionValid = false;
					for (int menuCheck = 0; menuCheck < vMenu.size(); menuCheck++)
					{
						if (vMenu.at(menuCheck).getCode() == operation)
						{
							//cout << "Operation entered is valid." << endl;
							if (operation != 43)
							{
								operationHolder = operation;
							}
							//Halt has no memory required, so skipping requirements for a memory location
							else
							{
								vMemory.push_back(Instruction(operation, 0));
								operationHolder = -1;
							}

							isInstructionValid = true;
							instructCount++;
						}
					}
					if (!isInstructionValid)
					{
						cout << "Invalid operation. Please try again." << endl;
					}
				}
				else
				{
					cout << "Invalid input. Please enter a Memory Location for the previous Operation." << endl;
				}
			}
			//Memory Location
			else if (input.at(0) == '0')
			{
				if (stoi(input) <= 999)
				{
					operand = stoi(input.substr(1, 3));

					if (operationHolder != 0)
					{
						vMemory.push_back(Instruction(operationHolder, operand));

						//Reset OperationHolder
						operationHolder = 0;
					}
					//For Halt, -11111, and 0000 inputs.
					else if (operationHolder == -1)
					{
						operationHolder = 0;
					}
					else
					{
						cout << "No Matching Operation for this Memory Location." << endl;
						cout << "Please enter a valid Operation before a Memory Location." << endl;
					}
				}
				else
				{
					cout << "Invalid memory location. Please try again." << endl;
				}
			}
		}
		else {
			cout << "Invalid instruction length. Please try again." << endl;
		}

	}
	else if (stoi(input) == TERMINATE) { // -99999
		std::cout << "Instruction input complete.\n";
		menuQuit = true;
	}
	else if (stoi(input) == PROGRAMEND) { // -11111
		if (isProgramEndFlagged == false)
		{
			std::cout << "Program Instructions complete. \n";
			std::cout << "Please enter the next Program's instructions,\n";
			std::cout << "or type -99999 to execute instructions.\n";

			vMemory.push_back(Instruction(0, 0));
			vMemory.back().setValue(PROGRAMEND);

			//Where -11111 was placed in memory as a starting point to Program 2.
			programTwoStartLoc = vMemory.size();
			operationHolder = -1;
			isProgramEndFlagged = true;
		}
		else
		{
			cout << "Program End has already been used. This Program allows a maximum of two programs at once." << endl;
		}
	}
	else
	{
		cout << "Invalid input. Please try again." << endl;
	}
}

//Initial loop thru our memory array.
//Calls overloaded method to determine and execute the instructions.
void Calculate_Operations(vector<Instruction> &memory) {

	string inputOne;
	string inputTwo;

	//Setting up Counters
	programOneCtr = 0;
	programTwoCtr = programTwoStartLoc;

	//Setting program done flags
	isProgramOneDone = false;
	isProgramTwoDone = false;

	//Loop through memory array, each respective counter works until bool flag saying they're done..
	while (isProgramOneDone == false || isProgramTwoDone == false)
	{

		//Program 1
		if (isProgramOneDone == false && programOneCtr < MEMORY_SIZE && (programOneCtr < programTwoStartLoc || memory.at(programOneCtr).getValue() != PROGRAMEND))
		{
			int command = memory.at(programOneCtr).getOperation();
			int memoryLocation = memory.at(programOneCtr).getOperand();

			//For Display purposes in memoryDump() 
			operationCodeDisplay = command;
			operandDisplay = memoryLocation;

			//To determine which Command has been sent, and execute calculations. -Scott
			switch (command) {

				// Read a word from the keyboard into a specific location in memory.
			case 10: {

				//Input loop, if they give a value bigger than 4 digits or leave it blank
				do {
					cout << "Program One: Please enter a number between 1 and 99999: ";
					cin >> inputOne;

				} while (inputOne.length() > 5 || inputOne.length() == 0);

				memory.at(memoryLocation).setValue(stoi(inputOne));
				//cout << "Saving to Memory Location: " << memoryLocation << endl;
				break;
			}
					 // Write output to screen.
			case 11: {

				//Need to work on how to display negative values.
				string outputStyle = fixMyString(memory.at(memoryLocation).getValue());
				cout << "Program One: Contents of Memory Location " << fixMyString(memoryLocation) << ": " << outputStyle << endl;
				break;
			}
					 //Load a word from a specific location in memory into the accumulator.
			case 20: {

				accumOneHolder = memory.at(memoryLocation).getValue();
				//cout << "Loaded " << memory.at(memoryLocation).getValue() << " from Memory Location " << memoryLocation << " into the Accumulator." << endl;
				break;
			}
					 //Store from accumulator to main memory
			case 21: {

				memory.at(memoryLocation).setValue(accumOneHolder);
				//cout << accumulator << " loaded from Accumulator to Memory Location " << memoryLocation << endl;
				break;
			}
					 //Addition: Add a word from a specific location in memory to the word in the accumulator (leave the result in the accumulator).
			case 30: {

				accumulator = accumOneHolder;
				accumulator += memory.at(memoryLocation).getValue();
				accumOneHolder = accumulator;
				accumulator = 0;
				break;
			}
					 //Subtraction: Subtract a word from a specific location in memory to the word in the accumulator (leave the result in the accumulator).
			case 31: {

				accumulator = accumOneHolder;
				accumulator = accumulator - memory.at(memoryLocation).getValue();
				accumOneHolder = accumulator;
				accumulator = 0;
				//cout << "Subtraction Complete" << endl;
				break;
			}
					 //Division: Divide the word in the accumulator by the word from a specific location in memory (leave the result in the accumulator).
			case 32: {

				//Divisor can't be 0
				int denominator = memory.at(memoryLocation).getValue();
				if (denominator != 0) {

					accumulator = accumOneHolder;
					accumulator = accumulator / denominator;
					accumOneHolder = accumulator;
					accumulator = 0;
				}
				else {
					cout << "Program One: CRITICAL ERROR: Cannot Divide by 0." << endl;
					memoryDump(memory);
				}
				break;
			}
					 //Multiplication: Multiply a word from a specific location in memory to the word in the accumulator (leave the result in the accumulator).
			case 33: {

				int multiplier = memory.at(memoryLocation).getValue();
				if (accumOneHolder * multiplier <= 99999) {

					accumulator = accumOneHolder;
					accumulator *= multiplier;
					accumOneHolder = accumulator;
					accumulator = 0;
					//cout << "Multiplication Complete " << accumulator << endl;
				}
				else
				{
					cout << "Program One: CRITICAL ERROR: Multiplication operation bigger than 99999." << endl;
					memoryDump(memory);
				}
				break;
			}
					 //Remainder: Divide a word from a specific location in memory by the word in the accumulator and store the remainder
			case 34: {
				//Verify the number isn't 0
				int denominator = memory.at(memoryLocation).getValue();
				if (denominator != 0) {

					accumulator = accumOneHolder;
					accumulator = accumulator % denominator;
					accumOneHolder = accumulator;
					accumulator = 0;

				}
				else {
					cout << "Program One: CRITICAL ERROR: Cannot Divide by 0." << endl;
					memoryDump(memory);
				}
				break;
			}
					 //Exponent: Raise a word in a specific location in memory by the value in the accumulator and store the result
			case 35: {

				//Make sure the result is in range
				int value = 1;
				int count = 0;
				accumulator = accumOneHolder;
				int exponent = accumulator;
				int multiplier = memory.at(memoryLocation).getValue();
				while (count < exponent)
				{
					value *= multiplier;
					count++;
				}

				if (value <= 99999) {

					accumulator = value;
					accumOneHolder = accumulator;
					accumulator = 0;
				}
				else
				{
					cout << "Program One: CRITICAL ERROR: Exponential operation bigger than 99999." << endl;
					memoryDump(memory);
				}
				break;
			}
					 //Branch to a specific location in memory.
			case 40: {

				/* Memory Locations relate to the for-loop position in the vector,
				simply setting the index value in our for loop to match(- 1
				so it'll run it) to simulate branching -Scott */
				//cout << "Branching to Memory Location: " << memoryLocation << endl;
				programOneCtr = memoryLocation - 1;
				break;
			}
					 //BranchNeg: Branch to a specific location in memory if result from input(s) is Negative.
			case 41: {

				accumulator = accumOneHolder;
				if (accumulator < 0)
				{
					//cout << "BranchNeg - Branching to Memory Location: " << memoryLocation << endl;
					programOneCtr = memoryLocation - 1;
				}
				accumOneHolder = accumulator;
				accumulator = 0;
				break;
			}
					 //BranchZero: Branch to a specific location in memory if result from input(s) is 0.
			case 42: {

				accumulator = accumOneHolder;
				if (accumulator == 0)
				{
					//cout << "BranchZero - Branching to Memory Location: " << memoryLocation << endl;
					programOneCtr = memoryLocation - 1;
				}
				accumOneHolder = accumulator;
				accumulator = 0;
				break;
			}
					 // Halt: Stop program completely and exit.
			case 43: {
				isProgramOneDone = true;
				//cout << "Program One Complete." << endl;
				break;
			}
			}
			programOneCtr++;
		}
		else
		{
			isProgramOneDone = true;
		}


		//Program 2
		if (isProgramEndFlagged == true && isProgramTwoDone == false && programTwoCtr < MEMORY_SIZE)
		{
			int command = memory.at(programTwoCtr).getOperation();
			int memoryLocation = memory.at(programTwoCtr).getOperand();

			//For Display purposes in memoryDump() 
			operationCodeDisplay = command;
			operandDisplay = memoryLocation;

			//To determine which Command has been sent, and execute calculations. -Scott
			switch (command) {

				// Read a word from the keyboard into a specific location in memory.
			case 10: {

				//Input loop, if they give a value bigger than 4 digits or leave it blank
				do {
					cout << "Program Two: Please enter a number between 1 and 99999: ";
					cin >> inputTwo;

				} while (inputTwo.length() > 5 || inputTwo.length() == 0);

				memory.at(memoryLocation).setValue(stoi(inputTwo));
				//cout << "Saving to Memory Location: " << memoryLocation << endl;
				break;
			}
					 // Write output to screen.
			case 11: {

				//Need to work on how to display negative values.
				string outputStyle = fixMyString(memory.at(memoryLocation).getValue());
				cout << "Program Two: Contents of Memory Location " << fixMyString(memoryLocation) << ": " << outputStyle << endl;
				break;
			}
					 //Load a word from a specific location in memory into the accumulator.
			case 20: {

				accumTwoHolder = memory.at(memoryLocation).getValue();
				//cout << "Loaded " << memory.at(memoryLocation).getValue() << " from Memory Location " << memoryLocation << " into the Accumulator." << endl;
				break;
			}
					 //Store from accumulator to main memory
			case 21: {

				memory.at(memoryLocation).setValue(accumTwoHolder);
				//cout << accumulator << " loaded from Accumulator to Memory Location " << memoryLocation << endl;
				break;
			}
					 //Addition: Add a word from a specific location in memory to the word in the accumulator (leave the result in the accumulator).
			case 30: {

				accumulator = accumTwoHolder;
				accumulator += memory.at(memoryLocation).getValue();
				accumTwoHolder = accumulator;
				accumulator = 0;

				//cout << "Addition Complete" << endl;
				break;
			}
					 //Subtraction: Subtract a word from a specific location in memory to the word in the accumulator (leave the result in the accumulator).
			case 31: {

				accumulator = accumTwoHolder;
				accumulator = accumulator - memory.at(memoryLocation).getValue();
				accumTwoHolder = accumulator;
				accumulator = 0;
				//cout << "Subtraction Complete" << endl;
				break;
			}
					 //Division: Divide the word in the accumulator by the word from a specific location in memory (leave the result in the accumulator).
			case 32: {

				//Divisor can't be 0
				int denominator = memory.at(memoryLocation).getValue();
				if (denominator != 0) {

					accumulator = accumTwoHolder;
					accumulator = accumulator / denominator;
					accumTwoHolder = accumulator;
					accumulator = 0;

				}
				else {
					cout << "Program Two: CRITICAL ERROR: Cannot Divide by 0." << endl;
					memoryDump(memory);
				}
				break;
			}
					 //Multiplication: Multiply a word from a specific location in memory to the word in the accumulator (leave the result in the accumulator).
			case 33: {

				int multiplier = memory.at(memoryLocation).getValue();
				if (accumTwoHolder * multiplier <= 99999) {

					accumulator = accumTwoHolder;
					accumulator *= multiplier;
					accumTwoHolder = accumulator;
					accumulator = 0;
					//cout << "Multiplication Complete " << accumulator << endl;
				}
				else
				{
					cout << "Program Two: CRITICAL ERROR: Multiplication operation bigger than 99999." << endl;
					memoryDump(memory);
				}
				break;
			}
					 //Remainder: Divide a word from a specific location in memory by the word in the accumulator and store the remainder
			case 34: {
				//Verify the number isn't 0
				int denominator = memory.at(memoryLocation).getValue();
				if (denominator != 0) {

					accumulator = accumTwoHolder;
					accumulator = accumulator % denominator;
					accumTwoHolder = accumulator;
					accumulator = 0;
				}
				else {
					cout << "Program Two: CRITICAL ERROR: Cannot Divide by 0." << endl;
					memoryDump(memory);
				}
				break;
			}
					 //Exponent: Raise a word in a specific location in memory by the value in the accumulator and store the result
			case 35: {

				//Make sure the result is in range
				int value = 1;
				int count = 0;
				accumulator = accumTwoHolder;
				int exponent = accumulator;
				int multiplier = memory.at(memoryLocation).getValue();
				while (count < exponent)
				{
					value *= multiplier;
					count++;
				}

				if (value <= 99999) {
					accumulator = value;
					accumTwoHolder = accumulator;
					accumulator = 0;
				}
				else
				{
					cout << "Program Two: CRITICAL ERROR: Exponential operation bigger than 99999." << endl;
					memoryDump(memory);
				}
				break;
			}
					 //Branch to a specific location in memory.
			case 40: {

				/* Memory Locations relate to the for-loop position in the vector,
				simply setting the index value in our for loop to match(- 1
				so it'll run it) to simulate branching -Scott */
				//cout << "Branching to Memory Location: " << memoryLocation << endl;
				programTwoCtr = memoryLocation - 1;
				break;
			}
					 //BranchNeg: Branch to a specific location in memory if result from input(s) is Negative.
			case 41: {

				accumulator = accumTwoHolder;
				if (accumulator < 0)
				{
					//cout << "BranchNeg - Branching to Memory Location: " << memoryLocation << endl;
					programTwoCtr = memoryLocation - 1;
				}
				accumTwoHolder = accumulator;
				accumulator = 0;
				break;
			}
					 //BranchZero: Branch to a specific location in memory if result from input(s) is 0.
			case 42: {

				accumulator = accumTwoHolder;
				if (accumulator == 0)
				{
					//cout << "BranchZero - Branching to Memory Location: " << memoryLocation << endl;
					programTwoCtr = memoryLocation - 1;
				}
				accumTwoHolder = accumulator;
				accumulator = 0;
				break;
			}
					 // Halt: Stop program completely and exit.
			case 43: {
				//cout << "Program Two complete." << endl;
				break;
			}
			}
			programTwoCtr++;
		}
		else
		{
			isProgramTwoDone = true;
		}

	}
}


/*
Takes in a string and checks the length of the string.
If it's length 1, then it'll add 3 zeros to the front.
2 zeros for length 2, etc.
Parameters: string
Returns: string
*/
string fixMyString(string rawValue) {

	string value = rawValue;
	if (rawValue.length() < 5) {
		if (value.length() == 1) {
			value.insert(0, "0000");
		}
		else if (rawValue.length() == 2) {
			value.insert(0, "000");
		}
		else if (rawValue.length() == 3) {
			value.insert(0, "00");
		}
		else if (rawValue.length() == 4) {
			value.insert(0, "0");
		}
	}
	return value;
}

/*
Takes in an int, converts it to a string, and checks
the length of the string. If it's length 1, then
it'll add 3 zeros to the front. 2 zeros for length 2, etc.
Parameters: int
Returns: string
*/
string fixMyString(int rawValue) {

	string value = to_string(rawValue);
	if (value.length() < 5) {
		if (value.length() == 1) {
			value.insert(0, "0000");
		}
		else if (value.length() == 2) {
			value.insert(0, "000");
		}
		else if (value.length() == 3) {
			value.insert(0, "00");
		}
		else if (value.length() == 4) {
			value.insert(0, "0");
		}
	}
	return value;
}

int main(int argc, const char * argv[])
{
	// Initializing mainMemory and menu(vecptr)
	vector<Instruction> *mainMemory = new vector<Instruction>;
	vector<Menu> *vecptr = new vector<Menu>;

	string input;
	bool menuQuit = false;

	// Operations that are available in the program
	vecptr->push_back(Menu(10, "READ"));
	vecptr->push_back(Menu(11, "WRITE"));
	vecptr->push_back(Menu(20, "LOAD"));
	vecptr->push_back(Menu(21, "STORE"));
	vecptr->push_back(Menu(30, "ADD"));
	vecptr->push_back(Menu(31, "SUB"));
	vecptr->push_back(Menu(32, "DIV"));
	vecptr->push_back(Menu(33, "MULT"));
	vecptr->push_back(Menu(34, "REMAIN"));
	vecptr->push_back(Menu(35, "EXPONENT"));
	vecptr->push_back(Menu(40, "BRANCH"));
	vecptr->push_back(Menu(41, "BRANCHNEG"));
	vecptr->push_back(Menu(42, "BRANCHZERO"));
	vecptr->push_back(Menu(43, "HALT"));
	vecptr->push_back(Menu(-11111, "PROGRAM END"));
	vecptr->push_back(Menu(-99999, "TERMINATE"));


	//Future use to establish one program has finished, but there's more to go.
	//vecptr->push_back(Menu(SOMETHING, "Program End"));

	cout << "What action would you like to take?\n" << endl;

	//Displaying the menu with the options
	showMenu(*vecptr);

	// Loop for entering instructions from the user
	while (menuQuit == false)
	{
		cin >> input;
		// Parsing the input
		parseInput(input, *vecptr, *mainMemory, menuQuit);
	}

	//Initialize the remainder of the 100 "data slots" to be blank for ease-of-use in outputting all the data.
	int amtToFill = MEMORY_SIZE - mainMemory->size();
	for (int initialize = 0; initialize < amtToFill; initialize++)
	{
		mainMemory->push_back(Instruction(0, 0));
	}

	//Process the Instructions
	Calculate_Operations(*mainMemory);

	// Displaying Main Memory -- If the code reaches here
	//The instructions didn't have a halt code, so it finished.
	memoryDump(*mainMemory);


	//M2 Test Cases:
	/*
	Test Case # 1 & # 2

	1000
	0007
	1000
	0008
	2000
	0007
	3000
	0008
	2100
	0009
	1100
	0009
	4300
	0000
	0000
	0000
	-11111

	1000
	0020
	1000
	0021
	2000
	0020
	3100
	0021
	4100
	0018
	1100
	0020
	4300
	1100
	0021
	4300
	0000
	0000
	-99999

	*/
	return 0;
}

