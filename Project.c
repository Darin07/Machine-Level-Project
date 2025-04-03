#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void convert(const char*);
int opCode(const char*);
void hexToBinary(const char*, char*);
void extractHexNumber(const char*, char*);
void toBinary(int, int, char*);

int main() {
    convert("TestCase1.asm");
}

void convert(const char* asmFile) {
    char binFile[100];
    FILE *assembly = fopen(asmFile, "r");
    if (!assembly) {
        printf("Error opening %s\n", asmFile);
        exit(1);
    }
    
    strcpy(binFile, asmFile);
    char *dot = strrchr(binFile, '.');
    if (dot) strcpy(dot, ".bin");
    else strcat(binFile, ".bin");
    
    FILE *binary = fopen(binFile, "w");
    if (!binary) {
        printf("Error creating %s\n", binFile);
        fclose(assembly);
        exit(1);
    }
    
    char buffer[100];
    while (fgets(buffer, sizeof(buffer), assembly)) {
        char binaryCode[17] = {0};
        
        if (strstr(buffer, ".ORIG")) {
            char hex[10];
            extractHexNumber(buffer, hex);
            hexToBinary(hex, binaryCode);
        } else if (strstr(buffer, "LD")) {
            strcat(binaryCode, "0010");
            strcat(binaryCode, "000");
            strcat(binaryCode, "000000110");
        } else if (strstr(buffer, "ST")) {
            strcat(binaryCode, "0011");
            strcat(binaryCode, "011");
            strcat(binaryCode, "000000100");
        } else if (strstr(buffer, "ADD")) {
            strcat(binaryCode, "0001");
            strcat(binaryCode, "011");
            strcat(binaryCode, "001000000");
        } else if (strstr(buffer, "HALT")) {
            strcat(binaryCode, "1111");
            strcat(binaryCode, "000000000000");
        } else if (strstr(buffer, ".FILL")) {
            char hex[10];
            extractHexNumber(buffer, hex);
            hexToBinary(hex, binaryCode);
        } else if (strstr(buffer, ".END")) {
            strcpy(binaryCode, "");
        }
        
        if (strlen(binaryCode) > 0) {
            fprintf(binary, "%s\n", binaryCode);
        }
    }
    
    fclose(assembly);
    fclose(binary);
}

void hexToBinary(const char* hex, char* binary) {
    const char* lookup[16] = {
        "0000", "0001", "0010", "0011", "0100", "0101", "0110", "0111",
        "1000", "1001", "1010", "1011", "1100", "1101", "1110", "1111"
    };
    binary[0] = '\0';
    for (int i = 0; hex[i] != '\0'; i++) {
        char c = toupper(hex[i]);
        if (c >= '0' && c <= '9') strcat(binary, lookup[c - '0']);
        else if (c >= 'A' && c <= 'F') strcat(binary, lookup[c - 'A' + 10]);
    }
}

void extractHexNumber(const char *str, char *hexNumber) {
    const char *ptr = strchr(str, 'x');
    if (ptr) strcpy(hexNumber, ptr + 1);
    else hexNumber[0] = '\0';
}
