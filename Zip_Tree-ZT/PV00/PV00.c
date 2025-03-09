#include "PV00.h"
#include <stdlib.h>
#include <stdio.h>

COE CreateVoidData(int length, DATA* data) {
	if (length <= 0 || !data) return UNA;

	if (data->Data) free(data->Data);
	data->Data = (BYTE*)malloc(length);
	if (!data->Data) {
		data->length = 0;
		return DIV;
	}

	data->length = length;
	return AG;
}

COE FreeData(DATA* data) {
	if (!data) return UNA;
	if (data->Data) {
		free(data->Data);
		data->Data = NULL;
		data->length = 0;
	}
	return AG;
}
// if DATA->lenght = 0 -> DATA->Data = NULL
// if DATA->Data = 0 -> DATA->lenght = NULL

COE WriteData(char* path, DATA* data, BYTE block) { // to 64 bytes write will be correct
	if (!data) return UNA;
	if (!data->length) return UNA;
	FILE* file = fopen(path, "ab");
	if (file == NULL) return FINO;

	BYTE head = 0; // head = (lenght of Data) + block(number) 
	head += block & 0b00000011;
	head += (data->length - 1) << 2; // 0 < LengthData <= 64

	int SumLength = 1 + data->length;

	int WrittenSymbolsHead = fwrite(&head, 1, 1, file);
	int WrittenSymbolsData = fwrite(data->Data, 1, data->length, file);

	fclose(file);
	if (WrittenSymbolsHead + WrittenSymbolsData != SumLength) return UCW; //I have problem with read-part protocol becouse if written was uncorrect I will be not know how it is detective

	return AG;
}

COE ReadSectionData(char* path, DATA* data, BYTE block, int FirstImos, int LengthSectionData) { //PoSym <-> position symbol
	FILE *file = fopen(path, "rb");
	if (file == NULL) return FINO;

	BYTE Head = 1;
	BYTE LengthDataBlock, BlockNum;

	fseek(file, 0, SEEK_SET);
	int SEEK_DATA = 0;
	int SEEK_SECTION_DATA = 0;

	int StartToPoint, EndToPoint;

	// find start needs section in data
	while (fread(&Head, 1, 1, file)) {
		//upload info of head
		LengthDataBlock = (Head >> 2) + 1;
		BlockNum = Head & 0b00000011;

		if (BlockNum == block) {

			StartToPoint = FirstImos - SEEK_DATA;
			EndToPoint = StartToPoint + LengthSectionData;

			// LOG: printf("first block\nLengthData: %hhu\nBlockNum: %hhu\nStartToPoint: %i\nEndToPoint: %i\n\n", LengthDataBlock, BlockNum, StartToPoint, EndToPoint);

			if (StartToPoint < LengthDataBlock) {
				fseek(file, StartToPoint, SEEK_CUR);
				if (EndToPoint < LengthDataBlock) {
					fread(data->Data, 1, EndToPoint - StartToPoint, file);
					SEEK_DATA += EndToPoint - StartToPoint;

					fclose(file);
					return AG;
				}
				else {
					fread(data->Data, 1, LengthDataBlock - StartToPoint, file);
					SEEK_DATA += LengthDataBlock;
					SEEK_SECTION_DATA += LengthDataBlock - StartToPoint;
					break;
				}
			}
			else {
				fseek(file, LengthDataBlock, SEEK_CUR);
				SEEK_DATA += LengthDataBlock;
			}
		}
		else fseek(file, LengthDataBlock, SEEK_CUR);
	}

	// write data to section data end find end section data in data 
	while (fread(&Head, 1, 1, file)) {
		//upload info of head
		LengthDataBlock = (Head >> 2) + 1;
		BlockNum = Head & 0b00000011;

		if (BlockNum == block) {

			StartToPoint = FirstImos - SEEK_DATA;
			EndToPoint = StartToPoint + LengthSectionData;

			// LOG: printf("second block\nLengthData: %hhu\nBlockNum: %hhu\nStartToPoint: %i\nEndToPoint: %i\nSEEK_SECTION_DATA: %i\n\n", LengthDataBlock, BlockNum, StartToPoint, EndToPoint, SEEK_SECTION_DATA);

			if (EndToPoint < LengthDataBlock) {
				fread(data->Data + SEEK_SECTION_DATA, 1, EndToPoint, file);

				fclose(file);
				return AG;
			}
			else {
				fread(data->Data + SEEK_SECTION_DATA, 1, LengthDataBlock, file);
				SEEK_DATA += LengthDataBlock;
				SEEK_SECTION_DATA += LengthDataBlock;
			}
		}
		else fseek(file, LengthDataBlock, SEEK_CUR);
	}

	fclose(file);
	return FATAL_ERROR;
}