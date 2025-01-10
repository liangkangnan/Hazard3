ifndef SRCS
$(error Must define list of test sources as SRCS)
endif

ifndef APP
$(error Must define application name as APP)
endif

SDK_PATH ?= ..

ifeq ($(RUN), FLASH)
LDSCRIPT     ?= $(SDK_PATH)/common/flash_dram.ld
else ifeq ($(RUN), DRAM)
LDSCRIPT     ?= $(SDK_PATH)/common/dram.ld
else
LDSCRIPT     ?= $(SDK_PATH)/common/iram_dram.ld
endif

CROSS_PREFIX ?= riscv32-unknown-elf-

INCDIR       += $(SDK_PATH)/common

CCFLAGS      += -mabi=ilp32 -Wl,--gc-sections -Wl,--no-warn-rwx-segments -nostartfiles --specs=nosys.specs -Wl,-Map,$(APP).map
CCFLAGS      += -Wl,--wrap=malloc -Wl,--wrap=calloc -Wl,--wrap=realloc -Wl,--wrap=free -Wl,--wrap=sprintf -Wl,--wrap=snprintf -Wl,--wrap=vsnprintf
CCFLAGS      += -Wl,--wrap=printf -Wl,--wrap=vprintf -Wl,--wrap=puts -Wl,--wrap=putchar -Wl,--wrap=getchar

SRCS += $(SDK_PATH)/drivers/uart/src/uart_drv.c
SRCS += $(SDK_PATH)/drivers/mach_timer/src/mach_timer_drv.c
SRCS += $(SDK_PATH)/drivers/flash/src/flash_drv.c
SRCS += $(SDK_PATH)/drivers/pio/src/pio_drv.c
SRCS += $(SDK_PATH)/common/crt0.S $(SDK_PATH)/common/exception_table.S $(SDK_PATH)/common/external_irq_table.S $(SDK_PATH)/common/system.c $(SDK_PATH)/common/exception_handler.c
SRCS += $(SDK_PATH)/common/syscalls.c
SRCS += $(SDK_PATH)/common/printf.c
SRCS += $(SDK_PATH)/common/wrap.c
SRCS += $(SDK_PATH)/common/delay.c

INCDIR += $(SDK_PATH)/drivers/uart/inc
INCDIR += $(SDK_PATH)/drivers/mach_timer/inc
INCDIR += $(SDK_PATH)/drivers/flash/inc
INCDIR += $(SDK_PATH)/drivers/pio/inc
INCDIR += $(SDK_PATH)/../../example_soc/libfpga/peris/uart
INCDIR += $(SDK_PATH)/../../example_soc/libfpga/peris/spi_03h_xip
INCDIR += $(SDK_PATH)/../../example_soc/libfpga/peris/pio

###############################################################################

.SUFFIXES:
.PHONY: all clean

all: $(SDK_PATH)/bootrom/bootrom.bin bin

bin: $(APP).bin

clean:
	rm -rf $(APP).bin $(APP).elf $(APP).dis $(APP).map $(APP).flash

###############################################################################

$(APP).bin: $(APP).elf
	$(CROSS_PREFIX)objcopy -O binary $^ $@
	$(CROSS_PREFIX)objdump -h $^ > $(APP).dis
	$(CROSS_PREFIX)objdump -S $^ >> $(APP).dis
	$(CROSS_PREFIX)size --format=berkeley $^
	$(SDK_PATH)/../../tools/mkflashbin.py $(SDK_PATH)/bootrom/bootrom.bin $(APP).bin $(APP).flash

$(APP).elf: $(SRCS) $(wildcard %.h)
	$(CROSS_PREFIX)gcc $(CCFLAGS) $(SRCS) -T $(LDSCRIPT) $(addprefix -I,$(INCDIR)) -o $@

$(SDK_PATH)/bootrom/bootrom.bin:
	make -C $(SDK_PATH)/bootrom/
