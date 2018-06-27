# dir declaration
MID_LWIP_DIR = $(EMBARC_ROOT)/middleware/lwip

MID_LWIP_ASMSRCDIR	=
MID_LWIP_CSRCDIR	= $(MID_LWIP_DIR)/src/core \
				$(MID_LWIP_DIR)/src/core/ipv4 \
				$(MID_LWIP_DIR)/src/core/ipv6 \
				$(MID_LWIP_DIR)/src/core/snmp \
				$(MID_LWIP_DIR)/src/netif \
				$(MID_LWIP_DIR)/src/netif/ppp \
				$(MID_LWIP_DIR)/src/netif/polarssl \
				$(MID_LWIP_DIR)/src/api

MID_LWIP_INCDIR		= $(MID_LWIP_DIR)/src/include \
				$(MID_LWIP_DIR)/src/include/posix

MID_INCDIR += $(MID_LWIP_INCDIR)
MID_CSRCDIR += $(MID_LWIP_CSRCDIR)
MID_ASMSRCDIR += $(MID_LWIP_ASMSRCDIR)

# the dir to generate objs
MID_LWIP_OBJDIR	= $(OUT_DIR)/middleware/lwip

# find all the srcs in the target dirs
MID_LWIP_CSRCS = $(call get_csrcs, $(MID_LWIP_CSRCDIR))
MID_LWIP_ASMSRCS = $(call get_asmsrcs, $(MID_LWIP_ASMSRCDIR))

# get obj files
MID_LWIP_COBJS = $(call get_objs, $(MID_LWIP_CSRCS), $(MID_LWIP_OBJDIR))
MID_LWIP_ASMOBJS = $(call get_objs, $(MID_LWIP_ASMSRCS), $(MID_LWIP_OBJDIR))

# include dependency files
ifneq ($(MAKECMDGOALS),clean)
 -include $(MID_LWIP_COBJS:.o=.o.d) $(MID_LWIP_ASMOBJS:.o=.o.d)
endif

# genearte library
MID_LIB_LWIP = $(OUT_DIR)/libmidlwip.a
MID_LIBS += $(MID_LIB_LWIP)

$(MID_LIB_LWIP): $(MID_LWIP_OBJDIR) $(MID_LWIP_COBJS) $(MID_LWIP_ASMOBJS)
	$(TRACE_ARCHIVE)
	$(Q)$(AR) $(AR_OPT) $@ $(MID_LWIP_COBJS) $(MID_LWIP_ASMOBJS)

# specific compile rules
.SECONDEXPANSION:
$(MID_LWIP_COBJS): $(MID_LWIP_OBJDIR)/%.o : $(call get_c_prerequisite, $(MID_LWIP_CSRCDIR)) $$(COMMON_COMPILE_PREREQUISITES)
	$(TRACE_COMPILE)
	$(Q)$(CC) -c $(COMPILE_OPT) $(MID_LWIP_COMPILE_OPT) $< -o $@

.SECONDEXPANSION:
$(MID_LWIP_ASMOBJS): $(MID_LWIP_OBJDIR)/%.o : $(call get_asm_prerequisite, $(MID_LWIP_ASMSRCDIR)) $$(COMMON_COMPILE_PREREQUISITES)
	$(TRACE_ASSEMBLE)
	$(Q)$(CC) -c $(ASM_OPT) $(MID_LWIP_COMPILE_OPT) $< -o $@

# generate obj dir
$(MID_LWIP_OBJDIR):
	$(TRACE_CREATE_DIR)
	$(Q)$(MKD) $(subst /,$(PS),$@)

# Middleware Definitions
MID_LWIP_DEFINES = -DMID_LWIP
MID_DEFINES += $(MID_LWIP_DEFINES)