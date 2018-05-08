##
# \defgroup	MK_OS	OS Makefile Configurations
# \brief	makefile related to operation system configurations
##

##
# OS
# select supported os
##
SUPPORTED_OSES = freertos contiki
OS_SEL ?= freertos

override OS_SEL := $(strip $(OS_SEL))

## Set Valid OS
VAILD_OS_SEL = $(call check_item_exist, $(OS_SEL), $(SUPPORTED_OSES))

## Try Check OS is valid
ifeq ($(VAILD_OS_SEL), )
$(info OS - $(SUPPORTED_OSES) are supported)
$(error OS $(OS_SEL) is not supported, please check it!)
endif

OS_CSRCDIR	= $(EMBARC_ROOT)/os
OS_ASMSRCDIR	= $(EMBARC_ROOT)/os
OS_INCDIR	= $(EMBARC_ROOT)/os

## OS HAL LAYER ##
COMMON_COMPILE_PREREQUISITES += $(EMBARC_ROOT)/os/hal/os_hal.mk
include $(EMBARC_ROOT)/os/hal/os_hal.mk
OS_CSRCDIR	+= $(OS_HAL_CSRCDIR)
OS_ASMSRCDIR	+= $(OS_HAL_ASMSRCDIR)
OS_INCDIR	+= $(OS_HAL_INCDIR)

ifeq ($(OS_SEL), freertos)
#os definition
OS_ID = OS_FREERTOS
#os usage settings
COMMON_COMPILE_PREREQUISITES += $(EMBARC_ROOT)/os/freertos/freertos.mk
include $(EMBARC_ROOT)/os/freertos/freertos.mk
OS_CSRCDIR	+= $(FREERTOS_CSRCDIR)
OS_ASMSRCDIR	+= $(FREERTOS_ASMSRCDIR)
OS_INCDIR	+= $(FREERTOS_INCDIR)

OS_EXTRA_DEFINES += $(FREERTOS_DEFINES)
else #end of freertos#
ifeq ($(OS_SEL), contiki)
#os definition
OS_ID = OS_CONTIKI
CONTIKI = $(EMBARC_ROOT)/os/contiki
#os usage settings
COMMON_COMPILE_PREREQUISITES += $(EMBARC_ROOT)/os/contiki/contiki.mk
include $(EMBARC_ROOT)/os/contiki/contiki.mk

OS_CSRCDIR	+= $(CONTIKI_CSRCDIR)
OS_ASMSRCDIR	+= $(CONTIKI_ASMSRCDIR)
OS_INCDIR	+= $(CONTIKI_INCDIR)

OS_EXTRA_DEFINES += $(CONTIKI_DEFINES)
else
## OTHER CASE ##
OS_EXTRA_DEFINES =
endif #end of contiki
endif #end of freertos

##
# \brief	add defines for os
##
ifdef OS_ID
OS_DEFINES = -D$(OS_ID) -DENABLE_OS $(OS_EXTRA_DEFINES)
else
OS_DEFINES =
endif
