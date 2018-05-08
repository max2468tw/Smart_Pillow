##
# \defgroup 	MK_OS_OS_HAL		FreeRTOS Related Makefile Configurations
# \ingroup		MK_OS
# \brief		makefile related to os_hal os configurations
# @{
##

##
# \brief 		current os directory definition
##
OS_HAL_DIR = $(EMBARC_ROOT)/os/hal


##
# \brief 		os_hal os related source and header
##
OS_HAL_CSRCDIR 		= 	$(OS_HAL_DIR)

OS_HAL_ASMSRCDIR 	= 	$(OS_HAL_DIR)

OS_HAL_INCDIR 		= 	$(OS_HAL_DIR)/inc

OS_HAL_OBJDIR	= $(OUT_DIR)/os_hal

OS_HAL_CSRCS = $(call get_csrcs, $(OS_HAL_CSRCDIR))
OS_HAL_ASMSRCS = $(call get_asmsrcs, $(OS_HAL_ASMSRCDIR))

OS_HAL_COBJS = $(call get_objs, $(OS_HAL_CSRCS), $(OS_HAL_OBJDIR))
OS_HAL_ASMOBJS = $(call get_objs, $(OS_HAL_ASMSRCS), $(OS_HAL_OBJDIR))

# include dependency files
ifneq ($(MAKECMDGOALS),clean)
 -include $(OS_HAL_COBJS:.o=.o.d) $(OS_HAL_ASMOBJS:.o=.o.d)
endif

OSHAL_LIB = $(OUT_DIR)/libos_hal.a

$(OSHAL_LIB): $(OS_HAL_OBJDIR) $(OS_HAL_COBJS) $(OS_HAL_ASMOBJS)
	@$(ECHO) "Generating $@"
	$(AR) $(AR_OPT) $@ $(OS_HAL_COBJS) $(OS_HAL_ASMOBJS)

$(OS_HAL_OBJDIR):
	$(MKD)	$(subst /,$(PS),$@)

.SECONDEXPANSION:
$(OS_HAL_COBJS): $(OS_HAL_OBJDIR)/%.o :$(call get_c_prerequisite, $(OS_HAL_CSRCDIR)) $$(COMMON_COMPILE_PREREQUISITES)
	@$(ECHO) "Compiling OS HAL Layer : $<"
	$(Q)$(CC) -c $(COMPILE_OPT) $(OS_HAL_COMPILE_OPT) $< -o $@

.SECONDEXPANSION:
$(OS_HAL_ASMOBJS): $(OS_HAL_OBJDIR)/%.o :$(call get_asm_prerequisite, $(OS_HAL_ASMSRCDIR)) $$(COMMON_COMPILE_PREREQUISITES)
	@$(ECHO) "Assembling OS HAL Layer : $<"
	$(Q)$(CC) -c $(ASM_OPT) $(OS_HAL_COMPILE_OPT) $< -o $@
