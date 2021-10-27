typedef struct {
	byte x; // low byte is sub-pixel
	byte y;

  	byte dir;
  	int collided:1;
        word bit1;
  word bit2;
  word bit3;
  word bit4;

} Hero;

typedef struct {
	byte x; // low byte is sub-pixel
	byte y;

} Heart;


void play(void);
void test_function(void);
void game_over(void);
void start_game(void);
void scroll_demo(void);
void clrscrn(void);
void add_point(Hero*);