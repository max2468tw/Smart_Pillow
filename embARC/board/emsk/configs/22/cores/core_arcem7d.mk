## Core Configuation Folder ##
EMSK_22_CORE_DIR = $(EMSK_22_BOARD_DIR)/cores

## Core Configuration Defines ##
ifeq ($(TOOLCHAIN), gnu)
  CORE_DEFINES = -D_ARCVER=0x42
endif

CORE_DEFINES += -DCURRENT_CORE=$(CUR_CORE) -DNUM_EXC_INT=20 -DINT_PRI_MIN=-4 -DARC_FEATURE_FIRQ=1 \
		-DARC_FEATURE_CODE_DENSITY

## Metaware Toolchain Options For Core ##
CCORE_OPT_MW += -arcv2em -core2 -HL -Xcode_density -Xdiv_rem=radix2 -Xswap \
		-Xbitscan -Xmpy_option=mpyd -Xshift_assist -Xbarrel_shifter -Xdsp2 -Xdsp_complex \
		-Xdsp_divsqrt=radix2 -Xdsp_accshift=limited \
		-Xtimer0 -Xtimer1 -Xstack_check -Xdmac -Hnosdata
LCORE_OPT_MW += $(CCORE_OPT_MW) -Hnocplus -Hnocrt -e_start
ACORE_OPT_MW += -Hasmcpp

## GNU Toolchain Options For Core ##
CCORE_OPT_GNU += -mcpu=arcem -mlittle-endian -mcode-density \
		-mdiv-rem -mswap -mnorm -mmpy-option=6 \
		-mbarrel-shifter \
		--param l1-cache-size=16384 --param l1-cache-line-size=32 \
		-mno-sdata
LCORE_OPT_GNU += $(CCORE_OPT_GNU) -nostartfiles -e_start
ACORE_OPT_GNU += -mcpu=arcem -x assembler-with-cpp

