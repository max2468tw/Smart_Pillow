EMSK_20_BOARD_DIR = $(EMBARC_ROOT)/board/emsk/configs/20

EMSK_20_EM5D_RUN_IN_DDR ?= 0

## Current Supported Core Configurations
##
# ====================EMSK 2.0===================================
#    arcem5d     -configuration 0 (switch 1 up,   switch 2 up):
#    arcem7d     -configuration 1 (switch 1 down, switch 2 up):
#    arcem7dfpu  -configuration 2 (switch 1 up,   switch 2 down):
##
SUPPORTED_CORES := arcem5d arcem7d arcem7dfpu

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
COMMON_COMPILE_PREREQUISITES += $(EMSK_20_BOARD_DIR)/cores/core_$(VAILD_CUR_CORE).mk
include $(EMSK_20_BOARD_DIR)/cores/core_$(VAILD_CUR_CORE).mk
else
$(info BOARD $(BOARD)-$(BD_VER) Core Configurations - $(SUPPORTED_CORES) are supported)
$(error $(CUR_CORE) is not supported in $(BOARD)-$(BD_VER), please check it!)
endif

##
# \brief	development board cpu and device running frequency definition(HZ)
##
# CPU CLOCK FREQ SETTINGS #
ifeq ($(strip $(VAILD_CUR_CORE)), arcem5d)
CPU_FREQ  ?= 35000000
else
ifeq ($(strip $(VAILD_CUR_CORE)), arcem7d)
CPU_FREQ  ?= 30000000
else
ifeq ($(strip $(VAILD_CUR_CORE)), arcem7dfpu)
CPU_FREQ  ?= 20000000
else
CPU_FREQ  ?= 20000000
endif
endif
endif
# PERPHERIAL CLOCK FREQ SETTINGS #
DEV_FREQ  ?= 50000000

##
# \brief	emsk board related source and header
##
EMSK_20_DEV_CSRCDIR 	=
EMSK_20_DEV_ASMSRCDIR 	=
EMSK_20_DEV_INCDIR	=

EMSK_20_CSRCDIR		= $(EMSK_20_DEV_CSRCDIR)   $(EMSK_20_BOARD_DIR)
EMSK_20_ASMSRCDIR	= $(EMSK_20_DEV_ASMSRCDIR) $(EMSK_20_BOARD_DIR)
EMSK_20_INCDIR		= $(EMSK_20_DEV_ASMSRCDIR) $(EMSK_20_BOARD_DIR)


EMSK_CSRCDIR	+= $(EMSK_20_CSRCDIR)
EMSK_ASMSRCDIR	+= $(EMSK_20_ASMSRCDIR)
EMSK_INCDIR	+= $(EMSK_20_INCDIR)

EMSK_20_DEFINES	+=

##
# \brief	linker script for EM Starter Kit for
#	both gnu gcc compiler and metaware compiler
##
ifeq ($(strip $(VAILD_CUR_CORE)), arcem5d)

ifeq ($(strip $(EMSK_20_EM5D_RUN_IN_DDR)), 1)
## For EM5D, running in DDR without cache is not recommended, because SPI frequence can't be faster ##
LINKER_SCRIPT_MW  ?= $(EMSK_20_BOARD_DIR)/ldscripts/emsk_em5d_ddr.met
LINKER_SCRIPT_GNU ?= $(EMSK_20_BOARD_DIR)/ldscripts/emsk_em5d_ddr.ld
EMSK_20_DEFINES += -DBOARD_SPI_FREQ=1000
else
## For EM5D in CCM using SPI, spi max working frequence need to set to 500KHz ##
LINKER_SCRIPT_MW  ?= $(EMSK_20_BOARD_DIR)/ldscripts/emsk_em5d_ccm.met
LINKER_SCRIPT_GNU ?= $(EMSK_20_BOARD_DIR)/ldscripts/emsk_em5d_ccm.ld
EMSK_20_DEFINES += -DBOARD_SPI_FREQ=500000
endif

endif

ifeq ($(strip $(VAILD_CUR_CORE)), arcem7d)
LINKER_SCRIPT_MW  ?= $(EMSK_20_BOARD_DIR)/ldscripts/emsk_em7d.met
LINKER_SCRIPT_GNU ?= $(EMSK_20_BOARD_DIR)/ldscripts/emsk_em7d.ld
EMSK_20_DEFINES += -DBOARD_SPI_FREQ=800000
endif

ifeq ($(strip $(VAILD_CUR_CORE)), arcem7dfpu)
LINKER_SCRIPT_MW  ?= $(EMSK_20_BOARD_DIR)/ldscripts/emsk_em7d.met
LINKER_SCRIPT_GNU ?= $(EMSK_20_BOARD_DIR)/ldscripts/emsk_em7d.ld
EMSK_20_DEFINES += -DBOARD_SPI_FREQ=800000
endif

EMSK_DEFINES += $(EMSK_20_DEFINES)