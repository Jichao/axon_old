#Makefile of Axon Game Engine


#check linux or freebsd
uname_s := $(shell sh -c 'uname -s 2>/dev/null || echo not')

ifeq ($(uname_s), Linux)
  OS_SPECIAL = -DUSE_EPOLL
  MAKE = make
else
  OS_SPECIAL = -DUSE_KQUEUE
  MAKE = gmake
endif

INCLUDES = -I./src -I./depends/include
CC = g++
LINK = ld
AR = ar
OPTIMIZE = -g3 -O2 -Wall -static -DRUN_INTERNAL
EXTRA_LIB = depends/lib/libjson.a

#==source===================================
OBJ_DIR = objs
BIN_DIR = bin
CORE_OBJ_DIR = objs/core
CORE_LIB = libax_core.a

#==link target==============================

include Makefile.dep

all: core engine test
	@echo "done!"

clean:
	-find objs/ -name "*.o" -print | xargs rm -f
	-find objs/ -name "*.o" -print | xargs rm -f
	-find objs/ -name "*.a" -print | xargs rm -f

engine: cgate 
	@echo "make engine done!!"

cgate: core $(CGATE_OBJS)
	$(CC) -g3 -pthread -o $(BIN_DIR)/cgate $(addprefix $(OBJ_DIR)/, $(CGATE_OBJS)) $(OBJ_DIR)/$(CORE_LIB) $(EXTRA_LIB) 

core: environ $(CORE_OBJS)
	$(AR) -r $(OBJ_DIR)/$(CORE_LIB) $(addprefix $(OBJ_DIR)/, $(CORE_OBJS))

.PHONY: test

test:
	cd test && $(MAKE) $@

environ:
	-mkdir -p $(OBJ_DIR)/core
	-mkdir -p $(OBJ_DIR)/test
	-mkdir -p $(OBJ_DIR)/cgate
	-mkdir -p $(OBJ_DIR)/master
	-mkdir -p $(BIN_DIR)


#==auto rule=============================

core/%.o : src/core/%.cc $(CORE_DEPS)
	$(CC) $(INCLUDES) $(OPTIMIZE) $(OS_SPECIAL) -o $(OBJ_DIR)/$@ -c $<

cgate/%.o : src/cgate/%.cc $(CGATE_DEPS)
	$(CC) $(INCLUDES) $(OPTIMIZE) $(OS_SPECIAL) -o $(OBJ_DIR)/$@ -c $<
