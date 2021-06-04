#include <system.h>



	
// switch to 320x200x256 graphics mode

/*
bool init_mouse()
{
	
	regs.ax = 0x0000;

	int32(0x33, &regs);

	return regs.ax;

	
}
*/


// testing
//bool mouse_pressed = FALSE;


// forward decl, only in this file
void mouse_write(byte a_write);
byte mouse_read();
void mouse_wait(byte type);
// irq handler
void mouse_handler(struct regs * r);

void init_mouse()
{
	mouse_wait(1);
	outportb(0x64,0xA8);
	
	// compaq
	mouse_wait(1);
	outportb(0x64,0x20);
	
	byte status_byte;

	mouse_wait(0);

	status_byte = (inportb(0x60) | 2);	
	
	
	mouse_wait(1);
	outportb(0x64, 0x60);
	// and then we need to send the modified status byte to port 0x60 :

	mouse_wait(1);
	outportb(0x60, status_byte);
	// We are almost at the end of the initialization. Next comes setting up the mouse, and, finally, enabling it. For the settings part, we will just tell the mouse to use default settings. The command that does this is 0xF6. But, to keep all this stuff complicated, we must first tell the mouse that we are sending a command. We won't care about this for now, so we will just use a function that we will write later. So we will tell the mouse to use default settings :

	mouse_write(0xF6);
	mouse_read();
	// Note the mouse_read part; you must wait for the mouse to accept your command. This again is a function that we will define later. Now, the last part is to enable the mouse, which is done by sending the mouse the 0xF4 command :

	mouse_write(0xF4);
	mouse_read();
	
	irq_install_handler(12, mouse_handler);	
}


void mouse_handler(struct regs * r)
{
  static byte cycle = 0;
  static char mouse_bytes[3];
  mouse_bytes[cycle++] = inportb(0x60);

//line(0, 0, 100, 0, 15);

  if (cycle == 3) { // if we have all the 3 bytes...
    cycle = 0; // reset the counter
    // do what you wish with the bytes, this is just a sample
    if ((mouse_bytes[0] & 0x80) || (mouse_bytes[0] & 0x40))
      return; // the mouse only sends information about overflowing, do not care about it and return

/*	
	if (!(mouse_bytes[0] & 0x20))
      y |= 0xFFFFFF00; //delta-y is a negative value
    if (!(mouse_bytes[0] & 0x10))
      x |= 0xFFFFFF00; //delta-x is a negative value

	  if (mouse_bytes[0] & 0x4)
      puts("Middle button is pressed!n");
    if (mouse_bytes[0] & 0x2)
      puts("Right button is pressed!n");
*/  
	 if (mouse_bytes[0] & 0x1)
	  {
//      puts("Left button is pressed!n");
		  line(0, 0, 100, 0, 14);
//		  mouse_pressed = TRUE;
	  }

	  
	  // do what you want here, just replace the puts's to execute an action for each button
    // to use the coordinate data, use mouse_bytes[1] for delta-x, and mouse_bytes[2] for delta-y
  }
}

void mouse_write(byte a_write)
{
	//Wait to be able to send a command
	mouse_wait(1);
	//Tell the mouse we are sending a command
	outportb(0x64, 0xD4);
	//Wait for the final part
	mouse_wait(1);
	//Finally write
	outportb(0x60, a_write);
}

byte mouse_read()
{
	//Get response from mouse
	mouse_wait(0);
	return inportb(0x60);
}

void mouse_wait(byte type)
{
  unsigned int _time_out=100000;
  if(type==0)
  {
    while(_time_out--) //Data
    {
      if((inportb(0x64) & 1)==1)
      {
        return;
      }
    }
    return;
  }
  else
  {
    while(_time_out--) //Signal
    {
      if((inportb(0x64) & 2)==0)
      {
        return;
      }
    }
    return;
  }
}


