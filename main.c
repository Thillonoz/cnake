#include "raylib.h"
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define SCREEN_W 800
#define SCREEN_H 600
#define CELL_SIZE 20
#define COLS (SCREEN_W / CELL_SIZE)
#define ROWS (SCREEN_H / CELL_SIZE)
#define MAX_SNAKE_SIZE (COLS * ROWS)

typedef struct
{
	int x, y;
} Cell;

static Cell snake[MAX_SNAKE_SIZE];
static int snakeLen = 0;
static int dirX = 1, dirY = 0;
static int nextDirX = 1, nextDirY = 0;
static Cell food;
static bool gameOver = false;
static bool paused = false;
static int score = 0;
static int mult = 1;
static int key = 0;

static void SpawnFood(void)
{
	if (snakeLen != 0)
	{
		int fx, fy;
		bool ok = false;
		while (!ok)
		{
			fx = GetRandomValue(0, COLS - 1);
			fy = GetRandomValue(0, ROWS - 1);
			ok = true;
			for (int i = 0; i < snakeLen; i++)
				if (snake[i].x == fx && snake[i].y == fy)
				{
					ok = false;
					break;
				}
		}
		food.x = fx;
		food.y = fy;
	}
}

static void ResetGame(void)
{
	snakeLen = 4;
	mult = 1;
	int midX = COLS / 2;
	int midY = ROWS / 2;
	for (int i = 0; i < snakeLen; i++)
	{
		snake[i].x = midX - i;
		snake[i].y = midY;
	}
	dirX = 1;
	dirY = 0;
	nextDirX = 1;
	nextDirY = 0;
	score = 0;
	gameOver = false;
	paused = false;
	SpawnFood();
}

static bool CheckSelfCollision(int hx, int hy)
{
	bool status = false;
	for (int i = 0; i < snakeLen; i++)
	{
		if (snake[i].x == hx && snake[i].y == hy)
		{
			status = true;
		}
	}
	return status;
}

int main(void)
{
	InitWindow(SCREEN_W, SCREEN_H, "Cnake");
	SetTargetFPS(60);
	SetRandomSeed(time(NULL));

	ResetGame();

	const float updatesPerSecond = 15.0f; // snake speed
	const float secondsPerUpdate = 1.0f / updatesPerSecond;
	float accumulator = 0.0f;

	while (!WindowShouldClose())
	{
		float dt = GetFrameTime();

		key = GetKeyPressed();

		if ((key == KEY_D || key == KEY_RIGHT) && !(dirX == -1 && dirY == 0))
		{
			nextDirX = 1;
			nextDirY = 0;
		}

		if ((key == KEY_A || key == KEY_LEFT) && !(dirX == 1 && dirY == 0))
		{
			nextDirX = -1;
			nextDirY = 0;
		}

		if ((key == KEY_W || key == KEY_UP) && !(dirX == 0 && dirY == 1))
		{
			nextDirX = 0;
			nextDirY = -1;
		}

		if ((key == KEY_S || key == KEY_DOWN) && !(dirX == 0 && dirY == -1))
		{
			nextDirX = 0;
			nextDirY = 1;
		}

		if (IsKeyPressed(KEY_P))
		{
			paused = !paused;
		}

		if (gameOver && IsKeyPressed(KEY_SPACE))
		{
			ResetGame();
		}

		if (!paused && !gameOver)
		{
			accumulator += dt;
			while (accumulator >= secondsPerUpdate)
			{
				accumulator -= secondsPerUpdate;

				dirX = nextDirX;
				dirY = nextDirY;

				int newX = snake[0].x + dirX;
				int newY = snake[0].y + dirY;

				// Wall collision
				if (newX < 0 || newX >= COLS || newY < 0 || newY >= ROWS)
				{
					gameOver = true;
					break;
				}

				// Self collision
				if (CheckSelfCollision(newX, newY))
				{
					gameOver = true;
					break;
				}

				for (int i = snakeLen; i > 0; i--)
				{
					snake[i] = snake[i - 1];
				}

				snake[0].x = newX;
				snake[0].y = newY;

				// Check food
				if (snake[0].x == food.x && snake[0].y == food.y)
				{
					snakeLen++;
					if (snakeLen > MAX_SNAKE_SIZE)
					{
						snakeLen = MAX_SNAKE_SIZE;
					}
					score += 10 * mult;
					SpawnFood();
					mult++;
				}
			}
		}

		BeginDrawing();
		ClearBackground(RAYWHITE);

		for (int x = 0; x < COLS; x++)
		{
			DrawLine(x * CELL_SIZE, 0, x * CELL_SIZE, SCREEN_H, (Color){200, 122, 255, 125});
		}

		for (int y = 0; y < ROWS; y++)
		{
			DrawLine(0, y * CELL_SIZE, SCREEN_W, y * CELL_SIZE, (Color){200, 122, 255, 125});
		}

		// Draw food
		DrawRectangle(food.x * CELL_SIZE, food.y * CELL_SIZE, CELL_SIZE, CELL_SIZE, MAGENTA);

		// Draw snake
		for (int i = 0; i < snakeLen; i++)
		{
			Color c = (i == 0) ? PURPLE : PINK;
			DrawRectangle(snake[i].x * CELL_SIZE, snake[i].y * CELL_SIZE, CELL_SIZE, CELL_SIZE, c);
		}

		// HUD
		DrawText(TextFormat("Score: %i", score), 10, 10, 20, BLACK);
		if (paused)
		{
			DrawText("PAUSED", SCREEN_W / 2 - 40, SCREEN_H / 2 - 10, 20, DARKGRAY);
		}

		if (gameOver)
		{
			DrawText("GAME OVER - Press Space to restart", SCREEN_W / 2 - 180, SCREEN_H / 2 - 10, 20, MAROON);
		}

		DrawFPS(SCREEN_W - 90, 10);
		EndDrawing();
	}

	CloseWindow();
	return 0;
}
