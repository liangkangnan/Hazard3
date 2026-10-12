#define APP_START_ADDR  0x100
#define APP_LEN_ADDR    (0x100 - 0x4)
#define IRAM_ADDR       0x10000000

.section .text

// ELF entry point:
.global _entry_point
_entry_point:

// Reset handler
_reset_handler:

copy_data:
    /* Load user APP from flash to IRAM */
    li a0, APP_START_ADDR
    li a1, IRAM_ADDR
    li a2, APP_LEN_ADDR
    lw a2, (a2)
    li a3, 0x0
1:
    lw t0, (a0)
    sw t0, (a1)
    addi a0, a0, 4
    addi a1, a1, 4
    addi a3, a3, 4
    bltu a3, a2, 1b

jump_to_iram:
    // app 的入口在 IRAM 区首: 它的 .vector 段是空的(异常向量表在 DRAM, 由 crt0 装到
    // mtvec), .reset 段直接落在区首。所以这里不能加偏移 —— 早先写的 +0x40 是按
    // bootrom 自己的布局(向量表占前 0x40)推的, 跳进去正好落在 app 代码中间。
    li a0, IRAM_ADDR
    jalr a0, a0
    ebreak
1:
    j 1b
