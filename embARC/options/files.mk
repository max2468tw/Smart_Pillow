##
# \brief	definitions for source codes directories
##

## Device HAL Related ##
DEVHAL_ASMSRCDIR	=
DEVHAL_CSRCDIR		=
DEVHAL_INCDIR		= $(EMBARC_ROOT)/device/device_hal/inc

## Extra Related ##
EXTRA_ASMSRCDIR		=
EXTRA_CSRCDIR		=
EXTRA_INCDIR		= $(EMBARC_ROOT)/inc $(EMBARC_ROOT)/inc/arc

###############################################################################
## ALL ##
CSRCS_DIR		= $(ARC_CSRCDIR) $(OS_CSRCDIR) $(DEVHAL_CSRCDIR) $(BOARD_CSRCDIR) \
				$(MID_CSRCDIR) $(LIB_CSRCDIR) $(EXTRA_CSRCDIR)
ASMSRCS_DIR		= $(ARC_ASMSRCDIR) $(OS_ASMSRCDIR) $(DEVHAL_ASMSRCDIR) $(BOARD_ASMSRCDIR) \
				$(MID_ASMSRCDIR) $(LIB_ASMSRCDIR) $(EXTRA_ASMSRCDIR)
INCS_DIR		= $(ARC_INCDIR) $(OS_INCDIR) $(DEVHAL_INCDIR) $(BOARD_INCDIR) \
				$(MID_INCDIR) $(LIB_INCDIR) $(EXTRA_INCDIR)
###############################################################################