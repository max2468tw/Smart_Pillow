##
# \defgroup	MK_OS_FREERTOS	FreeRTOS Related Makefile Configurations
# \ingroup	MK_OS
# \brief	makefile related to freertos os configurations
##

##
# \brief	current os directory definition
##
FREERTOS_OS_DIR = $(EMBARC_ROOT)/os/freertos
FREERTOS_HEAP_SEL = 2

##
# \brief 		freertos port sources and includes definition
##
FREERTOS_PORT_CSRCDIR	= $(FREERTOS_OS_DIR)/portable/Synopsys/ARC_EM \
				$(FREERTOS_OS_DIR)/portable
FREERTOS_PORT_ASMSRCDIR	= $(FREERTOS_OS_DIR)/portable/Synopsys/ARC_EM
FREERTOS_PORT_INCDIR	= $(FREERTOS_OS_DIR)/portable/Synopsys/ARC_EM

##
# \brief 		freertos os related source and header
##
FREERTOS_CSRCDIR	= $(FREERTOS_PORT_CSRCDIR) \
				$(FREERTOS_OS_DIR)
FREERTOS_ASMSRCDIR	= $(FREERTOS_PORT_ASMSRCDIR) \
				$(FREERTOS_OS_DIR)
FREERTOS_INCDIR		= $(FREERTOS_PORT_INCDIR) $(FREERTOS_OS_DIR)/include

FREERTOS_OBJDIR	= $(OUT_DIR)/freertos

FREERTOS_CSRCS = $(call get_csrcs, $(FREERTOS_CSRCDIR))
FREERTOS_ASMSRCS = $(call get_asmsrcs, $(FREERTOS_ASMSRCDIR))

FREERTOS_COBJS = $(call get_objs, $(FREERTOS_CSRCS), $(FREERTOS_OBJDIR))
FREERTOS_ASMOBJS = $(call get_objs, $(FREERTOS_ASMSRCS), $(FREERTOS_OBJDIR))

# include dependency files
ifneq ($(MAKECMDGOALS),clean)
 -include $(FREERTOS_COBJS:.o=.o.d) $(FREERTOS_ASMOBJS:.o=.o.d)
endif

OS_LIB = $(OUT_DIR)/libfreertos.a

$(OS_LIB): $(FREERTOS_OBJDIR) $(FREERTOS_COBJS) $(FREERTOS_ASMOBJS)
	$(TRACE_ARCHIVE)
	$(Q)$(AR) $(AR_OPT) $@ $(FREERTOS_COBJS) $(FREERTOS_ASMOBJS)

.SECONDEXPANSION:
$(FREERTOS_COBJS): $(FREERTOS_OBJDIR)/%.o :$(call get_c_prerequisite, $(FREERTOS_CSRCDIR)) $$(COMMON_COMPILE_PREREQUISITES)
	$(TRACE_COMPILE)
	$(Q)$(CC) -c $(COMPILE_OPT) $(FREERTOS_COMPILE_OPT) $< -o $@

.SECONDEXPANSION:
$(FREERTOS_ASMOBJS): $(FREERTOS_OBJDIR)/%.o :$(call get_asm_prerequisite, $(FREERTOS_ASMSRCDIR)) $$(COMMON_COMPILE_PREREQUISITES)
	$(TRACE_ASSEMBLE)
	$(Q)$(CC) -c $(ASM_OPT) $(FREERTOS_COMPILE_OPT) $< -o $@

$(FREERTOS_OBJDIR):
	$(TRACE_CREATE_DIR)
	@$(MKD) $(subst /,$(PS),$@)


FREERTOS_DEFINES = -DFREERTOS_HEAP_SEL=$(FREERTOS_HEAP_SEL) -DconfigNUM_THREAD_LOCAL_STORAGE_POINTERS=1 -DconfigUSE_RECURSIVE_MUTEXES=1

ifeq ($(DEBUG), 1)
	FREERTOS_DEFINES += -DINCLUDE_uxTaskGetStackHighWaterMark=1
	FREERTOS_DEFINES += -DconfigGENERATE_RUN_TIME_STATS=1
endif