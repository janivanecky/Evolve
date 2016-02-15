#pragma once
#include "Math.h"

namespace String
{
	bool Compare(char *str1, char *str2);
	bool Contains(char *container, char *containee);

	void Copy(char *src, char *dst);
	void CopySafe(char *src, char *dst, uint32 maxSize);
	void PrintToString(char *dest, uint32 size, char *format, ...);
	uint32 Length(char *string);

	void Serialize(Vector3 v, char *dst, uint32 maxSize);
	void Serialize(float x, char *dst, uint32 maxSize);
	void Serialize(uint32 x, char *dst, uint32 maxSize);

	float ToFloat(char *str);
}