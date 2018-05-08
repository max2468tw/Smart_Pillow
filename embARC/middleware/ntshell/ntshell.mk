# dir declaration
MID_NTSHELL_DIR = $(EMBARC_ROOT)/middleware/ntshell

MID_NTSHELL_ASMSRCDIR	= $(MID_NTSHELL_DIR)/src \
				$(MID_NTSHELL_DIR)/cmds \
				$(MID_NTSHELL_DIR)/cmds/cmds_arc \
				$(MID_NTSHELL_DIR)/cmds/cmds_fs \
				$(MID_NTSHELL_DIR)/cmds/cmds_peripheral \
				$(MID_NTSHELL_DIR)/cmds/cmds_extend \
				$(MID_NTSHELL_DIR)/cmds/cmds_fs/ymodem \
				$(MID_NTSHELL_DIR)/port
MID_NTSHELL_CSRCDIR	= $(MID_NTSHELL_DIR)/src \
				$(MID_NTSHELL_DIR)/cmds \
				$(MID_NTSHELL_DIR)/cmds/cmds_arc \
				$(MID_NTSHELL_DIR)/cmds/cmds_fs \
				$(MID_NTSHELL_DIR)/cmds/cmds_peripheral \
				$(MID_NTSHELL_DIR)/cmds/cmds_extend \
				$(MID_NTSHELL_DIR)/cmds/cmds_fs/ymodem \
				$(MID_NTSHELL_DIR)/port
MID_NTSHELL_INCDIR	= $(MID_NTSHELL_DIR)/src \
				$(MID_NTSHELL_DIR)/cmds \
				$(MID_NTSHELL_DIR)/cmds/cmds_arc \
				$(MID_NTSHELL_DIR)/cmds/cmds_fs \
				$(MID_NTSHELL_DIR)/cmds/cmds_peripheral \
				$(MID_NTSHELL_DIR)/cmds/cmds_extend \
				$(MID_NTSHELL_DIR)/cmds/cmds_fs/ymodem \
				$(MID_NTSHELL_DIR)/port

MID_INCDIR += $(MID_NTSHELL_INCDIR)
MID_CSRCDIR += $(MID_NTSHELL_CSRCDIR)
MID_ASMSRCDIR += $(MID_NTSHELL_ASMSRCDIR)

# the dir to generate objs
MID_NTSHELL_OBJDIR = $(OUT_DIR)/middleware/ntshell

# find all the srcs in the target dirs
MID_NTSHELL_CSRCS = $(call get_csrcs, $(MID_NTSHELL_CSRCDIR))
MID_NTSHELL_ASMSRCS = $(call get_asmsrcs, $(MID_NTSHELL_ASMSRCDIR))

MID_NTSHELL_COBJS = $(call get_objs, $(MID_NTSHELL_CSRCS), $(MID_NTSHELL_OBJDIR))
MID_NTSHELL_ASMOBJS = $(call get_objs, $(MID_NTSHELL_ASMSRCS), $(MID_NTSHELL_OBJDIR))

# include dependency files
ifneq ($(MAKECMDGOALS),clean)
 -include $(MID_NTSHELL_COBJS:.o=.o.d) $(MID_NTSHELL_ASMOBJS:.o=.o.d)
endif

# genearte library
MID_LIB_NTSHELL = $(OUT_DIR)/libmidntshell.a
MID_LIBS += $(MID_LIB_NTSHELL)

$(MID_LIB_NTSHELL): $(MID_NTSHELL_OBJDIR) $(MID_NTSHELL_COBJS) $(MID_NTSHELL_ASMOBJS)
	$(TRACE_ARCHIVE)
	$(Q)$(AR) $(AR_OPT) $@ $(MID_NTSHELL_COBJS) $(MID_NTSHELL_ASMOBJS)

# specific compile rules
SECONDEXPANSION:
$(MID_NTSHELL_COBJS): $(MID_NTSHELL_OBJDIR)/%.o :$(call get_c_prerequisite, $(MID_NTSHELL_CSRCDIR)) $$(COMMON_COMPILE_PREREQUISITES)
	$(TRACE_COMPILE)
	$(Q)$(CC) -c $(COMPILE_OPT) $(MID_NTSHELL_COMPILE_OPT) $< -o $@

SECONDEXPANSION:
$(MID_NTSHELL_ASMOBJS): $(MID_NTSHELL_OBJDIR)/%.o :$(call get_asm_prerequisite, $(MID_NTSHELL_ASMSRCDIR)) $$(COMMON_COMPILE_PREREQUISITES)
	$(TRACE_ASSEMBLE)
	$(Q)$(CC) -c $(ASM_OPT) $(MID_NTSHELL_COMPILE_OPT) $< -o $@

# generate obj dir
$(MID_NTSHELL_OBJDIR):
	$(TRACE_CREATE_DIR)
	$(Q)$(MKD) $(subst /,$(PS),$@)

# Middleware Definitions
MID_NTSHELL_DEFINES = -DMID_NTSHELL
MID_DEFINES += $(MID_NTSHELL_DEFINES)