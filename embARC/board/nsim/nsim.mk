##
# \defgroup 	MK_BOARD_NSIM	NSIM Related Makefile Configurations
# \ingroup	MK_BOARD
# \brief	makefile related to nsim configurations
##

##
# \brief	current board directory definition
##
NSIM_BOARD_DIR = $(EMBARC_ROOT)/board/nsim
##
# nsim version definition
# 1506 stands for 2015.06 release
# When you don't special BD_VER
# in any other place or through make options
# it will select a proper BD_VER according to your cur_core
##
SUPPORTED_BD_VERS = 1506

## Select Board Version
BD_VER ?= 1506

##
# Host Link option
# Unsupported Now
# Function Toggle
# 1 for enable
# other for disable
##
HOSTLINK = 1

##
# NSIM Test
# 1 for enable
# other for disable
##
NSIM ?= 1

override BD_VER := $(strip $(BD_VER))
override HOSTLINK := $(strip $(HOSTLINK))
override NSIM := $(strip $(NSIM))

## Set Valid Board Version
VAILD_BD_VER = $(call check_item_exist, $(BD_VER), $(SUPPORTED_BD_VERS))

## Try to include different board version makefiles
ifneq ($(VAILD_BD_VER),)
COMMON_COMPILE_PREREQUISITES += $(EMBARC_ROOT)/board/nsim/configs/$(VAILD_BD_VER)/nsim_$(VAILD_BD_VER).mk
include $(EMBARC_ROOT)/board/nsim/configs/$(VAILD_BD_VER)/nsim_$(VAILD_BD_VER).mk
else
$(info BOARD $(BOARD) Version - $(SUPPORTED_BD_VERS) are supported)
$(error $(BOARD) Version $(BD_VER) is not supported, please check it!)
endif

##
# \brief	nsim device driver related
##
NSIM_DEV_CSRCDIR	=

NSIM_DEV_ASMSRCDIR	=
NSIM_DEV_INCDIR		=

##
# \brief	nsim board related source and header
##
NSIM_CSRCDIR		+=	$(NSIM_DEV_CSRCDIR) \
				$(NSIM_BOARD_DIR)/common \
				$(NSIM_BOARD_DIR)/drivers/uart \
				$(NSIM_BOARD_DIR)/drivers/ntshell


NSIM_ASMSRCDIR	+= $(NSIM_DEV_ASMSRCDIR)
NSIM_INCDIR	+= $(NSIM_DEV_INCDIR)


BOARD_CSRCDIR	+= $(NSIM_CSRCDIR)
BOARD_ASMSRCDIR	+= $(NSIM_ASMSRCDIR)
BOARD_INCDIR	+= $(NSIM_INCDIR)

NSIM_DEFINES += -D_HOSTLINK_ -D_NSIM_
EXTRA_BOARD_DEFINES += $(CORE_DEFINES) $(NSIM_DEFINES)

# the dir to generate objs
NSIM_OBJDIR = $(OUT_DIR)/nsim

# find all the srcs in the target dirs
NSIM_CSRCS = $(call get_csrcs, $(NSIM_CSRCDIR))
NSIM_ASMSRCS = $(call get_asmsrcs, $(NSIM_ASMSRCDIR))

NSIM_COBJS = $(call get_objs, $(NSIM_CSRCS), $(NSIM_OBJDIR))
NSIM_ASMOBJS = $(call get_objs, $(NSIM_ASMSRCS), $(NSIM_OBJDIR))

# include dependency files
ifneq ($(MAKECMDGOALS),clean)
 -include $(NSIM_COBJS:.o=.o.d) $(NSIM_ASMOBJS:.o=.o.d)
endif

# genearte library

BOARD_LIB = $(OUT_DIR)/libnsim.a

NSIM_BOARD_DIR_FIXED=$(subst \,/,$(NSIM_BOARD_DIR))

$(BOARD_LIB): $(NSIM_OBJDIR) $(NSIM_COBJS) $(NSIM_ASMOBJS)
	$(TRACE_ARCHIVE)
	$(Q)$(AR) $(AR_OPT) $@ $(NSIM_COBJS) $(NSIM_ASMOBJS)

# specific compile rules
.SECONDEXPANSION:
$(NSIM_COBJS): $(NSIM_OBJDIR)/%.o :$(call get_c_prerequisite, $(NSIM_CSRCDIR)) $$(COMMON_COMPILE_PREREQUISITES)
	$(TRACE_COMPILE)
	$(Q)$(CC) -c $(COMPILE_OPT) $(NSIM_COMPILE_OPT) $< -o $@

$(NSIM_ASMOBJS): $(NSIM_OBJDIR)/%.o :$(call get_asm_prerequisite, $(NSIM_ASMSRCDIR)) $$(COMMON_COMPILE_PREREQUISITES)
	$(TRACE_ASSEMBLE)
	$(Q)$(CC) -c $(ASM_OPT) $(NSIM_COMPILE_OPT) $< -o $@

# generate obj dir
$(NSIM_OBJDIR):
	$(TRACE_CREATE_DIR)
	$(Q)$(MKD) $(subst /,$(PS),$@)