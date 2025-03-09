#include "GlobalDef.h"
#include <stdlib.h>

COE CreateVoidData(size_t length, DATA* data) {
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