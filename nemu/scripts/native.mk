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

RUN_FILENAME := .run.sh
RUN_FILE := $(BUILD_DIR)/$(RUN_FILENAME)
ifdef DEBUG_VSCODE
RUN_FILE_DELETE := 
else
RUN_FILE_DELETE := rm $(RUN_FILE)
endif

run-env: $(BINARY) $(DIFF_REF_SO)

run: run-env
	$(call git_commit, "run")
	@echo -e "#!/bin/sh\n$(NEMU_EXEC)" > $(RUN_FILE)
	# $(NEMU_EXEC)
	@sleep 0.2
	@chmod +x $(RUN_FILE)
	$(RUN_FILE)
	-$(RUN_FILE_DELETE)

gdb: run-env
	$(call git_commit, "gdb")
	gdb -s $(BINARY) --args $(NEMU_EXEC)

clean-tools = $(dir $(shell find ./tools -name "Makefile"))
$(clean-tools):
	-@$(MAKE) -s -C $@ clean
clean-tools: $(clean-tools)
clean-all: clean distclean clean-tools

.PHONY: run gdb run-env clean-tools clean-all $(clean-tools)
