#include "String.h"
#include <stdarg.h>
#include <string>

bool String::Compare(char *str1, char *str2)
{
	bool same = true;
	while (*str1 != 0 && *str2 != 0)
	{
		if (*str1++ != *str2++)
		{
			same = false;
			break;
		}
	}
	if (*str1 != *str2)
	{
		same = false;
	}
	return same;
}

bool String::Contains(char *str1, char *str2)
{
	bool same = true;
	while (*str1 != 0 && *str2 != 0)
	{
		if (*str1++ != *str2++)
		{
			same = false;
			break;
		}
	}
	if (*str2 != 0)
	{
		same = false;
	}
	return same;
}


void String::Copy(char *src, char *dst)
{
	while (*src != 0)
	{
		*dst++ = *src++;
	}
	*dst = 0;
}

void String::CopySafe(char *src, char *dst, uint32 maxSize)
{
	Assert(maxSize >= 0);
	uint32 i = 0;
	while (*src != 0 && i++ < maxSize - 1)
	{
		*dst++ = *src++;
	}
	*dst = 0;
}

uint32 String::Length(char *string)
{
	uint32 count = 0;
	while (*string++ != 0)
	{
		count++;
	}
	return count;
}

void String::PrintToString(char *destStr, uint32 size, char *format, ...)
{
	va_list argList;
	va_start(argList, format);
	vsprintf_s(destStr, size, format, argList);
	va_end(argList);
}

void String::Serialize(Vector3 v, char *dst, uint32 maxSize)
{
	PrintToString(dst, maxSize, "%.2f %.2f %.2f", v.x, v.y, v.z);
}

void String::Serialize(float x, char *dst, uint32 maxSize)
{
	PrintToString(dst, maxSize, "%.2f", x);
}

void String::Serialize(uint32 x, char *dst, uint32 maxSize)
{
	PrintToString(dst, maxSize, "%d", x);
}

float String::ToFloat(char *str)
{
	float result = 0;
	if (*str != 0)
	{
		float sign = 1;
		if (*str == '-')
		{
			sign = -1.0f;
			str++;
		}
		bool fraction = false;
		float fractionPlace = 10.0f;
		while (*str != 0)
		{
			if (*str == '.')
			{
				fraction = true;
			}
			else
			{
				uint32 num = *str - 48;
				if (!fraction)
				{
					result *= 10.0f;
					result += (float)num;
				}
				else
				{
					result += (float)num / fractionPlace;
					fractionPlace *= 10.0f;
				}
			}
			str++;
		}
		result *= sign;
	}
	return result;
}