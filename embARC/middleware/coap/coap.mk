# dir declaration
MID_COAP_DIR = $(EMBARC_ROOT)/middleware/coap

MID_COAP_ASMSRCDIR	= $(MID_COAP_DIR)
MID_COAP_CSRCDIR	= $(MID_COAP_DIR)
MID_COAP_INCDIR		= $(MID_COAP_DIR)

MID_INCDIR += $(MID_COAP_INCDIR)
MID_CSRCDIR += $(MID_COAP_CSRCDIR)
MID_ASMSRCDIR += $(MID_COAP_ASMSRCDIR)

# the dir to generate objs
MID_COAP_OBJDIR	= $(OUT_DIR)/middleware/coap

# find all the srcs in the target dirs
MID_COAP_CSRCS = $(call get_csrcs, $(MID_COAP_CSRCDIR))
MID_COAP_ASMSRCS = $(call get_asmsrcs, $(MID_COAP_ASMSRCDIR))

MID_COAP_COBJS = $(call get_objs, $(MID_COAP_CSRCS), $(MID_COAP_OBJDIR))
MID_COAP_ASMOBJS = $(call get_objs, $(MID_COAP_ASMSRCS), $(MID_COAP_OBJDIR))

# include dependency files
ifneq ($(MAKECMDGOALS),clean)
 -include $(MID_COAP_COBJS:.o=.o.d) $(MID_COAP_ASMOBJS:.o=.o.d)
endif

# genearte library
MID_LIB_COAP = $(OUT_DIR)/libmidcoap.a
MID_LIBS += $(MID_LIB_COAP)

$(MID_LIB_COAP): $(MID_COAP_OBJDIR) $(MID_COAP_COBJS) $(MID_COAP_ASMOBJS)
	$(TRACE_ARCHIVE)
	$(Q)$(AR) $(AR_OPT) $@ $(MID_COAP_COBJS) $(MID_COAP_ASMOBJS)

# specific compile rules
.SECONDEXPANSION:
$(MID_COAP_COBJS): $(MID_COAP_OBJDIR)/%.o :$(call get_c_prerequisite, $(MID_COAP_CSRCDIR)) $$(COMMON_COMPILE_PREREQUISITES)
	$(TRACE_COMPILE)
	$(Q)$(CC) -c $(COMPILE_OPT) $(MID_COAP_COMPILE_OPT) $< -o $@

.SECONDEXPANSION:
$(MID_COAP_ASMOBJS): $(MID_COAP_OBJDIR)/%.o :$(call get_asm_prerequisite, $(MID_COAP_ASMSRCDIR)) $$(COMMON_COMPILE_PREREQUISITES)
	$(TRACE_ASSEMBLE)
	$(Q)$(CC) -c $(ASM_OPT) $(MID_COAP_COMPILE_OPT) $< -o $@

# generate obj dir
$(MID_COAP_OBJDIR):
	$(TRACE_CREATE_DIR)
	$(Q)$(MKD) $(subst /,$(PS),$@)

# Middleware Definitions
MID_COAP_DEFINES = -DMID_COAP
MID_DEFINES += $(MID_COAP_DEFINES)
