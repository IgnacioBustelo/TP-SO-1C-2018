LIBS_DIRS := parsi/src so-commons-library
PROG_DIRS := coordinador esi instancia planificador

all: submodules $(LIBS_DIRS) $(PROG_DIRS)

clean: MAKE += clean
clean: $(PROG_DIRS)

submodules:
	git submodule init
	git submodule update

$(LIBS_DIRS):
	cd $@ && sudo $(MAKE) install

$(PROG_DIRS):
	@echo "**********************************"
	@echo "**********$(basename $@)**********"
	@echo "**********************************"
	$(MAKE) -C $@

.PHONY: all clean $(PROG_DIRS) $(LIBS_DIRS) submodules