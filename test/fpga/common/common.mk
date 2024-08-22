ifndef SRCS
$(error Must define list of test sources as SRCS)
endif

ifndef APP
$(error Must define application name as APP)
endif

CCFLAGS      ?=
LDSCRIPT     ?= ../common/ram.ld
CROSS_PREFIX ?= riscv32-unknown-elf-
INCDIR       ?= ../common

CCFLAGS      += -Wl,--no-warn-rwx-segments -nostartfiles

SRCS += ../drivers/uart/src/uart_drv.c
SRCS += ../common/crt0.S ../common/irq_dispatch.S

INCDIR += ../drivers/uart/inc
INCDIR += ../../../example_soc/libfpga/peris/uart

###############################################################################

.SUFFIXES:
.PHONY: all clean

all: bin

bin: $(APP).bin

clean:
	rm -rf $(APP).bin $(APP).elf $(APP).dis

###############################################################################

$(APP).bin: $(APP).elf
	$(CROSS_PREFIX)objcopy -O binary $^ $@
	$(CROSS_PREFIX)objdump -h $^ > $(APP).dis
	$(CROSS_PREFIX)objdump -d $^ >> $(APP).dis

$(APP).elf: $(SRCS) $(wildcard %.h)
	$(CROSS_PREFIX)gcc $(CCFLAGS) $(SRCS) -T $(LDSCRIPT) $(addprefix -I,$(INCDIR)) -o $@
