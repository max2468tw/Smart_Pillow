# Application name
APPL ?= secureshield_secret

# Optimization Level
# Please Refer to toolchain_xxx.mk for this option
OLEVEL ?= O2

BOARD ?= nsim

CUR_CORE = arcemfull

APPL_DEFINES  = -DLIB_SECURESHIELD_OVERRIDES -DNTSHELL_USE_CMDS_PERIPHERAL=0 -DNTSHELL_USE_CMDS_ARC=0

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

# include current project makefile
COMMON_COMPILE_PREREQUISITES += makefile

### Options above must be added before include options.mk ###
# include key embARC build system makefile
override EMBARC_ROOT := $(strip $(subst \,/,$(EMBARC_ROOT)))
include $(EMBARC_ROOT)/options/options.mk
