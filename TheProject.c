//include/ import lines
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/***********************************************************************************
 * Program: A C program to read as input a .asm assembly program and convert it 
 *          into a .bin binary program using multiple methods to split and make 
 *          this required conversion.
 
 *Creators: Fady Elgohary (3762991) and Darin Thomson (3776723) 
 
 *Date    : April 4th, 2025 (due 2pm)
 ************************************************************************************/



// Function prototypes(functions being used as part of this program

// Removes trailing commas and capitalizes register names
void normalizeRegister(char*);

// Returns a 3-bit binary string corresponding to a register name (e.g., "R3" -> "011")
char* registerToBinary(char*);

// Calculates the PC-relative offset between two addresses (with 9-bit signed range)
int calculateOffset(int currentAddress, int targetAddress);

// Converts LC-3 assembly file to binary file with field notation
void convert(char*);

// Returns the binary opcode for a given LC-3 instruction mnemonic
int opCode(char*);

// Converts a hexadecimal string to its 16-bit binary string equivalent
void hexToBinary(char*, char*);

// Extracts the hex number from a string (e.g., from a line like ".ORIG x3000")
void extractHexNumber(char*, char*);

// Formats a 16-bit binary string into human-readable field notation with spaces
void formatFieldNotation(char*, char*, const char*);










//To create a structure to store label information: name and its corresponding address
typedef struct 
{
    //Label name (up to 49 characters + null terminator)
    char name[50];   
    
    //Address associated with the label
    int address;     
} Label;

//An array called labels[] to hold up to 100 labels defined in the assembly program
Label labels[100];

//To define the starting address of the program (as per the .ORIG command in LC-3)
int origAddress = 0x3000;








//A main method to test our program
int main() 
{
    //to call the convert() and check our program by passing in the TestCase1.asm 
    convert("TestCase1.asm");
    return 0;
}









//A method to remove any trailing comma from a register string and converts it to uppercase
void normalizeRegister(char* reg) 
{
    // Find the first comma in the string (if any)
    char* comma = strchr(reg, ',');

    // If a comma is found, terminate the string at that point
    if (comma) *comma = '\0';

    // Convert each character in the string to uppercase
    for (int i = 0; reg[i]; i++) 
        reg[i] = toupper(reg[i]);
}















//A method to converts a register string (e.g., "R1") to its 3-bit binary string equivalent (ex: R0 = 000, R1 = 001..)
char* registerToBinary(char* reg)
{
    //to call the normalizeRegister() to clean up the register string: remove comma, convert to uppercase
    normalizeRegister(reg);

    // Match the register name and return its corresponding 3-bit binary value
    if (strcmp(reg, "R0") == 0) return "000";
    if (strcmp(reg, "R1") == 0) return "001";
    if (strcmp(reg, "R2") == 0) return "010";
    if (strcmp(reg, "R3") == 0) return "011";
    if (strcmp(reg, "R4") == 0) return "100";
    if (strcmp(reg, "R5") == 0) return "101";
    if (strcmp(reg, "R6") == 0) return "110";
    if (strcmp(reg, "R7") == 0) return "111";

    // Default case (invalid register): return "000"
    return "000";
}















//A method to calculate the PC-relative offset between the current instruction and a target address
//The method also ensures the offset fits within a signed 9-bit range (-256 to 255)
int calculateOffset(int currentAddress, int targetAddress) 
{
    //to calculate the offset (PC-relative is always relative to the next instruction)
    int offset = targetAddress - (currentAddress + 1);
    
    //if() to check if the offset is within the valid signed 9-bit range
    if (offset < -256 || offset > 255) 
    {
        fprintf(stderr, "Offset out of range: %d\n", offset);
        
        // Exit the program if the offset is invalid
        exit(1); 
    }
    
    //Check if the offset is negative, convert it to its 9-bit two's complement form
    if (offset < 0) 
    {
        offset = (1 << 9) + offset; // Adds 512 to wrap around negative values
    }
    
    // Return the final 9-bit offset value (as an unsigned int)
    return offset;
}















//A method to convert an LC-3 .asm assembly file into a .bin binary file
void convert(char* asmFile) 
{
    // Buffer to hold the name of the output binary file
    char binFile[100]; 

    
    // Open the assembly file for reading
    FILE *assembly = fopen(asmFile, "r");
    if (assembly == NULL) 
    {
        //To print an error message if file can't be opened
        printf("Error opening %s\n", asmFile); 
        exit(1);
    }

    
    //To copy the input filename and change the extension to ".bin"
    strcpy(binFile, asmFile);
    
    //to find last dot in filename
    char *dot = strrchr(binFile, '.'); 
    if (dot) 
    {
        // Replace the .asm extension into a .bin extension (TestCase1.asm -> TestCase1.bin)
        strcpy(dot, ".bin"); 
    } 
    else 
    {
        // If no extension, append ".bin"
        strcat(binFile, ".bin"); 
    }

    //to open the binary file for writing using the fopen to open the file
    FILE *binary = fopen(binFile, "w");
    if (binary == NULL) 
    {
        //To print an error message if the binary file can't be created
        printf("Error creating %s\n", binFile);
        fclose(assembly);
        exit(1);
    }



    //To read the .ORIG line
    //Buffer to read each line into an array of space [100]
    char buffer[100]; 
    while (fgets(buffer, sizeof(buffer), assembly)) 
    {
        //to look for .ORIG directive
        if (strstr(buffer, ".ORIG")) 
        {
            char hex[10];
            
            // Extract the hex number after .ORIG
            extractHexNumber(buffer, hex); 
            
            // Convert hex string to int
            sscanf(hex, "%x", &origAddress); 
            break;
        }
    }


    // First pass: build label table from file
    // Go back to beginning of the file    
    fseek(assembly, 0, SEEK_SET); 
    int labelCount = 0;
    int lineNumber = 0;

    while (fgets(buffer, sizeof(buffer), assembly)) 
    {
        char copy[100];
        // Create a copy to tokenize
        strcpy(copy, buffer); 
        
        // Get first word (label or opcode)
        char *token = strtok(copy, " \t\n");
        if (token && token[0] != ';' && strstr(token, ".") == NULL && strlen(token) > 0) 
        {
            // Save label and its address
            strcpy(labels[labelCount].name, token);
            labels[labelCount].address = origAddress + lineNumber;
            labelCount++;
        }

        // Check for .BLKW directive to adjust line count
        if (strstr(buffer, ".BLKW")) 
        {
            int count = 1;
            char *num = strtok(NULL, " \t\n");
            
            // Get the number of words reserved
            if (num) count = atoi(num); 
            lineNumber += count;
        } 
        else 
        {
            lineNumber++;
        }
    }


    // Second pass: generate binary output
    // Reset file pointer
    fseek(assembly, 0, SEEK_SET); 
    lineNumber = 0;

    while (fgets(buffer, sizeof(buffer), assembly)) 
    {
        // Remove the newline character
        buffer[strcspn(buffer, "\n")] = '\0'; 
        char originalLine[100];
      
       //to keep the original line of comments
        strcpy(originalLine, buffer); 

        // Buffers for storing binary and opcode info
        char binaryCode[17] = {0};
        char formattedBinary[32] = {0};
        char opcode[10] = "";

        // Handle .ORIG directive
        if (strstr(buffer, ".ORIG")) 
        {
            char hex[10];
            extractHexNumber(buffer, hex);
          
           // Convert address to binary
            hexToBinary(hex, binaryCode); 
            strcpy(opcode, ".ORIG");
        }

        // Handle LD instruction
        else if (strstr(buffer, "LD") && !strstr(buffer, "LDI") && !strstr(buffer, "LDR")) 
        {
           // Get opcode
            sscanf(buffer, "%s", opcode); 
            char reg[10], label[10];
         
           // Parse operands
            sscanf(buffer, "%*s %s %s", reg, label); 

           //to identify the LD opcode
            strcpy(binaryCode, "0010"); 
          // Destination register
            strcat(binaryCode, registerToBinary(reg)); 

            // Find target address from label table
            int targetAddress = -1;
            for (int i = 0; i < labelCount; i++) 
            {
                if (strcmp(label, labels[i].name) == 0) 
                {
                    targetAddress = labels[i].address;
                    break;
                }
            }

            // Calculate and append offset to binary
            int currentAddress = origAddress + lineNumber;
            int offset = calculateOffset(currentAddress, targetAddress);

            char offsetBits[10] = {0};
            for (int i = 8; i >= 0; --i) 
                offsetBits[8 - i] = (offset & (1 << i)) ? '1' : '0';

            strcat(binaryCode, offsetBits);
        }

        // Handle ST instruction
        else if (strstr(buffer, "ST") && !strstr(buffer, "STI") && !strstr(buffer, "STR")) 
        {
            sscanf(buffer, "%s", opcode);
            char reg[10], label[10];
            sscanf(buffer, "%*s %s %s", reg, label);

           // ST opcode
            strcpy(binaryCode, "0011"); 
            strcat(binaryCode, registerToBinary(reg));

            int targetAddress = -1;
            for (int i = 0; i < labelCount; i++) 
            {
                if (strcmp(label, labels[i].name) == 0) 
                {
                    targetAddress = labels[i].address;
                    break;
                }
            }

            int currentAddress = origAddress + lineNumber;
            int offset = calculateOffset(currentAddress, targetAddress);

            char offsetBits[10] = {0};
            for (int i = 8; i >= 0; --i) 
                offsetBits[8 - i] = (offset & (1 << i)) ? '1' : '0';

            strcat(binaryCode, offsetBits);
        }

        // Handle ADD instruction
        else if (strstr(buffer, "ADD")) 
        {
            sscanf(buffer, "%s", opcode);
            char dr[10], sr1[10], sr2[10];
            sscanf(buffer, "%*s %s %s %s", dr, sr1, sr2);

           // ADD opcode
            strcpy(binaryCode, "0001"); 
            strcat(binaryCode, registerToBinary(dr));
            strcat(binaryCode, registerToBinary(sr1));
           // Indicating register mode (not immediate)
            strcat(binaryCode, "000"); 
            strcat(binaryCode, registerToBinary(sr2));
        }

        // Handle HALT instruction
        else if (strstr(buffer, "HALT")) 
        {
           // HALT hardcoded binary
            strcpy(binaryCode, "1111000000100101"); 
            strcpy(opcode, "HALT");
        }

        // Handle .FILL directive
        else if (strstr(buffer, ".FILL") != NULL) 
        {
            char *token = strtok(buffer, " \t");
            token = strtok(NULL, " \t"); 
            
            if (token != NULL && strcmp(token, ".FILL") == 0) 
            {
                char *value = strtok(NULL, " \t");
                
                if (value != NULL) 
                {
                    char hex[10];
                    if (value[0] == 'x' || value[0] == 'X') 
                    {
                        strcpy(hex, value + 1);
                    } 
                    else 
                    {
                       // Strip '#' if needed
                        int num = atoi(value + (value[0] == '#' ? 1 : 0)); 
                      // Format to hex
                        sprintf(hex, "%04X", num & 0xFFFF); 
                    }
                   // Convert to binary
                    hexToBinary(hex, binaryCode); 
                    strcpy(opcode, ".FILL");
                }
            }
        }

        // Handle .BLKW directive (write zeros)
        else if (strstr(buffer, ".BLKW")) 
        {
           //Write 0 for each empty word if needed (default case)
            strcpy(binaryCode, "0000000000000000"); 
            strcpy(opcode, ".BLKW");
        }

        // Handle .END directive (nothing to write)
        else if (strstr(buffer, ".END")) 
        {
            strcpy(binaryCode, "");
        }

        // Write the final binary line with original comment
        if (strlen(binaryCode) > 0) 
        {
           // Format with field labels
            formatFieldNotation(binaryCode, formattedBinary, opcode); 
           // Write to binary file
            fprintf(binary, "%-26s ; %s\n", formattedBinary, originalLine); 
        }

       // Increment line counter for address calculation
        lineNumber++; 
    }

    // Close both files
    fclose(assembly);
    fclose(binary);
}













// Returns the 4-bit binary opcode for a given LC-3 assembly instruction as an integer
int opCode(char* opcode) 
{
    // Compare the input string with known opcodes and return the corresponding binary value
   // ADD opcode
    if (strcmp(opcode, "ADD") == 0)  return 0b0001; 
  
   // AND opcode
    if (strcmp(opcode, "AND") == 0)  return 0b0101; 
  
   // BR (branch) opcode
    if (strcmp(opcode, "BR")  == 0)  return 0b0000; 
  
   // Load from memory
    if (strcmp(opcode, "LD")  == 0)  return 0b0010; 
   
   // Load indirect
    if (strcmp(opcode, "LDI") == 0)  return 0b1010; 
  
   // Load base+offset
    if (strcmp(opcode, "LDR") == 0)  return 0b0110; 
  
   // Load effective address
    if (strcmp(opcode, "LEA") == 0)  return 0b1110; 
  
   // Bitwise NOT
    if (strcmp(opcode, "NOT") == 0)  return 0b1001; 
  
   // Store to memory
    if (strcmp(opcode, "ST")  == 0)  return 0b0011; 
  
   // Store indirect
    if (strcmp(opcode, "STI") == 0)  return 0b1011; 
  
   // Store base+offset
    if (strcmp(opcode, "STR") == 0)  return 0b0111; 
  
   // Trap (system call)
    if (strcmp(opcode, "TRAP") == 0) return 0b1111; 
    
    // If the opcode isn't recognized, return -1 to indicate error
    return -1;
}













// Converts a hexadecimal string to its binary string representation
void hexToBinary(char* hex, char* binary) 
{
    // Lookup table for each hexadecimal digit (0–F) to its 4-bit binary equivalent
    const char* lookup[16] = {
        "0000", "0001", "0010", "0011",
        "0100", "0101", "0110", "0111",
        "1000", "1001", "1010", "1011",
        "1100", "1101", "1110", "1111"
    };

   // Initialize the output binary string as empty
    binary[0] = '\0'; 
   // Get the length of the hex input
    int len = strlen(hex); 

    // Loop through each character in the hex string
    for (int i = 0; i < len; i++) 
    {
        // Convert the character to uppercase for consistency
        char c = toupper(hex[i]);

        // If character is a digit (0–9), map to binary using the lookup table
        if (c >= '0' && c <= '9') 
            strcat(binary, lookup[c - '0']);
        
        // If character is a letter (A–F), map to binary using offset in lookup
        else if (c >= 'A' && c <= 'F') 
            strcat(binary, lookup[c - 'A' + 10]);
    }
}














// Extracts the hexadecimal number from a string (e.g., ".ORIG x3000") 
void extractHexNumber(char *str, char *hexNumber) 
{
    // Search for the character 'x' in the input string
    char *ptr = strchr(str, 'x');

    // If 'x' is found, copy everything after it into hexNumber
    if(ptr) 
    {
        strcpy(hexNumber, ptr + 1);
    }
    else 
    {
        // If no 'x' found, set hexNumber to an empty string
        hexNumber[0] = '\0';
    }
}













//A method to Formats a 16-bit binary instruction string into spaced field notation for readability
void formatFieldNotation(char* binary, char* formatted, const char* instrType) 
{
   // Initialize the formatted string as empty
    formatted[0] = '\0'; 

    // Handle formatting for instructions like ADD and AND
    if (strcmp(instrType, "ADD") == 0 || strcmp(instrType, "AND") == 0) 
    {
        // Opcode (bits 0–3)
        strncat(formatted, binary, 4);
        strcat(formatted, " ");

        // DR (destination register) (bits 4–6)
        strncat(formatted + strlen(formatted), binary + 4, 3);
        strcat(formatted, " ");

        // SR1 (source register 1) (bits 7–9)
        strncat(formatted + strlen(formatted), binary + 7, 3);
        strcat(formatted, " ");

        // Mode bit (bit 10)
        strncat(formatted + strlen(formatted), binary + 10, 1);
        strcat(formatted, " ");

        // Either bits of immediate value or unused (bits 11–12)
        strncat(formatted + strlen(formatted), binary + 11, 2);
        strcat(formatted, " ");

        // SR2 or immediate (bits 13–15)
        strncat(formatted + strlen(formatted), binary + 13, 3);
    } 
   
    // Handle formatting for LD and ST instructions
    else if (strcmp(instrType, "LD") == 0 || strcmp(instrType, "ST") == 0) 
    {
        // Opcode (bits 0–3)
        strncat(formatted, binary, 4);
        strcat(formatted, " ");

        // DR or SR (bits 4–6)
        strncat(formatted + strlen(formatted), binary + 4, 3);
        strcat(formatted, " ");

        // PCoffset9 (bits 7–15)
        strncat(formatted + strlen(formatted), binary + 7, 9);
    } 
   
    // Default formatting: group bits into chunks of 4 with spaces
    else 
    {
        for (int i = 0; i < 16; i++) 
        {
            // Copy each bit into its proper position in formatted string, with spacing
            formatted[i + (i / 4)] = binary[i];

            // Insert a space after every 4 bits except the last group
            if ((i + 1) % 4 == 0 && i != 15) 
            {
                formatted[i + 1 + (i / 4)] = ' ';
            }
        }
       // Null-terminate the formatted string
        formatted[19] = '\0'; 
    }
}
