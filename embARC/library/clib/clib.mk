# dir declaration
LIB_CLIB_DIR = $(EMBARC_ROOT)/library/clib

LIB_CLIB_ASMSRCDIR	= $(LIB_CLIB_DIR)
LIB_CLIB_CSRCDIR	= $(LIB_CLIB_DIR)
LIB_CLIB_INCDIR		= $(LIB_CLIB_DIR)

LIB_INCDIR += $(LIB_CLIB_INCDIR)
LIB_CSRCDIR += $(LIB_CLIB_CSRCDIR)
LIB_ASMSRCDIR += $(LIB_CLIB_ASMSRCDIR)

LIB_CLIB_COMPILE_OPT = -DHAVE_MMAP=0 -DHAVE_MREMAP=0 -DMMAP_CLEARS=0 -DLACKS_SYS_PARAM_H

# the dir to generate objs
LIB_CLIB_OBJDIR = $(OUT_DIR)/library/clib

# find all the srcs in the target dirs
LIB_CLIB_CSRCS = $(call get_csrcs, $(LIB_CLIB_CSRCDIR))
LIB_CLIB_ASMSRCS = $(call get_asmsrcs, $(LIB_CLIB_ASMSRCDIR))

# get obj files
LIB_CLIB_COBJS = $(call get_objs, $(LIB_CLIB_CSRCS), $(LIB_CLIB_OBJDIR))
LIB_CLIB_ASMOBJS = $(call get_objs, $(LIB_CLIB_ASMSRCS), $(LIB_CLIB_OBJDIR))

# include dependency files
ifneq ($(MAKECMDGOALS),clean)
 -include $(LIB_CLIB_COBJS:.o=.o.d) $(LIB_CLIB_ASMOBJS:.o=.o.d)
endif

# generate library
LIB_LIB_CLIB = $(OUT_DIR)/liblibclib.a
LIB_LIBS += $(LIB_LIB_CLIB)

$(LIB_LIB_CLIB): $(LIB_CLIB_OBJDIR) $(LIB_CLIB_COBJS) $(LIB_CLIB_ASMOBJS)
	$(TRACE_ARCHIVE)
	$(Q)$(AR) $(AR_OPT) $@ $(LIB_CLIB_COBJS) $(LIB_CLIB_ASMOBJS)

# specific compile rules
.SECONDEXPANSION:
$(LIB_CLIB_COBJS): $(LIB_CLIB_OBJDIR)/%.o :$(call get_c_prerequisite, $(LIB_CLIB_CSRCDIR)) $$(COMMON_COMPILE_PREREQUISITES)
	$(TRACE_COMPILE)
	$(Q)$(CC) -c $(COMPILE_OPT) $(LIB_CLIB_COMPILE_OPT) $< -o $@

.SECONDEXPANSION:
$(LIB_CLIB_ASMOBJS): $(LIB_CLIB_OBJDIR)/%.o :$(call get_asm_prerequisite, $(LIB_CLIB_ASMSRCDIR)) $$(COMMON_COMPILE_PREREQUISITES)
	$(TRACE_ASSEMBLE)
	$(Q)$(CC) -c $(ASM_OPT) $(LIB_CLIB_COMPILE_OPT) $< -o $@

# generate obj dir
$(LIB_CLIB_OBJDIR):
	$(TRACE_CREATE_DIR)
	$(Q)$(MKD) $(subst /,$(PS),$@)

# Middleware Definitions
LIB_CLIB_DEFINES = -DLIB_CLIB
LIB_DEFINES += $(LIB_CLIB_DEFINES)
