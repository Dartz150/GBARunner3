#pragma once
#include "ThreadIpcService.h"
#include "DldiIpcCommand.h"
#include "IpcChannels.h"

class DldiIpcService : public ThreadIpcService
{
    u32 _threadStack[128];

    void SetupDldi(const dldi_ipc_cmd_t* cmd) const;
    void ReadSectors(const dldi_ipc_cmd_t* cmd) const;
    void WriteSectors(const dldi_ipc_cmd_t* cmd) const;

public:
    DldiIpcService()
        : ThreadIpcService(IPC_CHANNEL_DLDI, 6, _threadStack, sizeof(_threadStack)) { }

    void HandleMessage(u32 data) override;
};
