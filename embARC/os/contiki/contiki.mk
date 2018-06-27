# -*- makefile -*-

ifndef CONTIKI
  ${error CONTIKI not defined! You must specify where Contiki resides}
endif


TARGET = $(BOARD)

# Include IPv6, IPv4, and/or Rime
ifeq ($(CONTIKI_WITH_IPV4),1)
  COMPILE_OPT += -DNETSTACK_CONF_WITH_IPV4=1
  MODULES += core/net/ipv4 core/net/ip
endif

ifeq ($(CONTIKI_WITH_RIME),1)
  COMPILE_OPT += -DNETSTACK_CONF_WITH_RIME=1
  MODULES += core/net/rime
endif

ifeq ($(CONTIKI_WITH_IPV6),1)
  COMPILE_OPT += -DNETSTACK_CONF_WITH_IPV6=1
  MODULES += core/net/ipv6 core/net/ip

ifeq ($(CONTIKI_WITH_IPV6_IP64), 1)
  COMPILE_OPT += -DUIP_CONF_IPV6_IP64=1
  MODULES += core/net/ip64
endif

endif

ifeq ($(CONTIKI_WITH_RPL),1)
    COMPILE_OPT += -DUIP_CONF_IPV6_RPL=1
    MODULES += core/net/rpl
else
    COMPILE_OPT += -DUIP_CONF_IPV6_RPL=0
endif

TARGET_UPPERCASE := ${call uc, $(TARGET)}

COMPILE_OPT += -DCONTIKI=1 -DCONTIKI_TARGET_$(TARGET_UPPERCASE)=1

MODULES += core/sys core/lib core/dev

ifdef CONTIKIFILES
CONTIKI_SOURCEFILES += $(CONTIKIFILES)
endif

CONTIKIDIRS += ${addprefix $(CONTIKI)/core/,dev lib net net/mac net/rime \
                 net/rpl sys cfs ctk lib/ctk loader . }

oname = ${patsubst %.c,%.o,${patsubst %.S,%.o,$(1)}}


uniq = $(if $1,$(firstword $1) $(call uniq,$(filter-out $(firstword $1),$1)))

### Include application makefiles

ifdef APPS
  APPDS = ${wildcard ${addprefix $(CONTIKI)/apps/, $(APPS)} \
          ${addprefix $(CONTIKI)/platform/$(TARGET)/apps/, $(APPS)} \
          $(APPS)}
  APPINCLUDES = ${foreach APP, $(APPS), ${wildcard ${foreach DIR, $(APPDS), $(DIR)/Makefile.$(APP)}}}
  -include $(APPINCLUDES)
  APP_SOURCES = ${foreach APP, $(APPS), $($(APP)_src)}
  DSC_SOURCES = ${foreach APP, $(APPS), $($(APP)_dsc)}
  CONTIKI_SOURCEFILES += $(APP_SOURCES) $(DSC_SOURCES)
endif

### Include target makefile (TODO Unsafe?)

target_makefile := $(wildcard $(CONTIKI)/platform/$(TARGET)/Makefile.$(TARGET))

# Check if the target makefile exists, and create the object directory if necessary.
ifeq ($(strip $(target_makefile)),)
  ${error The target platform "$(TARGET)" does not exist (maybe it was misspelled?)}
else
  ifneq (1, ${words $(target_makefile)})
    ${error More than one TARGET Makefile found: $(target_makefile)}
  endif
  include $(target_makefile)
endif

ifdef MODULES
  UNIQUEMODULES = $(call uniq,$(MODULES))
  MODULESSUBST = ${subst /,-,$(UNIQUEMODULES)}
  MODULEDIRS = ${wildcard ${addprefix $(CONTIKI)/, $(UNIQUEMODULES)}}
  MODULES_SOURCES = ${foreach d, $(MODULEDIRS), ${subst ${d}/,,${wildcard $(d)/*.c}}}
  CONTIKI_SOURCEFILES += $(MODULES_SOURCES)
  APPDS += $(MODULEDIRS)
endif

### Forward comma-separated list of arbitrary defines to the compiler

### Setup directory search path for source and header files

CONTIKI_TARGET_DIRS_CONCAT = ${addprefix ${dir $(target_makefile)}, \
                               $(CONTIKI_TARGET_DIRS)}

COMPILE_OPT += -DAUTOSTART_ENABLE

CONTIKI_CSRCDIR = $(CONTIKI_TARGET_DIRS_CONCAT) \
          	$(APPDS) $(CONTIKI)/core
CONTIKI_INCDIR = $(CONTIKI_CSRCDIR)
CONTIKI_ASMSRCDIR = $(CONTIKI_CSRCDIR)

CONTIKI_OBJDIR	= $(OUT_DIR)/contiki

CONTIKI_CSRCS = $(sort $(filter %.c, $(CONTIKI_SOURCEFILES)))
CONTIKI_ASMSRCS = $(sort $(filter %.S %.s, $(CONTIKI_SOURCEFILES)))

CONTIKI_COBJS = $(call get_objs, $(CONTIKI_CSRCS), $(CONTIKI_OBJDIR))
CONTIKI_ASMOBJS = $(call get_objs, $(CONTIKI_ASMSRCS), $(CONTIKI_OBJDIR))

# include dependency files
ifneq ($(MAKECMDGOALS),clean)
 -include $(CONTIKI_COBJS:.o=.o.d) $(CONTIKI_ASMOBJS:.o=.o.d)
endif

OS_LIB = $(OUT_DIR)/libcontiki.a

$(OS_LIB): $(CONTIKI_OBJDIR) $(CONTIKI_COBJS) $(CONTIKI_ASMOBJS)
	$(TRACE_ARCHIVE)
	$(Q)$(AR) $(AR_OPT) $@ $(CONTIKI_COBJS) $(CONTIKI_ASMOBJS)

.SECONDEXPANSION:
$(CONTIKI_COBJS): $(CONTIKI_OBJDIR)/%.o :$(call get_c_prerequisite, $(CONTIKI_CSRCDIR)) $$(COMMON_COMPILE_PREREQUISITES)
	$(TRACE_COMPILE)
	$(Q)$(CC) -c $(COMPILE_OPT) $(CONTIKI_COMPILE_OPT) $< -o $@

.SECONDEXPANSION:
$(CONTIKI_ASMOBJS): $(CONTIKI_OBJDIR)/%.o :$(call get_asm_prerequisite, $(CONTIKI_ASMSRCDIR)) $$(COMMON_COMPILE_PREREQUISITES)
	$(TRACE_ASSEMBLE)
	$(Q)$(CC) -c $(ASM_OPT) $(CONTIKI_COMPILE_OPT) $< -o $@

$(CONTIKI_OBJDIR):
	$(TRACE_CREATE_DIR)
	@$(MKD) $(subst /,$(PS),$@)