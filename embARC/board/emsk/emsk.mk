##
# \defgroup 	MK_BOARD_EMSK	EM Starter Kit Board Related Makefile Configurations
# \ingroup	MK_BOARD
# \brief	makefile related to emsk board configurations
##

##
# \brief	current board directory definition
##
EMSK_BOARD_DIR = $(EMBARC_ROOT)/board/emsk
##
# board version definition
# 10 stands for v1.0 (no support anymore)
# 11 stands for v1.1
# 20 stands for v2.0
# 21 stands for v2.1
# 22 stands for v2.2
# When you don't special BD_VER
# in any other place or through make options
# it will select a proper BD_VER according to your cur_core
##
SUPPORTED_BD_VERS = 11 20 21 22

## Select Board Version
BD_VER ?= 22

##
# Host Link option
# Unsupported Now
# Function Toggle
# 1 for enable
# other for disable
##
HOSTLINK ?=

##
# NSIM Test
# 1 for enable
# other for disable
##
NSIM ?=

override BD_VER := $(strip $(BD_VER))
override HOSTLINK := $(strip $(HOSTLINK))
override NSIM := $(strip $(NSIM))

## Set Valid Board Version
VAILD_BD_VER = $(call check_item_exist, $(BD_VER), $(SUPPORTED_BD_VERS))

## Try to include different board version makefiles
ifneq ($(VAILD_BD_VER),)
COMMON_COMPILE_PREREQUISITES += $(EMBARC_ROOT)/board/emsk/configs/$(VAILD_BD_VER)/emsk_$(VAILD_BD_VER).mk
include $(EMBARC_ROOT)/board/emsk/configs/$(VAILD_BD_VER)/emsk_$(VAILD_BD_VER).mk
else
$(info BOARD $(BOARD) Version - $(SUPPORTED_BD_VERS) are supported)
$(error $(BOARD) Version $(BD_VER) is not supported, please check it!)
endif

##
# \brief	emsk device driver related
##
EMSK_DEV_CSRCDIR	= $(EMBARC_ROOT)/device/designware/iic \
				$(EMBARC_ROOT)/device/designware/spi \
				$(EMBARC_ROOT)/device/designware/uart \
				$(EMBARC_ROOT)/device/designware/gpio \
				$(EMBARC_ROOT)/device/microchip/mrf24g \
				$(EMBARC_ROOT)/device/microchip/mrf24g/driver \
				$(EMBARC_ROOT)/device/rtthread/rw009

EMSK_DEV_ASMSRCDIR	=
EMSK_DEV_INCDIR		= $(EMBARC_ROOT)/device/designware/iic \
				$(EMBARC_ROOT)/device/designware/spi \
				$(EMBARC_ROOT)/device/designware/uart \
				$(EMBARC_ROOT)/device/designware/gpio \
				$(EMBARC_ROOT)/device/microchip/mrf24g \
				$(EMBARC_ROOT)/device/rtthread/rw009

##
# \brief	emsk board related source and header
##
EMSK_CSRCDIR		+= $(EMSK_DEV_CSRCDIR)		\
				$(EMSK_BOARD_DIR)/common \
				$(EMSK_BOARD_DIR)/drivers/mux \
				$(EMSK_BOARD_DIR)/drivers/uart \
				$(EMSK_BOARD_DIR)/drivers/iic \
				$(EMSK_BOARD_DIR)/drivers/spi \
				$(EMSK_BOARD_DIR)/drivers/gpio \
				$(EMSK_BOARD_DIR)/drivers/spiflash \
				$(EMSK_BOARD_DIR)/drivers/pmwifi	\
				$(EMSK_BOARD_DIR)/drivers/temperature \
				$(EMSK_BOARD_DIR)/drivers/sdcard \
				$(EMSK_BOARD_DIR)/drivers/ntshell \
				$(EMSK_BOARD_DIR)/drivers/ble

EMSK_ASMSRCDIR	+= $(EMSK_DEV_ASMSRCDIR)
EMSK_INCDIR	+= $(EMSK_DEV_INCDIR)


BOARD_CSRCDIR	+= $(EMSK_CSRCDIR)
BOARD_ASMSRCDIR	+= $(EMSK_ASMSRCDIR)
BOARD_INCDIR	+= $(EMSK_INCDIR)

EMSK_DEFINES	+=
EXTRA_BOARD_DEFINES += $(CORE_DEFINES) $(EMSK_DEFINES)

# the dir to generate objs
EMSK_OBJDIR	= $(OUT_DIR)/emsk

# find all the srcs in the target dirs
EMSK_CSRCS = $(call get_csrcs, $(EMSK_CSRCDIR))
EMSK_ASMSRCS = $(call get_asmsrcs, $(EMSK_ASMSRCDIR))

EMSK_COBJS = $(call get_objs, $(EMSK_CSRCS), $(EMSK_OBJDIR))
EMSK_ASMOBJS = $(call get_objs, $(EMSK_ASMSRCS), $(EMSK_OBJDIR))

# include dependency files
ifneq ($(MAKECMDGOALS),clean)
 -include $(EMSK_COBJS:.o=.o.d) $(EMSK_ASMOBJS:.o=.o.d)
endif

# genearte library

BOARD_LIB = $(OUT_DIR)/libemsk.a

EMSK_BOARD_DIR_FIXED=$(subst \,/,$(EMSK_BOARD_DIR))

###
# You must define the following variable in the board related makefile
# OPENOCD_CFG_FILE is the absolute path using linux slash(/) for openocd config file for selected board
# OPENOCD_OPTIONS  is the openocd command options, run openocd -h to see help message
# OPENOCD_SCRIPT_ROOT was defined in options/toolchain/toolchain_gnu.mk,
# which need to be changed according to your OS and installation path
##

ifeq ($(VAILD_BD_VER),10)
## EMSK 1.0
OPENOCD_CFG_FILE = $(OPENOCD_SCRIPT_ROOT)/board/snps_em_sk_v1.cfg
OPENOCD_OPTIONS  = -s $(OPENOCD_SCRIPT_ROOT) -f $(OPENOCD_CFG_FILE)
else
## EMSK 1.1
ifeq ($(VAILD_BD_VER),11)
OPENOCD_CFG_FILE = $(OPENOCD_SCRIPT_ROOT)/board/snps_em_sk_v1.cfg
OPENOCD_OPTIONS  = -s $(OPENOCD_SCRIPT_ROOT) -f $(OPENOCD_CFG_FILE)
else
## EMSK 2.0
ifeq ($(VAILD_BD_VER),20)
OPENOCD_CFG_FILE = $(OPENOCD_SCRIPT_ROOT)/board/snps_em_sk.cfg
OPENOCD_OPTIONS  = -s $(OPENOCD_SCRIPT_ROOT) -f $(OPENOCD_CFG_FILE)
else
## EMSK 2.1
ifeq ($(VAILD_BD_VER),21)
OPENOCD_CFG_FILE = $(OPENOCD_SCRIPT_ROOT)/board/snps_em_sk.cfg
OPENOCD_OPTIONS  = -s $(OPENOCD_SCRIPT_ROOT) -f $(OPENOCD_CFG_FILE)
else
## EMSK 2.2
ifeq ($(VAILD_BD_VER),22)
OPENOCD_CFG_FILE = $(OPENOCD_SCRIPT_ROOT)/board/snps_em_sk.cfg
OPENOCD_OPTIONS  = -s $(OPENOCD_SCRIPT_ROOT) -f $(OPENOCD_CFG_FILE)
else
## Default
OPENOCD_CFG_FILE =
OPENOCD_OPTIONS  =
endif
endif
endif
endif
endif


$(BOARD_LIB): $(EMSK_OBJDIR) $(EMSK_COBJS) $(EMSK_ASMOBJS)
	$(TRACE_ARCHIVE)
	$(Q)$(AR) $(AR_OPT) $@ $(EMSK_COBJS) $(EMSK_ASMOBJS)

# specific compile rules
.SECONDEXPANSION:
$(EMSK_COBJS): $(EMSK_OBJDIR)/%.o :$(call get_c_prerequisite, $(EMSK_CSRCDIR)) $$(COMMON_COMPILE_PREREQUISITES)
	$(TRACE_COMPILE)
	$(Q)$(CC) -c $(COMPILE_OPT) $(EMSK_COMPILE_OPT) $< -o $@

$(EMSK_ASMOBJS): $(EMSK_OBJDIR)/%.o :$(call get_asm_prerequisite, $(EMSK_ASMSRCDIR)) $$(COMMON_COMPILE_PREREQUISITES)
	$(TRACE_ASSEMBLE)
	$(Q)$(CC) -c $(ASM_OPT) $(EMSK_COMPILE_OPT) $< -o $@

# generate obj dir
$(EMSK_OBJDIR):
	$(TRACE_CREATE_DIR)
	$(Q)$(MKD) $(subst /,$(PS),$@)