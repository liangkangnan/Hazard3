.program onewire
.side_set 1

entry:
        pull block                         ; wait push data
        set y, 7           side 0          ; loop 8 times

fetch_bit:
        out x, 1           side 0          ; shift next bit from OSR (autopull)     1
        jmp !x  send_0     side 1  [6]     ; pull bus low, branch if sending '0'    7

send_1: ; send a '1' bit
        set x, 7           side 0  [7]     ; release bus, wait for slave response   8
        in pins, 1         side 0  [4]     ; read bus, shift bit to ISR (autopush)  5
loop_e:
        jmp x-- loop_e     side 0  [5]     ;                                    8 x 6
        jmp y-- fetch_bit  side 0          ;                                        1
        jmp entry          side 0          ;                                        1


send_0: ; send a '0' bit
        set x, 7           side 1  [5]     ; continue pulling bus low               6
loop_d:
        jmp x-- loop_d     side 1  [5]     ;                                    8 x 6
        in pins, 1         side 0  [7]     ; release bus, shift 0 to ISR (autopush) 8
        jmp y-- fetch_bit  side 0  [1]     ;                                        1
        jmp entry          side 0          ;                                        1
