#pragma once
#include "Core\Math.h"

struct Ball
{
	Vector2 position;
	Vector2 velocity;
	bool out;
};

struct Bat
{
	Vector2 position;
	float rotation;
};

struct Game
{
	Bat *bats;
	Ball *balls;
	uint32 fired;
	float time;
	float timeOfEnd;
};

namespace Games
{
	void Init(Game *game, Vector2 *data);
	void Init(Game *game, float *data);
	void Reset(Game *game);
	void Update(Game *game, float dt, uint32 *buffer);
}