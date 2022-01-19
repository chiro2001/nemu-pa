ifdef CONFIG_DIFFTEST
DIFF_REF_PATH = $(NEMU_HOME)/$(call remove_quote,$(CONFIG_DIFFTEST_REF_PATH))
DIFF_REF_SO = $(DIFF_REF_PATH)/build/$(GUEST_ISA)-$(call remove_quote,$(CONFIG_DIFFTEST_REF_NAME))-so
MKFLAGS = GUEST_ISA=$(GUEST_ISA) SHARE=1 ENGINE=interpreter APREFIX=$(APREFIX)
ARGS_DIFF = --diff=$(DIFF_REF_SO)

DIFF_MAKE_ := $(MAKE) -C $(DIFF_REF_PATH) $(MKFLAGS)
ifeq ($(wildcard $(DIFF_REF_SO)),)
	DIFF_MAKE := $(DIFF_MAKE_)
else
	ifdef CONFIG_EXT_SKIP_COMPILE_DIFF
	DIFF_MAKE :=
	else
	DIFF_MAKE := $(DIFF_MAKE_)
	endif
endif

ifndef CONFIG_DIFFTEST_REF_NEMU
$(DIFF_REF_SO):
	$(DIFF_MAKE)
endif

rm-diff-so:
	-@rm -f $(DIFF_REF_SO)

diff: rm-diff-so $(DIFF_REF_SO)

.PHONY: $(DIFF_REF_SO) rm-diff-so diff
endif

