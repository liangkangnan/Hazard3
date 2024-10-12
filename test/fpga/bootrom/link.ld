OUTPUT_ARCH( "riscv" )
ENTRY( _entry_point )

SECTIONS
{
  . = 0x00000000;
  .text : { *(.text) }
  . = ALIGN(4);
  .data : { *(.data) }
  .bss : { *(.bss) }
  _end = .;
}
