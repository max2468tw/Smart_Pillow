EMSK_11_BOARD_DIR = $(EMBARC_ROOT)/board/emsk/configs/11

## Current Supported Core Configurations
##
# ====================EMSK 1.0 & EMSK 1.1========================
#    arcem4      -configuration 0 (switch 1 up,   switch 2 up):
#    arcem4cr16  -configuration 1 (switch 1 down, switch 2 up):
#    arcem6      -configuration 2 (switch 1 up,   switch 2 down):
#    arcem6gp    -configuration 3 (switch 1 down, switch 2 down):
##
SUPPORTED_CORES := arcem4 arcem4cr16 arcem6 arcem6gp

##
# CURRENT CORE CONFIGURATION
# Select the core configuration loaded into FPGA chip
##
CUR_CORE ?= arcem6

override CUR_CORE := $(strip $(CUR_CORE))

## Set Valid Core Configuration For Board
VAILD_CUR_CORE = $(call check_item_exist, $(CUR_CORE), $(SUPPORTED_CORES))

## Check Core Configuation Supported
ifneq ($(VAILD_CUR_CORE),)
COMMON_COMPILE_PREREQUISITES += $(EMSK_11_BOARD_DIR)/cores/core_$(VAILD_CUR_CORE).mk
include $(EMSK_11_BOARD_DIR)/cores/core_$(VAILD_CUR_CORE).mk
else
$(info BOARD $(BOARD)-$(BD_VER) Core Configurations - $(SUPPORTED_CORES) are supported)
$(error $(CUR_CORE) is not supported in $(BOARD)-$(BD_VER), please check it!)
endif

##
# \brief	development board cpu and device running frequency definition(HZ)
##

# CPU CLOCK FREQ SETTINGS #
ifeq ($(strip $(VAILD_CUR_CORE)), arcem4)
CPU_FREQ  ?= 35000000
else
ifeq ($(strip $(VAILD_CUR_CORE)), arcem4cr16)
CPU_FREQ  ?= 35000000
else
ifeq ($(strip $(VAILD_CUR_CORE)), arcem6)
CPU_FREQ  ?= 30000000
else
ifeq ($(strip $(VAILD_CUR_CORE)), arcem6gp)
CPU_FREQ  ?= 30000000
else
CPU_FREQ  ?= 30000000
endif
endif
endif
endif
# PERPHERIAL CLOCK FREQ SETTINGS #
DEV_FREQ  ?= 50000000

##
# \brief	emsk board related source and header
##
EMSK_11_DEV_CSRCDIR	=
EMSK_11_DEV_ASMSRCDIR	=
EMSK_11_DEV_INCDIR	=

EMSK_11_CSRCDIR		= $(EMSK_11_DEV_CSRCDIR)   $(EMSK_11_BOARD_DIR)
EMSK_11_ASMSRCDIR	= $(EMSK_11_DEV_ASMSRCDIR) $(EMSK_11_BOARD_DIR)
EMSK_11_INCDIR		= $(EMSK_11_DEV_ASMSRCDIR) $(EMSK_11_BOARD_DIR)


EMSK_CSRCDIR	+= $(EMSK_11_CSRCDIR)
EMSK_ASMSRCDIR	+= $(EMSK_11_ASMSRCDIR)
EMSK_INCDIR	+= $(EMSK_11_INCDIR)

EMSK_11_DEFINES	+=

##
# \brief	linker script for EM Starter Kit for
#	both gnu gcc compiler and metaware compiler
##
ifeq ($(strip $(VAILD_CUR_CORE)), arcem4)
LINKER_SCRIPT_MW  ?= $(EMSK_11_BOARD_DIR)/ldscripts/emsk_em4.met
LINKER_SCRIPT_GNU ?= $(EMSK_11_BOARD_DIR)/ldscripts/emsk_em4.ld
EMSK_11_DEFINES += -DBOARD_SPI_FREQ=800000
endif

ifeq ($(strip $(VAILD_CUR_CORE)), arcem4cr16)
LINKER_SCRIPT_MW  ?= $(EMSK_11_BOARD_DIR)/ldscripts/emsk_em4cr16.met
LINKER_SCRIPT_GNU ?= $(EMSK_11_BOARD_DIR)/ldscripts/emsk_em4cr16.ld
EMSK_11_DEFINES += -DBOARD_SPI_FREQ=800000
endif

ifeq ($(strip $(VAILD_CUR_CORE)), arcem6)
LINKER_SCRIPT_MW  ?= $(EMSK_11_BOARD_DIR)/ldscripts/emsk_em6.met
LINKER_SCRIPT_GNU ?= $(EMSK_11_BOARD_DIR)/ldscripts/emsk_em6.ld
EMSK_11_DEFINES += -DBOARD_SPI_FREQ=800000
endif

ifeq ($(strip $(VAILD_CUR_CORE)), arcem6gp)
LINKER_SCRIPT_MW  ?= $(EMSK_11_BOARD_DIR)/ldscripts/emsk_em6gp.met
LINKER_SCRIPT_GNU ?= $(EMSK_11_BOARD_DIR)/ldscripts/emsk_em6gp.ld
EMSK_11_DEFINES += -DBOARD_SPI_FREQ=800000
endif

EMSK_DEFINES += $(EMSK_11_DEFINES)