# library root declaration
LIB_ROOT_DIR = $(EMBARC_ROOT)/library
LIB_REQUIRED = clib

override LIB_SEL := $(strip $(LIB_SEL)) $(LIB_REQUIRED)
ifdef LIB_SEL
	LIB_SEL_SORTED = $(sort $(LIB_SEL))
	LIB_MKS = $(foreach LIB, $(LIB_SEL_SORTED), $(join $(LIB)/, $(LIB).mk))
	LIB_INCLUDES = $(foreach LIB_MK, $(LIB_MKS), $(wildcard $(addprefix $(LIB_ROOT_DIR)/, $(LIB_MK))))
	COMMON_COMPILE_PREREQUISITES += $(LIB_INCLUDES)
	include $(LIB_INCLUDES)
endif