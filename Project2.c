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
	char labelRead[6];
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


