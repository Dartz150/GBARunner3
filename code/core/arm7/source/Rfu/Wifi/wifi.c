#include "common.h"
#include <libtwl/rtos/rtosIrq.h>
#include <libtwl/sys/swi.h>
#include <libtwl/sys/sysPower.h>
#include <string.h>
#include "wifiCommon.h"
#include "wifiRx.h"
#include "wifiTx.h"
#include "wifiBb.h"
#include "wifiRf.h"
#include "wifiFlash.h"
#include "wifi.h"

wifi_work_t gWifiWork;

static wifi_rx_done_callback_t sMpDoneCallback;
static void* sMpDoneCallbackArg;

void wifi_irqPreTbtt()
{
    REG_WIFI_ISR = WIFI_IRQ_PRE_TBTT;
}

void wifi_irqTbtt()
{
    REG_WIFI_ISR = WIFI_IRQ_TBTT;
}

void wifi_irqActEnd()
{
    REG_WIFI_ISR = WIFI_IRQ_ACT_END;
    REG_WIFI_TXQ_CLOSE = WIFI_TXQ_TXQ_ALL;
}

void wifi_irqRfWakeup()
{
    REG_WIFI_ISR = WIFI_IRQ_RF_WAKEUP;
}

void wifi_irqTxErr()
{
    REG_WIFI_ISR = WIFI_IRQ_TX_ERROR;
}

void wifi_irqRxCntup()
{
    REG_WIFI_ISR = WIFI_IRQ_RX_COUNT;
}

void wifi_irqCntOvf()
{
    REG_WIFI_ISR = WIFI_IRQ_RX_COUNT_OVERFLOW | WIFI_IRQ_TX_ERROR_OVERFLOW;
}

void wifi_irqMpEnd()
{
    REG_WIFI_ISR = WIFI_IRQ_MP_END;
    if (sMpDoneCallback)
    {
        sMpDoneCallback(sMpDoneCallbackArg);
    }
}

void wifi_irq(u32)
{
    while (true)
    {
        u16 irqFlags = REG_WIFI_ISR & REG_WIFI_IMR;
        if (!irqFlags)
            break;
        if (irqFlags & WIFI_IRQ_TX_START)
            wifi_irqTxStart();
        if (irqFlags & WIFI_IRQ_RX_START)
            wifi_irqRxStart();
        if (irqFlags & WIFI_IRQ_PRE_TBTT)
            wifi_irqPreTbtt();
        if (irqFlags & WIFI_IRQ_TBTT)
            wifi_irqTbtt();
        if (irqFlags & WIFI_IRQ_ACT_END)
            wifi_irqActEnd();
        if (irqFlags & WIFI_IRQ_RF_WAKEUP)
            wifi_irqRfWakeup();
        if (irqFlags & WIFI_IRQ_TX_ERROR)
            wifi_irqTxErr();
        if (irqFlags & WIFI_IRQ_RX_COUNT)
            wifi_irqRxCntup();
        if (irqFlags & WIFI_IRQ_RX_END)
            wifi_irqRxEnd();
        if (irqFlags & (WIFI_IRQ_RX_COUNT_OVERFLOW | WIFI_IRQ_TX_ERROR_OVERFLOW))
            wifi_irqCntOvf();
        if (irqFlags & WIFI_IRQ_TX_END)
            wifi_irqTxEnd();
        if (irqFlags & WIFI_IRQ_MP_END)
            wifi_irqMpEnd();
    }
}

void wifi_setMpDoneCallback(wifi_mp_done_callback_t callback, void* arg)
{
    sMpDoneCallback = callback;
    sMpDoneCallbackArg = arg;
}

bool wifi_setMode(u8 mode)
{
    if(mode > 3)
    {
        return false;
    }

    REG_WIFI_MAC_CONFIG = (REG_WIFI_MAC_CONFIG & 0xFFF8) | mode;
    return true;
}

bool wifi_setWepMode(u8 mode)
{
    if(mode > 3)
    {
        return false;
    }

    REG_WIFI_MAC_CONFIG = (REG_WIFI_MAC_CONFIG & 0xFFC7) | (mode << 3);
    return true;
}

void wifi_setMacAddress(const wifi_macaddr_t* address)
{
    //write in 16 bits, wifi ram does not support byte writes
    REG_WIFI_MAC_ADRS.address16[0] = address->address16[0];
    REG_WIFI_MAC_ADRS.address16[1] = address->address16[1];
    REG_WIFI_MAC_ADRS.address16[2] = address->address16[2];
}

void wifi_setBssid(const wifi_macaddr_t* bssid)
{
    //write in 16 bits, wifi ram does not support byte writes
    REG_WIFI_BSSID.address16[0] = bssid->address16[0];
    REG_WIFI_BSSID.address16[1] = bssid->address16[1];
    REG_WIFI_BSSID.address16[2] = bssid->address16[2];
}

void wifi_setRetryLimit(u8 limit)
{
    REG_WIFI_RETRY_LIMIT = limit;
}

void wifi_setUseTmpttPowerSave(bool powerSave)
{
    if (powerSave)
    {
        REG_WIFI_WAKEUP_CTRL |= WIFI_WAKEUP_CTRL_TMPTT_POWER_SAVE;
    }
    else
    {
        REG_WIFI_WAKEUP_CTRL &= ~WIFI_WAKEUP_CTRL_TMPTT_POWER_SAVE;
        REG_WIFI_MP_POWER_SEQ = 0;
    }
}

void wifi_setPowerState(int state)
{
    REG_WIFI_SET_POWER = state;
}

bool wifi_setBeaconInterval(u16 interval)
{
    if (interval < 10 || interval > 1000)
    {
        return false;
    }

    REG_WIFI_BEACON_PERIOD = interval;
    return true;
}

bool wifi_setDtimInterval(u8 interval)
{
    if (interval == 0)
    {
        return false;
    }

    REG_WIFI_TIM_COUNT = interval;
    REG_WIFI_BCN_PARAM = 0;
    return true;
}

void wifi_updateTxTimeStampOffset()
{	
    u16 offset = WIFI_RAM->firmData.wifiData.wifiTsfTxOffset + 0x202;
    if(gWifiWork.use2Mbps)
    {
        offset -= 0x6161;
        if (REG_WIFI_TX_PREAMBLE_TYPE & 2)
        {
            offset -= 0x6060;
        }
    }
    REG_WIFI_TSF_TXOFFSET = offset;
}

void wifi_setUse2Mbps(bool use2Mbps)
{
    gWifiWork.use2Mbps = use2Mbps;
    wifi_updateTxTimeStampOffset();
}

void wifi_setShortPreamble(bool shortPreamble)
{
    if(shortPreamble)
    {
        REG_WIFI_TX_PREAMBLE_TYPE |= 6;
    }
    else
    {
        REG_WIFI_TX_PREAMBLE_TYPE &= ~6;
    }
    wifi_updateTxTimeStampOffset();
}

bool wifi_setChannel(int channel)
{
    if(channel < 1 || channel > 14)
        return false;
    channel -= 1;
    int oldPower = REG_WIFI_SET_FORCE_POWER;
    REG_WIFI_SET_FORCE_POWER = 0x8001;
    while(true)
    {
        u16 powerState = REG_WIFI_SET_POWER;
        u16 stat = REG_WIFI_MAC_STATE;
        if((powerState >> 8) == 2 && (stat == WIFI_MAC_STATE_0 || stat == WIFI_MAC_STATE_2))
            break;
    }
    if(WIFI_RAM->firmData.wifiData.rfType == 2 || WIFI_RAM->firmData.wifiData.rfType == 5)
    {
        u32 val = 0;
        memcpy(&val, &WIFI_RAM->firmData.wifiData.rest[0x24 + channel * 6], 3);
        wifi_writeRf(val);
        memcpy(&val, &WIFI_RAM->firmData.wifiData.rest[0x27 + channel * 6], 3);
        wifi_writeRf(val);

        if(gWifiWork.bkReg & 0x10000)
        {
            if(!(gWifiWork.bkReg & 0x8000))
            {
                u8 n = WIFI_RAM->firmData.wifiData.rest[0x86 + channel];
                wifi_writeRf(gWifiWork.bkReg | ((n & 0x1F) << 10));
            }
        }
        else
            wifi_writeBBReg(WIFI_BB_REG_GAIN, WIFI_RAM->firmData.wifiData.rest[0x78 + channel]);
    }
    else if(WIFI_RAM->firmData.wifiData.rfType == 3)
    {
        int n = WIFI_RAM->firmData.wifiData.rfEntryCount;
        int count = WIFI_RAM->firmData.wifiData.rest[n++];
        for (int i = 0; i < count; i++) 
        {
            int reg = WIFI_RAM->firmData.wifiData.rest[n];
            int val = WIFI_RAM->firmData.wifiData.rest[n + 1 + channel];
            wifi_writeBBReg(reg, val);
            n += 15;
        }
        for (int i = 0; i < WIFI_RAM->firmData.wifiData.unk43; i++) 
        {
            int reg = WIFI_RAM->firmData.wifiData.rest[n];
            int val = WIFI_RAM->firmData.wifiData.rest[n + 1 + channel];
            wifi_writeRf((reg << 8) | val | 0x50000);
            n += 15;
        }
    }
    REG_WIFI_SET_FORCE_POWER = oldPower;
    REG_WIFI_MP_POWER_SEQ = 3;
    return true;
}

bool wifi_setActiveZoneTime(u16 time)
{
    if (time < 10)
    {
        return false;
    }

    REG_WIFI_ACTIVE_ZONE_TIMER = time;
    return true;
}

void wifi_initRF()
{
    //initialize the mac tx rx registers from firmware
    wifi_flashdata_t* flashData = &WIFI_RAM->firmData.wifiData;
    REG_WIFI_TXPE_OFF_DELAY = flashData->wifiTxpeOffDelay;
    REG_WIFI_TX_DELAY = flashData->wifiTxDelay;
    REG_WIFI_RX_DELAY = flashData->wifiRxDelay;
    REG_WIFI_TRX_PE_INTERVAL = flashData->wifiTrxPeInterval;
    REG_WIFI_RDY_TIMEOUT = flashData->wifiRdyTimeout;
    REG_WIFI_RX_TIMEOUT = flashData->wifiRxTimeout;
    REG_WIFI_MULTIACK_DELAY_TIME = flashData->wifiMultiAckDelayTime;
    REG_WIFI_CCA_DELAY = flashData->wifiCcaDelay;
    REG_WIFI_ACK_CCA_TIMEOUT = flashData->wifiAckCcaTimeout1;
    REG_WIFI_ACK_CCA_TIMEOUT = flashData->wifiAckCcaTimeout2;
    REG_WIFI_TSF_TXOFFSET = flashData->wifiTsfTxOffset;
    REG_WIFI_TSF_RXOFFSET = flashData->wifiTsfRxOffset;
    REG_WIFI_WAKEUP_CTRL = flashData->wifiWakeupCtrl;
    REG_WIFI_TBTT_ACT_TIME = flashData->wifiTbttActTime;
    REG_WIFI_TMPTT_ACT_TIME = flashData->wifiTmpttActTime;
    REG_WIFI_RF_WAKEUP_TIME = flashData->wifiRfWakeupTime;
    REG_WIFI_RFR_CONFIG = ((flashData->rfBitsPerEntry >> 7) << 8) | (flashData->rfBitsPerEntry & 0x7F);
    if(flashData->rfType == 3)
    {
        //write rf calibration data for version 3
        gWifiWork.bbpCnt = flashData->rest[flashData->rfEntryCount];
        for (int i = 0; i < flashData->rfEntryCount; i++)
        {
            wifi_writeRf(flashData->rest[i] | (i << 8) | 0x50000);
        }
    }
    else
    {
        //write rf calibration data for others
        int entryLength = ((flashData->rfBitsPerEntry & 0x1F) + 7) / 8;
        for (int i = 0; i < flashData->rfEntryCount; i++)
        {
            u32 data = 0;
            memcpy(&data, &flashData->rest[i * entryLength], entryLength);
            wifi_writeRf(data);
            if(flashData->rfType == 2 && (data >> 18) == 9)
            {
                gWifiWork.bkReg = data & ~0x7C00;
            }
        }
    }
}

void wifi_wakeup()
{
    REG_WIFI_SHUTDOWN = WIFI_SHUTDOWN_POWERON;
    swi_waitByLoop(8 * 0x20BA);//we should wait for 8ms
    REG_WIFI_RFR_D_CTRL = 0;
    if (WIFI_RAM->firmData.wifiData.rfType == 2)
    {
        u8 val = wifi_readBBReg(WIFI_BB_REG_01);
        wifi_writeBBReg(WIFI_BB_REG_01, val & 0x7F);
        wifi_writeBBReg(WIFI_BB_REG_01, val);
        swi_waitByLoop(40 * 0x20BA); //we should wait for 40ms
    }
    if (WIFI_RAM->firmData.wifiData.rfType == 2 || WIFI_RAM->firmData.wifiData.rfType == 3)
    {
        wifi_initRF();
    }
}

void wifi_shutdown()
{
    if (WIFI_RAM->firmData.wifiData.rfType == 2)
    {
        wifi_writeRf(0xC008);
    }

    u8 val = wifi_readBBReg(WIFI_BB_REG_GAIN);
    wifi_writeBBReg(WIFI_BB_REG_GAIN, val | 0x3F);
    REG_WIFI_RFR_D_CTRL = 0x800D;
    REG_WIFI_SHUTDOWN = WIFI_SHUTDOWN_POWEROFF;
}

void wifi_initMac()
{
    REG_WIFI_MAC_CMD = 0;
    REG_WIFI_TX_CONFIG = 0;
    REG_WIFI_RX_CONFIG = 0;
    REG_WIFI_IMR = 0;
    REG_WIFI_ISR = 0xFFFF;
    REG_WIFI_DDO_PARA_DAT = 0;
    REG_WIFI_TXQ_RESET = 0xFFFF;
    REG_WIFI_BEACON_ADRS = 0;
    REG_WIFI_TIM_COUNT = 1;
    REG_WIFI_BCN_PARAM = 0;
    REG_WIFI_AID = 0;
    REG_WIFI_KSID = 0;
    REG_WIFI_TSF_ENABLE = 0;
    REG_WIFI_TBTT_ENABLE = 0;
    REG_WIFI_TMPTT_ENABLE = 1;
    REG_WIFI_NAV_ENABLE = 0x3F03;
    REG_WIFI_SERIAL_DAT_SELECT = 1;
    REG_WIFI_TRPE_DIRECT_CTL = 0;
    REG_WIFI_PRE_TBTT = 0x800;
    REG_WIFI_TX_PREAMBLE_TYPE = 1;
    REG_WIFI_RESPONSE_CONTROL = 3;
    REG_WIFI_OVF_THRESHOLD = 4;
    REG_WIFI_DEFRAG_OFFSET = 0x602;
    REG_WIFI_WDMA_JUMP_CNT = 0;
    REG_WIFI_TIMEOUT_PARAM = 0x146;
}

void wifi_init()
{
    memset(&gWifiWork, 0, sizeof(gWifiWork));

    //from dswifi
#define REG_GPIO_WIFI (*(vu16*)0x4004C04)
    REG_GPIO_WIFI = (REG_GPIO_WIFI & 1) | (1 << 8);
    swi_waitByLoop(0xA3A47); //5ms

    sys_setWifiPower(true);
    //set waitstates
    (*(vu16*)0x04000206) = 0x30; //wifi0 first = 9, second = 5; wifi1 first = 17, second = 9

    for (int i = 0; i < WIFI_RAM_SIZE; i += 2)
    {
        *(vu16*)(0x4804000 + i) = 0;
    }

    wifi_initFlashData();

    wifi_wakeup();
    wifi_initMac();
    wifi_initRF();
    wifi_initBB();
    wifi_setMacAddress(&WIFI_RAM->firmData.wifiData.macAddress);
    wifi_setRetryLimit(7);
    wifi_setMode(2);
    wifi_setUse2Mbps(true);
    wifi_setWepMode(0);
    wifi_setBeaconInterval(500);
    wifi_setActiveZoneTime(0xFFFF);
    wifi_setShortPreamble(true);

    wifi_setChannel(13);

    wifi_setReceiveMode(WIFI_BB_CCA_MODE_CS, 0x1F);
    //wifi_shutdown();

    rtos_disableIrqMask(RTOS_IRQ_WIFI);
    rtos_ackIrqMask(RTOS_IRQ_WIFI);
    rtos_setIrqFunc(RTOS_IRQ_WIFI, wifi_irq);
    rtos_enableIrqMask(RTOS_IRQ_WIFI);
    // pmic_setLedConfig(PMIC_CONTROL_LED_BLINK_FAST);
}

void wifi_deinit()
{
    wifi_stop();
    wifi_shutdown();
    sys_setWifiPower(false);
    rtos_disableIrqMask(RTOS_IRQ_WIFI);
    rtos_ackIrqMask(RTOS_IRQ_WIFI);
    // pmic_setLedConfig(PMIC_CONTROL_LED_ON);
}

void wifi_stop()
{
    REG_WIFI_IMR = 0;
    REG_WIFI_MAC_CMD = 0;
    REG_WIFI_TBTT_ENABLE = 0;
    REG_WIFI_TSF_ENABLE = 0;
    REG_WIFI_TX_CONFIG = 0;
    REG_WIFI_RX_CONFIG = 0;
    REG_WIFI_TXQ_CLOSE = 0xFFFF;
    REG_WIFI_TXQ_RESET = 0xFFFF;
}

void wifi_start()
{
    wifi_stop();
    REG_WIFI_WEP_CONFIG = 0x8000;
    REG_WIFI_ACTIVE_ZONE_TIMER = 0xFFFF;
    REG_WIFI_AID = 0;
    REG_WIFI_KSID = 0;
    REG_WIFI_WAKEUP_CTRL = 0xF;
    wifi_initTx();
    wifi_initRx();
    REG_WIFI_MDP_CONFIG = WIFI_MDP_CONFIG_RX_BUF_ENABLE;
    REG_WIFI_ISR = 0xFFFF;
    REG_WIFI_IMR = WIFI_IRQ_TX_END | WIFI_IRQ_RX_END;

    REG_WIFI_TX_CONFIG = 0;
    REG_WIFI_RX_CONFIG = 0;

    REG_WIFI_MAC_CMD = 1;
    REG_WIFI_MP_POWER_SEQ = 0;
    wifi_setUseTmpttPowerSave(false);
    //REG_WIFI_TXREQ_EN_SET = WIFI_TXREQ_CMD;
    wifi_setPowerState(WIFI_POWER_STATE_ACTIVE);
    for (int i = 0; i < 4000; i++)
    {
        if (REG_WIFI_SIGNAL_STATE & WIFI_SIGNAL_STATE_RXPE)
        {
           break;
        }
    }
}