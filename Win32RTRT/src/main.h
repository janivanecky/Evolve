#include "Core\Def.h"
#include "Game.h"
#include "Graphics.h"

struct Input
{
	float mouseScroll;
	bool leftButtonDown;
	Vector2 touchPosition;
	Vector2 previousPosition;
};

struct Context
{
	Renderer renderer;
	ScreenRenderTarget renderTarget;
	GraphicsPipeline finalPass;
	Model3D screenQuad;
	Texture screenTex;

	Game games[50];
	Game topGame;
	uint32 watching;

	uint32 *screenBuffer;
};

void Init(Context *context);
void Update(Context *context, Input *input, float dt);
void Release(Context *context);
