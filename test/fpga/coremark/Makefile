APP  := coremark

SRCS := core_list_join.c core_main.c core_matrix.c core_portme.c core_state.c core_util.c ee_printf.c

CCFLAGS := -march=rv32imac_zicsr_zifencei -O3 -g -mbranch-cost=1 -funroll-all-loops --param max-inline-insns-auto=200 -finline-limit=10000 -fno-code-hoisting -fno-if-conversion2 -falign-functions=4 -falign-jumps=4 -falign-loops=4
CCFLAGS += -DFLAGS_STR=\""$(CCFLAGS)"\"
CCFLAGS += -DITERATIONS=60 -DPERFORMANCE_RUN=1 -DHAS_FLOAT=0

INCDIR := .

include ../common/common.mk
