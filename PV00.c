#include <stdio.h>
// pv00 is a "just in file / JIF"
typedef unsigned char COE; // COE <-> Code Of End
typedef unsigned char BYTE;

#define DIV 0x00 // data is void
#define AG 0x01 // always good
#define FINO 0x02 // file is not open
#define UCW 0x03 // uncorrect write

typedef struct DATA_ {
	int length;
	char* Data;
} DATA;

BYTE IntegerLog2(int num) {
	BYTE result = 0;
	while (num != 0) {
		result++;
		num >>= 2;
	}
	return result;
}

COE WriteData(DATA path, DATA* data, BYTE block) {
	if (data->length == 0) return DIV;
	FILE* file = fopen(path.Data, "ab");
	if (file == NULL) return FINO;

	BYTE head = 0; // head = (lenght of lenght of Data) + block(number) 
	head += (block << 4) >> 4;
	head += (IntegerLog2(data->length) / 8 + 1) << 4;

	int SumLength = 1 + 
					(head >> 4) + 
					data->length;

	int WrittenSymbolsHead = fwrite(&head, 1, 1, file);
	int WrittenSymbolsLengthData = fwrite(&data->length, head >> 4, 1, file);
	int WrittenSymbolsData = fwrite(data->Data, data->length, 1, file);

	if (WrittenSymbolsHead +
		WrittenSymbolsLengthData +
		WrittenSymbolsData != SumLength) return UCW; //I have problem with read-part protocol becouse if written was uncorrect I will be not know how it is detective

	return AG;
}

int main() { // testing protocole pv00(It is time-code)
	DATA path = {0, "tet.txt"};
	DATA data = {6, "hello"};
	WriteData(path, &data, 2);
}
