; Side-set pin is used for PWM output

.program pwm
.side_set 1 opt

    pull noblock   side 0
    mov y, osr
loop_0:
    jmp y-- loop_0

    pull noblock   side 1
    mov y, osr
loop_1:
    jmp y-- loop_1
