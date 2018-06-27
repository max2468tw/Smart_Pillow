# dir declaration
MID_LWIP_CONTRIB_DIR		= $(EMBARC_ROOT)/middleware/lwip-contrib
MID_LWIP_CONTRIB_APP_DIR	= $(MID_LWIP_CONTRIB_DIR)/apps

MID_LWIP_CONTRIB_ASMSRCDIR	=

MID_LWIP_CONTRIB_CSRCDIR	= $(MID_LWIP_CONTRIB_DIR)/ports/arcem \
					$(MID_LWIP_CONTRIB_DIR)/ports/arcem/netif

MID_LWIP_CONTRIB_INCDIR		= $(MID_LWIP_CONTRIB_DIR)/ports/arcem/include \
					$(MID_LWIP_CONTRIB_DIR)/ports/arcem

ifdef OS_SEL
### OS SELECTED ###
ifeq ($(OS_SEL), freertos)
MID_LWIP_CONTRIB_CSRCDIR	+= $(MID_LWIP_CONTRIB_DIR)/ports/arcem/sys_arch/freertos
else #end of freertos#
ifeq ($(OS_SEL), contiki)
MID_LWIP_CONTRIB_CSRCDIR	+= $(MID_LWIP_CONTRIB_DIR)/ports/arcem/sys_arch/contiki
else #not supported os, use baremetal supported #
MID_LWIP_CONTRIB_CSRCDIR	+= $(MID_LWIP_CONTRIB_DIR)/ports/arcem/sys_arch/baremetal
endif #end of contiki
endif #end of freertos
### OS SELECTED ###
else  # NO OS #
MID_LWIP_CONTRIB_CSRCDIR	+= $(MID_LWIP_CONTRIB_DIR)/ports/arcem/sys_arch/baremetal
endif

##
# select what apps to be used
##
LWIP_CONTRIB_APPS ?=

MID_LWIP_CONTRIB_APP_CSRCDIR = $(wildcard $(addprefix $(MID_LWIP_CONTRIB_APP_DIR)/, $(LWIP_CONTRIB_APPS)))
MID_LWIP_CONTRIB_APP_INCDIR  = $(wildcard $(addprefix $(MID_LWIP_CONTRIB_APP_DIR)/, $(LWIP_CONTRIB_APPS)))

MID_LWIP_CONTRIB_CSRCDIR	+= $(MID_LWIP_CONTRIB_APP_CSRCDIR)
MID_LWIP_CONTRIB_INCDIR		+= $(MID_LWIP_CONTRIB_APP_INCDIR)

MID_INCDIR += $(MID_LWIP_CONTRIB_INCDIR)
MID_CSRCDIR += $(MID_LWIP_CONTRIB_CSRCDIR)
MID_ASMSRCDIR += $(MID_LWIP_CONTRIB_ASMSRCDIR)

# the dir to generate objs
MID_LWIP_CONTRIB_OBJDIR	= $(OUT_DIR)/middleware/lwip-contrib

# find all the srcs in the target dirs
MID_LWIP_CONTRIB_CSRCS = $(call get_csrcs, $(MID_LWIP_CONTRIB_CSRCDIR))
MID_LWIP_CONTRIB_ASMSRCS = $(call get_asmsrcs, $(MID_LWIP_CONTRIB_ASMSRCDIR))

# get obj files
MID_LWIP_CONTRIB_COBJS = $(call get_objs, $(MID_LWIP_CONTRIB_CSRCS), $(MID_LWIP_CONTRIB_OBJDIR))
MID_LWIP_CONTRIB_ASMOBJS = $(call get_objs, $(MID_LWIP_CONTRIB_ASMSRCS), $(MID_LWIP_CONTRIB_OBJDIR))

# include dependency files
ifneq ($(MAKECMDGOALS),clean)
 -include $(MID_LWIP_CONTRIB_COBJS:.o=.o.d) $(MID_LWIP_CONTRIB_ASMOBJS:.o=.o.d)
endif

# genearte library
MID_LIB_LWIP_CONTRIB = $(OUT_DIR)/libmidlwip-contrib.a
MID_LIBS += $(MID_LIB_LWIP_CONTRIB)

$(MID_LIB_LWIP_CONTRIB): $(MID_LWIP_CONTRIB_OBJDIR) $(MID_LWIP_CONTRIB_COBJS) $(MID_LWIP_CONTRIB_ASMOBJS)
	$(TRACE_ARCHIVE)
	$(Q)$(AR) $(AR_OPT) $@ $(MID_LWIP_CONTRIB_COBJS) $(MID_LWIP_CONTRIB_ASMOBJS)

# specific compile rules
.SECONDEXPANSION:
$(MID_LWIP_CONTRIB_COBJS): $(MID_LWIP_CONTRIB_OBJDIR)/%.o :$(call get_c_prerequisite, $(MID_LWIP_CONTRIB_CSRCDIR)) $$(COMMON_COMPILE_PREREQUISITES)
	$(TRACE_COMPILE)
	$(Q)$(CC) -c $(COMPILE_OPT) $(MID_LWIP_CONTRIB_COMPILE_OPT) $< -o $@

.SECONDEXPANSION:
$(MID_LWIP_CONTRIB_ASMOBJS): $(MID_LWIP_CONTRIB_OBJDIR)/%.o :$(call get_asm_prerequisite, $(MID_LWIP_CONTRIB_ASMSRCDIR)) $$(COMMON_COMPILE_PREREQUISITES)
	$(TRACE_ASSEMBLE)
	$(Q)$(CC) -c $(ASM_OPT) $(MID_LWIP_CONTRIB_COMPILE_OPT) $< -o $@

# generate obj dir
$(MID_LWIP_CONTRIB_OBJDIR):
	$(TRACE_CREATE_DIR)
	$(Q)$(MKD) $(subst /,$(PS),$@)

# Middleware Definitions
MID_LWIP_CONTRIB_DEFINES = -DMID_LWIP_CONTRIB
MID_DEFINES += $(MID_LWIP_CONTRIB_DEFINES)