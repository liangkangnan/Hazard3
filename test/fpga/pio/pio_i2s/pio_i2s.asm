.program i2s
.side_set 2

; 第0~30bit数据
bitloop0:
    out pins, 1       side 0   ; LRCLK = 0, BCLK = 0
    jmp y-- bitloop0  side 1   ; LRCLK = 0, BCLK = 1

; 第31bit数据
    out pins, 1       side 2   ; LRCLK = 1, BCLK = 0
    mov y,  x         side 3   ; LRCLK = 1, BCLK = 1

; 第32~62bit数据
bitloop1:
    out pins, 1       side 2   ; LRCLK = 1, BCLK = 0
    jmp y-- bitloop1  side 3   ; LRCLK = 1, BCLK = 1

; 第63bit数据
    out pins, 1       side 0   ; LRCLK = 0, BCLK = 0
    mov y,  x         side 1   ; LRCLK = 0, BCLK = 1
