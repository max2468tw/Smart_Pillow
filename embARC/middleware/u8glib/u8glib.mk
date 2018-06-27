# dir declaration
U8GLIB_DIR = $(EMBARC_ROOT)/middleware/u8glib

U8GLIB_ASMSRCDIR=
U8GLIB_CSRCDIR	= $(U8GLIB_DIR)/fntsrc $(U8GLIB_DIR)/csrc
U8GLIB_INCDIR	= $(U8GLIB_DIR)/csrc


MID_INCDIR += $(U8GLIB_INCDIR)
MID_CSRCDIR += $(U8GLIB_CSRCDIR)
MID_ASMSRCDIR += $(U8GLIB_ASMSRCDIR)

# the dir to generate objs
U8GLIB_OBJDIR = $(OUT_DIR)/middleware/u8glib

# find all the srcs in the target dirs
U8GLIB_CSRCS = $(call get_csrcs, $(U8GLIB_CSRCDIR))
U8GLIB_ASMSRCS = $(call get_asmsrcs, $(U8GLIB_ASMSRCDIR))

# get obj files
U8GLIB_COBJS = $(call get_objs, $(U8GLIB_CSRCS), $(U8GLIB_OBJDIR))
U8GLIB_ASMOBJS = $(call get_objs, $(U8GLIB_ASMSRCS), $(U8GLIB_OBJDIR))

# All obj files
U8GLIB_OBJS = $(U8GLIB_COBJS) $(U8GLIB_ASMOBJS)

# Divide all object files into 10 part
U8GLIB_OBJ_CNT = $(words $(U8GLIB_OBJS))
U8GLIB_OBJS_PART1  = $(wordlist   1,  50, $(U8GLIB_OBJS))
U8GLIB_OBJS_PART2  = $(wordlist  51, 100, $(U8GLIB_OBJS))
U8GLIB_OBJS_PART3  = $(wordlist 101, 150, $(U8GLIB_OBJS))
U8GLIB_OBJS_PART4  = $(wordlist 151, 200, $(U8GLIB_OBJS))
U8GLIB_OBJS_PART5  = $(wordlist 201, 250, $(U8GLIB_OBJS))
U8GLIB_OBJS_PART6  = $(wordlist 251, 300, $(U8GLIB_OBJS))
U8GLIB_OBJS_PART7  = $(wordlist 301, 350, $(U8GLIB_OBJS))
U8GLIB_OBJS_PART8  = $(wordlist 351, 400, $(U8GLIB_OBJS))
U8GLIB_OBJS_PART9  = $(wordlist 401, 450, $(U8GLIB_OBJS))
U8GLIB_OBJS_PART10 = $(wordlist 451, $(U8GLIB_OBJ_CNT), $(U8GLIB_OBJS))

# include dependency files
ifneq ($(MAKECMDGOALS),clean)
 -include $(U8GLIB_COBJS:.o=.o.d) $(U8GLIB_ASMOBJS:.o=.o.d)
endif

# genearte library
MID_LIB_U8GLIB = $(OUT_DIR)/libmidu8glib.a
MID_LIBS += $(MID_LIB_U8GLIB)

## A strange way to avoid limit argument length of windows CreateProcess function
## http://msdn.microsoft.com/en-us/library/windows/desktop/ms682425
$(MID_LIB_U8GLIB): $(U8GLIB_OBJDIR) $(U8GLIB_COBJS) $(U8GLIB_ASMOBJS)
	$(TRACE_ARCHIVE)
ifneq ($(strip $(U8GLIB_OBJS_PART10)),)
	$(Q)$(AR) $(AR_OPT) $@ $(U8GLIB_OBJS_PART10)
endif
ifneq ($(strip $(U8GLIB_OBJS_PART9)),)
	$(Q)$(AR) $(AR_OPT) $@ $(U8GLIB_OBJS_PART9)
endif
ifneq ($(strip $(U8GLIB_OBJS_PART8)),)
	$(Q)$(AR) $(AR_OPT) $@ $(U8GLIB_OBJS_PART8)
endif
ifneq ($(strip $(U8GLIB_OBJS_PART7)),)
	$(Q)$(AR) $(AR_OPT) $@ $(U8GLIB_OBJS_PART7)
endif
ifneq ($(strip $(U8GLIB_OBJS_PART6)),)
	$(Q)$(AR) $(AR_OPT) $@ $(U8GLIB_OBJS_PART6)
endif
ifneq ($(strip $(U8GLIB_OBJS_PART5)),)
	$(Q)$(AR) $(AR_OPT) $@ $(U8GLIB_OBJS_PART5)
endif
ifneq ($(strip $(U8GLIB_OBJS_PART4)),)
	$(Q)$(AR) $(AR_OPT) $@ $(U8GLIB_OBJS_PART4)
endif
ifneq ($(strip $(U8GLIB_OBJS_PART3)),)
	$(Q)$(AR) $(AR_OPT) $@ $(U8GLIB_OBJS_PART3)
endif
ifneq ($(strip $(U8GLIB_OBJS_PART2)),)
	$(Q)$(AR) $(AR_OPT) $@ $(U8GLIB_OBJS_PART2)
endif
ifneq ($(strip $(U8GLIB_OBJS_PART1)),)
	$(Q)$(AR) $(AR_OPT) $@ $(U8GLIB_OBJS_PART1)
endif

# specific compile rules
.SECONDEXPANSION:
$(U8GLIB_COBJS): $(U8GLIB_OBJDIR)/%.o :$(call get_c_prerequisite, $(U8GLIB_CSRCDIR)) $$(COMMON_COMPILE_PREREQUISITES)
	$(TRACE_COMPILE)
	$(Q)$(CC) -c $(COMPILE_OPT) $(U8GLIB_COMPILE_OPT) $< -o $@

.SECONDEXPANSION:
$(U8GLIB_ASMOBJS): $(U8GLIB_OBJDIR)/%.o :$(call get_asm_prerequisite, $(U8GLIB_ASMSRCDIR)) $$(COMMON_COMPILE_PREREQUISITES)
	$(TRACE_ASSEMBLE)
	$(Q)$(CC) -c $(ASM_OPT) $(U8GLIB_COMPILE_OPT) $< -o $@

# generate obj dir
$(U8GLIB_OBJDIR):
	$(TRACE_CREATE_DIR)
	$(Q)$(MKD) $(subst /,$(PS),$@)

# Middleware Definitions
U8GLIB_DEFINES = -DMID_U8GLIB
MID_DEFINES += $(U8GLIB_DEFINES)
