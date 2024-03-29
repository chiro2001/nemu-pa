include $(NEMU_HOME)/scripts/git.mk
include $(NEMU_HOME)/scripts/build.mk

include $(NEMU_HOME)/tools/difftest.mk

compile_git:
	$(call git_commit, "compile")
$(BINARY): compile_git

# Some convenient rules

override ARGS ?= --log=$(BUILD_DIR)/nemu-log.txt
override ARGS += $(ARGS_DIFF)

# Command to execute NEMU
IMG ?=
NEMU_EXEC := $(BINARY) $(ARGS) $(IMG)

ARG_FILENAME := $(CONFIG_EXT_RUN_CONFIG_FILE)
ARG_FILE := $(BUILD_DIR)/$(ARG_FILENAME)
ifdef DEBUG_VSCODE
EXEC_FILE := 
else
EXEC_FILE := $(NEMU_EXEC)
endif

run-env: $(BINARY) $(DIFF_REF_SO)

run: run-env
	$(call git_commit, "run")
	@echo -e "$(ARGS) $(IMG)" > $(ARG_FILE)
	@sleep 0.2
	$(EXEC_FILE)

gdb: run-env
	$(call git_commit, "gdb")
	gdb -s $(BINARY) --args $(NEMU_EXEC)

clean-tools = $(dir $(shell find ./tools -name "Makefile"))
$(clean-tools):
	-@$(MAKE) -s -C $@ clean
clean-tools: $(clean-tools)
clean-all: clean distclean clean-tools

.PHONY: run gdb run-env clean-tools clean-all $(clean-tools)
