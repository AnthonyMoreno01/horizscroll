typedef struct {
	byte x; // low byte is sub-pixel
	byte y;
	signed int vel_x; // speed, signed, low byte is sub-pixel
	signed int vel_y;
  	byte dir;
  	int collided:1;
        word score;

} Hero;

typedef struct {
	byte x; // low byte is sub-pixel
	byte y;
	signed int vel_x; // speed, signed, low byte is sub-pixel
	signed int vel_y;
  	byte dir;
  	int collided:1;
        word score;

} Heart;


void play(void);
void test_function(void);
void game_over(void);
void start_game(void);
void scroll_demo(void);
void clrscrn(void);
void add_point(Hero*);