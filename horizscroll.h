typedef struct {
	byte x; 	// hero x coordinate
	byte y; 	// hero y coordinate
  	byte dir; 	// hero direction of movement
  	int collided:1; // hero collided
        word bit1; 	// hero score    1-9
  	word bit2; 	// hero score   10-99
  	word bit3; 	// hero score  100-999
  	word bit4; 	// hero score 1000-9990

} Hero;

typedef struct {
	byte x; 	// heart x coordinate
	byte y;		// heart y coordinate

} Heart;


//prototypes
void play(void);
void init_game(void);
void game_over(void);
void start_game(void);
void main_scroll(void);
void clrscrn(void);
void add_point(Hero*);