lui t0, 0x100 # a
srli t0, t0, 12
lui t2, 0x1800 # c
srli t2, t2, 12
addi t3, zero, 0 # y
addi t4, zero, 0 # x
addi t5, zero, 0 # k
addi a1, zero, 4
addi a2, zero, 2
addi t6, zero, 64 # y counter
addi s0, zero, 60 # x counter
addi s1, zero, 32 # k counter# loop y start
    addi t4, zero, 0# loop x start
        lui t1, 0x900 # b
        srli t1, t1, 12
        addi s2, zero 0 # s
        mul a3, t4, a1
        add s4, t2, a3 # pc[x]
        addi t5, zero, 0# loop k start
          addi s5, zero, 0
          mul a4, t4, a2
          add s3, t1, a4 # pb[x]
          add s5, t0, t5 # pa[k]
            lw s6, 0, s5
            lw s7, 0, s3
            mul s8, s6, s7
            add s2, s2, s8
            addi t1, t1, 120
            addi t5, t5, 1
        blt t5, s1, -40
      sw s2, 0, s4
      addi t4, t4, 1 # x++
      blt t4, s0, -76 # loop x end
    addi t0, t0, 32
    addi t2, t2, 240
    addi t3, t3, 1 # y++
    blt t3, t6, -96 # loop y end