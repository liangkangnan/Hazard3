.program spi
.side_set 1
    pull block side 1
    set y 7

loop:
    out pins 1 side 0 [2] ; Stall here on empty (sideset proceeds even if
    in  pins 1 side 1 [1] ; instruction stalls, so we stall with SCK low)
    jmp y-- loop
    push
