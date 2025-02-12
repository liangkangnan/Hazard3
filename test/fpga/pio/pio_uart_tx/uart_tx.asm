.program uart_tx
.side_set 1

; An 8n1 UART transmit program.
; OUT pin 0 and side-set pin 0 are both mapped to UART TX pin.

    pull block side 1 [3]  ; Assert stop bit, or stall with line in idle state
    set x, 7   side 0 [3]  ; Preload bit counter, assert start bit for 4 clocks
bitloop:                   ; This loop will run 8 times (8n1 UART)
    out pins, 1            ; Shift 1 bit from OSR to the first OUT pin
    jmp x-- bitloop   [2]  ; Each loop iteration is 4 cycles.
