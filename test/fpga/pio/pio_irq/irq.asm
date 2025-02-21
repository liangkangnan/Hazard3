.program irq

    pull block            ; Pull from FIFO to OSR
    set y, 31             ; Loop 32 times
countloop:
    out pins 1
    jmp y-- countloop     ; Loop until Y hits 0
    irq wait 0            ; Set IRQ0 and wait for it to be cleared
