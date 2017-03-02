# Generic GNU Make driver file.
CONFIG= debug
GEN_SOURCE_DIR= Generated_Source/PSoC4

include platform_$(CONFIG).mk
include app_source.mk
include $(GEN_SOURCE_DIR)/gen_source.mk

GEN_OBJECTS= $(addprefix $(OUT_DIR)/,$(notdir $(addsuffix .o,$(basename $(GEN_C_SOURCE))))) $(addprefix $(OUT_DIR)/,$(notdir $(addsuffix .o,$(basename $(GEN_ASM_SOURCE)))))
DEPS= $(addprefix $(OUT_DIR)/,$(notdir $(addsuffix .c.d,$(basename $(APP_C_SOURCE))))) $(addprefix $(OUT_DIR)/,$(notdir $(addsuffix .c.d,$(basename $(GEN_C_SOURCE)))))
GEN_SEP_OBJECTS= $(addprefix $(OUT_DIR)/,$(notdir $(addsuffix .o,$(basename $(GEN_SEP_C_SOURCE)))))
APP_OBJECTS= $(addprefix $(OUT_DIR)/,$(notdir $(addsuffix .o,$(basename $(APP_C_SOURCE)))))

all : $(OUT_DIR)/$(DESIGN).hex

prebuild :
	Export/prebuild.sh

$(OUT_DIR)/$(DESIGN).hex : prebuild $(OUT_DIR)/$(DESIGN).elf
	Export/postbuild.sh $(OUT_DIR)/$(DESIGN).elf

$(OUT_DIR)/$(DESIGN).elf : $(APP_OBJECTS) $(GEN_SEP_OBJECTS) $(OUT_DIR)/$(DESIGN).a $(GEN_LIBS) $(APP_LIBS)
	"$(LD)" -Wl,--start-group $^ -o $@ $(LDFLAGS) -Wl,--end-group

$(OUT_DIR)/$(DESIGN).a : $(GEN_OBJECTS)
	"$(AR)" $(ARFLAGS) $@ $^

$(OUT_DIR)/%.o : $(GEN_SOURCE_DIR)/%.c
	@mkdir -p $(OUT_DIR)
	@"$(CC)" $(CDEPGEN)
	"$(CC)" -c $(CFLAGS) $< -o $@

$(OUT_DIR)/%.o : $(GEN_SOURCE_DIR)/%.s
	@mkdir -p $(OUT_DIR)
	@"$(AS)" $(ASFLAGS) $< -o $@

$(OUT_DIR)/%.o : %.c
	@mkdir -p $(OUT_DIR)
	@"$(CC)" $(CDEPGEN)
	"$(CC)" -c $(CFLAGS) $< -o $@

clean:
	$(RM) $(RMFLAGS) $(OUT_DIR)
-include $(DEPS)
