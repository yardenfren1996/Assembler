; file ps.as
.entry LIST
.extern W
MAIN:   split    r3,LIST
LOOP:   prn    #48
        lea    W, r6
        inc    r6
MAIN:   add    r3, K
        sub    r1, r4
        bne    END
        cmp    K, #-6
        dec    %END
        dec    W
.entry MAIN
        jmp    %LOOP
        addi    L3, L3
		add    L3, L3
END:    stop now
		stop
.entry W
STR:    .string "higkk,dfsdvm"
LIST:   .data 116, 29, -24 ,-40
        .data -100
K:      .data 31

.extern L3
