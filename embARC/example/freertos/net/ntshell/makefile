# Application name
APPL ?= freertos_net_ntshell

#
# root dir of embARC
#
EMBARC_ROOT = ../../../..

# Selected OS
OS_SEL ?= freertos
# Select Middleware Packages
MID_SEL = common ntshell lwip lwip-contrib fatfs
LWIP_CONTRIB_APPS ?=

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
