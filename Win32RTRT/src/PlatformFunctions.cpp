#include <windows.h>
#include "PlatformFunctions.h"

READFILE(Win32ReadFile)
{
	HANDLE fileHandle = 0;
	fileHandle = CreateFile2(fileName, GENERIC_READ, NULL, OPEN_EXISTING, NULL);
	FileData result = { 0 };
	if (fileHandle != INVALID_HANDLE_VALUE)
	{
		WIN32_FILE_ATTRIBUTE_DATA fileAttributes;
		if(GetFileAttributesExW(fileName, GetFileExInfoStandard, &fileAttributes))
		{
			uint32 fileSize = fileAttributes.nFileSizeLow;
			HANDLE heap = GetProcessHeap();
			result.data = HeapAlloc(heap, HEAP_ZERO_MEMORY, fileSize);
			if (result.data)
			{
				DWORD bytesReadFromFile = 0;
				if (ReadFile(fileHandle, result.data, fileSize, &bytesReadFromFile, NULL) &&
					bytesReadFromFile == fileSize)						
				{
					result.size = bytesReadFromFile;
				}
				else
				{
					HeapFree(heap, NULL, result.data);
					OutputDebugStringA("Error reading file\n");
				}
			}
			else
			{
				OutputDebugStringA("Allocation error\n");
			}
		}
		else
		{
			OutputDebugStringA("Error getting file attributes\n");
		}
		CloseHandle(fileHandle);
	}
	else
	{
		OutputDebugStringA("Error opening file ");
		OutputDebugStringW(fileName);
		OutputDebugStringA("\n");
	}
	return result;
}

RELEASEFILE(Win32ReleaseFile)
{
	HANDLE heap = GetProcessHeap();
	HeapFree(heap, NULL, file.data);
}
