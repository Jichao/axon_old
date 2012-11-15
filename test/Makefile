OBJ_DIR = ../objs
CORE_LIB = libax_core.a
INCLUDES = -I../src/ -L ../objs/
OPTIMIZE = -g3 -DRUN_INTERNAL
LINK_ALL = -Wl,--whole-archive
DONT_LINK_ALL = -Wl,--no-whole-archive

%.o : %.cc
	g++ $(INCLUDES) $(OPTIMIZE) -o $(OBJ_DIR)/$@ -c $<

all: test

test: tc_core 
	@echo "finish build test"

tc_core: tc_core.o
	g++ $(INCLUDES) $(OPTIMIZE) -o tc_core $(OBJ_DIR)/tc_core.o $(LINK_ALL) -lax_core $(DONT_LINK_ALL)
