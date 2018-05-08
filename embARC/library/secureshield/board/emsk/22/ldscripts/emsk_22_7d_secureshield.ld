MEMORY {
	iccm :	ORIGIN = 0x0,		LENGTH = 128K
	iccm_secureshield : ORIGIN = 0x20000,	LENGTH = 128K
	dccm :	ORIGIN = 0x80000000,	LENGTH = 64K
	dccm_secureshield : ORIGIN = 0x80010000, LENGTH = 64K
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
REGION_ALIAS("REGION_SECURESHIELD_ROM", iccm_secureshield)
REGION_ALIAS("REGION_SECURESHIELD_RAM", dccm_secureshield)


SECTIONS
{
	. = ORIGIN(REGION_SECURESHIELD_ROM);
	.secureshield.text : {
		/* secureshield code and rodata */
		. = ALIGN(4);
		__secureshield_main_start = .;
		*secureshield/*.o(.text .text.* .gnu.linkonce.t.*)
		*secureshield/*.o(.rodata .rodata.* .gnu.linkonce.r.*)
		*(.secureshield.secure_container.text)
		*(.secureshield.secure_container.rodata)
		*(.secureshield.main)
		__secureshield_main_end = .;
	} > REGION_SECURESHIELD_ROM

	.secureshield.cfg : {
		__secureshield_secure_start = .;
		/* secureshield secure shields configuration tables */
		. = ALIGN(32);
		__secureshield_cfgtbl_start = .;
		KEEP(*(.keep.secureshield.cfgtbl))
		. = ALIGN(32);
		__secureshield_cfgtbl_end = .;

		/* pointers to secureshield secure shields configuration tables */
		/* note: no further alignment here, we need to have the exact list of pointers */
		__secureshield_cfgtbl_ptr_start = .;
		KEEP(*(.keep.secureshield.cfgtbl_ptr_first))
		KEEP(*(.keep.secureshield.cfgtbl_ptr))
		__secureshield_cfgtbl_ptr_end = .;

		. = ALIGN(32);
		__secureshield_secure_end = .;
	} > REGION_SECURESHIELD_ROM

	. = ORIGIN(REGION_SECURESHIELD_RAM);
	.secureshield.data ALIGN(4) :
	{
		/* secureshield code and data */
		. = ALIGN(4);
		*secureshield/*.o(.data .data.* .gnu.linkonce.d.*)
		*secureshield/*.o(.sdata .sdata.* .gnu.linkonce.s.*)
		*(.secureshield.secure_container.data)
	} > REGION_SECURESHIELD_RAM

	.secureshield.bss (NOLOAD): {
		. = ALIGN(4);
		__secureshield_bss_start = .;
		/* protected secureshield main bss */
		__secureshield_bss_main_start = .;
		*secureshield/*.o(.bss .bss.* .gnu.linkonce.b.* .dynbss COMMON)
		*(.secureshield.secure_container.bss)
		KEEP(*(.keep.secureshield.bss.main))
		__secureshield_bss_main_end = .;
	} > REGION_SECURESHIELD_RAM

	.secureshield.bss.containers (NOLOAD): {
		. = ALIGN(2048);
		/* protected containers bss */
		__secureshield_bss_containers_start = .;
		KEEP(*(.keep.secureshield.bss.containers))
		. = ALIGN(2048);
		__secureshield_bss_containers_end = .;

		__secureshield_bss_end = .;
	} > REGION_SECURESHIELD_RAM


	. = ORIGIN(REGION_INIT);
	.init :
	{
		_f_init = .;
		KEEP (*(.init_vector))
		KEEP (*(.init_bootstrap))
		_e_init = .;
	} > REGION_INIT

	.vector : ALIGN(1024)
	{
		_f_vector = .;
		*(.vector)
		_e_vector = .;
	} > REGION_VECTOR

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


	.data : ALIGN(4)
	{
		_f_data = .;
		*(.data .data.* .gnu.linkonce.d.*)
	} > REGION_DATA

	.sdata : ALIGN(4)
	{
		_f_sdata = .;
		__SDATA_BEGIN__ = .;
		*(.sdata .sdata.* .gnu.linkonce.s.*)
		_e_sdata = .;
		_e_data = .;
	} > REGION_SDATA

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
	} > REGION_BSS

	.bss (NOLOAD) : ALIGN(8)
	{
		*(.dynbss)
		*(.bss .bss.* .gnu.linkonce.b.*)
		*( COMMON)
		_e_bss = .;
	} > REGION_BSS

	.stack (NOLOAD) :
	{
		. = ALIGN(4);
		_f_stack = .;
		. = . + (DEFINED(_STACKSIZE) ? _STACKSIZE : 2048) ;
		_e_stack = .;
	} > REGION_STACK

	.heap (NOLOAD) :
	{
		. = ALIGN(4);
		__start_heap = . ;
		_f_heap = .;
		. = . + (DEFINED(_HEAPSIZE) ? _HEAPSIZE : 2048) ;
		_e_heap = .;
		__end_heap = . ;
	} > REGION_HEAP

	_load_addr_text = LOADADDR(.text);
	_load_addr_rodata = LOADADDR(.rodata);
	_load_addr_data = LOADADDR(.data);
}
