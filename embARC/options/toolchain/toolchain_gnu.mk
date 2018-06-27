ifeq ($(DEBUG), 1)
	CDEBUG_OPTION := -g
endif

ifeq ($(MAP), 1)
	LMAP_OPTION = -Wl,-M,-Map=$(APPL_FULL_NAME).map
endif

ifdef HEAPSZ
	HEAPSZ_LOPT := -Wl,--defsym=_HEAPSIZE=$(strip $(HEAPSZ))
	HEAP_DEFINES := -D_HEAPSIZE=$(strip $(HEAPSZ))
else
	HEAPSZ_LOPT := -Wl,--defsym=_HEAPSIZE=$(strip $(DEFAULT_HEAPSZ))
	HEAP_DEFINES := -D_HEAPSIZE=$(strip $(DEFAULT_HEAPSZ))
endif

ifdef STACKSZ
	STACKSZ_LOPT := -Wl,--defsym=_STACKSIZE=$(strip $(STACKSZ))
	STACK_DEFINES := -D_STACKSIZE=$(strip $(STACKSZ))
else
	STACKSZ_LOPT := -Wl,--defsym=_STACKSIZE=$(strip $(DEFAULT_STACKSZ))
	STACK_DEFINES := -D_STACKSIZE=$(strip $(DEFAULT_STACKSZ))
endif

## GNU Don't Support Hostlink
# So don't define it
##
ifeq ($(HOSTLINK), 1)
endif
## Nsim Defines
ifeq ($(NSIM), 1)
endif

TOOLCHAIN_DEFINES += $(HEAP_DEFINES) $(STACK_DEFINES) -D__GNU__

SUPPORTED_OLEVELS = O O0 O1 O2 O3 Os Os1 Oz Ofast Og
##
# Optimization level settings
# GCC Version For ARC
# refer to Using the GNU Compiler Collection (GCC)
# 3.10 Options That Control Optimization
# You can invoke GCC with ‘-Q --help=optimizers’
# to find out the exact set of optimizations
# that are enabled at each level.
##
## -O<number>	Set optimization level to <number>
## -Ofast	Optimize for speed disregarding exact standards compliance
## -Og		Optimize for debugging experience rather than speed or size
## -Os		Optimize for space rather than speed
ifeq ($(OLEVEL), O)
	#Optimize. Optimizing compilation takes
	#somewhat more time, and a lot more memory
	#for a large function.
	#equivalent to -O1
	OPT_OLEVEL = -O
else
ifeq ($(OLEVEL), O0)
	#Reduce compilation time and
	#make debugging produce the expected results
	#This is the default.
	OPT_OLEVEL = -O0
else
ifeq ($(OLEVEL), O1)
	#equivalent to -O
	OPT_OLEVEL = -O1
else
ifeq ($(OLEVEL), O2)
	#Optimize even more. GCC performs nearly all
	#supported optimizations that do not involve
	#a space-speed tradeoff. As compared to ‘-O’,
	#this option increases both compilation time
	#and the performance of the generated code
	OPT_OLEVEL = -O2
else
ifeq ($(OLEVEL), O3)
	#Optimize yet more. ‘-O3’ turns on all optimizations
	#specified by ‘-O2’ and also turns on the ‘-finline-functions’,
	#‘-funswitch-loops’, ‘-fpredictive-commoning’, ‘-fgcse-after-reload’,
	#‘-ftree-vectorize’, ‘-fvect-cost-model’,
	#‘-ftree-partial-pre’ and ‘-fipa-cp-clone’ options.
	OPT_OLEVEL = -O3
else
ifeq ($(OLEVEL), Os)
	#Optimize for size. ‘-Os’ enables all ‘-O2’ optimizations
	#that do not typically increase code size. It also performs
	#further optimizations designed to reduce code size
	OPT_OLEVEL = -Os
else
ifeq ($(OLEVEL), Os1)
	#here equivalent to -Os
	OPT_OLEVEL = -Os
else
ifeq ($(OLEVEL), Oz)
	#here equivalent to -Os
	OPT_OLEVEL = -Os
else
ifeq ($(OLEVEL), Ofast)
	#Disregard strict standards compliance.
	#‘-Ofast’ enables all ‘-O3’ optimizations. It also enables
	#optimizations that are not valid for all standardcompliant programs.
	OPT_OLEVEL = -Ofast
else
ifeq ($(OLEVEL), Og)
	#Optimize debugging experience. ‘-Og’ enables optimizations
	#that do not interfere with debugging.
	OPT_OLEVEL = -Og
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
GNU_TOOLCHAIN_PREFIX := $(strip $(GNU_TOOLPATH))

ifneq ($(GNU_TOOLCHAIN_PREFIX), )
GNU_TOOLCHAIN_PREFIX := $(wildcard $(GNU_TOOLCHAIN_PREFIX))
endif

## GNU TOOLCHAIN TOOLS NAME DEFINITIONS ##
	CC	= arc-elf32-gcc
	AS	= arc-elf32-as
	DMP	= arc-elf32-objdump
	LD	= arc-elf32-gcc
	AR	= arc-elf32-ar
	NM	= arc-elf32-nm
	OBJCOPY	= arc-elf32-objcopy
	SIZE	= arc-elf32-size

## GNU TOOLCHAIN EXIST TESTING ##
GNU_TOOLCHAIN_PREFIX_TEST := $(wildcard $(GNU_TOOLCHAIN_PREFIX)/$(DMP)*)
ifneq ($(GNU_TOOLCHAIN_PREFIX_TEST), )
	CC	:= $(GNU_TOOLCHAIN_PREFIX)/$(CC)
	AS	:= $(GNU_TOOLCHAIN_PREFIX)/$(AS)
	DMP	:= $(GNU_TOOLCHAIN_PREFIX)/$(DMP)
	LD	:= $(GNU_TOOLCHAIN_PREFIX)/$(LD)
	AR	:= $(GNU_TOOLCHAIN_PREFIX)/$(AR)
	NM	:= $(GNU_TOOLCHAIN_PREFIX)/$(NM)
	OBJCOPY	:= $(GNU_TOOLCHAIN_PREFIX)/$(OBJCOPY)
	SIZE	:= $(GNU_TOOLCHAIN_PREFIX)/$(SIZE)
endif

	MAKE	= make
	DBG	?= arc-elf32-gdb
	NSIMDRV	= nsimdrv

##
#   Openocd script root location(OPENOCD_SCRIPT_ROOT)
#   Please take care with letter case of the path
#   Please use absolute path and linux slash(/)
##
## Set windows Path for Openocd script root
ifeq "$(HOST_OS)" "Msys"
	OPENOCD_SCRIPT_ROOT = C:/arc_gnu/share/openocd/scripts
else
## Set Cygwin for windows Path for Openocd script root
ifeq "$(HOST_OS)" "Cygwin"
	OPENOCD_SCRIPT_ROOT = C:/arc_gnu/share/openocd/scripts
else
## Set Linux Path for Openocd script root
ifeq "$(HOST_OS)" "GNU/Linux"
	OPENOCD_SCRIPT_ROOT = ~/arc_gnu/share/openocd/scripts
else
## Set default Path for Openocd script root
	OPENOCD_SCRIPT_ROOT = C:/arc_gnu/share/openocd/scripts
endif
endif
endif

## Don't change this line
override OPENOCD_SCRIPT_ROOT := $(subst \,/, $(strip $(OPENOCD_SCRIPT_ROOT)))

##
#   toolchain flags
##
libnsim = $(shell $(CC) -print-file-name=libnsim.a)
ifeq ($(libnsim),libnsim.a)
    # File doens't exists - old newlib.
    DEFINES += -U_HAVE_LIBGLOSS_
else
    DEFINES += -D_HAVE_LIBGLOSS_
endif

##
#   build options
##
	MKDEP_OPT	=  -MMD -MT '$@' -MF $@.d
	COMPILE_OPT	+= -std=gnu99 $(OPT_OLEVEL) $(CDEBUG_OPTION) $(CCORE_OPT_GNU) $(ADT_COPT) $(DEFINES) $(INCLUDE) $(MKDEP_OPT)
	LINK_OPT	+= $(HEAPSZ_LOPT) $(STACKSZ_LOPT) $(LCORE_OPT_GNU) $(ADT_LOPT) $(LMAP_OPTION) -lm -Wl,--script=$(LINKER_SCRIPT_GNU)
	ASM_OPT		+= $(COMPILE_OPT) $(ACORE_OPT_GNU) $(ADT_AOPT)
	AR_OPT		+= -rcsT
	DMP_OPT		+= -x
	DASM_OPT	+= -D
	SIZE_OPT	+=

	LD_START_GROUPLIB  = -Wl,--start-group
	LD_SYSTEMLIBS      = -lm -lc -lgcc
	LD_END_GROUPLIB    = -Wl,--end-group

	LINKER_SCRIPT	= $(LINKER_SCRIPT_GNU)
##
#   additional options
##
	ELF2HEX_OPT	= -V -i1 -n64 -p0x000 -ctldb
	ELF2BIN_OPT	= -O binary


