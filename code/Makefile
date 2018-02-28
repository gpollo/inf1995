include Makefile.common

TARGET=robot

LIB_DIR=lib
EXEC_DIR=exec

all: $(TARGET)

$(TARGET):
	@(cd $(LIB_DIR)  && $(MAKE))
	@(cd $(EXEC_DIR) && $(MAKE))
	

install:

clean:
	@(cd $(LIB_DIR)  && $(MAKE) $@)
	@(cd $(EXEC_DIR) && $(MAKE) $@)

mrproper: clean
	@(cd $(LIB_DIR)  && $(MAKE) $@)
	@(cd $(EXEC_DIR) && $(MAKE) $@)

