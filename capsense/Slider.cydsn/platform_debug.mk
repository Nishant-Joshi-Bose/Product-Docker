# Macros required for the generic driver for toolchain and/or operating system.

DESIGN= Slider
CONFIG= debug
OUT_DIR= output/$(CONFIG)
TOOLCHAIN_DIR ?= /usr/bin

CC_BASE= arm-none-eabi-gcc
CC_PATH= $(shell which $(CC_BASE) 2>/dev/null)
CC= $(if $(CC_PATH),$(CC_PATH),$(TOOLCHAIN_DIR)/$(CC_BASE))
CFLAGS= -mcpu=cortex-m0plus -mthumb -I. -IGenerated_Source/PSoC4 -Wa,-alh=$(OUT_DIR)/$(basename $(<F)).lst -g -D DEBUG -Wall -ffunction-sections -Og -ffat-lto-objects

CDEPGEN= -MM $< -MF $(OUT_DIR)/$(<F).d -MT $(OUT_DIR)/$(@F) -I. -I$(GEN_SOURCE_DIR)

LD_BASE= arm-none-eabi-gcc
LD_PATH= $(shell which $(LD_BASE) 2>/dev/null)
LD= $(if $(LD_PATH),$(LD_PATH),$(TOOLCHAIN_DIR)/$(LD_BASE))
LDFLAGS= -mcpu=cortex-m0plus -mthumb -g -ffunction-sections -Og -ffat-lto-objects -L Generated_Source/PSoC4 -Wl,-Map,$(OUT_DIR)/$(DESIGN).map -T ./Generated_Source/PSoC4/cm0plusgcc.ld -specs=nano.specs -Wl,--gc-sections

AS_BASE= arm-none-eabi-as
AS_PATH= $(shell which $(AS_BASE) 2>/dev/null)
AS= $(if $(AS_PATH),$(AS_PATH),$(TOOLCHAIN_DIR)/$(AS_BASE))
ASFLAGS= -mcpu=cortex-m0plus -mthumb -I. -IGenerated_Source/PSoC4 -alh=$(OUT_DIR)/$(basename $(<F)).lst -g -W

AR= $(TOOLCHAIN_DIR)/arm-none-eabi-ar
ARFLAGS= -rs

RM= rm
RMFLAGS= -rf
