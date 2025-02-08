; TX Encoding:
; | 31   | 30:23 |
; | READ | Data  |

.program i2c
.side_set 1
entry:
    pull block
    out x 1
    set y 7
    jmp !x write

read:
    set pindirs 0       side 0 ; Set SDA input mode
loop_read:
    in pins 1           side 1 ; read one bit
    jmp y-- loop_read   side 0

    set pindirs 1       side 0 ; Set SDA output mode
    set pins 0          side 0 ; send ACK
    nop                 side 1
    push
    jmp entry

write:
    set pindirs 1       side 0 ; Set SDA output mode
loop_write:
    out pins 1          side 0
    jmp y-- loop_write  side 1

    set pindirs 0       side 0 ; Set SDA input mode
    in pins 1           side 1 ; read ACK
    push
