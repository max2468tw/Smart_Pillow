# dir declaration
MID_COMMON_DIR = $(EMBARC_ROOT)/middleware/common

MID_COMMON_ASMSRCDIR	= $(MID_COMMON_DIR) \
				$(MID_COMMON_DIR)/ringbuffer $(MID_COMMON_DIR)/xprintf
MID_COMMON_CSRCDIR	= $(MID_COMMON_DIR) \
				$(MID_COMMON_DIR)/ringbuffer $(MID_COMMON_DIR)/xprintf
MID_COMMON_INCDIR	= $(MID_COMMON_DIR) \
				$(MID_COMMON_DIR)/ringbuffer $(MID_COMMON_DIR)/xprintf


MID_INCDIR += $(MID_COMMON_INCDIR)
MID_CSRCDIR += $(MID_COMMON_CSRCDIR)
MID_ASMSRCDIR += $(MID_COMMON_ASMSRCDIR)

# the dir to generate objs
MID_COMMON_OBJDIR = $(OUT_DIR)/middleware/common

# find all the srcs in the target dirs
MID_COMMON_CSRCS = $(call get_csrcs, $(MID_COMMON_CSRCDIR))
MID_COMMON_ASMSRCS = $(call get_asmsrcs, $(MID_COMMON_ASMSRCDIR))

# get obj files
MID_COMMON_COBJS = $(call get_objs, $(MID_COMMON_CSRCS), $(MID_COMMON_OBJDIR))
MID_COMMON_ASMOBJS = $(call get_objs, $(MID_COMMON_ASMSRCS), $(MID_COMMON_OBJDIR))

# include dependency files
ifneq ($(MAKECMDGOALS),clean)
 -include $(MID_COMMON_COBJS:.o=.o.d) $(MID_COMMON_ASMOBJS:.o=.o.d)
endif

# genearte library
MID_LIB_COMMON = $(OUT_DIR)/libmidcommon.a
MID_LIBS += $(MID_LIB_COMMON)

$(MID_LIB_COMMON): $(MID_COMMON_OBJDIR) $(MID_COMMON_COBJS) $(MID_COMMON_ASMOBJS)
	$(TRACE_ARCHIVE)
	$(Q)$(AR) $(AR_OPT) $@ $(MID_COMMON_COBJS) $(MID_COMMON_ASMOBJS)

# specific compile rules
.SECONDEXPANSION:
$(MID_COMMON_COBJS): $(MID_COMMON_OBJDIR)/%.o :$(call get_c_prerequisite, $(MID_COMMON_CSRCDIR)) $$(COMMON_COMPILE_PREREQUISITES)
	$(TRACE_COMPILE)
	$(Q)$(CC) -c $(COMPILE_OPT) $(MID_COMMON_COMPILE_OPT) $< -o $@

.SECONDEXPANSION:
$(MID_COMMON_ASMOBJS): $(MID_COMMON_OBJDIR)/%.o :$(call get_asm_prerequisite, $(MID_COMMON_ASMSRCDIR)) $$(COMMON_COMPILE_PREREQUISITES)
	$(TRACE_ASSEMBLE)
	$(Q)$(CC) -c $(ASM_OPT) $(MID_COMMON_COMPILE_OPT) $< -o $@

# generate obj dir
$(MID_COMMON_OBJDIR):
	$(TRACE_CREATE_DIR)
	$(Q)$(MKD) $(subst /,$(PS),$@)

# Middleware Definitions
MID_COMMON_DEFINES = -DMID_COMMON
MID_DEFINES += $(MID_COMMON_DEFINES)