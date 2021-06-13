ENTRY( start )

_boot_addr 		= 0x100000;
_ap_boot_addr 	= 0x1000;

SEARCH_DIR("./obj");

SECTIONS
{
	. = _boot_addr;

    .boot : {
        /* ensure that the multiboot header is at the beginning */
        
        multiboot_header.o(.multiboot_header)
    }
		
    .text : {
    	code = .;      
        start.o(.text)
		gdt_asm.o(.text)
		int32_beta.o(.text)
	 	cpuid.o(.text) 
	 	pic.o(.text)
	 	memasm.o(.text) 
	 	pit.o(.text) 
	 	main.o(.text) 
	 	common.o(.text) 
	 	scrn.o(.text) 
	 	gdt.o(.text) 
	 	idt.o(.text) 
	 	isrs.o(.text) 
	 	irq.o(.text) 
	 	timer.o(.text) 
	 	kb.o(.text) 
	 	vga.o(.text) 
	 	math.o(.text) 
	 	mouse.o(.text) 
	 	vbe.o(.text) 
	 	mem.o(.text) 
	 	cpudet.o(.text) 
	 	cpu.o(.text) 
	 	smp.o(.text) 
		apic.o(.text) 
	 	acpi.o(.text)
		 hpet.o(.text)
        _boot_end = .;
        . = ALIGN(4096);
        _ap_start = .;
        	main_ap.o(.text)
        _ap_end = .;
        

    	/**(.rodata)*/
    	. = ALIGN(4096);



 /*       _ap_boot_start = .;  */
        	ap_boot.o(.text)
 /*       _ap_boot_end = .;*/
        /*. = ALIGN(4096);*/
  }
 /*
    .ap_boot_text : {
        _ap_boot_start = .;  
        ap_boot.o(.ap_boot_text)
        _ap_boot_end = .;
  	}
  	*/ 
  	/*
	.ap_stack : {
		ap_boot.o(.ap_stack)
	}*/

/*			
     .ap_boot_data : {
		ap_boot.o(.ap_boot_data)
  	}  	
_ap_boot_end = .;
*/	
    /* Constant data */
    
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
       
        stack.o(.bss)
        
    }


  	end = .;
  	  	
  	  	
  	  	
}