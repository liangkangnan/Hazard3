/*
    Defines the following symbols for use by code:
    _stack_size
    _etext
    __preinit_array_start
    __preinit_array_end
    __init_array_start
    __init_array_end
    __fini_array_start
    __fini_array_end
    _data_vma
    _data_lma
    __global_pointer$
    _edata
    _sbss
    _ebss
    _end
    end
    _heap_end
    _stack_bottom
    _stack_top
*/

ENTRY(_entry_point)

__stack_size = 2048;

PROVIDE(_stack_size = __stack_size);

MEMORY
{
	FLASH (rx): ORIGIN = 0x10000000, LENGTH = 128K
	RAM (xrw) : ORIGIN = 0x20000000, LENGTH = 32K
}

SECTIONS
{
    .vector         :
    {
        *(.vectors);
        . = ALIGN(64);
    } >FLASH AT>FLASH

    .reset          :
    {
		. = ALIGN(4);
        *(.reset);
		. = ALIGN(4);
    } >FLASH AT>FLASH

	.text           :
	{
		. = ALIGN(4);
		*(.text)
		*(.text.*)
		*(.rodata)
		*(.rodata*)
		*(.gnu.linkonce.t.*)
		. = ALIGN(4);
	} >FLASH AT>FLASH 

	.fini :
	{
		KEEP(*(SORT_NONE(.fini)))
		. = ALIGN(4);
	} >FLASH AT>FLASH

	PROVIDE( _etext = . );

	.preinit_array  :
	{
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP (*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);
	} >FLASH AT>FLASH 

	.init_array     :
	{
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP (*(SORT_BY_INIT_PRIORITY(.init_array.*) SORT_BY_INIT_PRIORITY(.ctors.*)))
        KEEP (*(.init_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .ctors))
        PROVIDE_HIDDEN (__init_array_end = .);
	} >FLASH AT>FLASH 

	.fini_array     :
	{
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP (*(SORT_BY_INIT_PRIORITY(.fini_array.*) SORT_BY_INIT_PRIORITY(.dtors.*)))
        KEEP (*(.fini_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .dtors))
        PROVIDE_HIDDEN (__fini_array_end = .);
	} >FLASH AT>FLASH 

	.ctors          :
	{
        /* gcc uses crtbegin.o to find the start of
           the constructors, so we make sure it is
	       first.  Because this is a wildcard, it
	       doesn't matter if the user does not
	       actually link against crtbegin.o; the
	       linker won't look for a file to match a
	       wildcard.  The wildcard also means that it
	       doesn't matter which directory crtbegin.o
	       is in. */
        KEEP (*crtbegin.o(.ctors))
        KEEP (*crtbegin?.o(.ctors))
        /* We don't want to include the .ctor section from
	       the crtend.o file until after the sorted ctors.
	       The .ctor section from the crtend file contains the
	       end of ctors marker and it must be last */
        KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .ctors))
        KEEP (*(SORT(.ctors.*)))
        KEEP (*(.ctors))
	} >FLASH AT>FLASH 

	.dtors          :
	{
        KEEP (*crtbegin.o(.dtors))
        KEEP (*crtbegin?.o(.dtors))
        KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .dtors))
        KEEP (*(SORT(.dtors.*)))
        KEEP (*(.dtors))
	} >FLASH AT>FLASH 

	.dalign         :
	{
		. = ALIGN(4);
		PROVIDE(_data_vma = .);
	} >RAM AT>FLASH	

	.dlalign        :
	{
		. = ALIGN(4); 
		PROVIDE(_data_lma = .);
	} >FLASH AT>FLASH

	.data           :
	{
        . = ALIGN(4);
        *(.time_critical*)
    	*(.gnu.linkonce.r.*)
    	*(.data .data.*)
    	*(.gnu.linkonce.d.*)
		. = ALIGN(8);
    	PROVIDE( __global_pointer$ = . + 0x800 );
    	*(.sdata .sdata.*)
		*(.sdata2.*)
    	*(.gnu.linkonce.s.*)
    	. = ALIGN(8);
    	*(.srodata.cst16)
    	*(.srodata.cst8)
    	*(.srodata.cst4)
    	*(.srodata.cst2)
    	*(.srodata .srodata.*)
    	. = ALIGN(4);
    	*(.qspi.init.inram)
    	. = ALIGN(4);
		PROVIDE( _edata = .);
	} >RAM AT>FLASH

	.bss            :
	{
		. = ALIGN(4);
		PROVIDE( _sbss = .);
  	    *(.sbss*)
        *(.gnu.linkonce.sb.*)
		*(.bss*)
     	*(.gnu.linkonce.b.*)		
		*(COMMON*)
		. = ALIGN(4);
		PROVIDE( _ebss = .);
	} >RAM AT>FLASH

	PROVIDE( _end = _ebss);
	PROVIDE( end = . );

    .stack ORIGIN(RAM) + LENGTH(RAM) - __stack_size :
    {
        PROVIDE( _heap_end = . );    
        . = ALIGN(4);
        PROVIDE(_stack_bottom = . );
        . = . + __stack_size;
        PROVIDE( _stack_top = .);
    } >RAM
}
