#include <stdio.h>
#include <stdlib.h>
// pv00 is a "just in file / JIF" WITHOUT CHECK OF CORRECT DATA!!!
typedef unsigned char COE; // COE <-> Code Of End
typedef unsigned char BYTE;

#define DIV 0x00 // data is void
#define AG 0x01 // always good
#define FINO 0x02 // file is not open
#define UCW 0x03 // uncorrect write

typedef struct DATA_ {
	int length;
	BYTE* Data;
} DATA;

// BYTE IntegerLog2(int num) {
// 	BYTE result = 0;
// 	while (num != 0) {
// 		result++;
// 		num >>= 2;
// 	}
// 	return result;
// }

COE WriteData(DATA path, DATA* data, BYTE block) {
	if (data->length == 0) return DIV;
	FILE* file = fopen(path.Data, "ab");
	if (file == NULL) return FINO;

	BYTE head = 0; // head = (lenght of Data) + block(number) 
	head += (block << 6) >> 6;
	head += (data->length - 1) << 2; // 0 < LengthData <= 64

	int SumLength = 1 + data->length;

	int WrittenSymbolsHead = fwrite(&head, 1, 1, file);
	int WrittenSymbolsData = fwrite(data->Data, data->length, 1, file);

	if (WrittenSymbolsHead + WrittenSymbolsData != SumLength) return UCW; //I have problem with read-part protocol becouse if written was uncorrect I will be not know how it is detective

	fclose(file);
	return AG;
}

COE ReadSectionData(char* path, DATA* data, BYTE block, int StartPoSym, int LengthSectionData) { //PoSym <-> position symbol
	FILE *file = fopen(path, "rb");
	if (file == NULL) {
		return FINO;
	}

	BYTE Head = 1;
	BYTE LengthData, BlockNum;

	fseek(file, 0, SEEK_SET);
	int current_seek_in_block = 0;

	int StartToPoint, EndToPoint;

	int time = 0;

	while (Head != 0) {
		time++;
		if (time > 6) {
			return DIV;
		}
		//upload info of head
		fread(&Head, 1, 1, file);
		LengthData = (Head >> 2) + 1;
		BlockNum = Head & 0b00000011;
		printf("first block\nLengthData: %hhu\nBlockNum: %hhu\n\n", LengthData, BlockNum);
		//upload info of head

		if (BlockNum == block) {

			StartToPoint = (StartPoSym) - current_seek_in_block;
			EndToPoint = StartToPoint + LengthSectionData;

			if (StartToPoint > LengthData) {
				printf("0");
				current_seek_in_block += LengthData;
				fseek(file, LengthData + 1, SEEK_CUR);
			}
			else if (EndToPoint <= LengthData) {
				printf("1");
				fseek(file, (int)StartToPoint, SEEK_CUR);
				printf(" %hhu", StartToPoint);
				fread(data->Data, 1, (size_t)LengthSectionData, file); //
				fclose(file);
				return AG;
			}
			else {
				printf("2");
				fseek(file, StartToPoint, SEEK_CUR);
				fread(data->Data, 1, LengthData - StartToPoint, file); //
				break;
			}
		}
		else {
			fseek(file, LengthData + 1, SEEK_CUR);
		}
	}


	while (Head != 0) {
		//upload info of head
		fread(&Head, 1, 1, file);
		LengthData = (Head >> 2) + 1;
		BlockNum = Head & 0b00000011;
		//upload info of head
		printf("second block\nLengthData: %hhu\nBlockNum: %hhu\n\n", LengthData, BlockNum);

		if (BlockNum == block) {

			StartToPoint = (StartPoSym) - current_seek_in_block + 1; //
			EndToPoint = StartToPoint + LengthSectionData;			 // Time-code

			if (EndToPoint < LengthData) {
				fread(data->Data + current_seek_in_block, 1, EndToPoint, file); //
				fclose(file);
				return AG;
			}
			else {
				fread(data->Data + current_seek_in_block, 1, LengthData, file); //
			}
		}
		else {
			fseek(file, LengthData + 1, SEEK_CUR);
		}
	}
	return DIV;
}

int main() { // testing protocole pv00(It is time-code)
	DATA path = {0, "tet.bin"};
	DATA data = {6, "hibro"};

	DATA testdata = {2, "pp"};
	//WriteData(path, &data, 3);

	COE sec = ReadSectionData("tet.bin", &testdata, 3, 3, 2);
	if (sec == AG) {
		printf("it is fully pizda");
	} else {
		if (sec == DIV) {
			printf("diva");
		}
		else {
			printf("%i", sec);
		}
		if (sec == FINO) {
			printf("lll");
		}
	}
	printf("\n%c%c", *testdata.Data, *(testdata.Data + 1));
	int i;
		while(1) {
			i++;
		}
}
