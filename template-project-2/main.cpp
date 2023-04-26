#define _USE_MATH_DEFINES
#include<math.h>
#include<stdio.h>
#include<string.h>
#include <cstdlib>
#include <cstdio>
#include <ctime>

extern "C" {
#include"./SDL2-2.0.10/include/SDL.h"
#include"./SDL2-2.0.10/include/SDL_main.h"
}

#define SCREEN_WIDTH			1280
#define SCREEN_HEIGHT			800

#define LEVEL_WIDTH				2560
#define LEVEL_HEIGHT			1600

#define ENEMY_WIDTH				110
#define ENEMY_HEIGHT			180

#define FPS						60

#define NUM_OF_BULLETS			3000

#define BULLET_WIDTH			34
#define BULLET_HEIGHT			34

#define CHARACTER_WIDTH			50
#define CHARACTER_HEIGHT		100

#define POISON_WIDTH			180
#define POISON_HEIGHT			180

#define BULLET_SPEED			2
#define BULLET_SEPERATION		3 //time in seconds betwen shots

#define PLAYER_BULLET_WIDTH		16
#define PLAYER_BULLET_HEIGHT	50

#define HIT_COOLDOWN			2 //cooldown betwen character getting hit
#define COOLDOWN				2 //cooldown betwen character schooting

#define E1_HP					5 //HP of first enemy
#define E2_HP					4
#define E3_HP					5
#define E4_HP					5
#define PLAYER_HP				20

#define MOVE_VALUE				6 //only even

struct Bullet
{
	int pattern;
	int onScreen = 0;
	double bulletX = LEVEL_WIDTH / 4;
	double bulletY = LEVEL_HEIGHT / 2;
	int speed = 1;
	int bigBullet = 0;
};

void pociski(int i, Bullet bullets[NUM_OF_BULLETS])
{
	bullets[i].pattern;
	bullets[i].onScreen = 0;
	bullets[i].bulletX = LEVEL_WIDTH / 4;
	bullets[i].bulletY = LEVEL_HEIGHT / 2;
	bullets[i].speed = 1;
	bullets[i].bigBullet = 0;
};

void tworzenie(Bullet bullets[NUM_OF_BULLETS])
{
	for (int i = 0; i < NUM_OF_BULLETS; i++)
	{
		pociski(i, bullets);
	}
};

// draw a text txt on surface screen, starting from the point (x, y)
// charset is a 128x128 bitmap containing character images
void DrawString(SDL_Surface *screen, int x, int y, const char *text,SDL_Surface *charset) {
	int px, py, c;
	SDL_Rect s, d;
	s.w = 8;
	s.h = 8;
	d.w = 8;
	d.h = 8;
	while(*text) {
		c = *text & 255;
		px = (c % 16) * 8;
		py = (c / 16) * 8;
		s.x = px;
		s.y = py;
		d.x = x;
		d.y = y;
		SDL_BlitSurface(charset, &s, screen, &d);
		x += 8;
		text++;
		};
	};

// draw a surface sprite on a surface screen in point (x, y)
// (x, y) is the center of sprite on screen
void DrawSurface(SDL_Surface *screen, SDL_Surface *sprite, int x, int y) {
	SDL_Rect dest;
	dest.x = x - sprite->w / 2;
	dest.y = y - sprite->h / 2;
	dest.w = sprite->w;
	dest.h = sprite->h;
	SDL_BlitSurface(sprite, NULL, screen, &dest);
	};

// draw a single pixel
void DrawPixel(SDL_Surface *surface, int x, int y, Uint32 color) {
	int bpp = surface->format->BytesPerPixel;
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
	*(Uint32 *)p = color;
	};

// draw a vertical (when dx = 0, dy = 1) or horizontal (when dx = 1, dy = 0) line
void DrawLine(SDL_Surface *screen, int x, int y, int l, int dx, int dy, Uint32 color) {
	for(int i = 0; i < l; i++) {
		DrawPixel(screen, x, y, color);
		x += dx;
		y += dy;
		};
	};

// draw a rectangle of size l by k
void DrawRectangle(SDL_Surface *screen, int x, int y, int l, int k, Uint32 outlineColor, Uint32 fillColor) {
	int i;
	DrawLine(screen, x, y, k, 0, 1, outlineColor);
	DrawLine(screen, x + l - 1, y, k, 0, 1, outlineColor);
	DrawLine(screen, x, y, l, 1, 0, outlineColor);
	DrawLine(screen, x, y + k - 1, l, 1, 0, outlineColor);
	for(i = y + 1; i < y + k - 1; i++)
		DrawLine(screen, x + 1, i, l - 2, 1, 0, fillColor);
	};

void MoveCamera(double *backgroundX, double *backgroundY, double *enemyX, double *enemyY, double* enemy2X, double* enemy2Y, int* poisonX, int* poisonY)
{
	if (*backgroundY < 0)
	{
		*backgroundY = 0;
		*enemyY = LEVEL_HEIGHT / 2 - ENEMY_HEIGHT / 2;
		*enemy2Y = 0;
		*poisonY += MOVE_VALUE;
	}
	if (*backgroundX < 0)
	{
		*backgroundX = 0;
		*enemyX = 0;
		*enemy2X += MOVE_VALUE;
		*poisonX += MOVE_VALUE;
	}
	if (*backgroundY > LEVEL_HEIGHT - SCREEN_HEIGHT)
	{
		*backgroundY = LEVEL_HEIGHT - SCREEN_HEIGHT;
		*enemyY = LEVEL_HEIGHT - ENEMY_HEIGHT / 2;
		*enemy2Y = SCREEN_HEIGHT;
		*poisonY -= MOVE_VALUE;
	}
	if (*backgroundX > LEVEL_WIDTH - SCREEN_WIDTH)
	{
		*backgroundX = LEVEL_WIDTH - SCREEN_WIDTH;
		*enemyX = LEVEL_WIDTH - SCREEN_WIDTH;
		*enemy2X -= MOVE_VALUE;
		*poisonX -= MOVE_VALUE;
	}
}

void MoveBullet(double* backgroundX, double* backgroundY, Bullet bullets[NUM_OF_BULLETS])
{
	for (int i = 0; i < NUM_OF_BULLETS; i++)
	{
		if (bullets[i].onScreen == 1 || bullets[i].onScreen == 0)
		{
			if (*backgroundY < 0)
			{
				bullets[i].bulletY += MOVE_VALUE;
			}

			if (*backgroundX < 0)
			{
				bullets[i].bulletX += MOVE_VALUE;
			}

			if (*backgroundY > LEVEL_HEIGHT - SCREEN_HEIGHT)
			{
				bullets[i].bulletY -= MOVE_VALUE;
			}

			if (*backgroundX > LEVEL_WIDTH - SCREEN_WIDTH)
			{
				bullets[i].bulletX -= MOVE_VALUE;
			}
		}

		if (bullets[i].bulletY < - (LEVEL_HEIGHT / 2) || bullets[i].onScreen == 2 || bullets[i].bulletX > LEVEL_WIDTH * 1.2 || bullets[i].bulletY > LEVEL_HEIGHT * 2 || bullets[i].bulletX < -(LEVEL_WIDTH/2))
		{
			bullets[i].pattern = 0;
			bullets[i].onScreen = 0;
			bullets[i].bulletX = SCREEN_WIDTH / 2;
			bullets[i].bulletY = SCREEN_HEIGHT;
		}
	}
}

void keyboard(Bullet bullets[NUM_OF_BULLETS], double* backgroundX, double* backgroundY, double* enemyX, double* enemyY, double* enemy2X, double* enemy2Y, int* poisonX, int *poisonY, int *pomBullet, double *cooldown) {
	const Uint8* pressed = SDL_GetKeyboardState(NULL);
	if (pressed[SDL_SCANCODE_UP])
	{
		*backgroundY += MOVE_VALUE;
		*enemyY += MOVE_VALUE;
		*enemy2Y += MOVE_VALUE;
		*poisonY += MOVE_VALUE;

		for (int i = 0; i < NUM_OF_BULLETS; i++)
		{
			bullets[i].bulletY += MOVE_VALUE;
		}
	}
	if (pressed[SDL_SCANCODE_DOWN])
	{
		*backgroundY -= MOVE_VALUE;
		*enemyY -= MOVE_VALUE;
		*enemy2Y -= MOVE_VALUE;
		*poisonY -= MOVE_VALUE;
		for (int i = 0; i < NUM_OF_BULLETS; i++)
		{
			bullets[i].bulletY -= MOVE_VALUE;
		}
	}
	if (pressed[SDL_SCANCODE_LEFT])
	{
		*backgroundX += MOVE_VALUE;
		*enemyX += MOVE_VALUE;
		*enemy2X += MOVE_VALUE;
		*poisonX += MOVE_VALUE;
		for (int i = 0; i < NUM_OF_BULLETS; i++)
		{
			bullets[i].bulletX += MOVE_VALUE;
		}
	}
	if (pressed[SDL_SCANCODE_RIGHT])
	{
		*backgroundX -= MOVE_VALUE;
		*enemyX -= MOVE_VALUE;
		*enemy2X -= MOVE_VALUE;
		*poisonX -= MOVE_VALUE;
		for (int i = 0; i < NUM_OF_BULLETS; i++)
		{
			bullets[i].bulletX -= MOVE_VALUE;
		}
	}
	if (pressed[SDL_SCANCODE_SPACE])
	{
		if (*cooldown < 0)
		{
			*pomBullet += 1;
			bullets[*pomBullet].onScreen = 1;
			bullets[*pomBullet].bulletX = SCREEN_WIDTH / 2;
			bullets[*pomBullet].bulletY = (SCREEN_HEIGHT / 2) + CHARACTER_HEIGHT / 2 + BULLET_HEIGHT / 2;
			bullets[*pomBullet].pattern = 19;
			*cooldown = FPS * COOLDOWN;
			bullets[*pomBullet].bigBullet = 3;
			*pomBullet += 1;
		}
	}
}

//detects weyher players bullets colided with enemy
void PlayerBulletCollision(Bullet bullets[NUM_OF_BULLETS], int i, double enemyX, double enemyY, double enemy2X, double enemy2Y, double stage, int* enemy1HP, int* enemy2HP, int* enemy3HP, int* enemy4HP, int* playerHP)
{
	if (stage == 1)
		if (bullets[i].bulletX - PLAYER_BULLET_WIDTH / 2 < enemyX + ENEMY_WIDTH / 2 &&
			bullets[i].bulletX + PLAYER_BULLET_WIDTH / 2 > enemyX - ENEMY_WIDTH / 2 &&
			bullets[i].bulletY - PLAYER_BULLET_HEIGHT / 2 < enemyY + ENEMY_HEIGHT / 2 &&
			bullets[i].bulletY + PLAYER_BULLET_HEIGHT / 2 > enemyY - ENEMY_HEIGHT / 2)
		{
			*enemy1HP -= 1;
			bullets[i].onScreen = 2;
		}
	if (stage == 2)
		if (bullets[i].bulletX - PLAYER_BULLET_WIDTH / 2 < enemy2X + ENEMY_WIDTH / 2 &&
			bullets[i].bulletX + PLAYER_BULLET_WIDTH / 2 > enemy2X - ENEMY_WIDTH / 2 &&
			bullets[i].bulletY - PLAYER_BULLET_HEIGHT / 2 < enemy2Y + ENEMY_HEIGHT / 2 &&
			bullets[i].bulletY + PLAYER_BULLET_HEIGHT / 2 > enemy2Y - ENEMY_HEIGHT / 2)
		{
			*enemy2HP -= 1;
			bullets[i].onScreen = 2;
		}
	if (stage == 3)
		if (bullets[i].bulletX - PLAYER_BULLET_WIDTH / 2 < enemyX + ENEMY_WIDTH / 2 &&
			bullets[i].bulletX + PLAYER_BULLET_WIDTH / 2 > enemyX - ENEMY_WIDTH / 2 &&
			bullets[i].bulletY - PLAYER_BULLET_HEIGHT / 2 < enemyY + ENEMY_HEIGHT / 2 &&
			bullets[i].bulletY + PLAYER_BULLET_HEIGHT / 2 > enemyY - ENEMY_HEIGHT / 2)
		{
			*enemy3HP -= 1;
			bullets[i].onScreen = 2;
		}
	if (stage == 4)
		if (bullets[i].bulletX - PLAYER_BULLET_WIDTH / 2 < enemyX + ENEMY_WIDTH / 2 &&
			bullets[i].bulletX + PLAYER_BULLET_WIDTH / 2 > enemyX - ENEMY_WIDTH / 2 &&
			bullets[i].bulletY - PLAYER_BULLET_HEIGHT / 2 < enemyY + ENEMY_HEIGHT / 2 &&
			bullets[i].bulletY + PLAYER_BULLET_HEIGHT / 2 > enemyY - ENEMY_HEIGHT / 2)
		{
			*enemy4HP -= 1;
			bullets[i].onScreen = 2;
		}

}

void Collision(Bullet bullets[NUM_OF_BULLETS], double enemy2X, double enemy2Y, int stage, double enemyX, double enemyY, double *hitCooldown, int *enemy1HP, int *enemy2HP, int *enemy3HP, int *enemy4HP, int *playerHP)
{
	for (int i = 0; i < NUM_OF_BULLETS; i++)
	{
		if (bullets[i].onScreen == 1)
		{
			if (bullets[i].bigBullet == 3)
			{
				PlayerBulletCollision(bullets, i, enemyX, enemyY, enemy2X, enemy2Y, stage, enemy1HP, enemy2HP, enemy3HP, enemy4HP, playerHP);
			}
			if (*hitCooldown < 0)
			{
				if (bullets[i].bulletX - BULLET_WIDTH / 2 < SCREEN_WIDTH / 2 + CHARACTER_WIDTH / 2 &&
					bullets[i].bulletX + BULLET_WIDTH / 2 > SCREEN_WIDTH / 2 - CHARACTER_WIDTH / 2 &&
					bullets[i].bulletY - BULLET_HEIGHT / 2 < SCREEN_HEIGHT / 2 + CHARACTER_HEIGHT / 2 &&
					bullets[i].bulletY + BULLET_HEIGHT / 2 > SCREEN_HEIGHT / 2 - CHARACTER_HEIGHT / 2)
				{
					*hitCooldown = FPS * HIT_COOLDOWN;
					bullets[i].onScreen = 2;
					*playerHP -= 1;
				}
			}
		}
	}
}

void CollisionWithPoison(int* poisonX, int* poisonY, int *poisonOn, int *playerHP)
{		
		if (*poisonX - POISON_WIDTH / 2 < SCREEN_WIDTH / 2 + CHARACTER_WIDTH / 2 &&
			*poisonX + POISON_WIDTH / 2 > SCREEN_WIDTH / 2 - CHARACTER_WIDTH / 2 &&
			*poisonY - POISON_HEIGHT / 2 < SCREEN_HEIGHT / 2 + CHARACTER_HEIGHT / 2 &&
			*poisonY + POISON_HEIGHT / 2 > SCREEN_HEIGHT / 2 - CHARACTER_HEIGHT / 2)
		{
			*poisonOn = 2;
			*poisonX = 0;
			*poisonY = 0;
			*playerHP -= 1;
		}
}

void CirclePattern(Bullet bullets[NUM_OF_BULLETS], double distance, int i, double enemy2X, double enemy2Y)
{
	if (i % 8 == 0)
	{
		bullets[i].bulletX += sqrt(BULLET_SPEED);
		bullets[i].bulletY -= sqrt(BULLET_SPEED);
	}
	if (i % 8 == 1)
	{
		bullets[i].bulletX += sqrt(BULLET_SPEED);
		bullets[i].bulletY += sqrt(BULLET_SPEED);
	}
	if (i % 8 == 2)
	{
		bullets[i].bulletX -= sqrt(BULLET_SPEED);
		bullets[i].bulletY += sqrt(BULLET_SPEED);
	}
	if (i % 8 == 3)
	{
		bullets[i].bulletX -= sqrt(BULLET_SPEED);
		bullets[i].bulletY -= sqrt(BULLET_SPEED);
	}
	if (i % 8 == 4)
	{
		bullets[i].bulletX += (BULLET_SPEED);
	}
	if (i % 8 == 5)
	{
		bullets[i].bulletY += (BULLET_SPEED);
	}
	if (i % 8 == 6)
	{
		bullets[i].bulletX -= (BULLET_SPEED);
	}
	if (i % 8 == 7)
	{
		bullets[i].bulletY -= (BULLET_SPEED);
	}
}

void BulletPattern(Bullet bullets[NUM_OF_BULLETS], double distance, int i, double enemy2X, double enemy2Y)
{
		if (bullets[i].onScreen == 1 && bullets[i].pattern == -1)
		{
			bullets[i].bulletX = LEVEL_WIDTH / 4 + sin(distance) * LEVEL_HEIGHT / 6;
			bullets[i].bulletY -= BULLET_SPEED;
		}
		if (bullets[i].onScreen == 1 && bullets[i].pattern == 0)
		{
			bullets[i].bulletY -= BULLET_SPEED;
		}
		if (bullets[i].onScreen == 1 && bullets[i].pattern == 1)
		{
			bullets[i].bulletX += 0.9;
			bullets[i].bulletY -= BULLET_SPEED;
		}
		if (bullets[i].onScreen == 1 && bullets[i].pattern == 2)
		{
			bullets[i].bulletX += 1.8;
			bullets[i].bulletY -= BULLET_SPEED;
		}
		if (bullets[i].onScreen == 1 && bullets[i].pattern == 3)
		{
			bullets[i].bulletX += 2.7;
			bullets[i].bulletY -= BULLET_SPEED;
		}
		if (bullets[i].onScreen == 1 && bullets[i].pattern == 4)
		{
			bullets[i].bulletX += 3.6;
			bullets[i].bulletY -= BULLET_SPEED;
		}
		if (bullets[i].onScreen == 1 && bullets[i].pattern == 5)
		{
			bullets[i].bulletX += 4.5;
			bullets[i].bulletY -= BULLET_SPEED;
		}
		if (bullets[i].onScreen == 1 && bullets[i].pattern == 6)
		{
			bullets[i].bulletX -= 0.9;
			bullets[i].bulletY -= BULLET_SPEED;
		}
		if (bullets[i].onScreen == 1 && bullets[i].pattern == 7)
		{
			bullets[i].bulletX -= 1.8;
			bullets[i].bulletY -= BULLET_SPEED;
		}
		if (bullets[i].onScreen == 1 && bullets[i].pattern == 8)
		{
			bullets[i].bulletX -= 2.7;
			bullets[i].bulletY -= BULLET_SPEED;
		}
		if (bullets[i].onScreen == 1 && bullets[i].pattern == 9)
		{
			bullets[i].bulletX -= 3.6;
			bullets[i].bulletY -= BULLET_SPEED;
		}
		if (bullets[i].onScreen == 1 && bullets[i].pattern == 10)
		{
			bullets[i].bulletX -= 4.5;
			bullets[i].bulletY -= BULLET_SPEED;
		}
		if (bullets[i].onScreen == 1 && bullets[i].pattern == 11)
		{
			bullets[i].bulletX = enemy2X + sin(distance * 4) * LEVEL_HEIGHT / 12;
			bullets[i].bulletY = enemy2Y + cos(distance * 4) * LEVEL_WIDTH / 12;
		}
		if (bullets[i].onScreen == 1 && bullets[i].pattern == 12)
		{
			bullets[i].bulletX = enemy2X + sin(distance) * LEVEL_HEIGHT / 6;
			bullets[i].bulletY = enemy2Y + cos(distance) * LEVEL_WIDTH / 6;
		}
		if (bullets[i].onScreen == 1 && bullets[i].pattern == 13)
		{
			bullets[i].bulletX = enemy2X + sin(distance * 0.7) * LEVEL_HEIGHT / 3;
			bullets[i].bulletY = enemy2Y + cos(distance * 0.7) * LEVEL_WIDTH / 3;
		}
		if (bullets[i].onScreen == 1 && bullets[i].pattern == 14)
		{
			bullets[i].bulletX = enemy2X + sin(distance * 0.6) * LEVEL_HEIGHT / 2;
			bullets[i].bulletY = enemy2Y + cos(distance * 0.6) * LEVEL_WIDTH / 2;
		}
		if(bullets[i].onScreen == 1 && bullets[i].pattern == 15)
		{
			CirclePattern(bullets, distance, i, enemy2X, enemy2Y);
		}
		if (bullets[i].onScreen == 1 && bullets[i].pattern == 16)
		{
			bullets[i].bulletY -= BULLET_SPEED * bullets[i].speed;
		}
		if (bullets[i].pattern == 17)
		{
			bullets[i].bulletX += 0.9;
			bullets[i].bulletY -= BULLET_SPEED;
			if (bullets[i].bulletY < SCREEN_HEIGHT / 2)
			{
				for (int j = 0; j <= 8; j++)
				{
					bullets[j + i].onScreen = 1;
					bullets[j + i].pattern = 15;
				}
			}
		}
		if (bullets[i].pattern == 18)
		{
			bullets[i].bulletX -= 0.9;
			bullets[i].bulletY -= BULLET_SPEED;
			if (bullets[i].bulletY < SCREEN_HEIGHT / 2)
			{
				for (int j = 0; j <= 8; j++)
				{
					bullets[j + i].onScreen = 1;
					bullets[j + i].pattern = 15;
				}
			}
		}
		if (bullets[i].onScreen == 1 && bullets[i].pattern == 19)
		{
			bullets[i].bulletY += BULLET_SPEED;
		}
}

void AddBullet(Bullet bullets[NUM_OF_BULLETS], int pattern, int *pomBullet)
{
	*pomBullet += 1;
	bullets[*pomBullet].onScreen = 1;
	bullets[*pomBullet].pattern = pattern;
;
}
// main
#ifdef __cplusplus
extern "C"
#endif
int main(int argc, char **argv) {
	const int SCREEN_TICKS_PER_FRAME = 1000 / FPS;
	int t1, t2, quit, frames, rc, frameTime, pomBullet, time2, currentPattern, stage, poisonX, poisonY, poisonOn, exit;
	int enemy1HP, enemy2HP, enemy3HP, enemy4HP, playerHP, HPtoDisplay;
	double delta, worldTime, fpsTimer, fps, distance, cooldown, hitCooldown;
	double backgroundX, backgroundY, enemyX, enemyY, enemy2X, enemy2Y;
	SDL_Event event;
	SDL_Surface *screen, *charset;
	SDL_Surface *enemy, *bullet, *tlo, *character, *enemy2, *enemy3, *enemy4, *aura, *warning, *fireball, *playersBullet, *menu;
	SDL_Texture *scrtex;
	SDL_Window *window;
	SDL_Renderer *renderer;
	Bullet bullets[NUM_OF_BULLETS];
	
	srand(time(0));

	if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("SDL_Init error: %s\n", SDL_GetError());
		return 1;
	}

	rc = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer);
	if(rc != 0) {
		SDL_Quit();
		printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
		return 1;
	};
	
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_SetWindowTitle(window, "Bullet hell by Adam Gasiorowski");

	screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

	scrtex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);

	SDL_ShowCursor(SDL_DISABLE);

	charset = SDL_LoadBMP("./cs8x8.bmp");
	if(charset == NULL) {
		printf("SDL_LoadBMP(cs8x8.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
		};
	SDL_SetColorKey(charset, true, 0x000000);

	tlo = SDL_LoadBMP("./tlo.bmp");
	if (tlo == NULL) {
		printf("SDL_LoadBMP(tlo.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	enemy = SDL_LoadBMP("./enemy.bmp");
	if(enemy == NULL) {
		printf("SDL_LoadBMP(enemy.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	enemy2 = SDL_LoadBMP("./enemy2.bmp");
	if (enemy2 == NULL) {
		printf("SDL_LoadBMP(enemy2.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	enemy3 = SDL_LoadBMP("./enemy3.bmp");
	if (enemy3 == NULL) {
		printf("SDL_LoadBMP(enemy3.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	enemy4 = SDL_LoadBMP("./wizard.bmp");
	if (enemy4 == NULL) {
		printf("SDL_LoadBMP(wizard.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	character = SDL_LoadBMP("./character.bmp");
	if (character == NULL) {
		printf("SDL_LoadBMP(character.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	bullet = SDL_LoadBMP("./bullet.bmp");
	if (bullet == NULL) {
		printf("SDL_LoadBMP(bullet.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	aura = SDL_LoadBMP("./aura.bmp");
	if (aura == NULL) {
		printf("SDL_LoadBMP(aura.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	warning = SDL_LoadBMP("./warning.bmp");
	if (warning == NULL) {
		printf("SDL_LoadBMP(warning.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	fireball = SDL_LoadBMP("./fireball.bmp");
	if (fireball == NULL) {
		printf("SDL_LoadBMP(fireball.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	playersBullet = SDL_LoadBMP("./playersBullet.bmp");
	if (playersBullet == NULL) {
		printf("SDL_LoadBMP(playersBullet.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	menu = SDL_LoadBMP("./menu.bmp");
	if (menu == NULL) {
		printf("SDL_LoadBMP(menu.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	char text[128];
	int czarny = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);
	int zielony = SDL_MapRGB(screen->format, 0x00, 0xFF, 0x00);
	int czerwony = SDL_MapRGB(screen->format, 0xFF, 0x00, 0x00);
	int niebieski = SDL_MapRGB(screen->format, 0x11, 0x11, 0xCC);

	t1 = SDL_GetTicks();

	frames = 0;
	fpsTimer = 0;
	fps = 0;
	quit = 0;
	worldTime = 0;
	distance = 0;
	backgroundY = 0;
	backgroundX = LEVEL_WIDTH / 4;
	enemyX = SCREEN_WIDTH / 2; 
	enemyY = SCREEN_HEIGHT - ENEMY_HEIGHT / 2;
	enemy2X = 0;
	enemy2Y =  0;
	pomBullet = 0;
	time2 = 0;
	currentPattern = 0;
	stage = 1;
	distance = 0;
	poisonX = LEVEL_WIDTH / 4;
	poisonY = 0;
	poisonOn = 0;
	cooldown = FPS * COOLDOWN;
	hitCooldown = FPS * HIT_COOLDOWN;
	enemy1HP = E1_HP;
	enemy2HP = E2_HP;
	enemy3HP = E3_HP;
	enemy4HP = E4_HP;
	playerHP = -1;
	HPtoDisplay = 0;
	exit = 0;

	tworzenie(bullets);

	while (!quit) {
		t2 = SDL_GetTicks();

		if (playerHP == -1)
		{
			DrawSurface(screen, menu, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
		}
		if (playerHP > 0)
		{
			keyboard(bullets, &backgroundX, &backgroundY, &enemyX, &enemyY, &enemy2X, &enemy2Y, &poisonX, &poisonY, &pomBullet, &cooldown); //keyboard inputs

			delta = (t2 - t1) * 0.001; //delta betwen frames
			t1 = t2;

			cooldown -= delta * 100;    //counting cooldown
			hitCooldown -= delta * 100;

			worldTime += delta;
			distance += MOVE_VALUE / 4 * delta;

			DrawSurface(screen, tlo, backgroundX, backgroundY);

			//determine whether the stage should end
			if (enemy1HP == 0)
			{
				enemy1HP = -1;
				stage = 2;
			}

			if (enemy2HP == 0)
			{
				enemy2HP = -1;
				stage = 3;
			}

			if (enemy3HP == 0)
			{
				enemy3HP = -1;
				stage = 4;
			}

			if (enemy4HP == 0)
			{
				enemy4HP = -1;
				stage = 5;
			}

			if (time2 % (FPS * BULLET_SEPERATION) == 0) //fires bullet or bullet pttern every 3 seconds
			{
				if (stage == 1)
				{
					if (time2 < FPS * 15)
					{
						AddBullet(bullets, -1, &pomBullet);
					}

					else
					{
						for (int i = 0; i < 10; i++)
						{
							AddBullet(bullets, i, &pomBullet);

							if (i % 5 == 0)
							{
								AddBullet(bullets, -1, &pomBullet);
							}
						}
						currentPattern = 10;
					}
				}

				else if (stage == 2)
				{
					if (currentPattern == 10)
					{
						for (int i = 11; i <= 14; i++)
						{
							AddBullet(bullets, i, &pomBullet);
						}
						currentPattern++;
					}

					for (int i = 0; i < 8; i++)
					{
						pomBullet++;
						bullets[pomBullet].onScreen = 1;
						bullets[pomBullet].bulletX = enemy2X;
						bullets[pomBullet].bulletY = enemy2Y;
						bullets[pomBullet].pattern = 15;
					}

					if (enemy2X > LEVEL_WIDTH - (SCREEN_WIDTH / 2))
					{
						enemy2X = 0;
					}
				}

				else if (stage == 3)
				{
					for (int i = 0; i <= 20; i++)
					{
						pomBullet++;
						bullets[pomBullet].onScreen = 1;
						bullets[pomBullet].bulletX = rand() % LEVEL_WIDTH;
						bullets[pomBullet].speed = (rand() % 3) + 1;
						bullets[pomBullet].pattern = 16;
					}
				}

				else if (stage == 4)
				{
					AddBullet(bullets, 0, &pomBullet);

					if (pomBullet % 3 == 0)
					{
						poisonX = SCREEN_WIDTH / 2;
						poisonY = SCREEN_HEIGHT / 2;
						poisonOn = 2;
						poisonX += (rand() % SCREEN_WIDTH / 4) - SCREEN_HEIGHT / 8;
						poisonY += (rand() % SCREEN_HEIGHT / 4) - SCREEN_WIDTH / 8;
						bullets[pomBullet - 1].bigBullet = 1;
					}
					else if (pomBullet % 3 > 0)
					{
						bullets[pomBullet].onScreen = 1;

						for (int i = 0; i <= 8; i++)
						{
							AddBullet(bullets, 17, &pomBullet);
						}
						for (int i = 0; i <= 8; i++)
						{
							AddBullet(bullets, 18, &pomBullet);
						}
						poisonOn = 1;
					}
					pomBullet++;
					bullets[pomBullet].onScreen = 0;
				}

				if (pomBullet > NUM_OF_BULLETS)
				{
					pomBullet = 0;
					time2 = 0;
				}
			}

			for (int i = 0; i < NUM_OF_BULLETS; i++)
			{
				if (bullets[i].onScreen == 1)
				{
					BulletPattern(bullets, distance, i, enemy2X, enemy2Y);

					if (bullets[i].bigBullet == 0)
						DrawSurface(screen, bullet, bullets[i].bulletX, bullets[i].bulletY);
					if (bullets[i].bigBullet == 3)
						DrawSurface(screen, playersBullet, bullets[i].bulletX, bullets[i].bulletY);
					if (bullets[i].bigBullet == 1)
						DrawSurface(screen, fireball, bullets[i].bulletX, bullets[i].bulletY);
				}
			}

			if (stage == 1)
			{
				HPtoDisplay = enemy1HP;
				DrawSurface(screen, enemy, enemyX, enemyY);
			}
			else if (stage == 2)
			{
				HPtoDisplay = enemy2HP;
				enemy2X += MOVE_VALUE / 4;
				DrawSurface(screen, enemy2, enemy2X, enemy2Y);
			}
			else if (stage == 3)
			{
				HPtoDisplay = enemy3HP;
				DrawSurface(screen, enemy3, enemyX, enemyY);
			}
			else if (stage == 4)
			{
				HPtoDisplay = enemy4HP;
				DrawSurface(screen, enemy4, enemyX, enemyY);

				if (poisonOn == 1)
				{
					CollisionWithPoison(&poisonX, &poisonY, &poisonOn, &playerHP);
					DrawSurface(screen, aura, poisonX, poisonY);
				}
				else if (poisonOn == 2)
				{
					DrawSurface(screen, warning, poisonX, poisonY);
				}
			}

			DrawSurface(screen, character, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

			fpsTimer += delta;
			if (fpsTimer > 0.5) {
				fps = frames * 2;
				frames = 0;
				fpsTimer -= 0.5;
			};

			MoveBullet(&backgroundX, &backgroundY, bullets);

			MoveCamera(&backgroundX, &backgroundY, &enemyX, &enemyY, &enemy2X, &enemy2Y, &poisonX, &poisonY);

			Collision(bullets, enemy2X, enemy2Y, stage, enemyX, enemyY, &hitCooldown, &enemy1HP, &enemy2HP, &enemy3HP, &enemy4HP, &playerHP);

			DrawRectangle(screen, SCREEN_WIDTH - 250, SCREEN_HEIGHT - 50, PLAYER_HP * 10, 20, czarny, czarny);

			DrawRectangle(screen, SCREEN_WIDTH - 250, SCREEN_HEIGHT - 50, playerHP * 10, 20, czarny, czerwony);

			// tekst informacyjny / info text
			DrawRectangle(screen, 4, 4, SCREEN_WIDTH - 8, 36, czerwony, niebieski);
			//            "template for the second project, elapsed time = %.1lf s  %.0lf frames / s"
			sprintf(text, "Bullet Hell by Adam Gasiorowski   Czas trwania = %.1lf s  %.0lf klatek / s   Stage: %i   Player HP: %i   Enemy HP: %i", worldTime, fps, stage, playerHP, HPtoDisplay);
			DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 10, text, charset);
			//	      "Esc - exit, \030 - faster, \031 - slower"
			sprintf(text, "Esc - escape, \030 \031 \032 \033 - movment,  n - new game");
			DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 26, text, charset);
		}
		else if (playerHP == 0)
		{
			DrawSurface(screen, menu, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
		}

		SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
//		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, scrtex, NULL, NULL);
		SDL_RenderPresent(renderer);

		// obs³uga zdarzeñ (o ile jakieœ zasz³y) / handling of events (if there were any)
		while (SDL_PollEvent(&event)) {
			switch(event.type) {
				case SDL_KEYDOWN:
					if (event.key.keysym.sym == SDLK_ESCAPE) quit = 1;	
					else if(event.key.keysym.sym == SDLK_n)
					{
						for (int i = 0; i < NUM_OF_BULLETS; i++)
						{
							bullets[i].pattern = 0;
							bullets[i].onScreen = 0;
							bullets[i].bulletX = LEVEL_WIDTH / 4;
							bullets[i].bulletY = LEVEL_HEIGHT / 2;
							bullets[i].bigBullet = 0;
							bullets[i].speed = 1;
						}
						frames = 0;
						fpsTimer = 0;
						fps = 0;
						quit = 0;
						worldTime = 0;
						distance = 0;
						backgroundY = 0;
						backgroundX = LEVEL_WIDTH / 4;
						enemyX = SCREEN_WIDTH / 2;
						enemyY = SCREEN_HEIGHT - ENEMY_HEIGHT / 2;
						enemy2X = 0;
						enemy2Y = 0;
						pomBullet = 0;
						time2 = 0;
						currentPattern = 0;
						stage = 1;
						distance = 0;
						poisonX = LEVEL_WIDTH / 4;
						poisonY = 0;
						poisonOn = 0;
						cooldown = FPS * COOLDOWN;
						hitCooldown = FPS * HIT_COOLDOWN;
						enemy1HP = E1_HP;
						enemy2HP = E2_HP;
						enemy3HP = E3_HP;
						enemy4HP = E4_HP;
						playerHP = PLAYER_HP;
						HPtoDisplay = 0;
						exit = 0;
					}
					else if (event.key.keysym.sym == SDLK_1)
					{
						stage = 1;
						playerHP = PLAYER_HP;
					}
					else if (event.key.keysym.sym == SDLK_2)
					{
						stage = 2;
						playerHP = PLAYER_HP;
					}
					else if (event.key.keysym.sym == SDLK_3)
					{
						stage = 3;
						playerHP = PLAYER_HP;
					}
					else if (event.key.keysym.sym == SDLK_4)
					{
						stage = 4;
						playerHP = PLAYER_HP;
					}

			};
		};
		
		frames++;
		time2++;

		frameTime = SDL_GetTicks() - t2;

		if (SCREEN_TICKS_PER_FRAME > frameTime)
		{
			SDL_Delay(SCREEN_TICKS_PER_FRAME - frameTime);
		}
	};

	// zwolnienie powierzchni / freeing all surfaces
	SDL_FreeSurface(charset);
	SDL_FreeSurface(screen);
	SDL_DestroyTexture(scrtex);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();
	return 0;
};
