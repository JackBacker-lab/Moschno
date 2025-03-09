#include "D:\Programing\C\Virs\Potuzhno\GlobalDef.h"
#include <stdio.h>

COE WriteDataBlock(BYTE* data, size_t Length, BYTE block, FILE* file);

COE WriteData(char* path, DATA* data, BYTE block);

COE ReadSectionData(char* path, DATA* data, BYTE block, int FirstImos, int LengthSectionData);