# dir declaration
MID_PARSON_DIR = $(EMBARC_ROOT)/middleware/parson

MID_PARSON_ASMSRCDIR	=
MID_PARSON_CSRCDIR	= $(MID_PARSON_DIR)

MID_PARSON_INCDIR	= $(MID_PARSON_DIR)

MID_INCDIR += $(MID_PARSON_INCDIR)
MID_CSRCDIR += $(MID_PARSON_CSRCDIR)
MID_ASMSRCDIR += $(MID_PARSON_ASMSRCDIR)

# the dir to generate objs
MID_PARSON_OBJDIR	= $(OUT_DIR)/middleware/parson

# find all the srcs in the target dirs
MID_PARSON_CSRCS = $(call get_csrcs, $(MID_PARSON_CSRCDIR))
MID_PARSON_ASMSRCS = $(call get_asmsrcs, $(MID_PARSON_ASMSRCDIR))

# get obj files
MID_PARSON_COBJS = $(call get_objs, $(MID_PARSON_CSRCS), $(MID_PARSON_OBJDIR))
MID_PARSON_ASMOBJS = $(call get_objs, $(MID_PARSON_ASMSRCS), $(MID_PARSON_OBJDIR))

# include dependency files
ifneq ($(MAKECMDGOALS),clean)
 -include $(MID_PARSON_COBJS:.o=.o.d) $(MID_PARSON_ASMOBJS:.o=.o.d)
endif

# genearte library
MID_LIB_PARSON = $(OUT_DIR)/libmidparson.a
MID_LIBS += $(MID_LIB_PARSON)

$(MID_LIB_PARSON): $(MID_PARSON_OBJDIR) $(MID_PARSON_COBJS) $(MID_PARSON_ASMOBJS)
	$(TRACE_ARCHIVE)
	$(Q)$(AR) $(AR_OPT) $@ $(MID_PARSON_COBJS) $(MID_PARSON_ASMOBJS)

# specific compile rules
.SECONDEXPANSION:
$(MID_PARSON_COBJS): $(MID_PARSON_OBJDIR)/%.o : $(call get_c_prerequisite, $(MID_PARSON_CSRCDIR)) $$(COMMON_COMPILE_PREREQUISITES)
	$(TRACE_COMPILE)
	$(Q)$(CC) -c $(COMPILE_OPT) $(MID_PARSON_COMPILE_OPT) $< -o $@

.SECONDEXPANSION:
$(MID_PARSON_ASMOBJS): $(MID_PARSON_OBJDIR)/%.o : $(call get_asm_prerequisite, $(MID_PARSON_ASMSRCDIR)) $$(COMMON_COMPILE_PREREQUISITES)
	$(TRACE_ASSEMBLE)
	$(Q)$(CC) -c $(ASM_OPT) $(MID_PARSON_COMPILE_OPT) $< -o $@

# generate obj dir
$(MID_PARSON_OBJDIR):
	$(TRACE_CREATE_DIR)
	$(Q)$(MKD) $(subst /,$(PS),$@)

# Middleware Definitions
MID_PARSON_DEFINES = -DMID_PARSON
MID_DEFINES += $(MID_PARSON_DEFINES)