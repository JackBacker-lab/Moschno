#include <stddef.h>
#include <stdio.h>
// pv01 is a "just in file / JIF" WITHOUT CHECK OF CORRECT DATA!!!
typedef unsigned char COE; // COE <-> Code Of End
typedef unsigned char BYTE;

#define MAX_LENGTH_BLOCK 64 // max length of one block of data

#define DIV 0x00 // data is void for write
#define AG 0x01 // always good
#define FINO 0x02 // file is not open
#define UCW 0x03 // uncorrect write
#define FATAL_ERROR 0x04 // fatal error
#define UNA 0x05 // uncorrect aguments

typedef struct DATA_ {
	int length;
	BYTE* Data;
} DATA;

COE CreateVoidData(size_t length, DATA* data);

COE FreeData(DATA* data);

COE WriteDataBlock(BYTE* data, size_t Length, BYTE block, FILE* file);

COE WriteData(char* path, DATA* data, BYTE block);

COE ReadSectionData(char* path, DATA* data, BYTE block, int FirstImos, int LengthSectionData);