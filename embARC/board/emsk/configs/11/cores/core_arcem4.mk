## Core Configuation Folder ##
EMSK_11_CORE_DIR = $(EMSK_11_BOARD_DIR)/cores

## Core Configuration Defines ##
ifeq ($(TOOLCHAIN), gnu)
  CORE_DEFINES = -D_ARCVER=0x41
endif

CORE_DEFINES += -DCURRENT_CORE=$(CUR_CORE) -DNUM_EXC_INT=9 -DINT_PRI_MIN=-2 \
		-DARC_FEATURE_CODE_DENSITY

## Metaware Toolchain Options For Core ##
CCORE_OPT_MW += -arcv2em -core1 -Xswap -Xcode_density -Xshift_assist -Xbarrel_shifter \
		-Xnorm -Xdiv_rem -Xmpy -Xmpy16 -Xmpy_cycles=5 -Xtimer0 -Xtimer1 -Hnosdata
LCORE_OPT_MW += $(CCORE_OPT_MW) -Hnocplus -Hnocrt -e_start
ACORE_OPT_MW += -Hasmcpp

## GNU Toolchain Options For Core ##
CCORE_OPT_GNU += -mcpu=arcem -mno-sdata -mlittle-endian -mmpy-option=6 -mmpy16 \
			-mbarrel-shifter -mdiv-rem -mnorm -mswap
LCORE_OPT_GNU += $(CCORE_OPT_GNU) -nostartfiles -e_start
ACORE_OPT_GNU += -mcpu=arcem -x assembler-with-cpp
