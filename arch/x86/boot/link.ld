ENTRY( start16 )

_boot_addr 		= 0x1000;

SEARCH_DIR(".");

SECTIONS
{
	. = _boot_addr;

    .text : 
    {
    	code = .;      
        stage2.o(.text)
    }
    
    .rodata ALIGN( 0x1000 ) :
    {      
        *( .rodata )
    }
    
    /* Initialized data */
    
    .data ALIGN( 0x1000 ) :
    {      
        *( .data )
    }
    
    /* Uninitialized data */
    
    .bss ALIGN( 4096 ) :
    {
    	/*ap_boot.o(.bss)*/
        bss = .;
        *( .bss )
    }
  	end = .;  	  	
}