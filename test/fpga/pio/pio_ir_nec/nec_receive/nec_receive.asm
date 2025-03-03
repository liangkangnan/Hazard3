.program nec_receive

; Decode IR frames in NEC format and push 32-bit words to the input FIFO.
;
; The input pin should be connected to an IR detector with an 'active low' output.
;
; This program expects there to be 10 state machine clock ticks per 'normal' 562.5us burst period
; in order to permit timely detection of start of a burst. The initialisation function below sets
; the correct divisor to achieve this relative to the system clock.
;
; Within the 'NEC' protocol frames consists of 32 bits sent least-siginificant bit first; so the
; Input Shift Register should be configured to shift right and autopush after 32 bits, as in the
; initialisation function below.
;
;.define BURST_LOOP_COUNTER 30                   ; the detection threshold for a 'frame sync' burst
;.define BIT_SAMPLE_DELAY 15                     ; how long to wait after the end of the burst before sampling


next_burst:
    set x, 30
    wait 0 pin 0                                ; wait for the next burst to start

burst_loop:
    jmp pin data_bit                            ; the burst ended before the counter expired
    jmp x-- burst_loop                          ; wait for the burst to end

                                                ; the counter expired - this is a sync burst
    mov isr, null                               ; reset the Input Shift Register
    wait 1 pin 0                                ; wait for the sync burst to finish
    nop [14]                                    ; some delay
    nop [14]                                    ; some delay
    nop [12]                                    ; some delay
    jmp pin next_burst                          ; short key, got to wait for the first data bit
    mov isr, y                                  ; long key, set all data to zero
    push
    wait 1 pin 0                                ; wait for the long key to finish
    jmp next_burst

data_bit:
    nop [14]                                    ; wait for 1.5 burst periods before sampling the bit value
    in pins, 1                                  ; if the next burst has started then detect a '0' (short gap)
                                                ; otherwise detect a '1' (long gap)
                                                ; after 32 bits the ISR will autopush to the receive FIFO
