# CROSS_COMPILE := riscv64-linux-gnu-
CROSS_COMPILE := $(patsubst "%",%,$(CONFIG_EXT_RISCV_CROSS))
# CROSS_COMPILE := $(CONFIG_EXT_RISCV_CROSS)
# CROSS_COMPILE := riscv32-unknown-linux-gnu-
COMMON_FLAGS  := -fno-pic -march=rv32im -mabi=ilp32
CFLAGS        += $(COMMON_FLAGS) -static
ASFLAGS       += $(COMMON_FLAGS) -O0
LDFLAGS       += -melf32lriscv

-include $(NEMU_HOME)/include/config/auto.conf

CFLAGS += $(if $(CONFIG_CC_DEBUG),-g,)