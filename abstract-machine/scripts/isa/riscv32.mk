# CROSS_COMPILE := riscv64-linux-gnu-
CROSS_COMPILE := $(patsubst "%",%,$(CONFIG_EXT_RISCV_CROSS))
# CROSS_COMPILE := $(CONFIG_EXT_RISCV_CROSS)
# CROSS_COMPILE := riscv32-unknown-linux-gnu-
COMMON_FLAGS  := -fno-pic -march=rv32im -mabi=ilp32
CFLAGS        += $(COMMON_FLAGS) -static -nostdlib -L/home/chiro/Downloads/newlib-3.2.0/build/install/riscv32-unknown-elf/lib -I/home/chiro/Downloads/newlib-3.2.0/build/install/riscv32-unknown-elf/include
CFLAGS				+= -specs=/home/chiro/Downloads/newlib-3.2.0/build/install/riscv32-unknown-elf/lib/nosys.specs
CFLAGS				+= -lm -lc -lg -lgloss -lnosys -lsim
ASFLAGS       += $(COMMON_FLAGS) -O0
LDFLAGS       += -melf32lriscv

LDFLAGS       += -lm -lc -lg -lgloss -lnosys -lsim -static -nostdlib -L/home/chiro/Downloads/newlib-3.2.0/build/install/riscv32-unknown-elf/lib -I/home/chiro/Downloads/newlib-3.2.0/build/install/riscv32-unknown-elf/include
# LDFLAGS				+= -L/home/chiro/Downloads/newlib-3.2.0/build/riscv32-unknown-elf/newlib/libc/stdio -l_a-printf --verbose
LDFLAGS				+= --verbose

-include $(NEMU_HOME)/include/config/auto.conf

CFLAGS += $(if $(CONFIG_CC_DEBUG),-g,)