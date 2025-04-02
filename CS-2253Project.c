#include <stdio.h>   //create a header file and then use this line to import  
#include <string.h>    //create a header file and then use this line to import
#include <stdlib.h>      

// Function prototypes to declare functions used in the program


// Handles reading .asm file and writing .bin file
void processAssemblyFile(const char *inputFile, const char *outputFile); 
// Converts individual assembly instructions to binary
void translateInstruction(const char *line, int lineNumber, FILE *output); 
// Converts an integer to a binary string of a given length
char *convertToBinary(int value, int bits);
// Displays error message and exits the program
void handleError(const char *message); 





//main method
int main(int argc, char *argv[])
{
    // Checks if the user provided the correct number of arguments
    if (argc != 2) 
    {
      // Print usage message and exit
        handleError("Usage: <program_name> <input_file>");
    }

    // Prepares the output file name by appending ".bin" to the input file name
    char outputFile[100];
    snprintf(outputFile, sizeof(outputFile), "%s.bin", argv[1]);

    // Processes the input .asm file and creates the output .bin file
    processAssemblyFile(argv[1], outputFile);

    // Prints success message after conversion is complete
    printf("Conversion completed. Output saved to %s\n", outputFile);
    return 0; // Exit the program successfully
}






//method to process the assemblyFile
void processAssemblyFile(const char *inputFile, const char *outputFile) 
{
    // Opens the input assembly file for reading and output binary file for writing
    FILE *inFile = fopen(inputFile, "r");
    FILE *outFile = fopen(outputFile, "w");

    
    // Checks if the files are successfully opened; otherwise, displays an error message
    if (!inFile || !outFile) 
    {
      //error message if file not opened
        handleError("Error: Unable to open file(s).");
    }

    // Buffer to store each line of the input file
    char line[256];
    // Keeps track of the current line number (for output formatting)
    int lineNumber = 0; 

    
    // Copies the first line (header) from the .asm file to the .bin file
    fgets(line, sizeof(line), inFile);
    // Write the header line unchanged
    fprintf(outFile, "%s", line); 

    
    // Processes each subsequent line of the input file
    while (fgets(line, sizeof(line), inFile)) 
    {
        // Skips empty lines
        if (strlen(line) > 1) 
        { 
            // Writes line number in hexadecimal followed by spaces
            fprintf(outFile, "%04X  ", lineNumber); 
            // Converts the instruction to binary
            translateInstruction(line, lineNumber, outFile); 
            // Increments the line number for the next iteration
            lineNumber++; 
            
        }
    }

    
    // Closes both the input and output files
    fclose(inFile);
    fclose(outFile);
}






//method to translate the opcodes and labels from .asm to .bin
void translateInstruction(const char *line, int lineNumber, FILE *output)
{
    // Buffers to store the label, opcode, and operands parsed from the assembly line
    // Stores the 6-character label (if present)
    char label[7] = {0};    
    // Stores the opcode (e.g., ADD, AND, BR, etc.)
    char opcode[8] = {0};   
    // Stores the operand portion of the line
    char operands[50] = {0}; 

    
    // Parses the assembly line into label, opcode, and operands
    sscanf(line, "%6s %7s %[^\n]", label, opcode, operands);

    
    // Buffer for the 16-bit binary instruction
    char binary[17] = {0}; 

    
    //Converts specific opcodes into their binary representations based on their format
    if (strcmp(opcode, "ADD") == 0 || strcmp(opcode, "AND") == 0) 
    {
        // Buffers for the registers
        char dest[3], src1[3], src2[3]; 
        // Extracts operands
        sscanf(operands, "R%d, R%d, R%d", &dest[0], &src1[0], &src2[0]); 
        snprintf(binary, sizeof(binary), "%s%s%s%s", 
                 // Opcode binary
                 strcmp(opcode, "ADD") == 0 ? "0001" : "0101", 
                 // Destination register in binary
                 convertToBinary(atoi(dest), 3),               
                 // Source register 1 in binary
                 convertToBinary(atoi(src1), 3),               
                 // Source register 2 in binary
                 convertToBinary(atoi(src2), 3));              
    } 
    
    else if (strcmp(opcode, "BR") == 0) 
    {
        int offset;
        // Extracts the branch offset
        sscanf(operands, "%d", &offset); 
        // Opcode with offset
        snprintf(binary, sizeof(binary), "0000%s", convertToBinary(offset, 9)); 
    } 
    
    else if (strcmp(opcode, "LD") == 0 || strcmp(opcode, "ST") == 0)
    {
        char dest[3];
        int offset;
        // Extracts operands
        sscanf(operands, "R%d, %d", &dest[0], &offset); 
        snprintf(binary, sizeof(binary), "%s%s%s", 
                 // Opcode binary
                 strcmp(opcode, "LD") == 0 ? "0010" : "0011",
                 // Destination register in binary
                 convertToBinary(atoi(dest), 3),             
                 // Offset in binary
                 convertToBinary(offset, 9));                
    } 
    
    else if (strcmp(opcode, "LDR") == 0 || strcmp(opcode, "STR") == 0) 
    {
        char dest[3], base[3];
        int offset;
        // Extracts operands
        sscanf(operands, "R%d, R%d, %d", &dest[0], &base[0], &offset); 
        snprintf(binary, sizeof(binary), "%s%s%s%s", 
                 // Opcode binary
                 strcmp(opcode, "LDR") == 0 ? "0110" : "0111", 
                 // Destination register in binary
                 convertToBinary(atoi(dest), 3),               
                 // Base register in binary
                 convertToBinary(atoi(base), 3),               
                 // Offset in binary
                 convertToBinary(offset, 6));                  
    } 
    
    else if (strcmp(opcode, "LEA") == 0) 
    {
        char dest[3];
        int offset;
        // Extracts operands
        sscanf(operands, "R%d, %d", &dest[0], &offset); 
        snprintf(binary, sizeof(binary), "1110%s%s", 
                 // Destination register in binary
                 convertToBinary(atoi(dest), 3),        
                 // Offset in binary
                 convertToBinary(offset, 9));           
    } 
    
    else if (strcmp(opcode, "NOT") == 0) 
    {
        char dest[3], src[3];
        // Extracts operands
        sscanf(operands, "R%d, R%d", &dest[0], &src[0]); 
        snprintf(binary, sizeof(binary), "1001%s%s111111",
                 // Destination register in binary
                 convertToBinary(atoi(dest), 3),         
                 // Source register in binary
                 convertToBinary(atoi(src), 3));         
    } 
    
    else 
    {
        // Default case for unhandled opcodes
        snprintf(binary, sizeof(binary), "UNKNOWN INSTRUCTION"); 
    }

    
    // Writes the binary instruction and the original assembly line as an inline comment
    fprintf(output, "%s ; %s\n", binary, line);
}







//method to convert the .asm to .bin
char *convertToBinary(int value, int bits) 
{
    // Buffer for the binary string (maximum of 16 bits)
    static char binary[17]; 
    // Initialize the buffer
    memset(binary, 0, sizeof(binary)); 

    
    // Converts the integer value into a binary string of the specified bit length
    for (int i = bits - 1; i >= 0; i--) 
    {
        // Extracts each bit
        binary[bits - 1 - i] = (value & (1 << i)) ? '1' : '0'; 
    }

    // Null-terminates the string
    binary[bits] = '\0'; 
    return binary;
}







//to print an error message to handle any error cases.
void handleError(const char *message) 
{
    // Prints an error message to the standard error stream
    fprintf(stderr, "%s\n", message);
    // Exits the program with a failure status
    exit(1); 
}
