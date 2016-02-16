#include "main.h"

#include <D3D11.h>

#define WIDTH 400
#define HEIGHT 300

#if 1
int numbers[10][15] = 
{
	{
		1,1,1,
		1,0,1,
		1,0,1,
		1,0,1,
		1,1,1,
	},
	{
		0,1,0,
		0,1,0,
		0,1,0,
		0,1,0,
		0,1,0,
	},
	{
		1,1,1,
		0,0,1,
		1,1,1,
		1,0,0,
		1,1,1,
	},
	{
		1,1,1,
		0,0,1,
		1,1,1,
		0,0,1,
		1,1,1,
	},
	{
		1,0,0,
		1,0,0,
		1,1,1,
		0,0,1,
		0,0,1,
	},
	{
		1,1,1,
		1,0,0,
		1,1,1,
		0,0,1,
		1,1,1,
	},
	{
		1,1,1,
		1,0,0,
		1,1,1,
		1,0,1,
		1,1,1,
	},
	{
		1,1,1,
		0,0,1,
		0,0,1,
		0,0,1,
		0,0,1,
	},
	{
		1,1,1,
		1,0,1,
		1,1,1,
		1,0,1,
		1,1,1,
	},
	{
		1,1,1,
		1,0,1,
		1,1,1,
		0,0,1,
		0,0,1,
	}
};
#endif

static Vector2 RandomUnitCircle()
{
	float randomAngle = (rand() % 1000) / 1000.0f * PI2;
	Vector2 result(Math::Cos(randomAngle), Math::Sin(randomAngle));
	return result;
}

static void SetColor(uint32 *buffer, uint32 x, uint32 y, float r, float g, float b)
{
	if (x >= 0 && y >= 0 && x < WIDTH && y < HEIGHT)
	{
		buffer[(HEIGHT - 1) * WIDTH - y * WIDTH + x] = (0xFF << 24) | ((uint32)(0xFF * r) << 16) |
			((uint32)(0xFF * g) << 8) | (uint32)(0xFF * b);
	}
}

static void DrawNumber(uint32 *buffer, uint32 xPos, uint32 yPos, int number)
{
	int digit = number % 10;
	bool go = true;
	while (go)
	{
		for (uint32 y = 0; y < 5; y++)
		{
			for (uint32 x = 0; x < 3; ++x)
			{
				uint32 val = 1 - numbers[digit][(4 - y) * 3 + x];
				uint32 posX = xPos - 1 + x;
				uint32 posY = yPos - 2 + y;
				SetColor(buffer, posX, posY, val, val, val);
			}
		}
		number /= 10;
		if (number == 0)
		{
			go = false;
		}
		digit = number % 10;
		xPos -= 4;
	}
}

void Init(Context *context)
{
	LARGE_INTEGER l;
	QueryPerformanceCounter(&l);
	srand(l.LowPart);
	FileData vertexShader = Win32ReadFile(L"TextureVertexShader.cso");
	FileData pixelShader = Win32ReadFile(L"TexturePixelShader.cso");
	VertexInputLayout inputLayout = {};
	char *names[] = { "POSITION", "TEXCOORD" };
	InputType sizes[] = { FLOAT3, FLOAT2 };
	inputLayout.names = names;
	inputLayout.components = sizes;
	inputLayout.inputCount = 2;
	Graphics::InitPipeline(&context->renderer, &context->finalPass, inputLayout, &vertexShader, &pixelShader);
	Win32ReleaseFile(vertexShader);
	Win32ReleaseFile(pixelShader);

	float vertices[] =
	{
		-1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, -1.0f, 0.0f, 1.0f, 1.0f
	};

	uint32 indices[] = 
	{
		0,1,2,
		2,1,3
	};

	Graphics::InitModelSimple(&context->renderer, &context->screenQuad, vertices, ARRAYSIZE(vertices) * sizeof(float), sizeof(float) * 5, 
							  indices, ARRAYSIZE(indices));


	for (uint32 y = 0; y < HEIGHT; ++y)
	{
		for (uint32 x = 0; x < WIDTH; ++x)
		{
			SetColor(context->screenBuffer, x, y, 0,0,0);
		}
	}

//#define NEW_VALUES
#ifndef NEW_VALUES
	FileData rots = Win32ReadFile(L"rot");
	Vector2 *data = (Vector2 *)rots.data;
#endif

#ifdef NEW_VALUES
	Vector2 data[10];
	for (uint32 i = 0; i < 10; ++i)
	{
		float randomRadius = (rand() % 1000) / 1000.0f;
		data[i] = RandomUnitCircle() * 100.0f * randomRadius;
	}

	HANDLE fileHandle = CreateFile2(L"rot", GENERIC_WRITE, NULL, CREATE_NEW, NULL);
	if (fileHandle == INVALID_HANDLE_VALUE)
	{
		fileHandle = CreateFile2(L"rot", GENERIC_WRITE, NULL, OPEN_EXISTING, NULL);
	}
	DWORD written;
	WriteFile(fileHandle, data, sizeof(Vector2) * 10, &written, NULL);
	CloseHandle(fileHandle);
#endif

	for (uint32 i = 0; i < ARRAYSIZE(context->games); ++i)
	{
		Games::Init(&context->games[i], data);
	}
	Games::Init(&context->topGame, data);

	TextureDesc textureDesc = {};
	textureDesc.width = WIDTH;
	textureDesc.height = HEIGHT;
	textureDesc.mipCount = 1;
	textureDesc.data = context->screenBuffer;
	Graphics::InitTexture(&context->renderer, &context->screenTex, textureDesc);

	context->watching = 0;
}

void UpdateAt(uint32 *tops, uint32 *topIndexes, float *topTimes, uint32 position, uint32 val, uint32 index, float time)
{
	uint32 saveVal = val;
	uint32 saveID = index;
	float saveTime = time;
	for (uint32 i = position; i < 10; ++i)
	{
		uint32 currentVal = saveVal;
		uint32 currentID = saveID;
		float currentTime = saveTime;
		saveVal = tops[i];
		saveID = topIndexes[i];
		saveTime = topTimes[i];
		tops[i] = currentVal;
		topTimes[i] = currentTime;
		topIndexes[i] = currentID;
	}
}

void GetTop10(Game *games, uint32 count, uint32 *topVal, uint32 *topIndexes, float *topTimes)
{
	for (uint32 i = 0; i < count; ++i)
	{
		uint32 fireCount = games[i].fired;
		float time = 100.0f;
		if (fireCount > 0)
		{
			time = games[i].timeOfEnd / games[i].fired;
		}
		for (uint32 t = 0; t < 10; ++t)
		{
			uint32 currentTop = topVal[t];
			float currentTime = topTimes[t];
			if (currentTop < fireCount)
			{
				UpdateAt(topVal, topIndexes, topTimes, t, fireCount, i, time);
				break;
			}
			else if (currentTop == fireCount && currentTime < time)
			{
				UpdateAt(topVal, topIndexes, topTimes, t, fireCount, i, time);
				break;
			}
		}
	}
}

float LerpAngles(float a1, float a2)
{
	float dif = a1 - a2;
	if (Math::Abs(dif) > PI)
	{
		if (a1 > PI)
		{
			a1 -= PI2;
		}
		if (a2 > PI)
		{
			a2 -= PI2;
		}
	}
	float result = a1 * 0.5f + a2 * 0.5f;
	if (result < 0)
	{
		result += PI2;
	}
	result = Math::Fmod(result, PI2);
	return result;
}

uint32 topVal = 0;
int stagnation = 0;
uint32 score = 0;
float topTime = 100.0f;

float bestTime = 100.0f;
uint32 bestScore = 0;
void NextGen(Context *context)
{
	uint32 top[10] = {0};
	uint32 topID[10] = {0};
	float topTimes[10] = {0};
	GetTop10(context->games, 50, top, topID, topTimes);
	float rotations[10][10];
	uint32 newScore = 0;
	float newTime = 0;
	for (uint32 i = 0; i < 10; ++i)
	{
		newScore += top[i];
		newTime += topTimes[i];
		for (uint32 s = 0; s < 10; ++s)
		{
			rotations[i][s] = context->games[topID[i]].bats[s].rotation;
		}
	}
	newScore /= 10;
	newTime /= 10.0f;
	bool update = true;
	if (newScore < score || newTime > topTime)
	{
		update = false;
	}

	if (top[0] > bestScore || (top[0] == bestScore && topTimes[0] < bestTime))
	{
		bestScore = top[0];
		bestTime = topTimes[0];
		float angles[10];
		for (uint32 a = 0; a < 10; ++a)
		{
			angles[a] = rotations[0][a];
		}
		Games::Init(&context->topGame, angles);
	}
	else
	{
		++stagnation;
		Games::Reset(&context->topGame);
	}


	topVal = top[0];
	score = newScore;
	topTime = newTime;
	float nextRotations[5][10];
	float mutationRate = PI / 8.0f;
	if (stagnation > 20)
	{
		stagnation = 0;
		mutationRate *= 2.0f;
	}
	for (uint32 i = 0; i < ARRAYSIZE(context->games); ++i)
	{
		float angles[10];
		for (uint32 a = 0; a < 10; ++a)
		{
			float wiggled = rotations[i / 5][a];
			wiggled += (((rand() % 1000) / 1000.0f) * mutationRate) - mutationRate / 2.0f;
			if (wiggled < 0)
			{
				wiggled += PI2;
			}
			wiggled = Math::Fmod(wiggled, PI2);
			angles[a] = wiggled;
		}
		Games::Init(&context->games[i], angles);
	}
}

float time = 0;
int generation = 0;
bool boost = false;
void Update(Context *context, Input *input, float dt)
{
	Graphics::ClearRenderTarget(&context->renderer, &context->renderTarget);
	Graphics::BindRenderTarget(&context->renderer, &context->renderTarget);
	Graphics::BindPipeline(&context->renderer, &context->finalPass);

	if (input->mouseScroll > 0 || input->mouseScroll < 0)
	{
		float deltaLevel = input->mouseScroll / 120.0f;
		int32 watching = context->watching;
		watching += (int32)deltaLevel;
		if (watching < 0)
		{
			watching = 0;
		}
		if (watching >= ARRAYSIZE(context->games))
		{
			watching = ARRAYSIZE(context->games) - 1;
		}
		context->watching = watching;
	}

	if (input->leftButtonDown)
	{
		boost = !boost;
	}

	int cycles = 1;
	if(boost)
	{
		cycles = 30;
	}


	for (uint32 y = 0; y < HEIGHT; ++y)
	{
		for (uint32 x = 0; x < WIDTH; ++x)
		{
			SetColor(context->screenBuffer, x, y, 1, 1, 1);
		}
	}

	for (int m = 0; m < cycles; ++m)
	{
		time += 0.016f;
		if (time > 30.0f)
		{
			time = 0.0f;
			++generation;
			NextGen(context);
		}

		for (uint32 i = 0; i < ARRAYSIZE(context->games); ++i)
		{
			uint32 *buffer = NULL;
			if (i == context->watching)
			{
				//buffer = context->screenBuffer;
			}
			Games::Update(&context->games[i], 0.016f, buffer);
		}

		Games::Update(&context->topGame, 0.016f, context->screenBuffer);
	}

	DrawNumber(context->screenBuffer, 30, 50, context->topGame.fired);
	DrawNumber(context->screenBuffer, 60, 50, bestScore);
	DrawNumber(context->screenBuffer, 90, 50, bestTime * 1000.0f);
	DrawNumber(context->screenBuffer, 30, 100, generation);

	DrawNumber(context->screenBuffer, 30, 20, topVal);
	DrawNumber(context->screenBuffer, 60, 20, score);
	DrawNumber(context->screenBuffer, 90, 20, topTime * 1000.0f);

	D3D11_MAPPED_SUBRESOURCE resource = {};
	context->renderer.context->Map(context->screenTex.texResource, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	uint32 *tex = context->screenBuffer;
	char *dst = (char *)resource.pData;
	for (uint32 i = 0; i < HEIGHT; ++i)
	{
		memcpy(dst, tex, WIDTH * sizeof(uint32));
		dst += resource.RowPitch;
		tex += WIDTH;
	}
	context->renderer.context->Unmap(context->screenTex.texResource, 0);
	Graphics::BindTexture(&context->renderer, &context->screenTex);
	Graphics::RenderModel(&context->renderer, &context->screenQuad);
}

void Release(Context *context)
{
	Graphics::Release(&context->finalPass);
	Graphics::Release(&context->renderer);
	Graphics::Release(&context->screenQuad);
	Graphics::Release(&context->screenTex);
}