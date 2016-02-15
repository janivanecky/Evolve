#include "Game.h"
#include <cstdlib>

#define WIDTH 400
#define HEIGHT 300


static Vector2 RandomUnitCircle()
{
	float randomAngle = (rand() % 1000) / 1000.0f * PI2;
	Vector2 result(Math::Cos(randomAngle), Math::Sin(randomAngle));
	return result;
}

static void SetColor(uint32 *buffer, float xD, float yD, float r, float g, float b)
{
	if (!buffer)
	{
		return;
	}
	uint32 x = (uint32)xD;
	uint32 y = (uint32)yD;
	if (x >= 0 && y >= 0 && x < WIDTH && y < HEIGHT)
	{
		buffer[(HEIGHT - 1) * WIDTH - y * WIDTH + x] = (0xFF << 24) | ((uint32)(0xFF * r) << 16) |
			((uint32)(0xFF * g) << 8) | (uint32)(0xFF * b);
	}
}

void Games::Init(Game *game, float *data)
{
	game->time = 0;
	game->timeOfEnd = 120.0f;
	game->fired = 0;
	for (uint32 i = 0; i < 100; ++i)
	{
		uint32 randomIndex = rand() % 10;
		game->balls[i].position = game->bats[randomIndex].position;
		game->balls[i].velocity = RandomUnitCircle() * 30.0f;
		game->balls[i].out = false;
	}

	for (uint32 i = 0; i < 10; ++i)
	{
		game->bats[i].rotation = data[i];
	}
}

void Games::Init(Game *game, Vector2 *data)
{
	game->time = 0;
	game->timeOfEnd = 30.0f;
	game->fired = 0;
	game->balls = new Ball[100];
	game->bats = new Bat[10];

	for (uint32 i = 0; i < 10; ++i)
	{
		game->bats[i].position = data[i];
		game->bats[i].rotation = (rand() % 1000) / 1000.0f * PI2;
	}

	for (uint32 i = 0; i < 100; ++i)
	{
		uint32 randomIndex = rand() % 10;
		game->balls[i].position = game->bats[randomIndex].position;
		game->balls[i].velocity = RandomUnitCircle() * 30.0f;
		game->balls[i].out = false;
	}

}

void Games::Reset(Game *game)
{
	game->time = 0;
	game->timeOfEnd = 30.0f;
	game->fired = 0;
	for (uint32 i = 0; i < 100; ++i)
	{
		uint32 randomIndex = rand() % 10;
		game->balls[i].position = game->bats[randomIndex].position;
		game->balls[i].velocity = RandomUnitCircle() * 30.0f;
		game->balls[i].out = false;
	}
}


void Games::Update(Game *game, float dt, uint32 *buffer)
{
	game->time += dt;
	Vector2 center = Vector2(WIDTH / 2.0f, HEIGHT / 2.0f);
	Ball *balls = game->balls;
	for (uint32 i = 0; i < 100; ++i)
	{
		Ball *ball = &balls[i];
		for (uint32 b = 0; b < 10; ++b)
		{
			Bat bat = game->bats[b];
			Vector2 distance = bat.position - ball->position;
			if (Math::Length(distance) < 3.0f)
			{
				ball->velocity.x = Math::Cos(bat.rotation) * 50.0f;
				ball->velocity.y = Math::Sin(bat.rotation) * 50.0f;
				Vector2 offset = Math::Normalize(ball->velocity) * 3.5f;
				ball->position = bat.position + offset;
			}
		}

		if (Math::Length(balls[i].velocity) > 15.0f && !balls[i].out)
		{
			balls[i].velocity += -balls[i].velocity * 0.005f;
		}
		balls[i].position += balls[i].velocity * dt;
		if (Math::Length(balls[i].position) > 130.0f)
		{
			if (Math::Length(balls[i].velocity) > 31.0f)
			{
				if (!balls[i].out)
				{
					balls[i].out = true;
					++game->fired;
					game->timeOfEnd = game->time;
				}
			}
			else
			{
				Vector2 normal = -Math::Normalize(balls[i].position);
				Vector2 reflect = 2 * Math::Dot(-balls[i].velocity, normal) * normal + balls[i].velocity;
				balls[i].velocity = reflect;
			}
		}
		SetColor(buffer, balls[i].position.x + center.x, balls[i].position.y + center.y, 0, 0, 0);
	}

	for (uint32 i = 0; i < 10; ++i)
	{
		Bat bat = game->bats[i];
		Vector2 pos = bat.position + center;
		SetColor(buffer, pos.x, pos.y, 0, 1, 0);
		SetColor(buffer, pos.x + 1, pos.y, 0, 1, 0);
		SetColor(buffer, pos.x + 1, pos.y + 1, 0, 1, 0);
		SetColor(buffer, pos.x, pos.y + 1, 0, 1, 0);
		SetColor(buffer, pos.x - 1, pos.y + 1, 0, 1, 0);
		SetColor(buffer, pos.x - 1, pos.y, 0, 1, 0);
		SetColor(buffer, pos.x - 1, pos.y - 1, 0, 1, 0);
		SetColor(buffer, pos.x, pos.y - 1, 0, 1, 0);
		SetColor(buffer, pos.x + 1, pos.y - 1, 0, 1, 0);
	}

	for (uint32 i = 0; i < 100; ++i)
	{
		float angle = i * (PI2 / 100.0f);
		Vector2 pos(Math::Cos(angle) * 130.0f, Math::Sin(angle) * 130.0f);
		SetColor(buffer, pos.x + center.x, pos.y + center.y, 1, 0, 0);
	}
}