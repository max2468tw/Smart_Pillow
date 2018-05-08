NSIM_1506_BOARD_DIR = $(EMBARC_ROOT)/board/nsim/configs/1506

## Current Supported Core Configurations
##
# ====================NSIM 201506========================
#    arcem6gp
##
SUPPORTED_CORES := arcemfull

##
# CURRENT CORE CONFIGURATION
# Select the core configuration loaded into FPGA chip
##
CUR_CORE ?= arcemfull

override CUR_CORE := $(strip $(CUR_CORE))

## Set Valid Core Configuration For Board
VAILD_CUR_CORE = $(call check_item_exist, $(CUR_CORE), $(SUPPORTED_CORES))

## Check Core Configuation Supported
ifneq ($(VAILD_CUR_CORE),)
COMMON_COMPILE_PREREQUISITES += $(NSIM_1506_BOARD_DIR)/cores/core_$(VAILD_CUR_CORE).mk
include $(NSIM_1506_BOARD_DIR)/cores/core_$(VAILD_CUR_CORE).mk
else
$(info BOARD $(BOARD)-$(BD_VER) Core Configurations - $(SUPPORTED_CORES) are supported)
$(error $(CUR_CORE) is not supported in $(BOARD)-$(BD_VER), please check it!)
endif

##
# \brief	development board cpu and device running frequency definition(HZ)
##
CPU_FREQ  ?= 25000000
DEV_FREQ  ?= 50000000

##
# \brief	emsk board related source and header
##
##
# \brief	emsk device driver related
##
NSIM_1506_DEV_CSRCDIR	=
NSIM_1506_DEV_ASMSRCDIR	=
NSIM_1506_DEV_INCDIR	=

NSIM_1506_CSRCDIR	= $(NSIM_1506_DEV_CSRCDIR)   $(NSIM_1506_BOARD_DIR)
NSIM_1506_ASMSRCDIR	= $(NSIM_1506_DEV_ASMSRCDIR) $(NSIM_1506_BOARD_DIR)
NSIM_1506_INCDIR	= $(NSIM_1506_DEV_ASMSRCDIR) $(NSIM_1506_BOARD_DIR)


EMSK_CSRCDIR	+= $(NSIM_1506_CSRCDIR)
EMSK_ASMSRCDIR	+= $(NSIM_1506_ASMSRCDIR)
EMSK_INCDIR	+= $(NSIM_1506_INCDIR)

NSIM_1506_DEFINES	+=

##
# \brief	linker script for EM Starter Kit for
#	both gnu gcc compiler and metaware compiler
##
LINKER_SCRIPT_MW  ?= $(NSIM_1506_BOARD_DIR)/ldscripts/nsim_$(VAILD_CUR_CORE).met
LINKER_SCRIPT_GNU ?= $(NSIM_1506_BOARD_DIR)/ldscripts/nsim_$(VAILD_CUR_CORE).ld

NSIM_DEFINES += $(NSIM_1506_DEFINES)
