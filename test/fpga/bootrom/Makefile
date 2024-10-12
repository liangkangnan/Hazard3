APP := bootrom

CROSS_PREFIX ?= riscv32-unknown-elf-

LDSCRIPT     ?= link.ld

CCFLAGS = -march=rv32ima_zicsr_zifencei -g
CCFLAGS += -mabi=ilp32 -Wl,--gc-sections -Wl,--no-warn-rwx-segments -nostartfiles --specs=nosys.specs -Wl,-Map,$(APP).map

INCDIR := .

SRCS := crt0.S

###############################################################################

.SUFFIXES:
.PHONY: all clean

all: bin

bin: $(APP).bin

clean:
	rm -rf $(APP).bin $(APP).elf $(APP).dis $(APP).map

###############################################################################

$(APP).bin: $(APP).elf
	$(CROSS_PREFIX)objcopy -O binary $^ $@
	$(CROSS_PREFIX)objdump -h $^ > $(APP).dis
	$(CROSS_PREFIX)objdump -S $^ >> $(APP).dis
	$(CROSS_PREFIX)size --format=berkeley $^

$(APP).elf: $(SRCS) $(wildcard %.h)
	$(CROSS_PREFIX)gcc $(CCFLAGS) $(SRCS) -T $(LDSCRIPT) $(addprefix -I,$(INCDIR)) -o $@
