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
include src/proto/Makefile.dep

all: core engine test
	@echo "done!"

clean:
	-find objs/ -name "*.o" -print | xargs rm -f
	-find objs/ -name "*.o" -print | xargs rm -f
	-find objs/ -name "*.a" -print | xargs rm -f

engine: pgate 
	@echo "make engine done!!"

pgate: core $(PGATE_OBJS)
	$(CC) -g3 -pthread -o $(BIN_DIR)/pgate $(addprefix $(OBJ_DIR)/, $(PGATE_OBJS)) $(OBJ_DIR)/$(CORE_LIB) $(EXTRA_LIB)

core: environ $(CORE_OBJS) $(PROTO_OBJS)
	$(AR) -r $(OBJ_DIR)/$(CORE_LIB) $(addprefix $(OBJ_DIR)/, $(CORE_OBJS)) $(addprefix $(OBJ_DIR)/, $(PROTO_OBJS))

.PHONY: test

test:
	cd test && $(MAKE) $@

environ:
	-mkdir -p $(OBJ_DIR)/core
	-mkdir -p $(OBJ_DIR)/test
	-mkdir -p $(OBJ_DIR)/pgate
	-mkdir -p $(OBJ_DIR)/master
	-mkdir -p $(OBJ_DIR)/proto
	-mkdir -p $(BIN_DIR)


#==auto rule=============================

core/%.o : src/core/%.cc $(CORE_DEPS)
	$(CC) $(INCLUDES) $(OPTIMIZE) $(OS_SPECIAL) -o $(OBJ_DIR)/$@ -c $<

pgate/%.o : src/pgate/%.cc $(PGATE_DEPS)
	$(CC) $(INCLUDES) $(OPTIMIZE) $(OS_SPECIAL) -o $(OBJ_DIR)/$@ -c $<

proto/%.o : src/proto/%.cc $(PROTO_DEPS)
	$(CC) $(INCLUDES) $(OPTIMIZE) $(OS_SPECIAL) -o $(OBJ_DIR)/$@ -c $<

