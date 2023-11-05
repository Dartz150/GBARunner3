.section ".itcm", "ax"
.altmacro

#include "AsmMacros.inc"
#include "VMDtcmDefs.inc"

.macro vm_armUndefinedAluSPCImmRn rn
    arm_func vm_armUndefinedAluSPCImmR\rn
        ldr r9, [r13, #(vm_undefinedRegTmp - vm_armUndefinedDispatchTable)]! // dummy read, only used to compute an address
        .if \rn < 8
            mov r8, r\rn
            b vm_armUndefinedAluSPCImm
        .elseif \rn < 15
            stmia r13, {r\rn}^
            b vm_armUndefinedAluSPCImmHiReg
        .else
            // todo?: this value of pc will not be right for relocated code
            ldr r8, [r13, #(vm_undefinedInstructionAddr - vm_undefinedRegTmp)]
            add r8, r8, #8
            b vm_armUndefinedAluSPCImm
        .endif
.endm

generate vm_armUndefinedAluSPCImmRn, 16

vm_armUndefinedAluSPCImmHiReg:
    ldr r8, [r13]
vm_armUndefinedAluSPCImm:
    mov r11, lr, lsl #15
    mov r11, r11, lsr #20
    orr r11, r11, #0xE2000000
    orr r11, r11, #0x00188000
    and lr, lr, #0x1E0000
    orr r11, r11, lr, lsl #4
    str r11, 1f
    mov r9, r13
    ldr r11, [r9, #(vm_cpsr - vm_undefinedRegTmp)]
    nop
1:
    add r8, r8, #0
    and r13, r11, #0xF
    add r12, r9, r13, lsl #2
    ldr r12, [r12, #(vm_spsr - vm_undefinedRegTmp)] // r12 = spsr
    ldr lr, [r9, #(vm_modeSwitchTableAddr - vm_undefinedRegTmp)]
    and r10, r12, #0xDF
    strb r10, [r9, #(vm_cpsr - vm_undefinedRegTmp)]
    and r10, r12, #0xF
    add lr, lr, r10, lsl #5
    add lr, lr, r13, lsl #1
    ldrh lr, [lr]
    adr r13, 2f
    bx lr // call mode switch function
2:
    mov r13, r12
    bl emu_updateIrqs

vm_finishAlusImmNoIrq:
    ldrb r10, [r9, #(vm_undefinedSpsr - vm_undefinedRegTmp)]
    and r11, r13, #0xF0000000
    orr r10, r10, r11
    and r11, r13, #0x20
    orr r10, r10, r11
    msr spsr, r10
    movs pc, r8

.section ".dtcm", "aw"

.global vm_armUndefinedAluSPCImmRnTable
vm_armUndefinedAluSPCImmRnTable:
    .word vm_armUndefinedAluSPCImmR0
    .word vm_armUndefinedAluSPCImmR1
    .word vm_armUndefinedAluSPCImmR2
    .word vm_armUndefinedAluSPCImmR3
    .word vm_armUndefinedAluSPCImmR4
    .word vm_armUndefinedAluSPCImmR5
    .word vm_armUndefinedAluSPCImmR6
    .word vm_armUndefinedAluSPCImmR7
    .word vm_armUndefinedAluSPCImmR8
    .word vm_armUndefinedAluSPCImmR9
    .word vm_armUndefinedAluSPCImmR10
    .word vm_armUndefinedAluSPCImmR11
    .word vm_armUndefinedAluSPCImmR12
    .word vm_armUndefinedAluSPCImmR13
    .word vm_armUndefinedAluSPCImmR14
    .word vm_armUndefinedAluSPCImmR15
