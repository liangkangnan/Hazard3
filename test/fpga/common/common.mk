ifndef SRCS
$(error Must define list of test sources as SRCS)
endif

ifndef APP
$(error Must define application name as APP)
endif

LDSCRIPT     ?= ../common/ram.ld
CROSS_PREFIX ?= riscv32-unknown-elf-

INCDIR       += ../common

CCFLAGS      += -Wl,--no-warn-rwx-segments -nostartfiles

SRCS += ../drivers/uart/src/uart_drv.c
SRCS += ../drivers/mach_timer/src/mach_timer_drv.c
SRCS += ../common/xprintf.c
SRCS += ../common/crt0.S ../common/irq_dispatch.S ../common/system.c

INCDIR += ../drivers/uart/inc
INCDIR += ../drivers/mach_timer/inc
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
	$(CROSS_PREFIX)objdump -S $^ >> $(APP).dis

$(APP).elf: $(SRCS) $(wildcard %.h)
	$(CROSS_PREFIX)gcc $(CCFLAGS) $(SRCS) -T $(LDSCRIPT) $(addprefix -I,$(INCDIR)) -o $@
