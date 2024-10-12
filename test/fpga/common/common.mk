ifndef SRCS
$(error Must define list of test sources as SRCS)
endif

ifndef APP
$(error Must define application name as APP)
endif

LDSCRIPT     ?= ../common/iram_dram.ld
CROSS_PREFIX ?= riscv32-unknown-elf-

INCDIR       += ../common

CCFLAGS      += -mabi=ilp32 -Wl,--gc-sections -Wl,--no-warn-rwx-segments -nostartfiles --specs=nosys.specs -Wl,-Map,$(APP).map
CCFLAGS      += -Wl,--wrap=malloc -Wl,--wrap=calloc -Wl,--wrap=realloc -Wl,--wrap=free -Wl,--wrap=sprintf -Wl,--wrap=snprintf -Wl,--wrap=vsnprintf
CCFLAGS      += -Wl,--wrap=printf -Wl,--wrap=vprintf -Wl,--wrap=puts -Wl,--wrap=putchar -Wl,--wrap=getchar

SRCS += ../drivers/uart/src/uart_drv.c
SRCS += ../drivers/mach_timer/src/mach_timer_drv.c
SRCS += ../drivers/flash/src/flash_drv.c
SRCS += ../common/crt0.S ../common/exception_table.S ../common/external_irq_table.S ../common/system.c ../common/exception_handler.c
SRCS += ../common/syscalls.c
SRCS += ../common/printf.c
SRCS += ../common/wrap.c
SRCS += ../common/delay.c

INCDIR += ../drivers/uart/inc
INCDIR += ../drivers/mach_timer/inc
INCDIR += ../drivers/flash/inc
INCDIR += ../../../example_soc/libfpga/peris/uart
INCDIR += ../../../example_soc/libfpga/peris/spi_03h_xip

###############################################################################

.SUFFIXES:
.PHONY: all clean

all: bin

bin: $(APP).bin

clean:
	rm -rf $(APP).bin $(APP).elf $(APP).dis $(APP).map $(APP).flash

###############################################################################

$(APP).bin: $(APP).elf
	$(CROSS_PREFIX)objcopy -O binary $^ $@
	$(CROSS_PREFIX)objdump -h $^ > $(APP).dis
	$(CROSS_PREFIX)objdump -S $^ >> $(APP).dis
	$(CROSS_PREFIX)size --format=berkeley $^
	../../../tools/mkflashbin.py ../bootrom/bootrom.bin $(APP).bin $(APP).flash

$(APP).elf: $(SRCS) $(wildcard %.h)
	$(CROSS_PREFIX)gcc $(CCFLAGS) $(SRCS) -T $(LDSCRIPT) $(addprefix -I,$(INCDIR)) -o $@
