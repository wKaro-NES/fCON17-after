/*
 "fCON17 photo", TV CRT postcard coded by Wojciech "wKaro" Karolik 
  More info about
    event: Famicon 17, https://contrabanda.eu/o-zlocie-famicon/
    tools: https://8bitworkshop.com/ and Steven Hugg's book
*/

/*
Bank-switching using the MMC3 mapper.
We use a special linker config that sets up
64 KB of PRG ROM and 64 KB of CHR ROM.
Macros are used to set MMC3 registers and switch banks.
CC65 #pragma directives are used to put things in various
PRG ROM segments (CODE0-CODE6, CODE).
*/

/*
FIXME WKARO:
\repo\IDE\8bitworkshop-master\src\worker\lib\nes\
nesbanked.cfg vs neslib2.cfg ?!

SYMBOLS {
    __STACKSIZE__: type = weak, value = $0300; # 3 pages stack
}
MEMORY {
    ZP:     file = "", start = $0002, size = $00FE, type = rw, define = yes;

    # INES Cartridge Header
    HEADER: file = %O, start = $0000, size = $0010, fill = yes;

    # 3 16K ROM Banks @ $8000
    PRG0: start = $8000, size = $2000, file = %O ,fill = yes, define = yes;
    PRG1: start = $8000, size = $2000, file = %O ,fill = yes, define = yes;
    PRG2: start = $8000, size = $2000, file = %O ,fill = yes, define = yes;

    # 3 16K ROM Banks @ $a000
    PRG3: start = $a000, size = $2000, file = %O ,fill = yes, define = yes;
    PRG4: start = $a000, size = $2000, file = %O ,fill = yes, define = yes;
    PRG5: start = $a000, size = $2000, file = %O ,fill = yes, define = yes;

    # fixed 16K ROM banks @ $c000
    PRG6: start = $c000, size = $2000, file = %O ,fill = yes, define = yes;
    
    # final bank has 
    # - startup
    # - code
    # - vectors
    PRG7:    file = %O, start = $E000, size = $1FFA, fill = yes, define = yes;
    VECTORS: file = %O, start = $FFFA, size = $0006, fill = yes;

    # 8 8k CHR Banks (64k)
    CHR:   file = %O, start = $0000, size = $10000, fill = yes;

    # standard 2k SRAM (-zeropage)
    # $0100-$0200 cpu stack
    # $0200-$0500 3 pages for ppu memory write buffer
    # $0500-$0800 3 pages for cc65 parameter stack
    RAM:   file = "", start = $0500, size = __STACKSIZE__, define = yes;

    # additional 8K SRAM Bank
    # - data (run)
    # - bss
    # - heap
    SRAM:    file = "", start = $6000, size = $2000, define = yes;
}
SEGMENTS {
    ZEROPAGE: load = ZP,              type = zp;
    HEADER:   load = HEADER,          type = ro;
    CODE0:    load = PRG0,            type = ro,  define   = yes, optional = yes;
    CODE1:    load = PRG1,            type = ro,  define   = yes, optional = yes;
    CODE2:    load = PRG2,            type = ro,  define   = yes, optional = yes;
    CODE3:    load = PRG3,            type = ro,  define   = yes, optional = yes;
    CODE4:    load = PRG4,            type = ro,  define   = yes, optional = yes;
    CODE5:    load = PRG5,            type = ro,  define   = yes, optional = yes;
    CODE6:    load = PRG6,            type = ro,  define   = yes, optional = yes;
    RODATA:   load = PRG6,            type = ro,  define   = yes;
    ONCE:     load = PRG6,            type = ro,  optional = yes;
    DATA:     load = PRG6, run = RAM, type = rw,  define   = yes;
    STARTUP:  load = PRG6,            type = ro,  define   = yes;
    CODE:     load = PRG7,            type = ro,  define   = yes;
    VECTORS:  load = VECTORS,         type = ro;
    CHARS:    load = CHR,             type = ro;
    BSS:      load = RAM,            type = bss, define   = yes;
    XRAM:     load = SRAM, run = SRAM, type = rw,  define   = yes, optional = yes;
    
}
FEATURES {
    CONDES: type    = constructor,
            label   = __CONSTRUCTOR_TABLE__,
            count   = __CONSTRUCTOR_COUNT__,
            segment = ONCE;
    CONDES: type    = destructor,
            label   = __DESTRUCTOR_TABLE__,
            count   = __DESTRUCTOR_COUNT__,
            segment = RODATA;
    CONDES: type    = interruptor,
            label   = __INTERRUPTOR_TABLE__,
            count   = __INTERRUPTOR_COUNT__,
            segment = RODATA,
            import  = __CALLIRQ__;
}


*/

 
/* 
FIXME WKARO: This must be in separate file (see error below regarding RAM/BSS and "file = %0" mentioned in nesbanked.cfg:

    # 8 8k CHR Banks (64k)
    CHR:   file = %O, start = $0000, size = $10000, fill = yes;

ERROR:
  this.program: Warning: nesbanked.cfg:40: Segment 'BSS' overflows memory area 'RAM' by 3333 bytes
  this.program: Error: Cannot generate most of the files due to memory area overflow

#pragma rodata-name("CHARS")
const byte digit0[] = {
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // empty
    0x3C,0x7E,0x7E,0x7E,0x7E,0x7E,0x7E,0x3C, 0x38,0x6C,0x6C,0x6C,0x6C,0x6C,0x38,0x00, // 0
  };
const byte digit0_pad[4096-32];

const byte digit1[] = {
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // empty
    0x1C,0x3C,0x3C,0x1C,0x1C,0x1C,0x1C,0x1C, 0x18,0x38,0x18,0x18,0x18,0x18,0x18,0x00, // 1
  };
*/


// link the pattern table into CHR ROM
//#link "fCON17-photo-chr.s"


// bank-switching configuration
#define NES_MAPPER 4		// Mapper 4 (MMC3)
#define NES_PRG_BANKS 4		// # of 16KB PRG banks
#define NES_CHR_BANKS 8		// # of 8KB CHR banks

#include <stdlib.h>
#include <peekpoke.h>
#include "neslib.h"

// VRAM update buffer
#include "vrambuf.h"
//#link "vrambuf.c"

// simple sound
#include "apu.h"
//#link "apu.c"

#define MMC_MODE 0x00

#define MMC3_SET_REG(r,n)\
	POKE(0x8000, MMC_MODE|(r));\
	POKE(0x8001, (n));

#define MMC3_CHR_0000(n) MMC3_SET_REG(0,n)
#define MMC3_CHR_0800(n) MMC3_SET_REG(1,n)
#define MMC3_CHR_1000(n) MMC3_SET_REG(2,n)
#define MMC3_CHR_1400(n) MMC3_SET_REG(3,n)
#define MMC3_CHR_1800(n) MMC3_SET_REG(4,n)
#define MMC3_CHR_1C00(n) MMC3_SET_REG(5,n)
#define MMC3_PRG_8000(n) MMC3_SET_REG(6,n)
#define MMC3_PRG_A000(n) MMC3_SET_REG(7,n)

#define MMC3_MIRROR(n) POKE(0xa000, (n))


// "strobe" means "write any value"
#define STROBE(addr) __asm__ ("sta %w", addr)

#define MMC3_IRQ_SET_VALUE(n) POKE(0xc000, (n));
#define MMC3_IRQ_RELOAD()     STROBE(0xc001)
#define MMC3_IRQ_DISABLE()    STROBE(0xe000)
#define MMC3_IRQ_ENABLE()     STROBE(0xe001)


/*
Banks switching examples (for reference only):

  #pragma rodata-name("CODE0")
  const unsigned char TEXT0[]={"Bank 0 @ 8000"};
  #pragma rodata-name("CODE1")
  const unsigned char TEXT1[]={"Bank 1 @ 8000"};
  #pragma rodata-name("CODE5")
  const unsigned char TEXT5[]={"Bank 5 @ A000"};
  #pragma rodata-name("CODE6")
  const unsigned char TEXT6[]={"Bank 6 @ C000"};

  // put functions in bank 1
  #pragma code-name("CODE1")

  void draw_text(word addr, const char* text) {
    vram_adr(addr);
    vram_write(text, strlen(text));
  }
    
    ...
    // select bank 0 in $8000-$9fff
    MMC3_PRG_8000(0);
    vram_adr(NTADR_A(2,2));
    vram_write(TEXT0, 13);
    // select bank 1 in $8000-$9fff
    // also needed to call draw_text()
    MMC3_PRG_8000(1);
    draw_text(NTADR_A(2,3), TEXT1);
    // select bank 5 in $a000-$bfff
    MMC3_PRG_A000(5);
    draw_text(NTADR_A(2,4), TEXT5);
    // $c000-$dfff is fixed to bank 6
    draw_text(NTADR_A(2,5), TEXT6);  
    ...
*/

// back to main code segment
#pragma code-name("CODE")

// ---------------------------------------------------------------------
// 8x8 metasprite Cheep-cheep
 
const byte metasprite[]={
        0x10,0x00,0x01,0x00, 		//x,y,tile,attributes
        0x18,0x00,0x02,0x00, 
        0x20,0x00,0x03,0x00, 
        0x28,0x00,0x04,0x00, 

        0x08,0x08,0x05,0x00, 
        0x10,0x08,0x29,0x02,
        0x18,0x08,0x2A,0x02,
        0x20,0x08,0x2B,0x02,
        0x28,0x08,0x06,0x00, 
        0x30,0x08,0x07,0x00, 
        0x38,0x08,0x08,0x00, 

        0x08,0x10,0x09,0x00, 
        0x10,0x10,0x0E,0x01,
        0x18,0x10,0x0F,0x01,
        0x20,0x10,0x0A,0x00, 
        0x28,0x10,0x0B,0x00, 
        0x30,0x10,0x0C,0x00, 
        0x38,0x10,0x0D,0x00, 
 
        0x00,0x18,0x10,0x00, 
        0x08,0x18,0x11,0x00, 
        0x10,0x18,0x16,0x01,
        0x18,0x18,0x17,0x01,
        0x20,0x18,0x12,0x00, 
        0x28,0x18,0x13,0x00, 
        0x30,0x18,0x14,0x00, 
        0x38,0x18,0x15,0x00, 

        0x00,0x20,0x2C,0x02,
        0x08,0x20,0x2D,0x02,
        0x10,0x20,0x18,0x00, 
        0x18,0x20,0x1B,0x01,
        0x20,0x20,0x1C,0x01,
        0x28,0x20,0x19,0x00,
        0x30,0x20,0x2E,0x03,
        0x38,0x20,0x1A,0x00,
  
        0x00,0x28,0x1D,0x00, 
        0x08,0x28,0x2F,0x02,
        0x10,0x28,0x1E,0x00, 
        0x18,0x28,0x1F,0x00, 
        0x20,0x28,0x30,0x03,
        0x28,0x28,0x31,0x03,
        0x30,0x28,0x32,0x03,
        0x38,0x28,0x20,0x00, 

        0x08,0x30,0x21,0x00, 
        0x10,0x30,0x22,0x00, 
        0x18,0x30,0x23,0x00, 
        0x20,0x30,0x24,0x00, 
        0x28,0x30,0x33,0x03,
        0x30,0x30,0x34,0x03,
        0x38,0x30,0x25,0x00, 

        0x18,0x38,0x26,0x00, 
        0x20,0x38,0x27,0x00, 
        0x28,0x38,0x28,0x00, 
    
        0x80};				//mark end of oam definition

// ----------------------------------------
// color effects

#define COLOR_SCREEN 0x0D

/*{pal:"nes",layout:"nes"}*/
const byte palette[32] = { 
      0x0D,				// screen color == COLOR_SCREEN but overweritten by Asset Editor
      0x10,0x30,0x00,0x00,		// background palette 0
      0x27,0x28,0x38,0x00,		// background palette 1
      0x1C,0x2C,0x3C,0x00,		// background palette 2
      0x14,0x24,0x34,0x00,		// background palette 3

      0x30,0x20,0x0F,0x00,		// sprite palette 0
      0x26,0x16,0x0F,0x00,		// sprite palette 1
      0x27,0x38,0x0F,0x00,		// sprite palette 2
      0x35,0x38,0x0F			// sprite palette 3
};

#define VIRTUAL_BRIGHT_STAGES 10

const byte virtual_bright[VIRTUAL_BRIGHT_STAGES] = { 4,5,6,7,8,8,7,6,5,4 };
byte bright_stage = 0;

bool update_bright() {
  pal_bg_bright(virtual_bright[bright_stage++]);
  if (bright_stage == VIRTUAL_BRIGHT_STAGES) {
    bright_stage = 0;
    pal_col(0, COLOR_SCREEN);
  }
  else if (bright_stage == VIRTUAL_BRIGHT_STAGES / 2) {
   // turn on PPU sprites and background
    ppu_on_all();
  }
  
  return !bright_stage;
}

// ----------------------------------------
byte attrs[8];

void update_attributes(sbyte col) {
  sbyte i;

  for (i = 0; i < 8; i++) {
    if (i >= col && i <= col + 3)
      attrs[i] = 0x55; 		//0x55 background palette #1 or 0xAA = #2, 0xFF = #3
    else
      attrs[i] = 0x00;
  }

  vrambuf_put(NAMETABLE_A + 0x3C0, attrs, 8);
  vrambuf_put(NAMETABLE_A + 0x3C0 + 5 * 8, attrs, 8);
  vrambuf_put(NAMETABLE_A + 0x3C0 + 6 * 8, attrs, 8);
  vrambuf_put(NAMETABLE_A + 0x3C0 + 7 * 8, attrs, 8);
  vrambuf_flush();
}

// ----------------------------------------
// IRQ handler

#define bank_offest 16
byte bank_start = 8;
byte bank = 0;

void __fastcall__ irq_nmi_callback(void) {
  // check high bit of A to see if this is an IRQ
  if (__A__ & 0x80) {
    // it's an IRQ from the MMC3 mapper
    // advance to next bank

    // experimentally adjusted timings
    __asm__ ("nop");
    __asm__ ("nop");
    __asm__ ("nop");
    __asm__ ("nop");
    __asm__ ("nop");
    __asm__ ("nop");
    __asm__ ("nop");
    __asm__ ("nop");
    __asm__ ("nop");
    __asm__ ("nop");
    __asm__ ("nop");
    __asm__ ("nop");
    __asm__ ("nop");
    __asm__ ("nop");
    __asm__ ("nop");
    __asm__ ("nop");
    __asm__ ("nop");
    __asm__ ("nop");
    __asm__ ("nop");
    __asm__ ("nop");
    
    bank += 4;			//bit 0 is ignored
    MMC3_CHR_0000(bank);	
    MMC3_CHR_0800(bank+2);	//bit 0 is ignored

    // acknowledge interrupt
    MMC3_IRQ_DISABLE();
    MMC3_IRQ_ENABLE();
  } else {
    // this is a NMI
    // reload IRQ counter
    MMC3_IRQ_RELOAD();
    
    // reset bank number
    bank=bank_start;
    MMC3_CHR_0000(bank);
    MMC3_CHR_0800(bank+2);
  }
}

// ----------------------------------------
// init & main loop
 
void main(void)
{
  bool enable_bright = TRUE;
  
  word frame;				// frame counter
  
  byte sprite_x = 10 , sprite_y = 160;	// sprite x/y positions
  sbyte sprite_dx = 1, sprite_dy = -2;	// sprite x/y deltas per frame (signed)
  byte oam_id;				// sprite ID

  sbyte strip_col = 5;			// yellow column position
  
  word i;
  
  // setup CHR bank switching for background
  MMC3_CHR_0000(bank_start);
  MMC3_CHR_0800(bank_start+2);

  // fill nametable
  vram_adr(NTADR_A(0,0));
  for (i = 0; i < 960; i++) {
      vram_put(i & 0xFF);
  }
  
  //setup IRQ
  // More accurate NES emulators simulate the mapper's
  // monitoring of the A12 line, so the background and
  // sprite pattern tables must be different.
  // https://forums.nesdev.com/viewtopic.php?f=2&t=19686#p257380
    set_ppu_ctrl_var(get_ppu_ctrl_var() | 0x08);
    // Enable Work RAM
    POKE(0xA001, 0x80);
    // Mirroring - horizontal
    POKE(0xA000, 0x01);
  
  // set up MMC3 IRQs every 64 scanlines
  MMC3_IRQ_SET_VALUE(63);
  MMC3_IRQ_RELOAD();
  MMC3_IRQ_ENABLE();
  
  // enable CPU IRQ
  __asm__ ("cli");
  
  // set IRQ callback
  nmi_set_callback(irq_nmi_callback);
  
  // clear sprites
  oam_clear();

  // set palette colors
  pal_all(palette);
  
  // clear vram buffer
  vrambuf_clear();
  // set NMI handler
  set_vram_update(updbuf);

  // turn on PPU sprites/interrupts
  ppu_on_spr();

  // sound init
  apu_init();
  
  while(1) {
    frame++;

    // swap screen ?
    if (pad_trigger(0) & PAD_START) {
       bank_start ^= bank_offest;
    }

    // fade in/out
    if (enable_bright) {
      if ((frame & 0x07) == 0)
      	enable_bright = !update_bright();
    } else {
      // animate yellow strip (emulate TV color artifacts)
      strip_col = frame & 0x4F;
      if (strip_col > -8 || strip_col < 8) {
        update_attributes(8 - strip_col);

        //switch (pseudo randomly) between alternate images (emulate TV image artifacts)
        if (strip_col == 6 || strip_col == 4)
          if ((sprite_x ^ sprite_y) & 0x10) {
             bank_start ^= bank_offest;

             //sound (emulate TV signal noise)
             APU_ENABLE(8);
             APU_NOISE_DECAY(2, 20, 1);
          }
      }
    }
    
    // draw and move metasprite
    oam_id = 0;
    oam_id = oam_meta_spr(sprite_x, sprite_y, oam_id, metasprite);

    //check boundaries
    if (sprite_x < 5 || sprite_x > 50) {
      sprite_dx = -sprite_dx;
    }
    
    if (sprite_y < 140 || sprite_y > 170) {
      sprite_dy = -sprite_dy;
    }

    sprite_x += sprite_dx;
    sprite_y += sprite_dy;
    
    if (oam_id!=0) 
      oam_hide_rest(oam_id);
    
    ppu_wait_frame();
  }
    
}
