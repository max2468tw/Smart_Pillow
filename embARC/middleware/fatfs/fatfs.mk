# dir declaration
MID_FATFS_DIR = $(EMBARC_ROOT)/middleware/fatfs/src

MID_FATFS_ASMSRCDIR	= $(MID_FATFS_DIR)
MID_FATFS_CSRCDIR	= $(MID_FATFS_DIR) $(MID_FATFS_DIR)/diskdrv
MID_FATFS_INCDIR	= $(MID_FATFS_DIR) $(MID_FATFS_DIR)/diskdrv

MID_INCDIR += $(MID_FATFS_INCDIR)
MID_CSRCDIR += $(MID_FATFS_CSRCDIR)
MID_ASMSRCDIR += $(MID_FATFS_ASMSRCDIR)

# the dir to generate objs
MID_FATFS_OBJDIR = $(OUT_DIR)/middleware/fatfs

# find all the srcs in the target dirs
MID_FATFS_CSRCS = $(call get_csrcs, $(MID_FATFS_CSRCDIR))
MID_FATFS_ASMSRCS = $(call get_asmsrcs, $(MID_FATFS_ASMSRCDIR))

# get obj files
MID_FATFS_COBJS = $(call get_objs, $(MID_FATFS_CSRCS), $(MID_FATFS_OBJDIR))
MID_FATFS_ASMOBJS = $(call get_objs, $(MID_FATFS_ASMSRCS), $(MID_FATFS_OBJDIR))

# include dependency files
ifneq ($(MAKECMDGOALS),clean)
 -include $(MID_FATFS_COBJS:.o=.o.d) $(MID_FATFS_ASMOBJS:.o=.o.d)
endif

# genearte library
MID_LIB_FATFS = $(OUT_DIR)/libmidfatfs.a
MID_LIBS += $(MID_LIB_FATFS)

$(MID_LIB_FATFS): $(MID_FATFS_OBJDIR) $(MID_FATFS_COBJS) $(MID_FATFS_ASMOBJS)
	$(TRACE_ARCHIVE)
	$(Q)$(AR) $(AR_OPT) $@ $(MID_FATFS_COBJS) $(MID_FATFS_ASMOBJS)

# specific compile rules
.SECONDEXPANSION:
$(MID_FATFS_COBJS): $(MID_FATFS_OBJDIR)/%.o :$(call get_c_prerequisite, $(MID_FATFS_CSRCDIR)) $$(COMMON_COMPILE_PREREQUISITES)
	$(TRACE_COMPILE)
	$(Q)$(CC) -c $(COMPILE_OPT) $(MID_FATFS_COMPILE_OPT) $< -o $@

.SECONDEXPANSION:
$(MID_FATFS_ASMOBJS): $(MID_FATFS_OBJDIR)/%.o :$(call get_asm_prerequisite, $(MID_FATFS_ASMSRCDIR)) $$(COMMON_COMPILE_PREREQUISITES)
	$(TRACE_ASSEMBLE)
	$(Q)$(CC) -c $(ASM_OPT) $(MID_FATFS_COMPILE_OPT) $< -o $@

# generate obj dir
$(MID_FATFS_OBJDIR):
	$(TRACE_CREATE_DIR)
	$(Q)$(MKD) $(subst /,$(PS),$@)

# Middleware Definitions
MID_FATFS_DEFINES = -DMID_FATFS
MID_DEFINES += $(MID_FATFS_DEFINES)