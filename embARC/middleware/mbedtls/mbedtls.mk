# dir declaration
MID_MBEDTLS_DIR = $(EMBARC_ROOT)/middleware/mbedtls

MID_MBEDTLS_CSRCDIR	= $(MID_MBEDTLS_DIR)/library $(MID_MBEDTLS_DIR)/embARC
MID_MBEDTLS_INCDIR	= $(MID_MBEDTLS_DIR)/include $(MID_MBEDTLS_DIR)/embARC

MID_INCDIR += $(MID_MBEDTLS_INCDIR)
MID_CSRCDIR += $(MID_MBEDTLS_CSRCDIR)

# the dir to generate objs
MID_MBEDTLS_OBJDIR = $(OUT_DIR)/middleware/mbedtls

# find all the srcs in the target dirs
MID_MBEDTLS_CSRCS = $(call get_csrcs, $(MID_MBEDTLS_CSRCDIR))
MID_MBEDTLS_ASMSRCS = $(call get_asmsrcs, $(MID_MBEDTLS_ASMSRCDIR))

# get obj files
MID_MBEDTLS_COBJS = $(call get_objs, $(MID_MBEDTLS_CSRCS), $(MID_MBEDTLS_OBJDIR))

# include dependency files
ifneq ($(MAKECMDGOALS),clean)
 -include $(MID_MBEDTLS_COBJS:.o=.o.d) $(MID_MBEDTLS_ASMOBJS:.o=.o.d)
endif

# genearte library
MID_LIB_MBEDTLS = $(OUT_DIR)/libmidmbedtls.a
MID_LIBS += $(MID_LIB_MBEDTLS)

$(MID_LIB_MBEDTLS): $(MID_MBEDTLS_OBJDIR) $(MID_MBEDTLS_COBJS)
	$(TRACE_ARCHIVE)
	$(Q)$(AR) $(AR_OPT) $@ $(MID_MBEDTLS_COBJS)

# specific compile rules
.SECONDEXPANSION:
$(MID_MBEDTLS_COBJS): $(MID_MBEDTLS_OBJDIR)/%.o :$(call get_c_prerequisite, $(MID_MBEDTLS_CSRCDIR)) $$(COMMON_COMPILE_PREREQUISITES)
	$(TRACE_COMPILE)
	$(Q)$(CC) -c $(COMPILE_OPT) $(MID_MBEDTLS_COMPILE_OPT) $< -o $@

# generate obj dir
$(MID_MBEDTLS_OBJDIR):
	$(TRACE_CREATE_DIR)
	$(Q)$(MKD) $(subst /,$(PS),$@)

# Middleware Definitions
MID_MBEDTLS_DEFINES = -DMID_MBEDTLS
MID_DEFINES += $(MID_MBEDTLS_DEFINES)
