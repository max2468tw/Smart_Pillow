# dir declaration
MID_MQTT_DIR = $(EMBARC_ROOT)/middleware/mqtt


MID_MQTT_ASMSRCDIR = $(MID_MQTT_DIR)/MQTTClient/src \
			$(MID_MQTT_DIR)/MQTTClient/src/lwip \
			$(MID_MQTT_DIR)/MQTTPacket/src
MID_MQTT_CSRCDIR = $(MID_MQTT_DIR)/MQTTClient/src \
			$(MID_MQTT_DIR)/MQTTClient/src/lwip \
			$(MID_MQTT_DIR)/MQTTPacket/src
MID_MQTT_INCDIR	= $(MID_MQTT_DIR)/MQTTClient/src \
			$(MID_MQTT_DIR)/MQTTClient/src/lwip \
			$(MID_MQTT_DIR)/MQTTPacket/src

MID_INCDIR += $(MID_MQTT_INCDIR)
MID_CSRCDIR += $(MID_MQTT_CSRCDIR)
MID_ASMSRCDIR += $(MID_MQTT_ASMSRCDIR)

# the dir to generate objs
MID_MQTT_OBJDIR = $(OUT_DIR)/middleware/mqtt

# find all the srcs in the target dirs
MID_MQTT_CSRCS = $(call get_csrcs, $(MID_MQTT_CSRCDIR))
MID_MQTT_ASMSRCS = $(call get_asmsrcs, $(MID_MQTT_ASMSRCDIR))

MID_MQTT_COBJS = $(call get_objs, $(MID_MQTT_CSRCS), $(MID_MQTT_OBJDIR))
MID_MQTT_ASMOBJS = $(call get_objs, $(MID_MQTT_ASMSRCS), $(MID_MQTT_OBJDIR))

# include dependency files
ifneq ($(MAKECMDGOALS),clean)
 -include $(MID_MQTT_COBJS:.o=.o.d) $(MID_MQTT_ASMOBJS:.o=.o.d)
endif

# genearte library
MID_LIB_MQTT = $(OUT_DIR)/libmidmqtt.a
MID_LIBS += $(MID_LIB_MQTT)

$(MID_LIB_MQTT): $(MID_MQTT_OBJDIR) $(MID_MQTT_COBJS) $(MID_MQTT_ASMOBJS)
	$(TRACE_ARCHIVE)
	$(Q)$(AR) $(AR_OPT) $@ $(MID_MQTT_COBJS) $(MID_MQTT_ASMOBJS)

# specific compile rules
SECONDEXPANSION:
$(MID_MQTT_COBJS): $(MID_MQTT_OBJDIR)/%.o :$(call get_c_prerequisite, $(MID_MQTT_CSRCDIR)) $$(COMMON_COMPILE_PREREQUISITES)
	$(TRACE_COMPILE)
	$(Q)$(CC) -c $(COMPILE_OPT) $(MID_MQTT_COMPILE_OPT) $< -o $@

SECONDEXPANSION:
$(MID_MQTT_ASMOBJS): $(MID_MQTT_OBJDIR)/%.o :$(call get_asm_prerequisite, $(MID_MQTT_ASMSRCDIR)) $$(COMMON_COMPILE_PREREQUISITES)
	$(TRACE_ASSEMBLE)
	$(Q)$(CC) -c $(ASM_OPT) $(MID_MQTT_COMPILE_OPT) $< -o $@

# generate obj dir
$(MID_MQTT_OBJDIR):
	$(TRACE_CREATE_DIR)
	$(Q)$(MKD) $(subst /,$(PS),$@)

# Middleware Definitions
MID_MQTT_DEFINES = -DMID_MQTT
MID_DEFINES += $(MID_MQTT_DEFINES)
