#include <system.h>

#define SCREEN_WIDTH        320       /* width in pixels of mode 0x13 */
#define SCREEN_HEIGHT       240       /* height in pixels of mode 0x13 */
#define SCREEN_SIZE	(SCREEN_WIDTH * SCREEN_HEIGHT)
#define NUM_COLORS          256       /* number of colors in mode 0x13 */


#define INPUT_STATUS        0x03da
#define VRETRACE            0x08

#define VIDEO_INT			0x10	/* bios int number */

#define sgn(x) ((x<0)?-1:((x>0)?1:0)) /* macro to return the sign of a
                                         number */

/*	  MODE X		*/

#define MISC_OUTPUT         0x03c2    /* VGA misc. output register */
#define SC_INDEX            0x03c4    /* VGA sequence controller */
#define SC_DATA             0x03c5
#define PALETTE_INDEX       0x03c8    /* VGA digital-to-analog converter */
#define PALETTE_DATA        0x03c9
#define CRTC_INDEX          0x03d4    /* VGA CRT controller */

#define MAP_MASK            0x02      /* Sequence controller registers */
#define MEMORY_MODE         0x04

#define H_TOTAL             0x00      /* CRT controller registers */
#define H_DISPLAY_END       0x01
#define H_BLANK_START       0x02
#define H_BLANK_END         0x03
#define H_RETRACE_START     0x04
#define H_RETRACE_END       0x05
#define V_TOTAL             0x06
#define OVERFLOW            0x07
#define MAX_SCAN_LINE       0x09
#define V_RETRACE_START     0x10
#define V_RETRACE_END       0x11
#define V_DISPLAY_END       0x12
#define OFFSET              0x13
#define UNDERLINE_LOCATION  0x14
#define V_BLANK_START       0x15
#define V_BLANK_END         0x16
#define MODE_CONTROL        0x17

#define HIGH_ADDRESS        0x0C      /* CRT controller registers */
#define LOW_ADDRESS         0x0D


#define ALL_PLANES          0xff02
#define CRTC_DATA           0x03d5

/* macro to write a word to a port */
#define word_out(port,register,value) outportw(port,(((word)value<<8) + register))


//#define VERTICAL_RETRACE
#define INPUT_STATUS_1      0x03da
#define DISPLAY_ENABLE      0x01      /* VGA input status bits */

#define NORMAL_MODE
// globals
byte * VGA = (byte *)0xA0000;
byte normal_buffer[64000];

// flipping
word visual_page = 0;
word active_page = SCREEN_SIZE/4;


// double buffering
byte screen[4][SCREEN_SIZE / 4];

regs32_t regs;

word screen_width, screen_height;

// forward decl, only used inside this file
void wait_for_retrace(void);
void set_screen_mode(byte mode);


void set_screen_mode(byte mode)
{
  regs.eax = mode;
	
  int32_beta(VIDEO_INT, &regs);
}

void set_unchained_mode(int width, int height)
{
#ifdef NORMAL_MODE
  	set_screen_mode(0x0013);

	return;
#endif

	
  word i;
  dword *ptr=(dword *)VGA;

  /* set mode 13 */
  set_screen_mode(0x0013);

  /* turn off chain-4 mode */
  word_out(SC_INDEX, MEMORY_MODE,0x06);

  /* set map mask to all 4 planes for screen clearing */
  word_out(SC_INDEX, MAP_MASK, 0xff);

	/* clear all 256K of memory, ME: NOT TRUE, IS 64K / 4*/
	/* this makes verticale banden in de vga buffel */
  for(i=0;i<0x4000;i++)
    *ptr++ = 0;

  /* turn off long mode */
  word_out(CRTC_INDEX, UNDERLINE_LOCATION, 0x00);

  /* turn on byte mode */
  word_out(CRTC_INDEX, MODE_CONTROL, 0xe3);


  screen_width=320;
  screen_height=200;

  if (width==360)
  {
    /* turn off write protect */
    word_out(CRTC_INDEX, V_RETRACE_END, 0x2c);

//    outp(MISC_OUTPUT, 0xe7);
    outportb(MISC_OUTPUT, 0xe7);
    word_out(CRTC_INDEX, H_TOTAL, 0x6b);
    word_out(CRTC_INDEX, H_DISPLAY_END, 0x59);
    word_out(CRTC_INDEX, H_BLANK_START, 0x5a);
    word_out(CRTC_INDEX, H_BLANK_END, 0x8e);
    word_out(CRTC_INDEX, H_RETRACE_START, 0x5e);
    word_out(CRTC_INDEX, H_RETRACE_END, 0x8a);
    word_out(CRTC_INDEX, OFFSET, 0x2d);

    /* set vertical retrace back to normal */
    word_out(CRTC_INDEX, V_RETRACE_END, 0x8e);

    screen_width=360;
  }
  else
  {
//    outp(MISC_OUTPUT, 0xe3);
	  outportb(MISC_OUTPUT, 0xe3);
  }

  if (height==240 || height==480)
  {
    /* turn off write protect */
    word_out(CRTC_INDEX, V_RETRACE_END, 0x2c);

    word_out(CRTC_INDEX, V_TOTAL, 0x0d);
    word_out(CRTC_INDEX, OVERFLOW, 0x3e);
    word_out(CRTC_INDEX, V_RETRACE_START, 0xea);
    word_out(CRTC_INDEX, V_RETRACE_END, 0xac);
    word_out(CRTC_INDEX, V_DISPLAY_END, 0xdf);
    word_out(CRTC_INDEX, V_BLANK_START, 0xe7);
    word_out(CRTC_INDEX, V_BLANK_END, 0x06);
    screen_height=height;
  }

  if (height==400 || height==480)
  {
    word_out(CRTC_INDEX, MAX_SCAN_LINE, 0x40);
    screen_height=height;
  }



}

void set_unchained_mode2(int width, int height)
{
	set_screen_mode(0x0013);

	word i;
  visual_page = 0;
  active_page = SCREEN_SIZE/4;


  dword *ptr=(dword *)VGA;            /* used for faster screen clearing */

  outportb(SC_INDEX,  MEMORY_MODE);       /* turn off chain-4 mode */
  outportb(SC_DATA,   0x06);

  outportw(SC_INDEX, ALL_PLANES);        /* set map mask to all 4 planes */

  for(i=0;i<0x4000;i++)               /* clear all 256K of memory */
    *ptr++ = 0;

  outportb(CRTC_INDEX,UNDERLINE_LOCATION);/* turn off long mode */
  outportb(CRTC_DATA, 0x00);

  outportb(CRTC_INDEX,MODE_CONTROL);      /* turn on byte mode */
  outportb(CRTC_DATA, 0xe3);
	
	
	
	// TEST STUFF, ME, REMOVE FROM HERE
  screen_width=320;
  screen_height=200;
	outportb(MISC_OUTPUT, 0xe3);

	
  if (height==240 || height==480)
  {
    /* turn off write protect */
    word_out(CRTC_INDEX, V_RETRACE_END, 0x2c);

    word_out(CRTC_INDEX, V_TOTAL, 0x0d);
    word_out(CRTC_INDEX, OVERFLOW, 0x3e);
    word_out(CRTC_INDEX, V_RETRACE_START, 0xea);
    word_out(CRTC_INDEX, V_RETRACE_END, 0xac);
    word_out(CRTC_INDEX, V_DISPLAY_END, 0xdf);
    word_out(CRTC_INDEX, V_BLANK_START, 0xe7);
    word_out(CRTC_INDEX, V_BLANK_END, 0x06);
    screen_height=height;
  }

}



void set_palette(const byte *palette)
{
  int i;

  outportb(PALETTE_INDEX,0);              /* tell the VGA that palette data
                                         is coming. */
  for(i=0;i<256*3;i++)
    outportb(PALETTE_DATA,palette[i]);    /* write the data */
}



// draw pix, no clipping
/*
void put_pixel(int x, int y, byte color)
{
	//word offset;	// 0 - 64000

	// slow
	//offset = 320*y + x;
	
	// fast
	//offset = (y<<8) + (y<<6) + x;

	
	active_vga_buffer[(y<<8) + (y<<6) + x] = color;

}
*/
/*
void put_pixel(int x, int y, byte color)
{
  dword offset;

  outportb(SC_INDEX, MAP_MASK);         
  outportb(SC_DATA,  1 << (x&3) );

  offset=((dword)y*screen_width+x) >> 2;

  active_vga_buffer[(word)offset] = color;
//  VGA[(word)offset] = color;
}
*/
void put_pixel(int x,int y,byte color)
{
#ifdef NORMAL_MODE
	normal_buffer[(y<<8) + (y<<6) + x] = color;
//	normal_buffer[y*320 + x] = color;
	return;
#endif

/* flipping */
  outportb(SC_INDEX, MAP_MASK);          /* select plane */
  outportb(SC_DATA,  1 << (x&3) );

  VGA[(y<<6)+(y<<4)+(x>>2)]=color;

//	active_vga_buffer[(y<<6)+(y<<4)+x]=color;
	

/*	dubble buffering
	int offset = (y<<6)+(y<<4)+x;
	int num = offset % 4;
	
	screen[num][(y<<6)+(y<<4)+(x>>2)]=color;
*/	
	
	
}

void line(int x1, int y1, int x2, int y2, byte color)
{
  int i,dx,dy,sdx,sdy,dxabs,dyabs,x,y,px,py;

  dx=x2-x1;      /* the horizontal distance of the line */
  dy=y2-y1;      /* the vertical distance of the line */
  dxabs=abs(dx);
  dyabs=abs(dy);
  sdx=sgn(dx);
  sdy=sgn(dy);
  x=dyabs>>1;
  y=dxabs>>1;
  px=x1;
  py=y1;

	put_pixel(px, py, color);
// active_vga_buffer[(py<<8)+(py<<6)+px]=color;

  if (dxabs>=dyabs) /* the line is more horizontal than vertical */
  {
    for(i=0;i<dxabs;i++)
    {
      y+=dyabs;
      if (y>=dxabs)
      {
        y-=dxabs;
        py+=sdy;
      }
      px+=sdx;
      put_pixel(px,py,color);
    }
  }
  else /* the line is more vertical than horizontal */
  {
    for(i=0;i<dyabs;i++)
    {
      x+=dxabs;
      if (x>=dyabs)
      {
        x-=dyabs;
        px+=sdx;
      }
      py+=sdy;
      put_pixel(px,py,color);
    }
  }
}


/**************************************************************************
 *  wait_for_retrace                                                      *
 *    Wait until the *beginning* of a vertical retrace cycle (60hz).      *
 **************************************************************************/

void wait_for_retrace(void)
{
    /* wait until done with vertical retrace */
    while  ((inportb(INPUT_STATUS) & VRETRACE)) {};
    /* wait until done refreshing */
    while (!(inportb(INPUT_STATUS) & VRETRACE)) {};
}

void swap_vga_buffer(void)
{
#ifdef NORMAL_MODE
	memcpy(VGA, normal_buffer, 64000);	
	return;
#endif

		wait_for_retrace();
    //outportb(SC_INDEX, MAP_MASK);          

	//outportb(SC_DATA,  1 << (x&3) );

	
    //outportw(SC_INDEX,ALL_PLANES);

	//memcpy(VGA,active_vga_buffer,SCREEN_SIZE); // SCREEN_SIZE
	
	


	int plane;
      for(plane=0;plane<4;plane++)
      {
        outportb(SC_DATA,  1 << ((plane)&3) );
		  
		  
		memcpy(VGA,screen[plane],SCREEN_SIZE/4); // SCREEN_SIZE
      }
	
    //outportw(SC_INDEX,ALL_PLANES);
	
}

void flip_screen()
{
  word high_address,low_address;
  word temp;

	// flip
  temp = visual_page;
  visual_page = active_page;
  active_page = temp;

  high_address = HIGH_ADDRESS | (active_page & 0xff00);
  low_address  = LOW_ADDRESS  | (active_page << 8);

  #ifdef VERTICAL_RETRACE
    while ((inportb(INPUT_STATUS_1) & DISPLAY_ENABLE));
  #endif
  outportw(CRTC_INDEX, high_address);
  outportw(CRTC_INDEX, low_address);
  #ifdef VERTICAL_RETRACE
    while (!(inportb(INPUT_STATUS_1) & VRETRACE));
  #endif

}

void set_text_mode()
{
	set_screen_mode(0x0003);
}

void test_vga()
{

	set_screen_mode(0x0013);
	
//	memset(normal_buffer, 3, 64000);
/*
	int x, y;
	for(x = 0; x < 200; x++)
	{
		VGA[x*320 + x] = 15;
	}
*/
//	normal_buffer[0] = 15;
//	memset(normal_buffer, 15, 640);
	int y = 0;
	while(y < 200)
	{
		line(0,y,319,y,5);
		swap_vga_buffer();
		memset(normal_buffer, 0, 64000);
		y++;
	}
}
