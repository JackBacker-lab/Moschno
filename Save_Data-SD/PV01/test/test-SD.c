#include "D:\Programing\C\Virs\Potuzhno\Save_Data-SD\PV01\PV01.h"
#include <stdio.h>

int main () {
	char* path = "D:/Programing/C/Virs/Potuzhno/Save_Data-SD/Storage/Storage-01-0.bin";
	DATA data = {0, NULL};
	DATA dataWrite1 = {70, "01234567891011121314151617181920212223242526272829303132333435363738394041424344454647484950515253545556575859"};
	DATA dataWrite2 = {4, "p_bu"};
	DATA dataWrite3 = {8, "t okeypi"};
	DATA dataWrite4 = {2, "zd"};

	WriteData(path, &dataWrite1, 1);
	WriteData(path, &dataWrite2, 2);
	WriteData(path, &dataWrite3, 1);
	WriteData(path, &dataWrite4, 1);

	CreateVoidData(11, &data);

	ReadSectionData(path, &data, 1, 2, 11);

	for (int i = 0; i < 11; i++) printf("%c", *(data.Data + i));

	FreeData(&data);
	//FreeData(&dataWrite1);
	//FreeData(&dataWrite2);
	//FreeData(&dataWrite3);
	//FreeData(&dataWrite4);

	int j; while (1) {j++;}
}