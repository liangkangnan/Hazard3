.program pwm2
.side_set 1

pull side 1
mov x osr
loop_1:
    jmp x-- loop_1

pull side 0
mov x osr
loop_0:
    jmp x-- loop_0
