#include <allegro5\allegro.h>
#include <allegro5\allegro_primitives.h>
#include <allegro5\allegro_font.h>
#include <allegro5\allegro_ttf.h>
#include <allegro5\allegro_image.h>
#include <allegro5\allegro_audio.h>
#include <allegro5\allegro_acodec.h>
#include "objects.h"

const int WIDTH = 800;
const int HEIGHT = 400;
const int NUM_BULLETS = 5;
const int NUM_COMETS = 10;
const int NUM_EXPLOSIONS = 5;
enum STATE{TITLE, PLAYING, LOST};
enum KEYS{UP, DOWN, LEFT, RIGHT, SPACE};
bool keys[5] = {false, false, false, false, false};

SpaceShip ship;
Bullet bullets[NUM_BULLETS];
Comet comets[NUM_COMETS];
Explosion explosions[NUM_EXPLOSIONS];

ALLEGRO_SAMPLE *shotSound = NULL;
ALLEGRO_SAMPLE *explosionSound = NULL;
ALLEGRO_SAMPLE *backgroundSound = NULL;
ALLEGRO_SAMPLE *loserSound = NULL;
ALLEGRO_SAMPLE_INSTANCE *backgroundInstance = NULL;

void InitShip(SpaceShip &ship, ALLEGRO_BITMAP *image);
void ResetShipAnimation(SpaceShip &ship, int position);
void DrawShip(SpaceShip &ship);
void MoveShipUp(SpaceShip &ship);
void MoveShipDown(SpaceShip &ship);
void MoveShipLeft(SpaceShip &ship);
void MoveShipRight(SpaceShip &ship);

void InitBullet(Bullet bullet[], int size);
void DrawBullet(Bullet bullet[], int size);
void FireBullet(Bullet bullet[], int size, SpaceShip &ship);
void UpdateBullet(Bullet bullet[], int size);
void CollideBullet(Bullet bullet[], int bSize, Comet comets[], int cSize, SpaceShip &ship, Explosion explosions[], int eSize);

void InitComet(Comet comets[], int size, ALLEGRO_BITMAP *image);
void DrawComet(Comet comets[], int size);
void StartComet(Comet comets[], int size);
void UpdateComet(Comet comets[], int size);
void CollideComet(Comet comets[], int cSize, SpaceShip &ship, Explosion explosions[], int eSize);

void InitExplosion(Explosion explosions[], int size, ALLEGRO_BITMAP *image);
void DrawExplosion(Explosion explosions[], int size);
void StartExplosion(Explosion explosions[], int size, int x, int y);
void UpdateExplosion(Explosion explosions[], int size);

void InitBackground(Background &backgroundScreen, float x, float y, float velX, float velY, int width, int height, int dirX, int dirY, ALLEGRO_BITMAP *image);
void UpdateBackground(Background &backgroundScreen);
void DrawBackground(Background &backgroundScreen);

void ChangeState(int &state, int newState);

int main(int argc, char **argv)
{
	bool done = false;
	bool redraw = true;
	const int FPS = 60;
	int state = -1;

	Background BG;
	Background MG;
	Background FG;

	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;
	ALLEGRO_FONT *font13 = NULL;
	ALLEGRO_FONT *font18 = NULL;
	ALLEGRO_BITMAP *shipImage;
	ALLEGRO_BITMAP *cometImage;
	ALLEGRO_BITMAP *explosionImage;
	ALLEGRO_BITMAP *titleScreen = NULL;
	ALLEGRO_BITMAP *lostScreen = NULL;
	ALLEGRO_BITMAP *backgroundImage = NULL;
	ALLEGRO_BITMAP *middlegroundImage = NULL;
	ALLEGRO_BITMAP *foregroundImage = NULL;

	if(!al_init())
		return -1;

	display = al_create_display(WIDTH, HEIGHT);

	if(!display)
		return -1;

	al_init_primitives_addon();
	al_install_keyboard();
	al_init_font_addon();
	al_init_ttf_addon();
	al_init_image_addon();
	al_install_audio();
	al_init_acodec_addon();

	al_reserve_samples(10);
	shotSound = al_load_sample("shot.ogg");
	explosionSound = al_load_sample("explosion.ogg");
	loserSound = al_load_sample("loser.ogg");
	backgroundSound = al_load_sample("backgroundSound.ogg");
	backgroundInstance = al_create_sample_instance(backgroundSound);
	al_set_sample_instance_playmode(backgroundInstance, ALLEGRO_PLAYMODE_LOOP);
	al_attach_sample_instance_to_mixer(backgroundInstance, al_get_default_mixer());

	event_queue = al_create_event_queue();
	timer = al_create_timer(1.0 / FPS);

	shipImage = al_load_bitmap("spaceship.png");
	al_convert_mask_to_alpha(shipImage, al_map_rgb(255, 0, 255));
	cometImage = al_load_bitmap("comet.png");
	explosionImage = al_load_bitmap("explosion.png");

	titleScreen = al_load_bitmap("titleScreen.png");
	lostScreen = al_load_bitmap("lostScreen.png");

	backgroundImage = al_load_bitmap("starBG.png");
	middlegroundImage = al_load_bitmap("starMG.png");
	foregroundImage = al_load_bitmap("starFG.png");

	srand(time(NULL));

	ChangeState(state, TITLE);

	InitShip(ship, shipImage);
	InitBullet(bullets, NUM_BULLETS);
	InitComet(comets, NUM_COMETS, cometImage);
	InitExplosion(explosions, NUM_EXPLOSIONS, explosionImage);
	
	InitBackground(BG, 0, 0, 1, 0, 800, 600, -1, 1, backgroundImage);
	InitBackground(MG, 0, 0, 3, 0, 1600, 600, -1, 1, middlegroundImage);
	InitBackground(FG, 0, 0, 5, 0, 800, 600, -1, 1, foregroundImage);

	font18 = al_load_font("arial.ttf", 18, 0);
	font13 = al_load_font("arial.ttf", 13, 0);

	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_display_event_source(display));

	al_start_timer(timer);
	while(!done)
	{
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);

		if(ev.type == ALLEGRO_EVENT_TIMER)
		{
			redraw = true;
			if(keys[UP])
				MoveShipUp(ship);
			else if(keys[DOWN])
				MoveShipDown(ship);
			else 
				ResetShipAnimation(ship, 1);

			if(keys[LEFT])
				MoveShipLeft(ship);
			else if(keys[RIGHT])
				MoveShipRight(ship);
			else 
				ResetShipAnimation(ship, 2);
			
			if(state == TITLE)
			{

			}
			else if(state == PLAYING)
			{
				UpdateBackground(BG);
				UpdateBackground(MG);
				UpdateBackground(FG);

				UpdateExplosion(explosions, NUM_EXPLOSIONS);
				UpdateBullet(bullets, NUM_BULLETS);
				StartComet(comets, NUM_COMETS);
				UpdateComet(comets, NUM_COMETS);
				CollideBullet(bullets, NUM_BULLETS, comets, NUM_COMETS, ship, explosions, NUM_EXPLOSIONS);
				CollideComet(comets, NUM_COMETS, ship, explosions, NUM_EXPLOSIONS);

				if(ship.lives <= 0)
					ChangeState(state, LOST);
			}
			else if(state == LOST)
			{

			}
		}
		else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
		{
			done = true;
		}
		else if(ev.type == ALLEGRO_EVENT_KEY_DOWN)
		{
			switch(ev.keyboard.keycode)
			{
			case ALLEGRO_KEY_ESCAPE:
				done = true;
				break;
			case ALLEGRO_KEY_UP:
				keys[UP] = true;
				break;
			case ALLEGRO_KEY_DOWN:
				keys[DOWN] = true;
				break;
			case ALLEGRO_KEY_LEFT:
				keys[LEFT] = true;
				break;
			case ALLEGRO_KEY_RIGHT:
				keys[RIGHT] = true;
				break;
			case ALLEGRO_KEY_SPACE:
				keys[SPACE] = true;
				if(state == TITLE)
				{
					ChangeState(state, PLAYING);
				}
				else if(state == PLAYING)
				{
					FireBullet(bullets, NUM_BULLETS, ship);
				}
				else if(state == LOST)
				{
					ChangeState(state, PLAYING);
				}
				break;
			}
		}
		else if(ev.type == ALLEGRO_EVENT_KEY_UP)
		{
			switch(ev.keyboard.keycode)
			{
			case ALLEGRO_KEY_ESCAPE:
				done = true;
				break;
			case ALLEGRO_KEY_UP:
				keys[UP] = false;
				break;
			case ALLEGRO_KEY_DOWN:
				keys[DOWN] = false;
				break;
			case ALLEGRO_KEY_LEFT:
				keys[LEFT] = false;
				break;
			case ALLEGRO_KEY_RIGHT:
				keys[RIGHT] = false;
				break;
			case ALLEGRO_KEY_SPACE:
				keys[SPACE] = false;
				break;
			}
		}

		if(redraw && al_is_event_queue_empty(event_queue))
		{
			redraw = false; 

			if(state == TITLE)
			{
				al_draw_bitmap(titleScreen, 0, 0, 0);
				al_draw_text(font13, al_map_rgb(255, 255, 255), WIDTH - 10, HEIGHT - 20, ALLEGRO_ALIGN_RIGHT, "Made by Olaf A.A. Luijks");
			}
			else if(state == PLAYING)
			{
				DrawBackground(BG);
				DrawBackground(MG);
				DrawBackground(FG);

				DrawShip(ship);
				DrawBullet(bullets, NUM_BULLETS);
				DrawComet(comets, NUM_COMETS);
				DrawExplosion(explosions, NUM_EXPLOSIONS);

				al_draw_textf(font18, al_map_rgb(255, 0, 255), 5, 5, 0, "Player has %i lives left. Player has destroyed %i objects", ship.lives, ship.score);

				al_draw_text(font13, al_map_rgb(255, 255, 255), WIDTH - 10, HEIGHT - 20, ALLEGRO_ALIGN_RIGHT, "Made by Olaf A.A. Luijks");
			}
			else if(state == LOST)
			{
				al_draw_bitmap(lostScreen, 0, 0, 0);
				al_draw_textf(
					font18, al_map_rgb(0, 255, 255), WIDTH - 10 , 10, ALLEGRO_ALIGN_RIGHT, "Final Score: %i", ship.score);
				al_draw_text(font13, al_map_rgb(255, 255, 255), WIDTH - 10, HEIGHT - 20, ALLEGRO_ALIGN_RIGHT, "Made by Olaf A.A. Luijks");
			}
	
			al_flip_display();
			al_clear_to_color(al_map_rgb(0,0,0));
		}
	}

	al_destroy_bitmap(backgroundImage);
	al_destroy_bitmap(middlegroundImage);
	al_destroy_bitmap(foregroundImage);
	al_destroy_bitmap(titleScreen);
	al_destroy_bitmap(lostScreen);
	al_destroy_bitmap(cometImage);
	al_destroy_bitmap(shipImage);

	al_destroy_sample(shotSound);
	al_destroy_sample(explosionSound);
	al_destroy_sample(loserSound);
	al_destroy_sample(backgroundSound);
	al_destroy_sample_instance(backgroundInstance);

	al_destroy_event_queue(event_queue);
	al_destroy_timer(timer);
	al_destroy_font(font18);
	al_destroy_display(display);

	return 0;
}

void InitShip(SpaceShip &ship, ALLEGRO_BITMAP *image = NULL)
{
	ship.x = 20;
	ship.y = HEIGHT / 2;
	ship.ID = PLAYER;
	ship.lives = 3;
	ship.speed = 6;
	ship.boundx = 10;
	ship.boundy = 12;
	ship.score = 0;

	ship.maxFrame = 3;
	ship.curFrame = 0;
	ship.frameCount = 0;
	ship.frameDelay = 50;

	ship.frameWidth = 46;
	ship.frameHeight = 41;

	ship.animationColumns = 3;
	ship.animationDirection = 1;
	ship.animationRow = 1;

	if(image != NULL)
		ship.image = image;
}

void DrawShip(SpaceShip &ship)
{
	
	//al_draw_filled_rectangle(ship.x, ship.y - 9, ship.x + 10, ship.y - 7, al_map_rgb(255, 0, 0));
	//al_draw_filled_rectangle(ship.x, ship.y + 9, ship.x + 10, ship.y + 7, al_map_rgb(255, 0, 0));

	//al_draw_filled_triangle(ship.x - 12, ship.y - 17, ship.x +12, ship.y, ship.x - 12, ship.y + 17, al_map_rgb(0, 255, 0));
	//al_draw_filled_rectangle(ship.x - 12, ship.y - 2, ship.x +15, ship.y + 2, al_map_rgb(0, 0, 255));
	

	int fx = (ship.curFrame % ship.animationColumns) * ship.frameWidth;
	int fy = ship.animationRow * ship.frameHeight;

	al_draw_bitmap_region(
		ship.image, 
		fx, 
		fy, 
		ship.frameWidth, 
		ship.frameHeight, 
		ship.x - ship.frameWidth / 2, 
		ship.y - ship.frameHeight / 2, 
		0
	);

	//al_draw_filled_rectangle(
	//	ship.x - ship.boundx, 
	//	ship.y - ship.boundy, 
	//	ship.x + ship.boundx, 
	//	ship.y + ship.boundy, 
	//	al_map_rgba(255, 0, 255, 100));
}

void MoveShipUp(SpaceShip &ship)
{
	ship.animationRow = 0;
	ship.y -= ship.speed;
	if(ship.y < 0)
		ship.y = 0;
}

void MoveShipDown(SpaceShip &ship)
{
	ship.animationRow = 2;
	ship.y += ship.speed;
	if(ship.y > HEIGHT)
		ship.y = HEIGHT;
}

void MoveShipLeft(SpaceShip &ship)
{
	ship.curFrame = 2;
	ship.x -= ship.speed;
	if(ship.x < 0)
		ship.x = 0;
}

void MoveShipRight(SpaceShip &ship)
{
	ship.curFrame = 1;
	ship.x += ship.speed;
	if(ship.x > 300)
		ship.x = 300;
}

void ResetShipAnimation(SpaceShip &ship, int position)
{
	if(position == 1)
	{
		ship.animationRow = 1;
	}
	else
	{
		ship.curFrame = 0;
	}
}

void InitBullet(Bullet bullet[], int size)
{
	for(int i = 0; i < size; i++)
	{
		bullet[i].ID = BULLET;
		bullet[i].speed = 10;
		bullet[i].live = false;
	}
}

void DrawBullet(Bullet bullet[], int size)
{
	for( int i = 0; i < size; i++)
	{
		if(bullet[i].live)
			al_draw_filled_circle(bullet[i].x, bullet[i].y, 2, al_map_rgb(0, 255, 33));
	}
}

void FireBullet(Bullet bullet[], int size, SpaceShip &ship)
{
	for( int i = 0; i < size; i++)
	{
		if(!bullet[i].live)
		{
			bullet[i].x = ship.x + 17;
			bullet[i].y = ship.y;
			bullet[i].live = true;
			al_play_sample(shotSound, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
			break;
		}
	}
}

void UpdateBullet(Bullet bullet[], int size)
{
	for(int i = 0; i < size; i++)
	{
		if(bullet[i].live)
		{
			bullet[i].x += bullet[i].speed;
			if(bullet[i].x > WIDTH)
				bullet[i].live = false;
		}
	}
}

void CollideBullet(Bullet bullet[], int bSize, Comet comets[], int cSize, SpaceShip &ship, Explosion explosions[], int eSize)
{
	for(int i = 0; i < bSize; i++)
	{
		if(bullet[i].live)
		{
			for(int j =0; j < cSize; j++)
			{
				if(comets[j].live)
				{
					if(bullet[i].x > (comets[j].x - comets[j].boundx) &&
						bullet[i].x < (comets[j].x + comets[j].boundx) &&
						bullet[i].y > (comets[j].y - comets[j].boundy) &&
						bullet[i].y < (comets[j].y + comets[j].boundy))
					{
						bullet[i].live = false;
						comets[j].live = false;

						ship.score++;

						StartExplosion(explosions, eSize, bullet[i].x, bullet[i].y);
						al_play_sample(explosionSound, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
					}
				}
			}
		}
	}
}

void InitComet(Comet comets[], int size, ALLEGRO_BITMAP *image = NULL)
{
	for(int i = 0; i < size; i++)
	{
		comets[i].ID = ENEMY;
		comets[i].live = false;
		comets[i].speed = 5;
		comets[i].boundx = 35;
		comets[i].boundy = 35;

		comets[i].maxFrame = 143;
		comets[i].curFrame = 0;
		comets[i].frameCount = 0;
		comets[i].frameDelay = 2;
		comets[i].frameWidth = 96;
		comets[i].frameHeight = 96;
		comets[i].animationColumns = 21;
		if(rand() % 2)
		{
			comets[i].animationDirection = 1;
		}
		else
		{
			comets[i].animationDirection = -1;
		}

		if(image != NULL)
			comets[i].image = image;
	}
}

void DrawComet(Comet comets[], int size)
{
	for(int i = 0; i < size; i++)
	{
		if(comets[i].live)
		{
			// al_draw_filled_circle(comets[i].x, comets[i].y, 20, al_map_rgb(255, 0, 0));

			int fx = (comets[i].curFrame % comets[i].animationColumns) * comets[i].frameWidth;
			int fy = (comets[i].curFrame / comets[i].animationColumns) * comets[i].frameHeight;

			al_draw_bitmap_region(
				comets[i].image, 
				fx, 
				fy, 
				comets[i].frameWidth, 
				comets[i].frameHeight, 
				comets[i].x - comets[i].frameWidth / 2, 
				comets[i].y - comets[i].frameHeight / 2, 
				0
			);

			//al_draw_filled_rectangle(
			//	comets[i].x - comets[i].boundx, 
			//	comets[i].y - comets[i].boundy, 
			//	comets[i].x + comets[i].boundx, 
			//	comets[i].y + comets[i].boundy, 
			//	al_map_rgba(255, 0, 255, 100)
			//);
		}
	}
}

void StartComet(Comet comets[], int size)
{
	for(int i = 0; i < size; i++)
	{
		if(!comets[i].live)
		{
			if(rand() % 500 == 0)
			{
				comets[i].live = true;
				comets[i].x = WIDTH;
				comets[i].y = 30 + rand() % (HEIGHT - 60);

				break;
			}
		}
	}
}

void UpdateComet(Comet comets[], int size)
{
	for(int i = 0; i < size; i++)
	{
		if(comets[i].live)
		{
			// comets[i].x -= comets[i].speed;
			if(++comets[i].frameCount >= comets[i].frameDelay)
			{
				comets[i].curFrame += comets[i].animationDirection;
				if(comets[i].curFrame >= comets[i].maxFrame)
				{
					comets[i].curFrame = 0;
				}
				else if(comets[i].curFrame <= 0)
				{
					comets[i].curFrame = comets[i].maxFrame - 1;
				}
				comets[i].frameCount = 0;
			}
			comets[i].x -= comets[i].speed;
		}
	}
}

void CollideComet(Comet comets[], int cSize, SpaceShip &ship, Explosion explosions[], int eSize)
{
	for(int i = 0; i < cSize; i++)
	{
		if(comets[i].live)
		{
			if(comets[i].x - comets[i].boundx < ship.x + ship.boundx &&
				comets[i].x + comets[i].boundx > ship.x - ship.boundx &&
				comets[i].y - comets[i].boundy < ship.y + ship.boundy &&
				comets[i].y + comets[i].boundy > ship.y - ship.boundy)
			{
				ship.lives--;
				comets[i].live = false;

				StartExplosion(explosions, eSize, ship.x, ship.y);
				al_play_sample(explosionSound, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
			}
			else if(comets[i].x < 0)
			{
				comets[i].live = false;
				ship.lives--;
			}
		}
	}
}

void InitExplosion(Explosion explosions[], int size, ALLEGRO_BITMAP *image = NULL)
{
	for(int i = 0; i < size; i++)
	{
		explosions[i].live = false;
		explosions[i].maxFrame = 31;
		explosions[i].curFrame = 0;
		explosions[i].frameCount = 0;
		explosions[i].frameDelay = 1;
		explosions[i].frameWidth = 128;
		explosions[i].frameHeight = 128;
		explosions[i].animationColumns = 8;
		explosions[i].animationDirection = 1;

		if(image != NULL)
			explosions[i].image = image;
	}
}

void DrawExplosion(Explosion explosions[], int size)
{
	for(int i = 0; i < size; i++)
	{
		if(explosions[i].live)
		{
			int fx = (explosions[i].curFrame % explosions[i].animationColumns) * explosions[i].frameWidth;
			int fy = (explosions[i].curFrame / explosions[i].animationColumns) * explosions[i].frameHeight;

			al_draw_bitmap_region(
				explosions[i].image, 
				fx, 
				fy, 
				explosions[i].frameWidth, 
				explosions[i].frameHeight, 
				explosions[i].x - explosions[i].frameWidth / 2, 
				explosions[i].y - explosions[i].frameHeight / 2, 
				0
			);
		}
	}
}

void StartExplosion(Explosion explosions[], int size, int x, int y)
{
	for(int i = 0; i < size; i++)
	{
		if(!explosions[i].live)
		{
			explosions[i].live = true;
			explosions[i].x = x;
			explosions[i].y = y;
			break;
		}
	}
}

void UpdateExplosion(Explosion explosions[], int size)
{
	for(int i = 0; i < size; i++)
	{
		if(explosions[i].live)
		{
			if(++explosions[i].frameCount >= explosions[i].frameDelay)
			{
				explosions[i].curFrame += explosions[i].animationDirection;
				if(explosions[i].curFrame >= explosions[i].maxFrame)
				{
					explosions[i].curFrame = 0;
					explosions[i].live = false;
				}

				explosions[i].frameCount = 0;
			}
		}
	}
}

void ChangeState(int &state, int newState)
{
	if(state == TITLE)
	{

	}
	else if(state == PLAYING)
	{
		al_stop_sample_instance(backgroundInstance);
	}
	else if(state == LOST)
	{

	}

	state = newState;

	if(state == TITLE)
	{

	}
	else if(state == PLAYING)
	{
		InitShip(ship);
		InitBullet(bullets, NUM_BULLETS);
		InitComet(comets, NUM_COMETS);
		InitExplosion(explosions, NUM_EXPLOSIONS);

		al_play_sample_instance(backgroundInstance);
	}
	else if(state == LOST)
	{
		al_play_sample(loserSound, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
	}
}

void InitBackground(Background &backgroundScreen, float x, float y, float velX, float velY, int width, int height, int dirX, int dirY, ALLEGRO_BITMAP *image)
{
	backgroundScreen.x = x;
	backgroundScreen.y = y;
	backgroundScreen.velX = velX;
	backgroundScreen.velY = velY;
	backgroundScreen.width = width;
	backgroundScreen.height = height;
	backgroundScreen.dirX = dirX;
	backgroundScreen.dirY = dirY;
	backgroundScreen.image = image;
}

void UpdateBackground(Background &backgroundScreen)
{
	backgroundScreen.x += backgroundScreen.velX * backgroundScreen.dirX;
	if(backgroundScreen.x + backgroundScreen.width <= 0)
		backgroundScreen.x = 0;
}

void DrawBackground(Background &backgroundScreen)
{
	al_draw_bitmap(backgroundScreen.image, backgroundScreen.x, backgroundScreen.y, 0);

	if(backgroundScreen.x + backgroundScreen.width < WIDTH)
		al_draw_bitmap(backgroundScreen.image, backgroundScreen.x + backgroundScreen.width, backgroundScreen.y, 0);
}
