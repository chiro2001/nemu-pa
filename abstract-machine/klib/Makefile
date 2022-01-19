NAME = klib
# SRCS = $(shell find src/ -name "*.c")
ifeq ($(ARCH),native)
SRCS = $(shell find src/ -name "*.c" | grep -v "libgcc")
else
SRCS = $(shell find src/ -name "*.c")
endif
include $(AM_HOME)/Makefile
