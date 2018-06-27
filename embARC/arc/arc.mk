## CPU Related ##
# dir declaration
ARC_ASMSRCDIR	= $(EMBARC_ROOT)/arc
ARC_CSRCDIR	= $(EMBARC_ROOT)/arc
ARC_INCDIR	= $(EMBARC_ROOT)/arc

# the dir to generate objs
ARC_OBJDIR	= $(OUT_DIR)/arc

# find all the srcs in the target dirs
ARC_CSRCS = $(call get_csrcs, $(ARC_CSRCDIR))
ARC_ASMSRCS = $(call get_asmsrcs, $(ARC_ASMSRCDIR))

# get obj files
ARC_COBJS = $(call get_objs, $(ARC_CSRCS), $(ARC_OBJDIR))
ARC_ASMOBJS = $(call get_objs, $(ARC_ASMSRCS), $(ARC_OBJDIR))

# include dependency files
ifneq ($(MAKECMDGOALS),clean)
 -include $(ARC_COBJS:.o=.o.d) $(ARC_ASMOBJS:.o=.o.d)
endif

CPU_LIB = $(OUT_DIR)/libarc.a

# generate library
$(CPU_LIB): $(ARC_OBJDIR) $(ARC_COBJS) $(ARC_ASMOBJS)
	$(TRACE_ARCHIVE)
	$(Q)$(AR) $(AR_OPT) $@ $(ARC_COBJS) $(ARC_ASMOBJS)

# specific compile rules
.SECONDEXPANSION:
$(ARC_COBJS): $(ARC_OBJDIR)/%.o :$(call get_c_prerequisite, $(ARC_CSRCDIR)) $$(COMMON_COMPILE_PREREQUISITES)
	$(TRACE_COMPILE)
	$(Q)$(CC) -c $(COMPILE_OPT) $(ARC_COMPILE_OPT) $< -o $@

.SECONDEXPANSION:
$(ARC_ASMOBJS): $(ARC_OBJDIR)/%.o :$(call get_asm_prerequisite, $(ARC_ASMSRCDIR)) $$(COMMON_COMPILE_PREREQUISITES)
	$(TRACE_ASSEMBLE)
	$(Q)$(CC) -c $(ASM_OPT) $(ARC_COMPILE_OPT) $< -o $@

# generate obj dir
$(ARC_OBJDIR):
	$(TRACE_CREATE_DIR)
	$(Q)$(MKD) $(subst /,$(PS),$@)

# Cpu Definitions
CPU_DEFINES = -DCPU_ARCEM