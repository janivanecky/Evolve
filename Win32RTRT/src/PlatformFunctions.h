#pragma once
#include "Core\Def.h"

struct FileData
{
	void *data;
	uint32 size;
};

#define READFILE(name) FileData name(wchar_t *fileName)
#define RELEASEFILE(name) void name(FileData file)

typedef READFILE(PlatformReadFile);
typedef RELEASEFILE(PlatformReleaseFile);

READFILE(Win32ReadFile);
RELEASEFILE(Win32ReleaseFile);

