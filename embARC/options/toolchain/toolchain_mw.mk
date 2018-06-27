ifeq ($(DEBUG), 1)
	CDEBUG_OPTION := -g
endif

ifeq ($(MAP), 1)
	LMAP_OPTION = -Hldopt=-Coutput=$(APPL_FULL_NAME).map -Hldopt=-Csections -Hldopt=-Ccrossfunc -Hldopt=-Csize -zstdout
endif

ifdef HEAPSZ
	HEAPSZ_LOPT := -Hheap=$(strip $(HEAPSZ))
	HEAP_DEFINES := -D_HEAPSIZE=$(strip $(HEAPSZ))
else
	HEAPSZ_LOPT := -Hheap=$(strip $(DEFAULT_HEAPSZ))
	HEAP_DEFINES += -D_HEAPSIZE=$(strip $(DEFAULT_HEAPSZ))
endif

ifdef STACKSZ
	STACKSZ_LOPT := -Hstack=$(strip $(STACKSZ))
	STACK_DEFINES := -D_STACKSIZE=$(strip $(STACKSZ))
else
	STACKSZ_LOPT := -Hstack=$(strip $(DEFAULT_STACKSZ))
	STACK_DEFINES += -D_STACKSIZE=$(strip $(DEFAULT_STACKSZ))
endif

## MW Support Hostlink
# So define it
##
ifeq ($(HOSTLINK), 1)
	LCORE_OPT_MW += -Hhostlink
else
	LCORE_OPT_MW += -Hhostlib=
endif
## Nsim Defines
ifeq ($(NSIM), 1)
endif

TOOLCHAIN_DEFINES += $(HEAP_DEFINES) $(STACK_DEFINES) -D__MW__

SUPPORTED_OLEVELS = O O0 O1 O2 O3 Os Os1 Oz Ofast Og
##
# Optimization level settings
# Metaware Compiler Version
# refer to C/C++ Programmer's Guide for the MetaWare Compiler
# 7 Optimizing Performance
##
## -O		Compile for maximum code performance. (alias for -O3)
## -O0		Compile with no optimization. (Default if -g specified.)
## -O1		Compile fast with minimal optimization.
## -O2		Compile for reasonably performing code.
## -O3		Compile for maximum code performance.
## -Os		Compile for minimum code size, yet reasonable performance.
## -Os1		Compile for minimum code size, regardless of performance. (alias for Oz)
## -Oz		Compile for minimum code size, regardless of performance.
##
ifeq ($(OLEVEL), O)
	#For the ccac compiler, equivalent to -O3
	OPT_OLEVEL = -O
else
ifeq ($(OLEVEL), O0)
	#Compile with no optimization. (Default if -g specified.)
	OPT_OLEVEL = -O0
else
ifeq ($(OLEVEL), O1)
	#Level 1 (debugging, faster)
	OPT_OLEVEL = -O1
else
## Level 2 to 6 (progressively faster code at the possible expense of code size.
## The higher the level, lesser is the code-size taken into consideration).
## For the ccac, -O3 is the highest optimization. -O4 to -O6 are equivalent to -O3.
ifeq ($(OLEVEL), O2)
	#Compile for reasonably performing code
	OPT_OLEVEL = -O2
else
ifeq ($(OLEVEL), O3)
	#Compile for maximum code performance.
	OPT_OLEVEL = -O3
else
ifeq ($(OLEVEL), Os)
	#Level s (applies performance optimizations that do not sacrifice code size, and
	#a few additional optimizations to improve code size with only minor performance reduction)
	OPT_OLEVEL = -Os
else
ifeq ($(OLEVEL), Os1)
	#Level s1 (similar to -Os, but the compiler is given addition freedom to reduce code size at the expense of performance)
	OPT_OLEVEL = -Os1
else
ifeq ($(OLEVEL), Oz)
	#here equivalent to -Os1
	OPT_OLEVEL = -Oz
else
ifeq ($(OLEVEL), Ofast)
	#here equivalent to -O3
	OPT_OLEVEL = -O3
else
ifeq ($(OLEVEL), Og)
	#Level 1 (debugging, faster, smaller)
	OPT_OLEVEL = -O1 -g
else
	#no optimization option defined
	OPT_OLEVEL =
endif
endif
endif
endif
endif
endif
endif
endif
endif
endif

##
# define tools
##
MW_TOOLCHAIN_PREFIX := $(strip $(MW_TOOLPATH))

ifneq ($(MW_TOOLCHAIN_PREFIX), )
MW_TOOLCHAIN_PREFIX := $(wildcard $(MW_TOOLCHAIN_PREFIX))
endif

## METAWARE TOOLCHAIN TOOLS NAME DEFINITIONS ##
	AR	= arac
	AS	= ccac
	CC	= ccac
	LD   	= ccac
	DMP 	= elfdumpac
	NM	= nmac
	OBJCOPY	= elf2bin
	SIZE	= sizeac

##
# when defined path has exists
# define new environment path
##
MW_TOOLCHAIN_PREFIX_TEST := $(wildcard $(MW_TOOLCHAIN_PREFIX)/$(DMP)*)
ifneq ($(MW_TOOLCHAIN_PREFIX_TEST), )
	CC	:= $(MW_TOOLCHAIN_PREFIX)/$(CC)
	AS	:= $(MW_TOOLCHAIN_PREFIX)/$(AS)
	DMP	:= $(MW_TOOLCHAIN_PREFIX)/$(DMP)
	LD	:= $(MW_TOOLCHAIN_PREFIX)/$(LD)
	AR	:= $(MW_TOOLCHAIN_PREFIX)/$(AR)
	NM	:= $(MW_TOOLCHAIN_PREFIX)/$(NM)
	OBJCOPY	:= $(MW_TOOLCHAIN_PREFIX)/$(OBJCOPY)
	SIZE	:= $(MW_TOOLCHAIN_PREFIX)/$(SIZE)
endif
	MAKE	= gmake
	DBG	= mdb
	NSIMDRV	= nsimdrv
##
#   build options
##
	MKDEP_OPT	= -MMD -MT '$@' -MF $@.d
	COMPILE_OPT	+= $(OPT_OLEVEL) $(CDEBUG_OPTION) $(CCORE_OPT_MW) $(ADT_COPT) -Wincompatible-pointer-types -Hnocplus -Hnocopyr $(DEFINES) $(INCLUDE) $(MKDEP_OPT)
	LINK_OPT	+= $(HEAPSZ_LOPT) $(STACKSZ_LOPT) $(LCORE_OPT_MW) $(ADT_LOPT) -Hnocopyr $(LMAP_OPTION) $(LINKER_SCRIPT_MW)
	ASM_OPT		+= $(COMPILE_OPT) $(ACORE_OPT_MW) $(ADT_AOPT)
	AR_OPT		+= -rsq
	DMP_OPT		+= -hIpst
	DASM_OPT	+= -z
	SIZE_OPT	+= -gq

	LD_START_GROUPLIB  = -Hldopt=-Bgrouplib
	LD_SYSTEMLIBS      =
	LD_END_GROUPLIB    =

	LINKER_SCRIPT	= $(LINKER_SCRIPT_MW)

##
#   additional options
##
	ELF2HEX_OPT	= -V -i1 -n64 -p0x000 -ctldb
	ELF2BIN_OPT	=
