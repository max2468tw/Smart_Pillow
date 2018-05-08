# Application name
APPL ?= secureshield_secret2

# Optimization Level
# Please Refer to toolchain_xxx.mk for this option
OLEVEL ?= O2

BOARD ?= emsk

BD_VER ?= 22

CUR_CORE ?= arcem7d

# The application overrides the memory map defined in lib/secure/shield/board/xxx
# Indicate the new ROM/RAM settings here, consistent with the new linker files
# Note that for current ARC EM MPU region sizes should be power of 2
APPL_DEFINES  = -DLIB_SECURESHIELD_OVERRIDES -DROM_LENGTH=0x20000 -DSECURESHIELD_ROM_ORIGIN=0x20000 \
		-DSECURESHIELD_ROM_LENGTH=0x8000 -DRAM_LENGTH=0x8000 -DSECURESHIELD_RAM_ORIGIN=0x80008000 \
		-DSECURESHIELD_RAM_LENGTH=0x8000 -DNTSHELL_USE_CMDS_PERIPHERAL=0 -DNTSHELL_USE_CMDS_ARC=0

SECURESHIELD_LINKER_SCRIPT_MW = linker.met
SECURESHIELD_LINKER_SCRIPT_GNU = linker.ld

#
# root dir of embARC
#
EMBARC_ROOT = ../../../..

LIB_SEL = secureshield
MID_SEL = common ntshell

# application source dirs
APPL_CSRC_DIR = .
APPL_ASMSRC_DIR = .

# application include dirs
APPL_INC_DIR = .

ifneq ($(TOOLCHAIN),gnu)
ADT_COPT = -Ml
else
ADT_COPT = -mlong-calls
endif

# include current project makefile
COMMON_COMPILE_PREREQUISITES += makefile

### Options above must be added before include options.mk ###
# include key embARC build system makefile
override EMBARC_ROOT := $(strip $(subst \,/,$(EMBARC_ROOT)))
include $(EMBARC_ROOT)/options/options.mk
