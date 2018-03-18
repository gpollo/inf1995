include Makefile.common

all:
	@(cd $(MAIN_DIR) && $(MAKE))

install:
	@(cd $(MAIN_DIR) && $(MAKE) $@)

clean:
	@(cd $(LIB_DIR)     && $(MAKE) $@)
	@(cd $(MAIN_DIR)    && $(MAKE) $@)
	@(cd $(TEST_DIR)    && $(MAKE) $@)
	@(cd $(FLASHER_DIR) && $(MAKE) $@)

mrproper: clean
	@(cd $(LIB_DIR)     && $(MAKE) $@)
	@(cd $(MAIN_DIR)    && $(MAKE) $@)
	@(cd $(TEST_DIR)    && $(MAKE) $@)
	@(cd $(FLASHER_DIR) && $(MAKE) $@)

