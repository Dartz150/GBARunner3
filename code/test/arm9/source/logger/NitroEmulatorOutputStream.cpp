#include <nds.h>
#include "NitroEmulatorOutputStream.h"

#define ISND_DBGINFO_ADDRESS                    0x027FFF60
#define ISND_DBGINFO_AGB_ADDR_OFFSET            0x1C
#define ISND_DBGINFO_AGB_ADDR                   (*(u32*)(ISND_DBGINFO_ADDRESS + ISND_DBGINFO_AGB_ADDR_OFFSET))

#define ISND_AGB_PRINT_ARM9_WRITE_PTR_OFFSET    0x90
#define ISND_AGB_PRINT_ARM7_WRITE_PTR_OFFSET    0x92
#define ISND_AGB_PRINT_ARM9_READ_PTR_OFFSET     0x94
#define ISND_AGB_PRINT_ARM7_READ_PTR_OFFSET     0x96
#define ISND_AGB_SOMETHING_OFFSET               0xFE
#define ISND_AGB_PRINT_ARM9_RING_OFFSET         0x8000
#define ISND_AGB_PRINT_ARM7_RING_OFFSET         0xC000
#define ISND_AGB_PRINT_RING_LENGTH              0x4000

NitroEmulatorOutputStream::NitroEmulatorOutputStream()
{
#ifdef LIBTWL_ARM9
    *(vu16*)(ISND_DBGINFO_AGB_ADDR + ISND_AGB_SOMETHING_OFFSET) = 0x202;
#endif
}

void NitroEmulatorOutputStream::Write(const char* str)
{
#ifdef LIBTWL_ARM9
    char c;
    vu16* ring = (vu16*)(ISND_DBGINFO_AGB_ADDR + ISND_AGB_PRINT_ARM9_RING_OFFSET);
    u32 writePtr = *(vu16*)(ISND_DBGINFO_AGB_ADDR + ISND_AGB_PRINT_ARM9_WRITE_PTR_OFFSET);
    u32 readPtr = *(vu16*)(ISND_DBGINFO_AGB_ADDR + ISND_AGB_PRINT_ARM9_READ_PTR_OFFSET);
    while ((c = *str++) != 0)
    {
        u32 newWritePtr = (writePtr + 1) & (ISND_AGB_PRINT_RING_LENGTH - 1);
        while (newWritePtr == readPtr)
        {
            *(vu16*)(ISND_DBGINFO_AGB_ADDR + ISND_AGB_PRINT_ARM9_WRITE_PTR_OFFSET) = writePtr;
            readPtr = *(vu16*)(ISND_DBGINFO_AGB_ADDR + ISND_AGB_PRINT_ARM9_READ_PTR_OFFSET);
        }
        if (writePtr & 1)
            ring[writePtr >> 1] = (ring[writePtr >> 1] & 0xFF) | (c << 8);
        else
            ring[writePtr >> 1] = (ring[writePtr >> 1] & 0xFF00) | c;
        writePtr = newWritePtr;
    }
    *(vu16*)(ISND_DBGINFO_AGB_ADDR + ISND_AGB_PRINT_ARM9_WRITE_PTR_OFFSET) = writePtr;
#endif
}