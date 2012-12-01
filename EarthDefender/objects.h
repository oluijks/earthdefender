// Declare Id's
enum IDS{ PLAYER, BULLET, ENEMY };

// Our player object
struct SpaceShip
{
	int ID, 
		x, 
		y, 
		lives, 
		speed, 
		boundx, 
		boundy, 
		score, 
		maxFrame, 
		curFrame, 
		frameCount, 
		frameDelay, 
		frameWidth, 
		frameHeight, 
		animationColumns, 
		animationDirection, 
		animationRow;

	ALLEGRO_BITMAP *image;
};

// Out bullet object
struct Bullet
{
	int ID, 
		x, 
		y, 
		speed;
	bool live;
};

// Our comet object
struct Comet
{
	int ID, 
		x, 
		y, 
		speed, 
		boundx, 
		boundy, 
		maxFrame, 
		curFrame, 
		frameCount, 
		frameDelay, 
		frameWidth, 
		frameHeight, 
		animationColumns, 
		animationDirection;

	bool live;

	ALLEGRO_BITMAP *image;
};

// Our explosion object
struct Explosion
{
	int x, 
		y, 
		maxFrame, 
		curFrame, 
		frameCount, 
		frameDelay, 
		frameWidth, 
		frameHeight, 
		animationColumns, 
		animationDirection;

	bool live;

	ALLEGRO_BITMAP *image;
};

struct Background
{
	float x, y, velX, velY;
	int dirX, dirY, width, height;

	ALLEGRO_BITMAP *image;
};