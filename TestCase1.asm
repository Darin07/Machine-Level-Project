
        .ORIG x3000
        LD R0, NUM1
        LD R1, NUM2
        LD R2, NUM3
        
        
        ADD R3, R1, R0
        ADD R3, R3, R2
        ST  R3, ANSWER

        HALT

NUM1    .FILL   #5
NUM2    .FILL   #15
NUM3    .FILL   #25
ANSWER  .BLKW   #1
        .END