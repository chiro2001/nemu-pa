# CROSS_COMPILE := riscv64-linux-gnu-
CROSS_COMPILE := $(CONFIG_EXT_RISCV_CROSS)
COMMON_FLAGS  := -fno-pic -march=rv32im -mabi=ilp32
CFLAGS        += $(COMMON_FLAGS) -static
ASFLAGS       += $(COMMON_FLAGS) -O0
LDFLAGS       += -melf32lriscv
