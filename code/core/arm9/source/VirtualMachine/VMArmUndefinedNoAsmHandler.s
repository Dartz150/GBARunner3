.section ".itcm", "ax"
.altmacro

#include "AsmMacros.inc"
#include "VMDtcmDefs.inc"

arm_func vm_armUndefinedNoAsmHandler
    ldr r10,= vm_undefinedSpsr
    ldr sp,= (dtcmStackEnd - 0x40)
    ldr r10, [r10]

    add r11, r11, #4
    str r11, [sp, #0x3C]
    stmia sp, {r0-lr}^
    sub r1, r11, #8

    mov r0, lr
    mov r2, sp
    mov r3, r10
#ifndef GBAR3_TEST
    bl jit_handleArmUndefined
#endif
    movs r8, r0, lsr #1
        orrcs r10, r10, #0x20 // thumb bit
    ldmia sp, {r0-lr}^
    msr spsr, r10
    movs pc, r8, lsl #1
