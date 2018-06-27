DEFINES += -DPLATFORM_EMBARC $(CPU_DEFINES) $(BOARD_DEFINES) \
		$(MID_DEFINES) $(LIB_DEFINES) $(OS_DEFINES) \
		$(TOOLCHAIN_DEFINES) $(APPL_DEFINES)

##
#	include directories definition
##
ALL_INC_DIRS = $(wildcard $(sort $(INCS_DIR) $(APPL_INC_DIR)))

##
#	c source directories definition
##
ALL_CSRC_DIRS = $(CSRCS_DIR) $(APPL_CSRC_DIR)

##
# 	asm source directories definition
##
ALL_ASMSRC_DIRS = $(ASMSRCS_DIR) $(APPL_ASMSRC_DIR)

## include conversion  ##
INCLUDE	= $(foreach dir,$(ALL_INC_DIRS),-I$(dir))

##
# Collect Application Source Files
##
APPL_CSRCS = $(call get_csrcs, $(APPL_CSRC_DIR))
APPL_ASMSRCS = $(call get_asmsrcs, $(APPL_ASMSRC_DIR))

##
# Collect all sources files
##
APPL_CSRC_DIR += $(EMBARC_ROOT)/board
ALL_CSRCS = $(call get_csrcs, $(ALL_CSRC_DIRS))
ALL_ASMSRCS = $(call get_asmsrcs, $(ALL_ASMSRC_DIRS))

##
# Collect Application Object Files
##
APPL_COBJS = $(call get_objs, $(APPL_CSRCS), $(OUT_DIR))
APPL_ASMOBJS = $(call get_objs, $(APPL_ASMSRCS), $(OUT_DIR))


# include dependency files of application
ifneq ($(MAKECMDGOALS),clean)
 -include $(APPL_COBJS:.o=.o.d) $(APPL_ASMOBJS:.o=.o.d)
endif

##
# Collect all object files
##
ALL_COBJS = $(call get_objs, $(ALL_CSRCS), $(OUT_DIR))
ALL_ASMOBJS = $(call get_objs, $(ALL_ASMSRCS), $(OUT_DIR)) $(START_OBJ)
ALL_OBJS =  $(ALL_COBJS) $(ALL_ASMOBJS)
ALL_LIBS = $(BOARD_LIB) $(MID_LIBS) $(OS_LIB) $(CPU_LIB) $(LIB_LIBS)
EMBARC_LIB = $(OUT_DIR)/libembarc.a

##
# common prerequisites like makefiles & ldscripts
# only used in compile and link process
##
COMMON_LINK_PREREQUISITES = $(LINKER_SCRIPT) $(MAKEFILES)

##
# Clean Related
##
OUT_DIR_PREFIX_WILDCARD = $(subst \,/,$(OUT_DIR_PREFIX))
OUT_DIRS_WILDCARD = $(sort $(wildcard $(OUT_DIR_PREFIX_WILDCARD)*))
OUT_DIRS = $(subst /,$(PS), $(strip $(OUT_DIRS_WILDCARD)))

ifeq ($(strip $(OUT_DIRS)), )
DIST_DIR_CLEAN =
else
DIST_DIR_CLEAN = $(RMD) $(OUT_DIRS)
endif

TEMP_FILES2CLEAN = $(sort $(wildcard *.o *.out *.bin *.dis *.elf *.a *.hex *.map *.bak *.dump *.d *.img *.dasm *.log))
ifeq ($(strip $(TEMP_FILES2CLEAN)), )
TEMPFILES_CLEAN =
else
TEMPFILES_CLEAN = $(RM) $(TEMP_FILES2CLEAN)
endif

#
#  the definition of source file directory
#
vpath %.C $(ALL_CSRC_DIRS)
vpath %.c $(ALL_CSRC_DIRS)
vpath %.S $(ALL_ASMSRC_DIRS)
vpath %.s $(ALL_ASMSRC_DIRS)

.PHONY : all build dump dasm bin size clean boardclean distclean run gui mdbnsim mdbnsimdbg cfg opt spopt help

all : $(APPL_FULL_NAME).elf

build : clean all

dump : $(APPL_FULL_NAME).dump

dasm : $(APPL_FULL_NAME).dasm

bin : $(APPL_FULL_NAME).bin

help :
	@$(ECHO) 'Build Targets for selected configuration:'
	@$(ECHO) '  all         - Build example'
	@$(ECHO) '  bin         - Build and Generate binary for example'
	@$(ECHO) '  build       - Clean first then compile example'
	@$(ECHO) '  dump        - Generate dump information for example'
	@$(ECHO) '  dasm        - Disassemble object file'
	@$(ECHO) '  size        - Display size information of object file'
	@$(ECHO) 'Clean Targets:'
	@$(ECHO) '  clean       - Remove object files of selected configuration'
	@$(ECHO) '  boardclean  - Remove object files of selected board'
	@$(ECHO) '  distclean   - Remove object files of all boards'
	@$(ECHO) 'Debug & Run Targets for selected configuration:'
	@$(ECHO) '  run         - Use MDB & JTAG to download and run object elf file'
	@$(ECHO) '  gui         - Use MDB & JTAG to download and debug object elf file'
	@$(ECHO) 'Other Targets:'
	@$(ECHO) '  cfg         - Display build target configuration'
	@$(ECHO) '  opt         - Display Current MAKE options'
	@$(ECHO) '  spopt       - Display Supported MAKE options'
	@$(ECHO) 'Available Configurations:'
	@$(ECHO) '  BOARD=emsk|nsim                             - Build for which board(EMSK, NSIM)'
	@$(ECHO) '  BD_VER=11|20|21|22                          - Board Version of development board'
	@$(ECHO) '  OLEVEL=Os|O0|O1|O2|O3                       - Optimization Level of examples to be built'
	@$(ECHO) '  CUR_CORE=arcem4|arcem4cr16|arcem6|arcem6gp|arcem5d|arcem7d|arcem7dfpu|...'
	@$(ECHO) '                                              - Current core configuration'
	@$(ECHO) '  TOOLCHAIN=mw|gnu                            - Current selected compiling toolchain'
	@$(ECHO) '  JTAG=usb|opella                             - Current debug jtag(Digilent JTAG or Ashling Opella-XD JTAG)'
	@$(ECHO) '  OUT_DIR_ROOT=.|xxx                          - Specify where to generate object files default current example makefile folder'
	@$(ECHO) '  SILENT=0|1                                  - Disable or enable message output'
	@$(ECHO) '  V=0|1                                       - Disable or enable verbose compiling information'
	@$(ECHO) '  DIG_NAME=xxx                                - Specify Digilent JTAG which to be used, most useful when more than one Digilent USB-JTAG plugged in'
	@$(ECHO) '  HEAPSZ=xxx                                  - Specify heap size for program, xxx stands for size in bytes'
	@$(ECHO) '  STACKSZ=xxx                                 - Specify stack size for program, xxx stands for size in bytes'
	@$(ECHO) 'Example Usage:'
	@$(ECHO) '  make all                                                                - build all examples in current directory using default configuration'
	@$(ECHO) '  make BOARD=emsk BD_VER=11 CUR_CORE=arcem6 OLEVEL=O2 TOOLCHAIN=gnu all   - build examples using configuration (emsk, 11, arcem6, O2, gnu)'

cfg :
	@$(ECHO) '=======Current Configuration======='
	@$(ECHO) 'Host OS            : $(HOST_OS)'
	@$(ECHO) 'Board              : $(BOARD)'
	@$(ECHO) 'Hardware Version   : $(BD_VER)'
	@$(ECHO) 'Core Configuration : $(CUR_CORE)'
	@$(ECHO) 'CPU Clock HZ       : $(CPU_FREQ)'
	@$(ECHO) 'Peripheral Clock HZ: $(DEV_FREQ)'
	@$(ECHO) 'Build Toolchain    : $(TOOLCHAIN)'
	@$(ECHO) 'Optimization Level : $(OLEVEL)'
	@$(ECHO) 'Debug Jtag         : $(JTAG)'
	@$(ECHO) '======Supported Configurations of $(BOARD)-$(BD_VER)======'
	@$(ECHO) 'Boards (BOARD)                 : $(SUPPORTED_BOARDS)'
	@$(ECHO) 'Core Configurations (CUR_CORE) : $(SUPPORTED_CORES)'
	@$(ECHO) 'Build Toolchains (TOOLCHAIN)   : $(SUPPORTED_TOOLCHAINS)'
	@$(ECHO) 'Debug Jtags (JTAG)             : $(SUPPORTED_JTAGS)'

opt :
	@$(ECHO) ======CURRENT CONFIGURATION=====
	@$(ECHO) BOARD : $(BOARD)
	@$(ECHO) BD_VER : $(BD_VER)
	@$(ECHO) CUR_CORE : $(CUR_CORE)
	@$(ECHO) TOOLCHAIN : $(TOOLCHAIN)
	@$(ECHO) OLEVEL : $(OLEVEL)
	@$(ECHO) JTAG : $(JTAG)
	@$(ECHO) EMBARC_ROOT : $(EMBARC_ROOT)
	@$(ECHO) COMPILE_OPT : $(subst $(MKDEP_OPT), , $(COMPILE_OPT))
	@$(ECHO) ASM_OPT : $(subst $(MKDEP_OPT), , $(ASM_OPT))
	@$(ECHO) AR_OPT : $(AR_OPT)
	@$(ECHO) LINK_OPT : $(LINK_OPT)
	@$(ECHO) DEBUGGER : $(DBG)
	@$(ECHO) DBG_HW_FLAGS : $(DBG_HW_FLAGS)
	@$(ECHO) MDB_NSIM_OPT : $(MDB_NSIM_OPT)

spopt :
	@$(ECHO) ======SUPPORTED CONFIGURATIONS=====
	@$(ECHO) SUPPORTED_BOARDS : $(SUPPORTED_BOARDS)
	@$(ECHO) SUPPORTED_BD_VERS : $(SUPPORTED_BD_VERS)
	@$(ECHO) SUPPORTED_CORES : $(SUPPORTED_CORES)
	@$(ECHO) SUPPORTED_OLEVELS : $(SUPPORTED_OLEVELS)
	@$(ECHO) SUPPORTED_TOOLCHAINS : $(SUPPORTED_TOOLCHAINS)
	@$(ECHO) SUPPORTED_JTAGS : $(SUPPORTED_JTAGS)

size : $(APPL_FULL_NAME).elf
	@$(ECHO) "Print Application Program Size"
	$(Q)$(SIZE) $(SIZE_OPT) $<

clean :
	@$(ECHO) "Clean Workspace For Selected Configuration : $(SELECTED_CONFIG)"
	-$(IFEXISTDIR) $(subst /,$(PS),$(OUT_DIR)) $(ENDIFEXISTDIR) $(RMD) $(subst /,$(PS),$(OUT_DIR))
	-$(IFEXISTDIR) .sc.project $(ENDIFEXISTDIR) $(RMD) .sc.project
	-$(TEMPFILES_CLEAN)

boardclean :
	@$(ECHO) "Clean Workspace For Selected Board : $(BOARD_INFO)"
	-$(IFEXISTDIR) $(subst /,$(PS),$(BOARD_OUT_DIR)) $(ENDIFEXISTDIR) $(RMD) $(subst /,$(PS),$(BOARD_OUT_DIR))
	-$(IFEXISTDIR) .sc.project $(ENDIFEXISTDIR) $(RMD) .sc.project
	-$(TEMPFILES_CLEAN)

distclean :
	@$(ECHO) "Clean All"
	-$(IFEXISTDIR) .sc.project $(ENDIFEXISTDIR) $(RMD) .sc.project
	-$(DIST_DIR_CLEAN)
	-$(TEMPFILES_CLEAN)

run : $(APPL_FULL_NAME).elf
	@$(ECHO) "Download & Run $<"
	$(DBG) $(DBG_HW_FLAGS) $< $(CMD_LINE)

gui : $(APPL_FULL_NAME).elf
	@$(ECHO) "Download & Debug $<"
	$(DBG) $(DBG_HW_FLAGS) $< $(CMD_LINE)

ifeq ($(BOARD), nsim)
ifeq ($(DBG), arc-elf32-gdb)
nsim : $(APPL_FULL_NAME).elf
	@$(ECHO) "Start nsim standalone "
	nsimdrv -gdb -port 1234 -propsfile $(NSIM_PROPS_FILE)
endif
endif

.SECONDEXPANSION:
$(APPL_FULL_NAME).elf : $(OUT_DIR) $(APPL_COBJS) $(APPL_ASMOBJS) $(EMBARC_LIB) $$(COMMON_LINK_PREREQUISITES)
	$(TRACE_LINK)
	$(Q)$(LD) $(LINK_OPT) $(APPL_COBJS) $(APPL_ASMOBJS) $(LD_START_GROUPLIB) $(EMBARC_LIB) $(APPL_LIBS) $(LD_SYSTEMLIBS) $(LD_END_GROUPLIB) -o $@

$(EMBARC_LIB) : $(ALL_LIBS)
	$(TRACE_ARCHIVE)
	$(Q)-$(IFEXISTFILE) $(subst /,$(PS),$(EMBARC_LIB)) $(ENDIFEXISTFILE) $(RM) $(subst /,$(PS),$(EMBARC_LIB))
	$(Q)$(AR) $(AR_OPT) $@ $(ALL_LIBS)

.SECONDEXPANSION:
$(APPL_COBJS) : $(OUT_DIR)/%.o: %.c $$(COMMON_COMPILE_PREREQUISITES)
	$(TRACE_COMPILE)
	$(Q)$(CC) -c $(COMPILE_OPT) $< -o $@

.SECONDEXPANSION:
$(APPL_ASMOBJS): $(OUT_DIR)/%.o: %.s $$(COMMON_COMPILE_PREREQUISITES)
	$(TRACE_ASSEMBLE)
	$(Q)$(CC) -c $(ASM_OPT) $< -o $@

$(OUT_DIR) :
	$(TRACE_CREATE_DIR)
	@$(MKD) $(subst /,$(PS),$@)

$(APPL_FULL_NAME).dump : $(APPL_FULL_NAME).elf
	@$(ECHO) "Dumping $<"
	$(Q)$(DMP) $(DMP_OPT) $<  > $@

$(APPL_FULL_NAME).dasm : $(APPL_FULL_NAME).elf
	@$(ECHO) "Disassembling $<"
	$(Q)$(DMP) $(DASM_OPT) $<  > $@

$(APPL_FULL_NAME).bin : $(APPL_FULL_NAME).elf
	@$(ECHO) "Generating Binary $@"
	$(Q)$(OBJCOPY) $(ELF2BIN_OPT) $< $@

<built-in> :
	@$(ECHO) "Rules to fix built-in missing caused by metaware compiler 2014.12"
