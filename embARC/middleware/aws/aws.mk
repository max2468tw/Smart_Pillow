# dir declaration
MID_AWS_DIR = $(EMBARC_ROOT)/middleware/aws

MID_AWS_CSRCDIR	= $(MID_AWS_DIR)/aws_iot_src/utils \
		  $(MID_AWS_DIR)/aws_iot_src/shadow \
		  $(MID_AWS_DIR)/aws_iot_src/protocol/mqtt \
		  $(MID_AWS_DIR)/aws_iot_src/protocol/mqtt/aws_iot_embedded_client_wrapper \
		  $(MID_AWS_DIR)/aws_iot_src/protocol/mqtt/aws_iot_embedded_client_wrapper/platform_embARC \
		  $(MID_AWS_DIR)/aws_iot_src/protocol/mqtt/aws_iot_embedded_client_wrapper/platform_embARC/mbedtls \
		  $(MID_AWS_DIR)/aws_mqtt_embedded_client_lib/MQTTPacket/src \
		  $(MID_AWS_DIR)/aws_mqtt_embedded_client_lib/MQTTClient-C/src

MID_AWS_INCDIR	= $(MID_AWS_CSRCDIR)

MID_INCDIR += $(MID_AWS_INCDIR)
MID_CSRCDIR += $(MID_AWS_CSRCDIR)

# the dir to generate objs
MID_AWS_OBJDIR = $(OUT_DIR)/middleware/aws

# find all the srcs in the target dirs
MID_AWS_CSRCS = $(call get_csrcs, $(MID_AWS_CSRCDIR))
MID_AWS_ASMSRCS = $(call get_asmsrcs, $(MID_AWS_ASMSRCDIR))

# get obj files
MID_AWS_COBJS = $(call get_objs, $(MID_AWS_CSRCS), $(MID_AWS_OBJDIR))

# include dependency files
ifneq ($(MAKECMDGOALS),clean)
 -include $(MID_AWS_COBJS:.o=.o.d) $(MID_AWS_ASMOBJS:.o=.o.d)
endif

# genearte library
MID_LIB_AWS = $(OUT_DIR)/libmidaws.a
MID_LIBS += $(MID_LIB_AWS)

$(MID_LIB_AWS): $(MID_AWS_OBJDIR) $(MID_AWS_COBJS)
	$(TRACE_ARCHIVE)
	$(Q)$(AR) $(AR_OPT) $@ $(MID_AWS_COBJS)

# specific compile rules
.SECONDEXPANSION:
$(MID_AWS_COBJS): $(MID_AWS_OBJDIR)/%.o :$(call get_c_prerequisite, $(MID_AWS_CSRCDIR)) $$(COMMON_COMPILE_PREREQUISITES)
	$(TRACE_COMPILE)
	$(Q)$(CC) -c $(COMPILE_OPT) $(MID_AWS_COMPILE_OPT) $< -o $@

# generate obj dir
$(MID_AWS_OBJDIR):
	$(TRACE_CREATE_DIR)
	$(Q)$(MKD) $(subst /,$(PS),$@)

# Middleware Definitions
MID_AWS_DEFINES = -DMID_AWS
MID_DEFINES += $(MID_AWS_DEFINES)
