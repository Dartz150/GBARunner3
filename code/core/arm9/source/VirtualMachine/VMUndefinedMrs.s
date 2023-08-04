.section ".itcm", "ax"
.altmacro

#include "AsmMacros.inc"
#include "VMDtcmDefs.inc"

.macro vm_armUndefinedMrsCpsrRm rm
    arm_func vm_armUndefinedMrsCpsrR\rm
        ldrb r10, [r13, #(vm_undefinedSpsr - vm_armUndefinedDispatchTable + 3)]
        ldr r9, [r13, #(vm_cpsr - vm_armUndefinedDispatchTable)]
        .if \rm < 8
            orr r\rm, r9, r10, lsl #24
        .elseif \rm < 15
            orr r9, r9, r10, lsl #24
            str r9, [r13, #(vm_cpsr - vm_undefinedRegTmp)]!
            ldmia r13, {r\rm}^
        .else
            // pc is not allowed
        .endif
        msr cpsr_c, #0xDB
        movs pc, lr
.endm

generate vm_armUndefinedMrsCpsrRm, 16

.macro vm_armUndefinedMrsSpsrRm rm
    arm_func vm_armUndefinedMrsSpsrR\rm
        ldr r9, [r13, #(vm_cpsr - vm_armUndefinedDispatchTable)]
        and r9, r9, #0xF
        add r12, r13, r9, lsl #2
        .if \rm < 8
            ldr r\rm, [r12, #(vm_spsr - vm_armUndefinedDispatchTable)]
        .elseif \rm < 15
            ldr r9, [r12, #(vm_spsr - vm_armUndefinedDispatchTable)]
            str r9, [r13, #(vm_cpsr - vm_undefinedRegTmp)]!
            ldmia r13, {r\rm}^
        .else
            // pc is not allowed
        .endif
        msr cpsr_c, #0xDB
        movs pc, lr
.endm

generate vm_armUndefinedMrsSpsrRm, 16

.section ".dtcm", "aw"

.global vm_armUndefinedMrsCpsrRmTable
vm_armUndefinedMrsCpsrRmTable:
    .word vm_armUndefinedMrsCpsrR0
    .word vm_armUndefinedMrsCpsrR1
    .word vm_armUndefinedMrsCpsrR2
    .word vm_armUndefinedMrsCpsrR3
    .word vm_armUndefinedMrsCpsrR4
    .word vm_armUndefinedMrsCpsrR5
    .word vm_armUndefinedMrsCpsrR6
    .word vm_armUndefinedMrsCpsrR7
    .word vm_armUndefinedMrsCpsrR8
    .word vm_armUndefinedMrsCpsrR9
    .word vm_armUndefinedMrsCpsrR10
    .word vm_armUndefinedMrsCpsrR11
    .word vm_armUndefinedMrsCpsrR12
    .word vm_armUndefinedMrsCpsrR13
    .word vm_armUndefinedMrsCpsrR14
    .word vm_armUndefinedMrsCpsrR15

.global vm_armUndefinedMrsSpsrRmTable
vm_armUndefinedMrsSpsrRmTable:
    .word vm_armUndefinedMrsSpsrR0
    .word vm_armUndefinedMrsSpsrR1
    .word vm_armUndefinedMrsSpsrR2
    .word vm_armUndefinedMrsSpsrR3
    .word vm_armUndefinedMrsSpsrR4
    .word vm_armUndefinedMrsSpsrR5
    .word vm_armUndefinedMrsSpsrR6
    .word vm_armUndefinedMrsSpsrR7
    .word vm_armUndefinedMrsSpsrR8
    .word vm_armUndefinedMrsSpsrR9
    .word vm_armUndefinedMrsSpsrR10
    .word vm_armUndefinedMrsSpsrR11
    .word vm_armUndefinedMrsSpsrR12
    .word vm_armUndefinedMrsSpsrR13
    .word vm_armUndefinedMrsSpsrR14
    .word vm_armUndefinedMrsSpsrR15
