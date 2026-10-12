ifndef SRCS
$(error Must define list of test sources as SRCS)
endif

ifndef APP
$(error Must define application name as APP)
endif

SDK_PATH ?= ..

# 记录上次用的 RUN(见下面 $(RUN_STAMP) 的规则)
RUN_STAMP := $(APP).run

ifeq ($(RUN), FLASH)
LDSCRIPT     ?= $(SDK_PATH)/common/flash_dram.ld
else ifeq ($(RUN), DRAM)
LDSCRIPT     ?= $(SDK_PATH)/common/dram.ld
else ifeq ($(RUN), IRAM)
LDSCRIPT     ?= $(SDK_PATH)/common/iram_dram.ld
else
$(error please check the RUN parameter)
endif

CROSS_PREFIX ?= riscv32-unknown-elf-

INCDIR       += $(SDK_PATH)/common

ifdef NO_PERI_RESET
# cmod-a7 的 SoC 里没有 peri_reset 外设, 加这个开关让 runtime_init() 跳过它
CCFLAGS      += -DNO_PERI_RESET
endif

CCFLAGS      += -mabi=ilp32 -Wl,--gc-sections -Wl,--no-warn-rwx-segments -nostartfiles --specs=nosys.specs -Wl,-Map,$(APP).map
CCFLAGS      += -Wl,--wrap=malloc -Wl,--wrap=calloc -Wl,--wrap=realloc -Wl,--wrap=free -Wl,--wrap=sprintf -Wl,--wrap=snprintf -Wl,--wrap=vsnprintf
CCFLAGS      += -Wl,--wrap=printf -Wl,--wrap=vprintf -Wl,--wrap=puts -Wl,--wrap=putchar -Wl,--wrap=getchar

SRCS += $(SDK_PATH)/drivers/uart/src/uart_drv.c
SRCS += $(SDK_PATH)/drivers/mach_timer/src/mach_timer_drv.c
SRCS += $(SDK_PATH)/drivers/flash/src/flash_drv.c
SRCS += $(SDK_PATH)/drivers/pio/src/pio_drv.c
SRCS += $(SDK_PATH)/drivers/timer/src/timer_drv.c
SRCS += $(SDK_PATH)/drivers/dma/src/dma_drv.c
SRCS += $(SDK_PATH)/common/crt0.S $(SDK_PATH)/common/exception_table.S $(SDK_PATH)/common/external_irq_table.S $(SDK_PATH)/common/system.c $(SDK_PATH)/common/exception_handler.c
SRCS += $(SDK_PATH)/common/syscalls.c
ifdef USE_QUEUE
SRCS += $(SDK_PATH)/common/queue.c
endif
SRCS += $(SDK_PATH)/common/printf.c
SRCS += $(SDK_PATH)/common/wrap.c
SRCS += $(SDK_PATH)/common/delay.c
SRCS += $(SDK_PATH)/common/xip_qspi.c
SRCS += $(SDK_PATH)/common/utils.c

INCDIR += $(SDK_PATH)/drivers/uart/inc
INCDIR += $(SDK_PATH)/drivers/mach_timer/inc
INCDIR += $(SDK_PATH)/drivers/flash/inc
INCDIR += $(SDK_PATH)/drivers/pio/inc
INCDIR += $(SDK_PATH)/drivers/perireset/inc
INCDIR += $(SDK_PATH)/drivers/timer/inc
INCDIR += $(SDK_PATH)/drivers/dma/inc
INCDIR += $(SDK_PATH)/drivers/crc/inc
INCDIR += $(SDK_PATH)/drivers/tfpu/inc
INCDIR += $(SDK_PATH)/../../example_soc/libfpga/peris/uart
INCDIR += $(SDK_PATH)/../../example_soc/libfpga/peris/spi_qspi_xip
INCDIR += $(SDK_PATH)/../../example_soc/libfpga/peris/pio
INCDIR += $(SDK_PATH)/../../example_soc/libfpga/peris/timer
INCDIR += $(SDK_PATH)/../../example_soc/libfpga/peris/dma
INCDIR += $(SDK_PATH)/../../example_soc/libfpga/peris/crc
INCDIR += $(SDK_PATH)/../../example_soc/libfpga/peris/tfpu

###############################################################################

.SUFFIXES:
.PHONY: all clean

all: $(SDK_PATH)/bootrom/bootrom.bin bin

bin: $(APP).bin

clean:
	rm -rf $(APP).bin $(APP).elf $(APP).dis $(APP).map $(APP).flash $(RUN_STAMP)

###############################################################################

$(APP).bin: $(APP).elf
	$(CROSS_PREFIX)objcopy -O binary $^ $@
	$(CROSS_PREFIX)objdump -h $^ > $(APP).dis
	$(CROSS_PREFIX)objdump -S $^ >> $(APP).dis
	$(CROSS_PREFIX)size --format=berkeley $^
	$(SDK_PATH)/../../tools/mkflashbin.py $(SDK_PATH)/bootrom/bootrom.bin $(APP).bin $(APP).flash

# 换 RUN 必须重新链接: make 不跟踪命令行变量, 只改 RUN 的话 $(APP).elf 看着还是新的,
# 拿到的就是上一种链接地址的镜像(FLASH 在 0x0, IRAM 在 0x10000000), 烧进去直接跑飞。
# 用一个 stamp 记住上次用的 RUN, 它一变就比 $(APP).elf 新, 于是触发重链接。
$(RUN_STAMP): FORCE
	@if [ "$$(cat $@ 2>/dev/null)" != "$(RUN)" ]; then echo "$(RUN)" > $@; fi

.PHONY: FORCE
FORCE:

$(APP).elf: $(SRCS) $(LDSCRIPT) $(RUN_STAMP)
	$(CROSS_PREFIX)gcc $(CCFLAGS) $(SRCS) -T $(LDSCRIPT) $(addprefix -I,$(INCDIR)) -o $@ $(LDFLAG)

$(SDK_PATH)/bootrom/bootrom.bin:
	make -C $(SDK_PATH)/bootrom/
