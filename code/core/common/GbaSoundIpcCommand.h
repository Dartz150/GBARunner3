#pragma once

typedef enum
{
    GBA_SOUND_IPC_CMD_SETUP,
    GBA_SOUND_IPC_CMD_SET_TIMER_RELOAD,
    GBA_SOUND_IPC_CMD_SET_TIMER_CONTROL,
    GBA_SOUND_IPC_CMD_GB_REG_WRITE
} GbaSoundIpcCommand;