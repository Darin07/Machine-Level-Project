#include <stdio.h>
#include <stdlib.h>



/**********************************************
 * This program is a compiler that converts
 * properly formated assembly code into binary
 * code
 *
 * By: Fady Elgohary and Darin Thompson
 * 
 *
*********************************************** */

typedef struct 
{
	char *name;
	int lineNum;
	
}label;

// This method gets the labels and stores them in an array
void getLabels();

// This is the method that will do the converting
void convert(char*)

// Returns the corresponding opCode
int opCode(char*);




label* makeLabel(char* var, int line)
{
	// Creates a new node and mallocates space for it
	label *newLabel = (label*) malloc(sizeof(label));
	
	//assigns the value to the node
	newLabel -> name = var;
	newLabel -> lineNum = line;
	
	return newLabel;
}



int main()
{

	
	

}


void getLabels()
{
	// Open the file and select read mode
	FILE *file = fopen("/home/fady/Desktop/Project/TestCase1.asm", "r");
	
	// variables that will be needed
	char* labelRead[6];
	label* allLabels[10];
	int line = 1;
	int index = 0;
	
	// While loop that will scan the document and store
	// any label in a hashmap
	while(fgets(labelRead, 6, file) != NULL)
	{
		if(labelRead != "      ")
		{
			label* newLabel = makeLabel(labelRead, line);
			allLabels[index] = newLabel;
			index++;
		}
		line++;
	}	
	
	
	// Close the file after we are done
	fclose(file);
	
}

void convert(char* asmFile)
{

	// Open the file and select read mode
	FILE *assembly = fopen(asmFile, "r");
	
	// replace .asm with .bin
	char* origName = strtok(asmFile, ".");
	char* binFile =  origName;
	strcat(binFile, ".bin");
	
	// open a new file that will store the binary code
	FILE *binary = fopen(binFile, "w");
	
	// variables that will be needed
	char* line[16];
	
	
   	// While loop that will go through the program
   	// and convert assembly file to binary
    while(fgets(line, 16, assembly) != NULL)
	{
		if(line != NULL)
		{
			
			
			
				
		}
	}
	
	
	
	
	
	
	
}

int opCode(char* opcode)
{
	int code = 0;
	
	if(opcode == "ADD")
		code = 0001;
	
	else if(opcode == "AND")
		code = 0101;
		
	else if(opcode == "BR")
		code = 0000;
	
	else if(opcode == "LD")
		code = 0010;
		
	else if(opcode == "LDI")
		code = 1010;
		
	else if(opcode == "LDR")
		code = 0110;
		
	else if(opcode == "LEA")
		code = 1110;
	
	else if(opcode == "NOT")
		code = 1001;
	
	else if(opcode == "ST")
		code = 0011;
	
	else if(opcode == "STI")
		code = 1011;
		
	else if(opcode == "STR")
		code = 0111;
		
	else if(opcode == "TRAP")	
		code = 1111;
		
	return code;
}


