uname_s := $(shell sh -c 'uname -s 2>/dev/null || echo not')

ifeq ($(uname_s), Linux)
  OS_SPECIAL = -DUSE_EPOLL
else
  OS_SPECIAL = -DUSE_KQUEUE
endif


OBJ_DIR = ../objs
CORE_LIB = libax_core.a
EXTRA_LIB = ../depends/lib/libjson.a
INCLUDES = -I../src/ -L ../objs/
OPTIMIZE = -g3 -DRUN_INTERNAL
LINK_ALL = -Wl,--whole-archive
DONT_LINK_ALL = -Wl,--no-whole-archive

%.o : %.cc
	g++ $(INCLUDES) $(OS_SPECIAL) $(OPTIMIZE) -o $(OBJ_DIR)/$@ -c $<

all: test

test: tc_core 
	@echo "finish build test"

tc_core: tc_core.o
	g++ $(INCLUDES) $(OPTIMIZE) -pthread -o tc_core $(OBJ_DIR)/tc_core.o $(LINK_ALL) -lax_core $(DONT_LINK_ALL) $(EXTRA_LIB)
