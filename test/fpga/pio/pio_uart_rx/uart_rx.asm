.program uart_rx

; Minimum viable 8n1 UART receiver. Wait for the start bit, then sample 8 bits
; with the correct timing.
; IN pin 0 is mapped to the GPIO used as UART RX.
; Autopush must be enabled, with a threshold of 8.

    wait 0 pin 0        ; Wait for start bit
    set x, 7 [3]        ; Preload bit counter, delay until eye of first data bit
bitloop:                ; Loop 8 times
    in pins, 1          ; Sample data
    jmp x-- bitloop [2] ; Each iteration is 8 cycles
    push
