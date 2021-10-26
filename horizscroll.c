
/*
We demonstrate horizontal scrolling of two nametables.
Vertical mirroring is set, so nametables A and B are
to the left and right of each other.

New playfield data is randomly generated and updated
offscreen using the vrambuf module.
We update the nametable in 16-pixel-wide vertical strips,
using 2x2 blocks of tiles ("metatiles").

We also use the split() function to create a status bar.
*/

#include "neslib.h"
#include <string.h>
#include <joystick.h>
#include <nes.h>
// 0 = horizontal mirroring
// 1 = vertical mirroring
#define NES_MIRRORING 1


#include "horizscroll.h"

// VRAM update buffer
#include "vrambuf.h"
//#link "vrambuf.c"

// link the pattern table into CHR ROM
//#link "chr_generic.s"

/// GLOBAL VARIABLES

word x_scroll;		// X scroll amount in pixels
byte seg_height;	// segment height in metatiles
byte seg_width;		// segment width in metatiles
byte seg_char;		// character to draw
byte seg_palette;	// attribute table value
byte seg_gap;

// number of rows in scrolling playfield (without status bar)
#define PLAYROWS 24

#define TILE 0xd8
#define TILE1 0xCC
#define ATTR 02

const unsigned char metasprite[]={
        0,      0,      TILE,     ATTR, 
        0,      8,      TILE+1,   ATTR, 
        8,      0,      TILE+2,   ATTR, 
        8,      8,      TILE+3,   ATTR, 
        128};

const unsigned char metasprite1[]={
        0,      0,      TILE,     ATTR, 
        0,      8,      TILE+1,   ATTR, 
        8,      0,      TILE+2,   ATTR, 
        8,      8,      TILE+3,   ATTR, 
        128};

Hero heros;


/*{pal:"nes",layout:"nes"}*/
const char PALETTE[32] = { 
  0x03,			// background color

  0x25,0x30,0x27,0x00,	// ladders and pickups
  0x1C,0x20,0x2C,0x00,	// floor blocks
  0x00,0x10,0x20,0x00,
  0x06,0x16,0x26,0x00,

  0x16,0x35,0x24,0x00,	// enemy sprites
  0x00,0x37,0x25,0x00,	// rescue person
  0x0D,0x2D,0x1A,0x00,
  0x0D,0x27,0x2A	// player sprites
};

// function to write a string into the name table
//   adr = start address in name table
//   str = pointer to string
void put_str(unsigned int adr, const char *str) {
  vram_adr(adr);        // set PPU read/write address
  vram_write(str, strlen(str)); // write bytes to PPU
}

void cputcxy(byte x, byte y, char ch) {
  vrambuf_put(NTADR_A(x,y), &ch, 1);
}

//function displayes text
void cputsxy(byte x, byte y, const char* str) {
  vrambuf_put(NTADR_A(x,y), str, strlen(str));
}



typedef enum { D_RIGHT, D_DOWN, D_LEFT, D_UP } dir_t;
const char DIR_X[4] = { 1, 0, -1, 0 };
const char DIR_Y[4] = { 0, 2, 0, -2 };

unsigned char pad1;
unsigned char pad1_new;
// actor x/y positions

// buffers that hold vertical slices of nametable data
char ntbuf1[PLAYROWS];	// left side
char ntbuf2[PLAYROWS];	// right side

// a vertical slice of attribute table entries
char attrbuf[PLAYROWS/4];

/// FUNCTIONS

// convert from nametable address to attribute table address
word nt2attraddr(word a) {
  return (a & 0x2c00) | 0x3c0 |
    ((a >> 4) & 0x38) | ((a >> 2) & 0x07);
}

// generate new segment
void new_segment() {
  seg_height = PLAYROWS;
  seg_width = 6;
  seg_palette = 3;
  seg_char = 0xf4;
  seg_gap = (rand8() & 3) + 2;
  
}

byte getchar(byte x, byte y) {
  // compute VRAM read address
  word addr = NTADR_A(x,y);
  // result goes into rd
  byte rd;
  // wait for VBLANK to start
  ppu_wait_nmi();
  // set vram address and read byte into rd
  vram_adr(addr);
  vram_read(&rd, 1);
  // scroll registers are corrupt
  // fix by setting vram address
  vram_adr(0x0);
  return rd;
}

// draw metatile into nametable buffers
// y is the metatile coordinate (row * 2)
// ch is the starting tile index in the pattern table
void set_metatile(byte y, byte ch) {
  ntbuf1[y*2] = ch;
  ntbuf1[y*2+1] = ch+1;
  ntbuf2[y*2] = ch+2;
  ntbuf2[y*2+1] = ch+3;
}

// set attribute table entry in attrbuf
// x and y are metatile coordinates
// pal is the index to set
void set_attr_entry(byte x, byte y, byte pal) {
  if (y&1) pal <<= 4;
  if (x&1) pal <<= 2;
  attrbuf[y/2] |= pal;
}

// fill ntbuf with tile data
// x = metatile coordinate
void fill_buffer(byte x) {
  byte i,y;

  // clear nametable buffers
  memset(ntbuf1, 0, sizeof(ntbuf1));
  memset(ntbuf2, 0, sizeof(ntbuf2));
  // draw a random star
  ntbuf1[rand8() & 15] = '.';
  // draw segment slice to both nametable buffers
  for (i=0; i<seg_height; i++) {
    if( i == seg_gap +1 ){
      if(seg_width == 5){
        
        
    	        y = PLAYROWS/2-1-i;
 
    		set_metatile(y, 0xCC);
    		set_attr_entry(x, y, seg_palette);
    	}
    }
    else if(i == seg_gap  || i == seg_gap + 2 || i == seg_gap + 3){
    	
    }else if
        (seg_width == 3 || seg_width == 2 || seg_width == 1)
  {
    }else{
    y = PLAYROWS/2-1-i;
    set_metatile(y, seg_char);
    set_attr_entry(x, y, seg_palette);
    }
  }
}

// write attribute table buffer to vram buffer
void put_attr_entries(word addr) {
  byte i;
  for (i=0; i<PLAYROWS/4; i++) {
    VRAMBUF_PUT(addr, attrbuf[i], 0);
    addr += 8;
  }
  vrambuf_end();
}

// update the nametable offscreen
// called every 8 horizontal pixels
void update_offscreen() {
  register word addr;
  byte x;

  
  // divide x_scroll by 8
  // to get nametable X position
  x = (x_scroll/8 + 32) & 63;
  // fill the ntbuf arrays with tiles
  fill_buffer(x/2);
  // get address in either nametable A or B
  if (x < 32){
    addr = NTADR_A(x, 4);
  
  }else
    addr = NTADR_B(x&31, 4);
  // draw vertical slice from ntbuf arrays to name table
  // starting with leftmost slice
  vrambuf_put(addr | VRAMBUF_VERT, ntbuf1, PLAYROWS);
  // then the rightmost slice
  vrambuf_put((addr+1) | VRAMBUF_VERT, ntbuf2, PLAYROWS);
  // compute attribute table address
  // then set attribute table entries
  // we update these twice to prevent right-side artifacts
  put_attr_entries(nt2attraddr(addr));
  // every 4 columns, clear attribute table buffer
  if ((x & 2) == 2) {
    memset(attrbuf, 0, sizeof(attrbuf));
  }
  // decrement segment width, create new segment when it hits zero

  if (--seg_width == 0) {

    new_segment();
  }
  
}

void check_for_collision(Hero* h){
byte i; 
  if (h->y == 238 || h->y == 26)
    h->collided = 1 ;
  
  if( i == 0xCC || i == 0xCD 
     || i == 0xCE|| i == 0xCF){
      h->score++;
      cputcxy(14,1,h->score+'0');
    }
}

void move_player(Hero* h){
  h->x += DIR_X[h->dir];
  h->y += DIR_Y[h->dir];

}

void movement(Hero* h){
   
   byte dir;
   pad1_new = pad_trigger(0); // read the first controller
   pad1 = pad_state(0);
   

  if (pad1 & JOY_BTN_A_MASK) dir = D_UP;
	else dir = D_DOWN;
  h->dir = dir;
  
}

void add_point(Hero* h){
  h->score++;
  cputcxy(14,1,h-> score+'0');
}

// scrolls the screen left one pixel
void scroll_left(int x, int y) {

  //oam_meta_spr(hearts.x, hearts.y, 6, metasprite1);
  
  if (x == 161){
    
  if(y == 90){
    add_point(&heros);
  }
    }
  if ((x_scroll & 15) == 0) {
    
    update_offscreen();
    
  }
  if((x_scroll & 3) == 0){
    movement(&heros);
  }
  // increment x_scroll
  ++x_scroll;
  
  check_for_collision(&heros);
  move_player(&heros);
  oam_meta_spr(heros.x, heros.y, 4, metasprite);
  
}

// main loop, scrolls left continuously
void scroll_demo() {
int x,y;
  x_scroll = 0;
  
  new_segment();
  
  
  x = 0;
  y = 0;
  // infinite loop
  while (1) {
    // ensure VRAM buffer is cleared
    
    ppu_wait_nmi();
    
    vrambuf_clear();
    
    // split at sprite zero and set X scroll
    split(x_scroll, 0); 

    // scroll to the left
    if(x <= 160){
    scroll_left(x,y);
      x++;
    }else
    if(x >= 160){
      scroll_left(x,y); 
      y++;
    }
    if(y == 91)
      y = 0;
    
  if(heros.collided == 1){
    	game_over();
    	break;
  }
    
  }
  test_function();
}

void game_over(){
  
  
  clrscrn();
}

void clrscrn(){
  vrambuf_clear();
  ppu_off();
  vram_adr(0x2000);
  vram_fill(0, 32*28);
  vram_adr(0x0);
}

void test_function(){
  // write text to name table
  vrambuf_clear();
  heros.score = 0;
  //put_str(NTADR_A(7,1), "Score: ");
   
  //put_str(NTADR_A(7,2), "Nametable A, Line 2");
  vram_adr(NTADR_A(0,3));
  vram_fill(5, 32);

  vram_adr(0x23c0);
  vram_fill(0x55, 8);
  
  // set sprite 0
  oam_clear();
  
  heros.x = 40;
  heros.y = 120;
  
  vrambuf_clear();
  oam_spr(0, 30, 0xa5, 0, 0);
  
  oam_meta_spr(heros.x, heros.y, 4, metasprite);
  // clear vram buffer
  vrambuf_clear();
  set_vram_update(updbuf);
  
  // enable PPU rendering (turn on screen)
  ppu_on_all();
  
 heros.collided =0;
  cputsxy(5,1,"Score:");
cputcxy(14,1,heros.score+'0');
  scroll_demo();

}


// main function, run after console reset
void main(void) {
pal_all(PALETTE);

  // get data for initial segment

  test_function();

    
}
