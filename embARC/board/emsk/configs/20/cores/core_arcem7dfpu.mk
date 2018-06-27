## Core Configuation Folder ##
EMSK_20_CORE_DIR = $(EMSK_20_BOARD_DIR)/cores

## Core Configuration Defines ##
ifeq ($(TOOLCHAIN), gnu)
  CORE_DEFINES = -D_ARCVER=0x41
endif

CORE_DEFINES += -DCURRENT_CORE=$(CUR_CORE) -DNUM_EXC_INT=13 -DINT_PRI_MIN=-2 \
		-DARC_FEATURE_CODE_DENSITY


## Metaware Toolchain Options For Core ##
CCORE_OPT_MW += -arcv2em -core1 -Xcode_density -Xswap -Xnorm -Xmpy16 -Xmpy -Xmpyd \
			-Xshift_assist -Xbarrel_shifter -Xdsp_complex \
			-Xfpus_div -Xfpu_mac -Xfpuda -Xtimer0 -Xtimer1 -Hnosdata
LCORE_OPT_MW += $(CCORE_OPT_MW) -Hnocplus -Hnocrt -e_start
ACORE_OPT_MW += -Hasmcpp

## GNU Toolchain Options For Core ##
CCORE_OPT_GNU += -mcpu=arcem -mno-sdata -mlittle-endian -mmpy16 \
			-mbarrel-shifter -mnorm -mswap -mfpu=fpus_div -mfpu=fpuda -mfpu=fpus_fma

LCORE_OPT_GNU += $(CCORE_OPT_GNU) -nostartfiles -e_start
ACORE_OPT_GNU += -mcpu=arcem -x assembler-with-cpp

