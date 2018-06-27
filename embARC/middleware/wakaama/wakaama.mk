# dir declaration
MID_WAKAAMA_DIR = $(EMBARC_ROOT)/middleware/wakaama

MID_WAKAAMA_ASMSRCDIR	=
MID_WAKAAMA_CSRCDIR	= $(MID_WAKAAMA_DIR)/core \
				$(MID_WAKAAMA_DIR)/core/er-coap-13 \
				$(MID_WAKAAMA_DIR)/port \
				$(MID_WAKAAMA_DIR)/port/conn/udp

MID_WAKAAMA_INCDIR	= $(MID_WAKAAMA_DIR)/core \
				$(MID_WAKAAMA_DIR)/core/er-coap-13 \
				$(MID_WAKAAMA_DIR)/port/conn/udp

MID_INCDIR += $(MID_WAKAAMA_INCDIR)
MID_CSRCDIR += $(MID_WAKAAMA_CSRCDIR)
MID_ASMSRCDIR += $(MID_WAKAAMA_ASMSRCDIR)

# the dir to generate objs
MID_WAKAAMA_OBJDIR	= $(OUT_DIR)/middleware/wakaama

# find all the srcs in the target dirs
MID_WAKAAMA_CSRCS = $(call get_csrcs, $(MID_WAKAAMA_CSRCDIR))
MID_WAKAAMA_ASMSRCS = $(call get_asmsrcs, $(MID_WAKAAMA_ASMSRCDIR))

# get obj files
MID_WAKAAMA_COBJS = $(call get_objs, $(MID_WAKAAMA_CSRCS), $(MID_WAKAAMA_OBJDIR))
MID_WAKAAMA_ASMOBJS = $(call get_objs, $(MID_WAKAAMA_ASMSRCS), $(MID_WAKAAMA_OBJDIR))

# include dependency files
ifneq ($(MAKECMDGOALS),clean)
 -include $(MID_WAKAAMA_COBJS:.o=.o.d) $(MID_WAKAAMA_ASMOBJS:.o=.o.d)
endif

# genearte library
MID_LIB_WAKAAMA = $(OUT_DIR)/libmidwakaama.a
MID_LIBS += $(MID_LIB_WAKAAMA)

$(MID_LIB_WAKAAMA): $(MID_WAKAAMA_OBJDIR) $(MID_WAKAAMA_COBJS) $(MID_WAKAAMA_ASMOBJS)
	$(TRACE_ARCHIVE)
	$(Q)$(AR) $(AR_OPT) $@ $(MID_WAKAAMA_COBJS) $(MID_WAKAAMA_ASMOBJS)

# specific compile rules
.SECONDEXPANSION:
$(MID_WAKAAMA_COBJS): $(MID_WAKAAMA_OBJDIR)/%.o : $(call get_c_prerequisite, $(MID_WAKAAMA_CSRCDIR)) $$(COMMON_COMPILE_PREREQUISITES)
	$(TRACE_COMPILE)
	$(Q)$(CC) -c $(COMPILE_OPT) $(MID_WAKAAMA_COMPILE_OPT) $< -o $@

.SECONDEXPANSION:
$(MID_WAKAAMA_ASMOBJS): $(MID_WAKAAMA_OBJDIR)/%.o : $(call get_asm_prerequisite, $(MID_WAKAAMA_ASMSRCDIR)) $$(COMMON_COMPILE_PREREQUISITES)
	$(TRACE_ASSEMBLE)
	$(Q)$(CC) -c $(ASM_OPT) $(MID_WAKAAMA_COMPILE_OPT) $< -o $@

# generate obj dir
$(MID_WAKAAMA_OBJDIR):
	$(TRACE_CREATE_DIR)
	$(Q)$(MKD) $(subst /,$(PS),$@)

# Middleware Definitions
MID_WAKAAMA_DEFINES = -DMID_WAKAAMA -DLWM2M_EMBEDDED_MODE -DLWM2M_LITTLE_ENDIAN
MID_DEFINES += $(MID_WAKAAMA_DEFINES)