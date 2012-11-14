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

INCLUDES = -I./src
CC = g++
LINK = ld
AR = ar
OPTIMIZE = -g3 -O2 -Wall -static
EXTRA_LIB = 

#==source===================================
OBJ_DIR = objs
BIN_DIR = bin
CORE_OBJ_DIR = objs/core
CORE_LIB = ax_core.a

#==link target==============================

include Makefile.dep

all: core engine
	@echo "done!"

clean:
	-find objs/ -name "*.o" -print | xargs rm -f
	-find objs/ -name "*.o" -print | xargs rm -f


engine: netd
	@echo "make engine done!!"

netd: core $(NETD_OBJS)
	$(CC) -g3 -o $(BIN_DIR)/netd $(EXTRA_LIB) $(OBJ_DIR)/$(NETD_OBJS) $(OBJ_DIR)/$(CORE_LIB)

core: environ $(CORE_OBJS)
	$(AR) -r $(OBJ_DIR)/$(CORE_LIB) $(addprefix $(OBJ_DIR)/, $(CORE_OBJS))

environ:
	-mkdir -p $(OBJ_DIR)/core
	-mkdir -p $(OBJ_DIR)/netd
	-mkdir -p $(BIN_DIR)


#==auto rule=============================

core/%.o : src/core/%.cc $(CORE_DEPS)
	$(CC) $(INCLUDES) $(OPTIMIZE) $(OS_SPECIAL) -o $(OBJ_DIR)/$@ -c $<

netd/%.o : src/netd/%.cc $(NETD_DEPS)
	$(CC) $(INCLUDES) $(OPTIMIZE) $(OS_SPECIAL) -o $(OBJ_DIR)/$@ -c $<
