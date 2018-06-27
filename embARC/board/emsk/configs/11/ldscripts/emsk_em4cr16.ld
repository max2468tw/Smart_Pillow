MEMORY
{
	iccm :	ORIGIN = 0x0,		LENGTH = 128K
	dccm :	ORIGIN = 0x0800000,	LENGTH = 64K
}

REGION_ALIAS("REGION_INIT", iccm)
REGION_ALIAS("REGION_VECTOR", iccm)
REGION_ALIAS("REGION_TEXT", iccm)
REGION_ALIAS("REGION_RODATA", iccm)
REGION_ALIAS("REGION_DATA", dccm)
REGION_ALIAS("REGION_SDATA", dccm)
REGION_ALIAS("REGION_SBSS", dccm)
REGION_ALIAS("REGION_BSS", dccm)
REGION_ALIAS("REGION_HEAP", dccm)
REGION_ALIAS("REGION_STACK", dccm)

SECTIONS
{
	.init :
	{
		_f_init = .;
		KEEP (*(.init_vector))
		KEEP (*(.init_bootstrap))
		_e_init = .;
	} > REGION_INIT

	.text : ALIGN(4)
	{
		_f_text = .;
		*(.text .text.* .gnu.linkonce.t.*)
		_e_text = .;
	} > REGION_TEXT

	.rodata : ALIGN(4)
	{
		_f_rodata = .;
		*(.rodata .rodata.* .gnu.linkonce.r.*)
		_e_rodata = .;
	} > REGION_RODATA

	.vector : ALIGN(1024)
	{
		_f_vector = .;
		*(.vector)
		_e_vector = .;
	} > REGION_VECTOR

	. = ALIGN(4);
	.data : ALIGN(4)
	{
		_f_data = .;
		*(.data .data.* .gnu.linkonce.d.*)
	} > REGION_DATA	AT > REGION_RODATA

	. = ALIGN(4);
	.sdata : ALIGN(4)
	{
		_f_sdata = .;
		__SDATA_BEGIN__ = .;
		*(.sdata .sdata.* .gnu.linkonce.s.*)
		_e_sdata = .;
		_e_data = .;
	} > REGION_DATA	AT > REGION_RODATA

	.sbss (NOLOAD) : ALIGN(8)
	{
		PROVIDE (__sbss_start = .);
		PROVIDE (___sbss_start = .);
		_f_bss = .;
		_f_sbss = .;
		*(.dynsbss)
		*(.sbss .sbss.* .gnu.linkonce.sb.*)
		*(.scommon)
		_e_sbss = .;
		PROVIDE (__sbss_end = .);
		PROVIDE (___sbss_end = .);
	} > REGION_BSS AT > REGION_DATA

	.bss (NOLOAD) : ALIGN(8)
	{
		*(.dynbss)
		*(.bss .bss.* .gnu.linkonce.b.*)
		*(COMMON)
		_e_bss = .;
	} > REGION_BSS AT > REGION_DATA

	.stack (NOLOAD) :
	{
		. = ALIGN(4);
		_f_stack = .;
		. = . + (DEFINED(_STACKSIZE) ? _STACKSIZE : 2048) ;
		_e_stack = .;
	} > REGION_STACK AT > REGION_DATA

	.heap (NOLOAD) :
	{
		. = ALIGN(4);
		__start_heap = . ;
		_f_heap = .;
		. = . + (DEFINED(_HEAPSIZE) ? _HEAPSIZE : 2048) ;
		_e_heap = .;
		__end_heap = . ;
	} > REGION_HEAP AT > REGION_DATA

	_load_addr_text = LOADADDR(.text);
	_load_addr_rodata = LOADADDR(.rodata);
	_load_addr_data = LOADADDR(.data);
}