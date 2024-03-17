.section ".itcm", "ax"

#include "AsmMacros.inc"
#include "VirtualMachine/VMDtcmDefs.inc"
#include "GbaIoRegOffsets.h"
#include "SdCache/SdCacheDefs.h"
#include "MemoryEmulator/RomDefs.h"
#include "MemoryEmulator/MemoryLoadStoreTableDefs.inc"

/// @brief Loads a 16-bit value from the given GBA memory address.
///        When unaligned rotation is applied.
/// @param r0-r7 Preserved.
/// @param r8 The address to load from. This register is preserved.
/// @param r9 Returns the loaded value. If aligned 0x0000XXYY, otherwise 0xYY0000XX.
/// @param r10-r12 Trashed.
/// @param r13 Preserved.
/// @param lr Return address.
arm_func memu_load16
    mov r10, r8, lsr #23
    ldrh r10, [r10, #memu_load16Table]
    cmp r8, #0x10000000
        bhs memu_load16Undefined
    bx r10

arm_func memu_load16UndefinedZero
    mov r9, #0
    bx lr

arm_func memu_load16Undefined
arm_func memu_load8Undefined
    mov r10, #0
    msr cpsr_c, #0xD7
    mov r13, #0
    str lr, [r13, #memu_inst_addr]
    mrs r13, spsr
    movs r13, r13, lsl #27
    msr cpsr_c, #0xD1
    ldr r10, [r10, #memu_inst_addr]
    subcs r10, r10, #4
    cmp r10, #0x08000000
        bhs undefinedFromRom16

    ldrh r9, [r10]
undefined16Continue:
    tst r8, #1
        bxeq lr
    mov r9, r9, ror #8
    bx lr

undefinedFromRom16:
    push {r8, lr}
    mov r8, r10
    bl memu_load16Rom
    pop {r8, lr}
    b undefined16Continue

arm_func memu_load16Bios
    cmp r8, #0x4000
        bhs memu_load16Undefined
    ldr r9,= memu_biosOpcodeId
    ldr r10,= memu_biosOpcodes
    ldrb r9, [r9]
    tst r8, #2
    ldr r9, [r10, r9, lsl #2]
    moveq r9, r9, lsl #16
    mov r9, r9, lsr #16
    tst r8, #1
        bxeq lr
    mov r9, r9, ror #8
    bx lr

arm_func memu_load16Ewram
    cmp r8, #ROM_LINEAR_END_DS_ADDRESS
        addhs r9, r8, #(ROM_LINEAR_GBA_ADDRESS - ROM_LINEAR_DS_ADDRESS)
        bhs memu_load16RomHiContinue

    bic r9, r8, #0x00FC0000
    ldrh r9, [r9]
    tst r8, #1
        bxeq lr
    mov r9, r9, ror #8
    bx lr

arm_func memu_load16Iwram
    bic r9, r8, #0x00FF0000
    bic r9, r9, #0x00008000
    ldrh r9, [r9]
    tst r8, #1
        bxeq lr
    mov r9, r9, ror #8
    bx lr

arm_func memu_load16Io
    ldr r12,= memu_load16IoTable
    sub r9, r8, #0x04000000
    cmp r9, #0x20C
        bhs memu_load16Undefined
    ldrh r12, [r12, r9]
    tst r8, #1
        bne load16IoUnaligned
    bx r12

load16IoUnaligned:
    push {lr}
    bic r8, r8, #1
    blx r12
    pop {lr}
    mov r9, r9, ror #8
    orr r8, r8, #1
    bx lr

arm_func memu_load16Pltt
    ldr r10,= gShadowPalette
    mov r9, r8, lsl #22
    mov r9, r9, lsr #22
    ldrh r9, [r10, r9]
    tst r8, #1
        movne r9, r9, ror #8
    bx lr

arm_func memu_load16Vram012
    bic r11, r8, #0xFE0000
    tst r11, #0x10000
        addne r11, r11, #0x3F0000
    ldrh r9, [r11]
    tst r8, #1
        bxeq lr
    mov r9, r9, ror #8
    bx lr

arm_func memu_load16Vram345
    bic r11, r8, #0xFE0000
    sub r10, r11, #0x06000000
    cmp r10, #0x14000
        addhs r11, r11, #0x3F0000
    ldrh r9, [r11]
    tst r8, #1
        bxeq lr
    mov r9, r9, ror #8
    bx lr

arm_func memu_load16Oam
    bic r9, r8, #0x400
    ldrh r9, [r9]
    tst r8, #1
        movne r9, r9, ror #8
    bx lr

arm_func memu_load16RomHi
    bic r9, r8, #0x06000000
memu_load16RomHiContinue:
    ldr r11,= (sdc_romBlockToCacheBlock - (0x08000000 >> (SDC_BLOCK_SHIFT - 2)))
    mov r12, r9, lsr #SDC_BLOCK_SHIFT
    ldr r11, [r11, r12, lsl #2]
    bic r9, r9, r12, lsl #SDC_BLOCK_SHIFT
    cmn r11, r8, lsl #31
        ldrgth r9, [r11, r9]
        bxgt lr

    cmp r11, #0
    ldrneh r9, [r11, r9]
    movne r9, r9, ror #8
    bxne lr

arm_func memu_load16RomCacheMiss
    push {r0-r3,lr}
    mov r0, r12, lsl #SDC_BLOCK_SHIFT
    bl sdc_loadRomBlockDirect
    ldrh r9, [r0, r9]
    pop {r0-r3,lr}
    tst r8, #1
        bxeq lr
    mov r9, r9, ror #8
    bx lr

arm_func memu_load16Sram
    ldr r10,= gSaveData
    mov r11, r8, lsl #17
    ldrb r9, [r10, r11, lsr #17]
    orr r9, r9, lsl #8
    tst r8, #1
        bxeq lr
    mov r9, r9, ror #8
    bx lr
