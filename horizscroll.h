typedef struct {
	byte x; // low byte is sub-pixel
	byte y;

  	byte dir;
  	int collided:1;
        word bit1;
  	word bit2;
  	word bit3;
  	word bit4;
	
  	 char asset1;
  	 char asset2;
  	 char asset3;
  	 char asset4;
} Hero;

typedef struct {
	byte x; // low byte is sub-pixel
	byte y;

} Heart;


void play(void);
void init_game(void);
void game_over(void);
void start_game(void);
void main_scroll(void);
void clrscrn(void);
void add_point(Hero*);