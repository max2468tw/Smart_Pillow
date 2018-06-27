EMSK_22_BOARD_DIR = $(EMBARC_ROOT)/board/emsk/configs/22

# EMSK 2.2 EM9D run in 1-DDR or 0-CCM
EMSK_22_EM9D_RUN_IN_DDR ?= 0

## Current Supported Core Configurations
##
# ====================EMSK 2.2===================================
#  arcem7d     (em7d with DSP, without XY)	-configuration 0 (switch 1 up,   switch 2 up):
#  arcem9d     (em9d with XY, DSP, FPUsp)	-configuration 1 (switch 1 down, switch 2 up):
#  arcem11d    (em11d with XY, DSP, FPUdp)	-configuration 2 (switch 1 up, switch 2 down):
##
SUPPORTED_CORES := arcem7d arcem9d arcem11d

##
# CURRENT CORE CONFIGURATION
# Select the core configuration loaded into FPGA chip
##
CUR_CORE ?= arcem7d

override CUR_CORE := $(strip $(CUR_CORE))

## Set Valid Core Configuration For Board
VAILD_CUR_CORE = $(call check_item_exist, $(CUR_CORE), $(SUPPORTED_CORES))

## Check Core Configuation Supported
ifneq ($(VAILD_CUR_CORE),)
COMMON_COMPILE_PREREQUISITES += $(EMSK_22_BOARD_DIR)/cores/core_$(VAILD_CUR_CORE).mk
include $(EMSK_22_BOARD_DIR)/cores/core_$(VAILD_CUR_CORE).mk
else
$(info BOARD $(BOARD)-$(BD_VER) Core Configurations - $(SUPPORTED_CORES) are supported)
$(error $(CUR_CORE) is not supported in $(BOARD)-$(BD_VER), please check it!)
endif

##
# \brief	development board cpu and device running frequency definition(HZ)
##
# CPU CLOCK FREQ SETTINGS #
# EMSK_EM7D for 30 MHz
ifeq ($(strip $(VAILD_CUR_CORE)), arcem7d)
CPU_FREQ  ?= 30000000
override DIG_SPEED := 10000000
else
# EMSK_EM9D for 18 MHz
ifeq ($(strip $(VAILD_CUR_CORE)), arcem9d)
CPU_FREQ  ?= 20000000
override DIG_SPEED := 5000000
else
# EMSK_EM11D for 18 MHz
ifeq ($(strip $(VAILD_CUR_CORE)), arcem11d)
CPU_FREQ  ?= 20000000
override DIG_SPEED := 5000000
else
CPU_FREQ  ?= 10000000
override DIG_SPEED := 2000000
endif
endif
endif
# PERPHERIAL CLOCK FREQ SETTINGS #
DEV_FREQ  ?= 50000000

##
# \brief	emsk board related source and header
##
EMSK_22_DEV_CSRCDIR 	=
EMSK_22_DEV_ASMSRCDIR 	=
EMSK_22_DEV_INCDIR	=

EMSK_22_CSRCDIR		= $(EMSK_22_DEV_CSRCDIR)   $(EMSK_22_BOARD_DIR)
EMSK_22_ASMSRCDIR	= $(EMSK_22_DEV_ASMSRCDIR) $(EMSK_22_BOARD_DIR)
EMSK_22_INCDIR		= $(EMSK_22_DEV_ASMSRCDIR) $(EMSK_22_BOARD_DIR)


EMSK_CSRCDIR	+= $(EMSK_22_CSRCDIR)
EMSK_ASMSRCDIR	+= $(EMSK_22_ASMSRCDIR)
EMSK_INCDIR	+= $(EMSK_22_INCDIR)

EMSK_22_DEFINES	+=

##
# \brief	linker script for EM Starter Kit for
#	both gnu gcc compiler and metaware compiler
##
ifeq ($(strip $(VAILD_CUR_CORE)), arcem9d)

ifeq ($(strip $(EMSK_22_EM9D_RUN_IN_DDR)), 1)
## For EM9D using SPI, SPI max working frequence should be set 1000 Hz ##
LINKER_SCRIPT_MW  ?= $(EMSK_22_BOARD_DIR)/ldscripts/emsk_em9d_ddr.met
LINKER_SCRIPT_GNU ?= $(EMSK_22_BOARD_DIR)/ldscripts/emsk_em9d_ddr.ld
EMSK_22_DEFINES += -DBOARD_SPI_FREQ=1000
else
LINKER_SCRIPT_MW  ?= $(EMSK_22_BOARD_DIR)/ldscripts/emsk_em9d_ccm.met
LINKER_SCRIPT_GNU ?= $(EMSK_22_BOARD_DIR)/ldscripts/emsk_em9d_ccm.ld
EMSK_22_DEFINES += -DBOARD_SPI_FREQ=800000
endif

endif

ifeq ($(strip $(VAILD_CUR_CORE)), arcem11d)
LINKER_SCRIPT_MW  ?= $(EMSK_22_BOARD_DIR)/ldscripts/emsk_em11d.met
LINKER_SCRIPT_GNU ?= $(EMSK_22_BOARD_DIR)/ldscripts/emsk_em11d.ld
EMSK_22_DEFINES += -DBOARD_SPI_FREQ=800000
endif

ifeq ($(strip $(VAILD_CUR_CORE)), arcem7d)
LINKER_SCRIPT_MW  ?= $(EMSK_22_BOARD_DIR)/ldscripts/emsk_em7d.met
LINKER_SCRIPT_GNU ?= $(EMSK_22_BOARD_DIR)/ldscripts/emsk_em7d.ld
EMSK_22_DEFINES += -DBOARD_SPI_FREQ=800000
endif

EMSK_DEFINES += $(EMSK_22_DEFINES)
