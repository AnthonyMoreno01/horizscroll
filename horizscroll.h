typedef struct {
	byte x; // low byte is sub-pixel
	byte y;
	signed int vel_x; // speed, signed, low byte is sub-pixel
	signed int vel_y;
  	byte dir;
  	int collided:1;
        word score;
  	char x1,x2,x3,x4;
  	char y1,y2,y3,y4;
} Hero;

typedef struct {
	byte x; // low byte is sub-pixel
	byte y;
	signed int vel_x; // speed, signed, low byte is sub-pixel
	signed int vel_y;
  	byte dir;
  	int collided:1;
        word score;
  	char x1,x2,x3,x4;
  	char y1,y2,y3,y4;
} Heart;


void play(void);
void test_function(void);
void game_over(void);
void start_game(void);
void scroll_demo(void);
void clrscrn(void);
void add_point(Hero*);