include nemu/scripts/git.mk

default:
	@echo "Please run 'make' under any subprojects to compile."

submit:
	git gc
	STUID=$(STUID) STUNAME=$(STUNAME) bash -c "$$(curl -s http://jyywiki.cn/static/submit.sh)"

MKFILES := $(shell find . -name "Makefile" | xargs -I {} echo {} | grep -v "tools" | grep -v "\./Makefile")

$(MKFILES): %
	echo %, $<

clean: $(MKFILES)
	@echo $(MKFILES)
	# find . -name "Makefile" | xargs -I {} echo {} | grep -v "tools" | grep -v "\./Makefile" | xargs -I {} echo {}
	# find . -name "Makefile" | xargs -I {} echo {} | grep -v "tools" | grep -v "\./Makefile" | xargs -I {} make -f {} clean


.PHONY: default submit clean
