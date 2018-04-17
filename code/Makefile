include Makefile.common

$(TRAJET_NAME):
	@(cd $(TRAJET_DIR) && $(MAKE))

$(TEST_NAME):
	@(cd $(TEST_DIR) && $(MAKE))

$(INTERPRETER_NAME):
	@(cd $(INTERPRETER_DIR) && $(MAKE))

$(FLASHER_NAME):
	@(cd $(FLASHER_DIR) && $(MAKE))

$(LIB_NAME):
	@(cd $(LIB_DIR) && $(MAKE))

install:
	@(cd $(MAIN_DIR) && $(MAKE) $@)

clean:
	@(cd $(LIB_DIR)         && $(MAKE) $@)
	@(cd $(TEST_DIR)        && $(MAKE) $@)
	@(cd $(INTERPRETER_DIR) && $(MAKE) $@)
	@(cd $(FLASHER_DIR)     && $(MAKE) $@)
	@(cd $(TRAJET_DIR)      && $(MAKE) $@)

mrproper: clean
	@(cd $(LIB_DIR)         && $(MAKE) $@)
	@(cd $(TEST_DIR)        && $(MAKE) $@)
	@(cd $(INTERPRETER_DIR) && $(MAKE) $@)
	@(cd $(FLASHER_DIR)     && $(MAKE) $@)
	@(cd $(TRAJET_DIR)      && $(MAKE) $@)

